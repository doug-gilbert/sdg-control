
/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QAbstractItemView>
#include <QApplication>
#include <QEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QTimer>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QLineEdit>
#include <QLabel>
#include <QSignalBlocker>
#include <QUndoCommand>
#include <QUndoStack>


#include <limits>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "QuantityEdit.h"
#include "StepAdjustSpinBox.h"
#include "debug.h"


// QuantityRepresentation has default implementation for convert()
// and convertible()
double QuantityRepresentation::convert(double value,
                                       const QString &from,
                                       const QString &to) const
{
    const auto reps = representations();

    const Representation *fromRep = nullptr;
    const Representation *toRep = nullptr;

    for (const auto &rep : reps)
    {
        if (rep.uiRep == from)
            fromRep = &rep;

        if (rep.uiRep == to)
            toRep = &rep;
    }

    Q_ASSERT(fromRep != nullptr);
    Q_ASSERT(toRep != nullptr);
    Q_ASSERT(fromRep->canonicalRep == toRep->canonicalRep);

    const double canonicalValue =
        value * fromRep->ui2CanonicalScale;

    return canonicalValue / toRep->ui2CanonicalScale;
}


bool QuantityRepresentation::convertible(const QString &from,
                                         const QString &to) const
{
    const auto reps = representations();

    const Representation *fromRep = nullptr;
    const Representation *toRep = nullptr;

    for (const auto &rep : reps)
    {
        if (rep.uiRep == from)
            fromRep = &rep;

        if (rep.uiRep == to)
            toRep = &rep;
    }

    if (!fromRep || !toRep)
        return false;

    return fromRep->canonicalRep == toRep->canonicalRep;
}


// ---------------------------------------------------------------------------
// QuantityEdit
// ---------------------------------------------------------------------------

QuantityEdit::QuantityEdit(AppController *controller,
                           const QuantityRepresentation &representation,
                           const bool &dirtyFlag,
                           QWidget *parent)
    : QWidget(parent),
      m_controller(controller),
      m_representation(representation),
      m_dirtyFlag(dirtyFlag)
{
    m_valueSpin = new StepAdjustSpinBox(m_controller, this);
    m_valueSpin->setObjectName("quantityValueSpin");
    m_valueSpin->setRange(-1.0e9, 1.0e9);
    m_valueSpin->setDecimals(6);
    m_valueSpin->setSingleStep(0.1);
    m_valueSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    const auto representations = m_representation.representations();

    if (representations.size() == 1)
    {
        m_representationLabel =
            new QLabel(representations.front().uiRep, this);

        m_representationLabel->setObjectName(
            "quantityRepresentationLabel");

        m_representationLabel->setSizePolicy(
            QSizePolicy::Preferred,
            QSizePolicy::Fixed);

        m_representationLabel->setContentsMargins(4, 0, 0, 0);
    }
    else
    {
        m_representationCombo = new QComboBox(this);
        m_representationCombo->setObjectName(
            "quantityRepresentationCombo");

        m_representationCombo->setSizePolicy(
            QSizePolicy::Preferred,
            QSizePolicy::Fixed);

        m_representationCombo->setSizeAdjustPolicy(
            QComboBox::AdjustToContents);

        for (const auto &text : representations)
            m_representationCombo->addItem(text.uiRep);
    }

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_valueSpin->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed);

    layout->addWidget(m_valueSpin, 1);

    if (m_representationCombo)
        layout->addWidget(m_representationCombo, 0);
    else
        layout->addWidget(m_representationLabel, 0);

    m_valueSpin->installEventFilter(this);

    auto *lineEdit = m_valueSpin->edit();

    lineEdit->setObjectName("quantityValueLineEdit");
    lineEdit->installEventFilter(this);

    if (m_representationCombo)
        m_representationCombo->installEventFilter(this);

    /*
     * textEdited() is emitted only for user changes to the text.
     *
     * This is important when the user has finished an editing transaction
     * with the context-menu "Finish editing" action but keeps focus in the
     * same field. A subsequent edit must begin a new QuantityEdit
     * transaction even though there was no new FocusIn event.
     */
    connect(lineEdit,
            &QLineEdit::textEdited,
            this,
            [this](const QString &)
            {
                if (!m_undoRedoInProgress)
                {
                    beginEditing();
                    m_qdirty = true;
                }
            });

    /*
     * This is needed even though (without sdgDebug()) it does not seem to
     * do anything useful. Qt6 magic.
     */
    connect(m_valueSpin,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value)
            {
                Q_UNUSED(value);
            });

    if (m_representationCombo)
    {
        connect(m_representationCombo,
                &QComboBox::currentTextChanged,
                this,
                [this](const QString &representationText)
                {
                    sdgDebug()
                        << objectName()
                        << "ComboBox::currentTextChanged:"
                        << representationText;

                    emit representationChanged(representationText);
                });
    }

    // This picks up digit spins and SpinBox up and down buttons
    connect(m_valueSpin,
            &StepAdjustSpinBox::userValueChanged,
            this,
            [this](double)
            {
                if (!m_undoRedoInProgress)
                {
                    beginEditing();
                    m_qdirty = true;
                }
            });

    /*
     * The SpinBox owns its numeric/step context menu.
     *
     * QuantityEdit adds its semantic Undo/Redo actions to that menu.
     */
    connect(m_valueSpin,
            &StepAdjustSpinBox::contextMenuAboutToShow,
            this,
            [this](QMenu *menu)
            {
                menu->addSeparator();

                auto *undoAction = menu->addAction("Undo value",
                                    this,
                                    [this]
                                    {
                                        undo();
                                    });
                undoAction->setEnabled(m_qdirty || m_undoStack.canUndo());

                auto *redoAction = menu->addAction("Redo value",
                                    this,
                                    [this]
                                    {
                                        redo();
                                    });
                redoAction->setEnabled(!m_editing && m_undoStack.canRedo());

                menu->addSeparator();

                auto *finish = menu->addAction("Finish editing",
                                    this,
                                    [this]
                                    {
                                        commit();
                                    });
                finish->setEnabled(m_qdirty);
            });
}

