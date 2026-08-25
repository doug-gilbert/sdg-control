
#include <QAbstractItemView>
#include <QApplication>
#include <QEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QTimer>

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


QuantityEdit::QuantityEdit(
    const QuantityRepresentation &representation,
    QWidget *parent)
    : QWidget(parent),
      m_representation(representation)
{
    m_valueSpin = new StepAdjustSpinBox(this);
    m_valueSpin->setObjectName("quantityValueSpin");
    m_valueSpin->setRange(-1.0e9, 1.0e9);
    m_valueSpin->setDecimals(3);
    m_valueSpin->setSingleStep(0.1);
    m_valueSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_representationCombo = new QComboBox(this);
    m_representationCombo->setObjectName("quantityRepresentationCombo");
    m_representationCombo->setSizePolicy(QSizePolicy::Preferred,
                                         QSizePolicy::Fixed);
    m_representationCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    for (const auto &text : m_representation.representations())
        m_representationCombo->addItem(text);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_valueSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_representationCombo->setSizePolicy(QSizePolicy::Preferred,
                                         QSizePolicy::Fixed);

    layout->addWidget(m_valueSpin, 1);
    layout->addWidget(m_representationCombo, 0);

    m_valueSpin->installEventFilter(this);
    m_representationCombo->installEventFilter(this);

    connect(m_valueSpin,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value)
            {
                sdgDebug()
                    << objectName()
                    << "SpinBox::valueChanged:"
                    << value;
            });

    connect(m_representationCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &representationText)
            {
                sdgDebug()
                    << objectName()
                    << "ComboBox::currentTextChanged:"
                    << representationText;
            });
}


QuantityEdit::Value QuantityEdit::currentValue() const
{
    return {
        m_valueSpin->value(),
        m_representationCombo->currentText()
    };
}


void QuantityEdit::setValue(
    double value,
    const QString &representation)
{
    m_valueSpin->blockSignals(true);
    m_representationCombo->blockSignals(true);

    m_valueSpin->setValue(value);

    const int index =
        m_representationCombo->findText(representation);

    if (index >= 0)
        m_representationCombo->setCurrentIndex(index);

    m_valueSpin->blockSignals(false);
    m_representationCombo->blockSignals(false);

    m_originalValue = currentValue();
    m_editing = false;
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

bool QuantityEdit::eventFilter(
    QObject *watched,
    QEvent *event)
{
    if (watched != m_valueSpin &&
        watched != m_representationCombo)
    {
        return QWidget::eventFilter(watched, event);
    }

    if (event->type() == QEvent::FocusIn)
    {
        beginEditing();
    }
    else if (event->type() == QEvent::FocusOut)
    {
        /*
         * FocusOut happens before Qt has necessarily finished
         * moving focus to the next widget.  Check it after the
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
                if (m_representationCombo->view() &&
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

    sdgDebug() << objectName() << __func__;
    QWidget::focusInEvent(event);
}

void QuantityEdit::focusOutEvent(QFocusEvent *event)
{
    sdgDebug() << objectName() << __func__;
    QWidget::focusOutEvent(event);
}

void QuantityEdit::setSingleStep(double step)
{
    m_valueSpin->setSingleStep(step);
}

void QuantityEdit::setStepLimits(double minimum, double maximum)
{
    m_valueSpin->setStepLimits(minimum, maximum);
}

QString QuantityEdit::debugString() const
{
    const Value cval = currentValue();

    return QString("current: [%1, %2]  orig: [%3, %4]  editing: %5")
                   .arg(cval.value, 0, 'g', 6)
                   .arg(cval.representation)
                   .arg(m_originalValue.value, 0, 'g', 6)
                   .arg(m_originalValue.representation)
                   .arg(m_editing ? "true" : "false");
}
