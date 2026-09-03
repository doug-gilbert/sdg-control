
#include <QAbstractItemView>
#include <QApplication>
#include <QEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QTimer>
#include <QContextMenuEvent>
#include <QMenu>
#include <QLineEdit>
#include <QLabel>

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


// Start of QuantityEdit mehods
QuantityEdit::QuantityEdit(AppController *controller,
                           const QuantityRepresentation &representation,
                           QWidget *parent)
    : QWidget(parent),
      m_controller(controller),
      m_representation(representation)
{
    m_valueSpin = new StepAdjustSpinBox(m_controller, this);
    m_valueSpin->setObjectName("quantityValueSpin");
    m_valueSpin->setRange(-1.0e9, 1.0e9);
    m_valueSpin->setDecimals(6);     // changeable via this->setDecimals()
    m_valueSpin->setSingleStep(0.1);
    m_valueSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    const auto representations = m_representation.representations();

    if (representations.size() == 1)
    {
        m_representationLabel = new QLabel(representations.front().uiRep,
                                           this);
        m_representationLabel->setObjectName("quantityRepresentationLabel");
        m_representationLabel->setSizePolicy(QSizePolicy::Preferred,
                                             QSizePolicy::Fixed);
        m_representationLabel->setContentsMargins(4, 0, 0, 0);
    }
    else
    {
        m_representationCombo = new QComboBox(this);
        m_representationCombo->setObjectName("quantityRepresentationCombo");
        m_representationCombo->setSizePolicy(QSizePolicy::Preferred,
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
    m_valueSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout->addWidget(m_valueSpin, 1);
    if (m_representationCombo)
        layout->addWidget(m_representationCombo, 0);
    else
        layout->addWidget(m_representationLabel, 0);

    m_valueSpin->installEventFilter(this);

    auto *lineEdit = m_valueSpin->lineEditWidget();
    lineEdit->setObjectName("quantityValueLineEdit");
    lineEdit->installEventFilter(this);
    if (m_representationCombo)
        m_representationCombo->installEventFilter(this);

    // this is needed even though (without sdgDebug()) it does not see to
    // do anything useful. Qt6 magic.
    connect(m_valueSpin,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value)
            {
#if 0
                sdgDebug()
                    << objectName()
                    << "SpinBox::valueChanged:"
                    << value;
#else
                Q_UNUSED(value);
#endif
                if (! m_dirty)
                    m_dirty = true;
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
                    m_dirty = true;
                });
    }
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
    const QString &representation,
    bool make_dirty)
{
sdgDebug() << Q_FUNC_INFO << " value=" << value;
    m_valueSpin->blockSignals(true);
    if (m_representationCombo)
        m_representationCombo->blockSignals(true);

    m_valueSpin->setValue(value);

    if (m_representationCombo)
    {
        const int index = m_representationCombo->findText(representation);

        if (index >= 0)
            m_representationCombo->setCurrentIndex(index);
    }
    else
    {
        m_representationLabel->setText(representation);
    }

    if (m_representationCombo)
        m_representationCombo->blockSignals(false);
    m_valueSpin->blockSignals(false);

    m_originalValue = currentValue();
    m_editing = false;
    m_dirty = make_dirty;
}


void QuantityEdit::beginEditing()
{
    if (m_editing)
        return;

    m_originalValue = currentValue();
    m_editing = true;

    emit editingStarted();
}


void QuantityEdit::commit()
{
    if (!m_editing)
        return;

    const Value final = currentValue();

    if (final != m_originalValue)
    {
        emit committed(m_originalValue, final);
    }

    m_editing = false;
}

bool QuantityEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_valueSpin &&
        watched != m_valueSpin->lineEditWidget() &&
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

// This is NOT a Qt6 method, it is implemented in StepAdjustSpinBox
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

double QuantityEdit::convertedValue(double value, const QString &from,
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
                [this, from, to]
                {
                    const double value =
                        convertedValue(
                            m_valueSpin->value(),
                            from,
                            to.uiRep);

                    m_valueSpin->setValue(value);
                    m_representationCombo->setCurrentText(to.uiRep);
                });
    }

    if (menu.isEmpty())
        return;

    menu.exec(globalPos);
}

QString QuantityEdit::cleanText() const
{
     return m_valueSpin ? m_valueSpin->cleanText() : "";
}

QString QuantityEdit::debugString() const
{
    const Value cval = currentValue();

    if (m_representationCombo)
        return QString("current: [%1, %2]  orig: [%3, %4]  editing: %5")
                       .arg(cval.value, 0, 'g', 6)
                       .arg(cval.representation)
                       .arg(m_originalValue.value, 0, 'g', 6)
                       .arg(m_originalValue.representation)
                       .arg(m_editing ? "true" : "false");
    else
        return QString("current: %1  orig: %2  editing: %3")
                       .arg(cval.value, 0, 'g', 6)
                       .arg(m_originalValue.value, 0, 'g', 6)
                       .arg(m_editing ? "true" : "false");
}
