
/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QUndoStack>
#include <QWidget>

#include <functional>
#include <vector>

class QLabel;
class QUndoCommand;

class StepAdjustSpinBox;
class AppController;
class QuantityEdit;


class QuantityRepresentation
{
public:
    struct Representation
    {
        QString uiRep;            // Example: mVpp (selection in comboBox)
        QString canonicalRep;     // Thus: Vpp
        double ui2CanonicalScale; // If 0.0 implies no scaling

        // Optional action appended to the ComboBox context menu.
        std::function<void(const QuantityEdit *, int)> callback = nullptr;
        int callbackArg = 0;
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
                             const QString &to) const;
};


/// QuantityEdit is an abstraction over a numeric field and a very closely
/// associated representation selector (e.g. 'mVpp' meaning the unit is
/// (lower) peak to (upper) peak milliVolts). More precisely it is a type
/// of QWidget that includes a StepAdjustSpinBox on the left and a ComboBox
/// on the right. The StepAdjustSpinBox is derived for Qt's QDoubleSpinBox.
/// The comboBox is a QComboBox and in the degenerate case where
/// QuantityRepresentation::representations().size() is 1 then the
/// QComboBox is replaced a QLabel.
/// This class has the concept of 'committed' when editing an instance of
/// this class. After editing a "QuantityEdit" field A, clicking on another
/// field will cause a commit() on field A.
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
                          const bool &dirtyFlag,
                          QWidget *parent = nullptr);

    Value originalValue() const { return m_originalValue; }

    Value finalValue() const { return currentValue(); }

    bool modified() const { return currentValue() != m_originalValue; }

    bool isEditing() const { return m_editing; }

    // Sets the displayed value and resets the editing/dirty state.
    // The undo stack is not altered by this call.
    void setValue(double value, const QString &representation);

    void setCanonicalValue(double value);

    void setRepresentation(const QString &representation);

    Value value() const { return currentValue(); }

    double canonicalValue() const;

    void undo();         // also invoked by Ctrl+Z
    void redo();         // also invoked by Ctrl+Shift+Z (sometimes Ctrl-Y)

    // This is accessing PendingChannelState owned by MainWindow and is not
    // necessarily 1 to 1. For example the Frequency and Period fields share
    // the same dirty/modified flag.
    bool isModified() const { return m_dirtyFlag; }

    // Select (highlight) the numeric field if m_dirtyFlag is true.
    // "Deselecting" is removing the highlight.
    void selectIfDirty() const;
    void selectAll() const;	// select all characters in field
    void deselectIfDirty() const;
    void deselect() const;

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
    void setSuffix(const QString &suffix);
    void setRange(double minimum, double maximum);
    double minimum() const;
    double maximum() const;
    void setValueToolTip(const QString &toolTip);
    void setUnitToolTip(const QString &toolTip);

    // setMinimumWidth() passes through to base class (QWidget)
    // setObjectName() passes through to base class (QWidget)
    QString toolTip() const;

    // get the contents of the SpinBox without leading and trailing
    // spaces as well as any prefix or suffix
    QString cleanText() const;

    void setCanonicalRange(double minimum, double maximum)
    {
        m_canonicalMinimum = minimum;
        m_canonicalMaximum = maximum;
    }

    // Yields current and original state of this widget as a QString
    QString debugString() const;

signals:
    void editingStarted();

    void representationChanged(const QString &representation);

    // Having begun an edit on this instance (containing both a SpinBox and
    // a ComboBox) the user has done something that indicates this edit has
    // finished. That 'something' includes clicking on another field (focus
    // out) or selected 'Finish editing' in the context menu (right click).
    void committed(const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final);

    void valueRestored(const QuantityEdit::Value &oldValue,
                       const QuantityEdit::Value &newValue);

    void frequencySwap(int flag) const;
    void periodSwap(int flag) const;
    void amplitudeModeSwitch(int flag) const;

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    class ValueCommand : public QUndoCommand
    {
    public:
        ValueCommand(QuantityEdit *quantityEdit,
                     const Value &oldValue,
                     const Value &newValue)
            : m_quantityEdit(quantityEdit),
              m_oldValue(oldValue),
              m_newValue(newValue)
        {
        }

        void undo() override
        {
            m_quantityEdit->setValueForUndoRedo(m_oldValue);
        }

        void redo() override
        {
            m_quantityEdit->setValueForUndoRedo(m_newValue);
        }

    private:
        QuantityEdit *m_quantityEdit;
        Value m_oldValue;
        Value m_newValue;
    };

    void setValueWithoutHistory(const Value &value);

    Value currentValue() const;

    void beginEditing();

    // Sends committed signal with original and final values.
    void commit();

    // Applies a value/representation pair during undo/redo without
    // starting or finishing an editing transaction.
    void setValueForUndoRedo(const Value &value);

    // Adds one complete QuantityEdit transaction to the undo stack.
    void pushValueCommand(const Value &oldValue,
                          const Value &newValue);

    double convertedValue(double value,
                          const QString &from,
                          const QString &to) const;

    // Sub-classes may override this simple implementation.
    virtual bool checkCanonicalRange();

    void showRepresentationContextMenu(const QPoint &globalPos);

    AppController *m_controller = nullptr;

    // Note that StepAdjustSpinBox is derived from QDoubleSpinBox.
    StepAdjustSpinBox *m_valueSpin = nullptr;
    QComboBox *m_representationCombo = nullptr;
    QLabel *m_representationLabel = nullptr;

    const QuantityRepresentation &m_representation;

    Value m_originalValue{1.0, {}};

    // Checked when leaving the QuantityEdit field.
    double m_canonicalMinimum = 0.000'000'001;
    double m_canonicalMaximum = 1'000'000'000;

    // Set when user has started editing this field, awaiting commit().
    bool m_editing = false;

    // True while a ValueCommand is restoring the widget.
    bool m_undoRedoInProgress = false;

    // True when this QuantityEdit has been changed during the current
    // editing transaction, regardless of whether the normalized quantity
    // differs from the original value.
    bool m_qdirty = false;

    // Application-level dirty state. This may be shared by multiple
    // QuantityEdit instances (e.g. Frequency and Period).
    const bool &m_dirtyFlag;

    // One undo history for the complete QuantityEdit state:
    // { numeric value, representation }.
    QUndoStack m_undoStack;
};
