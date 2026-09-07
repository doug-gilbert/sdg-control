
#pragma once

#include <QDoubleSpinBox>

class QLineEdit;
class QAction;

class AppController;


// Refines QDoubleSpinBox so ContextMenu on right click can be extended.
// Add choice to allow spin on second MSDigit or user selected digit.
// When user selected digit is chosen (it is the default) then options
// are added to change which digit will be spun.
// Note that in the MainWindows edit MenuBar there is an option to use second
// digit MSD on _all_ instances of this class. That is conveyed to this class
// via the secondMSD_changed signal.
class StepAdjustSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    StepAdjustSpinBox(AppController *controller, QWidget *parent = nullptr);

    void setSingleStep(double step);

    // Expected that minimum and maximum are both > 0.0 and preferably
    // integer powers of ten (including negative powers, e.g. 10^-2 = 0.01).
    void setStepLimits(double minimum, double maximum);

    double minimumStep() const { return m_minimumStep; }
    double maximumStep() const { return m_maximumStep; }

    // Yields the _current_ default
    double defaultStep() const { return m_defaultStep; }

    QLineEdit *edit() const { return lineEdit(); }

    QLineEdit *lineEditWidget() const;

signals:
    void contextMenuAboutToShow(QMenu *);

public slots:
    void onSecondMSD_changed(bool checked);

private:
    // does NOT change m_defaultStep but changes QDoubleSpinBox::singleStep
    void setAdjustedStep(double step);

    void setAdaptiveStepType(bool enabled);

    AppController *m_controller = nullptr;

    double m_minimumStep = 0.000'000'1;
    double m_maximumStep = 100'000'000.0;
    double m_defaultStep = 1.0;   // changeable with setSingleStep()

    bool m_secondMSD = false;     // false: user can choose singleStep
};
