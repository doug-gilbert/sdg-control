
#pragma once

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QWidget>

#include <vector>

class StepAdjustSpinBox;


class QuantityRepresentation
{
public:
    virtual ~QuantityRepresentation() = default;

    virtual std::vector<QString> representations() const = 0;
};


class QuantityEdit : public QWidget
{
    Q_OBJECT

public:
    struct Value
    {
        double value;
        QString representation;

        bool operator==(const Value &other) const
        {
            return value == other.value &&
                   representation == other.representation;
        }

        bool operator!=(const Value &other) const
        {
            return !(*this == other);
        }
    };

    explicit QuantityEdit(
        const QuantityRepresentation &representation,
        QWidget *parent = nullptr);

    Value originalValue() const
    {
        return m_originalValue;
    }

    Value finalValue() const
    {
        return currentValue();
    }

    bool modified() const
    {
        return currentValue() != m_originalValue;
    }

    bool isEditing() const
    {
        return m_editing;
    }

    void setValue(double value, const QString &representation);

    Value value() const
    {
        return currentValue();
    } 

    void setSingleStep(double step);
    void setStepLimits(double minimum, double maximum);

    QString debugString() const;

signals:
    void editingStarted();

    void committed(const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final);

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Value currentValue() const;

    void beginEditing();
    void commit();

    // Note that StepAdjustSpinBox is derived from QDoubleSpinBox
    StepAdjustSpinBox *m_valueSpin = nullptr;
    QComboBox *m_representationCombo = nullptr;

    const QuantityRepresentation &m_representation;

    Value m_originalValue{1.0, {}};

    bool m_editing = false;
};