QuantityEdit::Value QuantityEdit::currentValue() const
{
    return {m_valueSpin->value(),
            m_representationCombo ?
                  m_representationCombo->currentText()
                : m_representationLabel->text()
           };
}

void QuantityEdit::setValue(
    double value,
    const QString &representation)
{
    setValueWithoutHistory({value, representation});

    m_originalValue = currentValue();
    m_editing = false;
    m_qdirty = false;
}

void QuantityEdit::setCanonicalValue(double value)
{
    const QString representation = currentValue().representation;

    const double displayValue =
        m_representation.convert(
            value,
            m_representation.canonicalRepresentation(),
            representation);

    setValue(displayValue, representation);
}

void QuantityEdit::setRepresentation(const QString &representation)
{
    const Value current = currentValue();

    if (current.representation == representation)
        return;

    if (!m_representation.convertible(current.representation,
                                      representation))
    {
        sdgDebug() << Q_FUNC_INFO
                   << " conversion rejected:"
                   << current.value << current.representation
                   << "->"
                   << representation;
        return;
    }

    const double value =
        m_representation.convert(
            current.value,
            current.representation,
            representation);

    m_valueSpin->blockSignals(true);
    if (m_representationCombo)
        m_representationCombo->blockSignals(true);

    m_valueSpin->setValue(value);

    if (m_representationCombo)
        m_representationCombo->setCurrentText(representation);
    else
        m_representationLabel->setText(representation);

    if (m_representationCombo)
        m_representationCombo->blockSignals(false);
    m_valueSpin->blockSignals(false);

    // Deliberately preserve m_originalValue, m_editing and m_dirty.
}

void QuantityEdit::deselect() const
{
    if (m_valueSpin->edit()->hasSelectedText())
        m_valueSpin->edit()->deselect();
}

void QuantityEdit::deselectIfDirty() const
{
    if (m_dirtyFlag)
        deselect();
}

// Highlight all characters in a field
void QuantityEdit::selectAll() const
{
    m_valueSpin->edit()->selectAll();
}

void QuantityEdit::selectIfDirty() const
{
    if (m_dirtyFlag)
        selectAll();
}

void QuantityEdit::beginEditing()
{
    if (m_editing)
        return;

    DEBUG_FUNC << objectName()
               << "current =" << currentValue().value
               << currentValue().representation;

    m_originalValue = currentValue();
    m_editing = true;
    m_qdirty = false;

    DEBUG_FUNC << objectName()
               << "captured original ="
               << m_originalValue.value
               << m_originalValue.representation;

    emit editingStarted();
}

void QuantityEdit::commit()
{
    if (!m_editing)
        return;

    m_editing = false;

    const Value final = currentValue();

    if (final != m_originalValue)
    {
        pushValueCommand(m_originalValue, final);
        emit committed(m_originalValue, final);
    }
}

