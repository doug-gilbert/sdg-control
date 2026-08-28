
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

    virtual double scale(const QString &representation) const = 0;

    virtual bool convertible(const QString &from,
                             const QString &to) const = 0;
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
    void setAdaptiveStepType(bool enabled);
    void setDecimals(int num);
    void setSuffix(const QString & suffix);
    void setRange(double minimum, double maximum);

    // get the contents of the SpinBox without leading and trailing
    // spaces as well as any prefix or suffix
    QString cleanText() const;

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

    double convertedValue(double value, const QString &from,
                          const QString &to) const;

    void showRepresentationContextMenu(const QPoint &globalPos);

    // Note that StepAdjustSpinBox is derived from QDoubleSpinBox
    StepAdjustSpinBox *m_valueSpin = nullptr;
    QComboBox *m_representationCombo = nullptr;

    const QuantityRepresentation &m_representation;

    Value m_originalValue{1.0, {}};

    bool m_editing = false;
};

