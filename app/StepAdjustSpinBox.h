#include <QDoubleSpinBox>

// Helper class for extending ContextMenu on right click
// Want to allow user to change the step size of a spinner
class StepAdjustSpinBox : public QDoubleSpinBox
{
public:
    StepAdjustSpinBox(QWidget *parent = nullptr);

    void setSingleStep(double step);

    // Expected that minimum and maximum are both > 0.0 and preferably
    // integer powers of ten (including negative powers, e.g. 10^-2 = 0.01).
    void setStepLimits(double minimum, double maximum);

    double minimumStep() const { return m_minimumStep; }
    double maximumStep() const { return m_maximumStep; }

    double defaultStep() const { return m_defaultStep; }

private:
    void setAdjustedStep(double step);

    double m_minimumStep = 0.000'000'1;
    double m_maximumStep = 100'000'000.0;
    double m_defaultStep = 1.0;
};