bool QuantityEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_valueSpin &&
        watched != m_valueSpin->edit() &&
        watched != m_representationCombo)
    {
        return QWidget::eventFilter(watched, event);
    }

    if (watched == m_representationCombo &&
        event->type() == QEvent::ContextMenu)
    {
        auto *contextEvent = static_cast<QContextMenuEvent *>(event);

        showRepresentationContextMenu(contextEvent->globalPos());
        return true;
    }

    if ((watched == m_valueSpin ||
         watched == m_valueSpin->edit()) &&
        event->type() == QEvent::KeyPress)
    {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->matches(QKeySequence::Undo))
        {
            undo();
            return true;
        }

        if (keyEvent->matches(QKeySequence::Redo))
        {
            redo();
            return true;
        }
    }

    if (event->type() == QEvent::FocusIn)
    {
        beginEditing();
    }
    else if (event->type() == QEvent::FocusOut)
    {
        /*
         * FocusOut happens before Qt has necessarily finished
         * moving focus to the next widget. Check it after the
         * focus transition has completed.
         */
        QTimer::singleShot(
            0,
            this,
            [this]
            {
                QWidget *newFocus = QApplication::focusWidget();

                if (!newFocus)
                {
                    commit();
                    return;
                }

                /*
                 * Moving between the spinbox and combo is still
                 * inside this QuantityEdit.
                 */
                if (newFocus == m_valueSpin ||
                    newFocus == m_representationCombo ||
                    isAncestorOf(newFocus))
                {
                    return;
                }

                /*
                 * QComboBox's popup is a separate widget/window,
                 * so it is not an ancestor of QuantityEdit.
                 *
                 * If the combo currently has an open popup,
                 * don't commit merely because focus moved to it.
                 */
                if (m_representationCombo &&
                    m_representationCombo->view() &&
                    (newFocus == m_representationCombo->view() ||
                     m_representationCombo->view()->isAncestorOf(newFocus)))
                {
                    return;
                }

                commit();
            });
    }

    return QWidget::eventFilter(watched, event);
}

void QuantityEdit::focusInEvent(QFocusEvent *event)
{
    beginEditing();

    sdgDebug() << objectName() << Q_FUNC_INFO;

    QWidget::focusInEvent(event);
}

void QuantityEdit::focusOutEvent(QFocusEvent *event)
{
    sdgDebug() << objectName() << Q_FUNC_INFO;

    QWidget::focusOutEvent(event);
}

double QuantityEdit::canonicalValue() const
{
    const Value value = currentValue();

    return m_representation.convert(
        value.value,
        value.representation,
        m_representation.canonicalRepresentation());
}

// ctor sets this to 0.1
void QuantityEdit::setSingleStep(double step)
{
    m_valueSpin->setSingleStep(step);
}

double QuantityEdit::singleStep() const
{
    return m_valueSpin->singleStep();
}

// This is NOT a Qt6 method, it is implemented in StepAdjustSpinBox.
void QuantityEdit::setStepLimits(double minimum, double maximum)
{
    m_valueSpin->setStepLimits(minimum, maximum);
}

double QuantityEdit::minimumStep() const
{
    return m_valueSpin->minimumStep();
}

double QuantityEdit::maximumStep() const
{
    return m_valueSpin->maximumStep();
}

QAbstractSpinBox::StepType QuantityEdit::stepType() const
{
    return m_valueSpin->stepType();
}

bool QuantityEdit::isStepType2MSD() const
{
    return m_valueSpin->stepType() ==
           QAbstractSpinBox::AdaptiveDecimalStepType;
}

// ctor sets this to 6
void QuantityEdit::setDecimals(int num)
{
    m_valueSpin->setDecimals(num);
}

int QuantityEdit::decimals() const
{
    return m_valueSpin->decimals();
}

// ctor sets this to [-1.0e9, 1.0e9]
void QuantityEdit::setRange(double minimum, double maximum)
{
    m_valueSpin->setRange(minimum, maximum);
}

double QuantityEdit::maximum() const
{
    return m_valueSpin->maximum();
}

double QuantityEdit::minimum() const
{
    return m_valueSpin->minimum();
}

// ctor does NOT set this so there is no suffix by default
void QuantityEdit::setSuffix(const QString &suffix)
{
    m_valueSpin->setSuffix(suffix);
}

void QuantityEdit::setToolTip(const QString &toolTip)
{
    m_valueSpin->setToolTip(toolTip);
}

QString QuantityEdit::toolTip() const
{
    return m_valueSpin->toolTip();
}

double QuantityEdit::convertedValue(double value,
                                    const QString &from,
                                    const QString &to) const
{
    return m_representation.convert(value, from, to);
}

