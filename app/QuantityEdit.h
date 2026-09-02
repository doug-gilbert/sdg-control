
#pragma once

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QWidget>

#include <vector>

class QLabel;

class StepAdjustSpinBox;
class AppController;


class QuantityRepresentation
{
public:
    struct Representation
    {
        QString uiRep;
        QString canonicalRep;
        double ui2CanonicalScale;
    };

    virtual ~QuantityRepresentation() = default;

    virtual std::vector<Representation> representations() const = 0;

    // Returns which one of the representations is canonical (e.g. Hz or V)
    virtual QString canonicalRepresentation() const = 0;

    // Convert a value expressed in `from` representation to `to`.
    // This function has a default implementation that may be overridden
    // by sub-classes.
    virtual double convert(double value,
                           const QString &from,
                           const QString &to) const;

    virtual bool convertible(const QString &from,
                             const QString &to) const = 0;
};

// Holds a [doubleSpinBox, comboBox] or a [doubleSpinBox, label] pair inside
// a widget.
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

    explicit QuantityEdit(AppController *controller,
                          const QuantityRepresentation &representation,
                          QWidget *parent = nullptr);

    Value originalValue() const { return m_originalValue; }

    Value finalValue() const { return currentValue(); }

    bool modified() const { return currentValue() != m_originalValue; }

    bool isEditing() const { return m_editing; }

    void setValue(double value, const QString &representation);

    Value value() const { return currentValue(); }

    double canonicalValue() const;

    // These setters and getters are forwarded to the spinBox (input field)
    void setSingleStep(double step);
    double singleStep() const;
    void setStepLimits(double minimum, double maximum);
    double minimumStep() const;
    double maximumStep() const;
    QAbstractSpinBox::StepType stepType() const;
    bool isStepType2MSD() const; // spins second Most Significant Digit
    void setDecimals(int num);
    int decimals() const;
    void setSuffix(const QString & suffix);
    void setRange(double minimum, double maximum);
    double minimum() const;
    double maximum() const;
    void setToolTip(const QString &toolTip);
    QString toolTip() const;

    // get the contents of the SpinBox without leading and trailing
    // spaces as well as any prefix or suffix
    QString cleanText() const;

    // Yields current and original state of this widget as a QString
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

    AppController *m_controller = nullptr;

    // Note that StepAdjustSpinBox is derived from QDoubleSpinBox
    StepAdjustSpinBox *m_valueSpin = nullptr;
    QComboBox *m_representationCombo = nullptr;
    QLabel *m_representationLabel = nullptr;

    const QuantityRepresentation &m_representation;

    Value m_originalValue{1.0, {}};

    bool m_editing = false;
};
