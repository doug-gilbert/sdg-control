
#include <QLineEdit>
#include <QMenu>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "StepAdjustSpinBox.h"
#include "debug.h"


StepAdjustSpinBox::StepAdjustSpinBox(QWidget *parent)
        : QDoubleSpinBox(parent)
{
    lineEdit()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(lineEdit(), &QLineEdit::customContextMenuRequested,
            this, [this](const QPoint &pos) {
        QMenu *menu = lineEdit()->createStandardContextMenu();

        for (QAction *action : menu->actions())
        {
            if (action->text().contains("Paste"))
            {
                connect(action, &QAction::triggered,
                        this, [this]
                        {
                            sdgDebug()
                                << objectName()
                                << "context Paste triggered";

                            lineEdit()->paste();
                        });

                break;
            }
        }
        menu->addSeparator();

        const double currentStep = singleStep();

        QString stepText = QString::number(currentStep, 'f', 6);
        while (stepText.endsWith('0'))
            stepText.chop(1);
        if (stepText.endsWith('.'))
            stepText.chop(1);

        auto *cs = menu->addAction(
            QString("Current step size: %1").arg(stepText));
        cs->setEnabled(false);

        auto *multiply = menu->addAction("Multiply by 10", this, [this] {
            sdgDebug() << objectName() << "Multiply by 10 selected";

            const double currStep = singleStep();
            const double newStep = currStep * 10.0;

            if (newStep <= m_maximumStep)
                setAdjustedStep(newStep);
            else
                sdgDebug() << objectName() << "Multiplication by 10 ignored";
        });

        multiply->setEnabled(currentStep * 10.0 <= m_maximumStep);

        auto *divide = menu->addAction("Divide by 10", this, [this] {
            sdgDebug() << objectName() << "Divide by 10 selected";

            const double currStep = singleStep();
            const double newStep = currStep / 10.0;

            if (newStep >= m_minimumStep)
                setAdjustedStep(newStep);
            else
                sdgDebug() << objectName() << "Divide by 10 ignored";
        });

        divide->setEnabled(currentStep / 10.0 >= m_minimumStep);

        menu->addSeparator();

        auto *defaultStep = menu->addAction(
            "Set to default step",
            this,
            [this] {
                sdgDebug() << objectName() << "Set to default step selected";
                setAdjustedStep(m_defaultStep);
            });

        defaultStep->setEnabled(singleStep() != m_defaultStep);

        menu->exec(lineEdit()->mapToGlobal(pos));
        delete menu;
    });
}

void StepAdjustSpinBox::setStepLimits(double minimum, double maximum)
{
    m_minimumStep = minimum;
    m_maximumStep = maximum;
}

void StepAdjustSpinBox::setSingleStep(double step)
{
    m_defaultStep = step;
    QDoubleSpinBox::setSingleStep(step);
}

void StepAdjustSpinBox::setAdjustedStep(double step)
{
    QDoubleSpinBox::setSingleStep(step);
}

QLineEdit *StepAdjustSpinBox::lineEditWidget() const
{
    return lineEdit();
}