void QuantityEdit::showRepresentationContextMenu(
    const QPoint &globalPos)
{
    const QString from = m_representationCombo->currentText();

    QMenu menu(this);

    for (const auto &to : m_representation.representations())
    {
        if (to.uiRep == from)
            continue;

        if (!m_representation.convertible(from, to.uiRep))
            continue;

        auto *action = menu.addAction(
            QString("Convert %1 to %2").arg(from, to.uiRep));

        connect(action,
                &QAction::triggered,
                this,
                [this, to]
                {
                    /*
                     * setRepresentation() deliberately changes only
                     * the current state. The eventual commit() creates
                     * the single undo command containing both value and
                     * representation.
                     */
                    setRepresentation(to.uiRep);
                });
    }

    /*
     * Keep Undo/Redo/Finish_editing in the representation context menu as
     * well.
     * Unlike the old conversion-only menu, the menu must exist even
     * when there are no conversion actions.
     */
    menu.addSeparator();

    auto *undoAction = menu.addAction("Undo value",
        this,
        [this] {
            undo();
        });
    undoAction->setEnabled(m_qdirty || m_undoStack.canUndo());

    auto *redoAction = menu.addAction("Redo value",
        this,
        [this] {
            redo();
        });
    redoAction->setEnabled(!m_editing && m_undoStack.canRedo());

    menu.addSeparator();

    auto *finish = menu.addAction("Finish editing",
        this,
        [this] {
            commit();
        });
    finish->setEnabled(m_qdirty);

    /*
     * Don't show an entirely empty menu. This shouldn't normally
     * happen because Undo/Redo have just been added, but keeping
     * the guard makes the intention explicit.
     */
    if (menu.isEmpty())
        return;

    menu.exec(globalPos);
}

QString QuantityEdit::cleanText() const
{
    return m_valueSpin ? m_valueSpin->cleanText() : "";
}

bool QuantityEdit::checkCanonicalRange()
{
    double c_value = canonicalValue();

    return c_value >= m_canonicalMinimum &&
           c_value <= m_canonicalMaximum;
}

void QuantityEdit::pushValueCommand(const Value &oldValue,
                                    const Value &newValue)
{
    if (m_undoRedoInProgress || oldValue == newValue)
        return;

    m_undoStack.push(new ValueCommand(this, oldValue, newValue));
}

void QuantityEdit::setValueWithoutHistory(const Value &value)
{
    m_undoRedoInProgress = true;

    m_valueSpin->blockSignals(true);

    if (m_representationCombo)
        m_representationCombo->blockSignals(true);

    m_valueSpin->setValue(value.value);

    if (m_representationCombo)
    {
        const int index =
            m_representationCombo->findText(value.representation);

        if (index >= 0)
            m_representationCombo->setCurrentIndex(index);
    }
    else
    {
        m_representationLabel->setText(value.representation);
    }

    if (m_representationCombo)
        m_representationCombo->blockSignals(false);

    m_valueSpin->blockSignals(false);

    m_undoRedoInProgress = false;
}

void QuantityEdit::undo()
{
    if (m_editing)
    {
        const Value current = currentValue();

        if (current != m_originalValue)
        {
            setValueWithoutHistory(m_originalValue);
            m_qdirty = false;
            m_editing = false;
            return;
        }

        m_editing = false;
    }

    if (!m_undoStack.canUndo())
    {
        return;
    }

    m_undoRedoInProgress = true;
    m_undoStack.undo();
    m_undoRedoInProgress = false;

    m_originalValue = currentValue();
    m_qdirty = false;
}

void QuantityEdit::redo()
{
    if (!m_undoStack.canRedo())
        return;

    m_undoRedoInProgress = true;
    m_undoStack.redo();
    m_undoRedoInProgress = false;

    /*
     * Redo restores a committed history state.
     */
    m_originalValue = currentValue();
    m_editing = false;
    m_qdirty = false;
}

void QuantityEdit::setValueForUndoRedo(const Value &value)
{
    const Value before = currentValue();

    setValueWithoutHistory(value);

    const Value after = currentValue();

    if (before != after)
        emit valueRestored(before, after);

    m_originalValue = after;
    m_editing = false;
    m_qdirty = false;
}

QString QuantityEdit::debugString() const
{
    const Value cval = currentValue();

    if (m_representationCombo)
    {
        return QString(
            "objectName: %1 current: [%2, %3]  "
            "orig: [%4, %5]  editing: %6")
            .arg(objectName())
            .arg(cval.value, 0, 'g', 6)
            .arg(cval.representation)
            .arg(m_originalValue.value, 0, 'g', 6)
            .arg(m_originalValue.representation)
            .arg(m_editing ? "true" : "false");
    }

    return QString(
        "objectName: %1  current: %2  orig: %3  editing: %4")
        .arg(objectName())
        .arg(cval.value, 0, 'g', 6)
        .arg(m_originalValue.value, 0, 'g', 6)
        .arg(m_editing ? "true" : "false");
}
