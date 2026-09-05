
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
#include "AppController.h"
#include "debug.h"


StepAdjustSpinBox::StepAdjustSpinBox(AppController *controller,
                                     QWidget *parent)
        : QDoubleSpinBox(parent),
          m_controller(controller)
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

        if (m_secondMSD) {
            auto *cs = menu->addAction("Current spin: second MS digit");
            cs->setEnabled(false);

            auto *choose = menu->addAction("Let user choose spin digit",
                                             this, [this] {
                onSecondMSD_changed(false);
            });
            choose->setEnabled(true);
        }
        else
        {
            const double currentStep = singleStep();

            QString stepText = QString::number(currentStep, 'f', 6);
            while (stepText.endsWith('0'))
                stepText.chop(1);
            if (stepText.endsWith('.'))
                stepText.chop(1);

            auto *cs = menu->addAction(
                QString("Current step size: %1").arg(stepText));
            cs->setEnabled(false);

            auto *multiply = menu->addAction("<-- 1 digit", this, [this] {
                sdgDebug() << objectName() << "Multiply by 10 selected";

                const double currStep = singleStep();
                const double newStep = currStep * 10.0;

                if (newStep <= m_maximumStep)
                    setAdjustedStep(newStep);
                else
                    sdgDebug() << objectName() << "Multiply by 10 ignored";
            });
            multiply->setEnabled(currentStep * 10.0 <= m_maximumStep);

            auto *divide = menu->addAction("1 digit -->", this, [this] {
                sdgDebug() << objectName() << "Divide by 10 selected";

                const double currStep = singleStep();
                const double newStep = currStep / 10.0;

                if (newStep >= m_minimumStep)
                    setAdjustedStep(newStep);
                else
                    sdgDebug() << objectName() << "Divide by 10 ignored";
            });
            divide->setEnabled(currentStep / 10.0 >= m_minimumStep);

            auto *choose = menu->addAction("second MSDigit", this, [this] {
                onSecondMSD_changed(true);
            });
            choose->setEnabled(true);

            menu->addSeparator();

            auto * defaultStepAct = menu->addAction("Set to default step",
                this,
                [this] {
                    setAdjustedStep(m_defaultStep);
                });
            defaultStepAct->setEnabled(singleStep() != m_defaultStep);
        }

        menu->exec(lineEdit()->mapToGlobal(pos));
        delete menu;  // Note the context menu is destroyed on losing focus
    });

    connect(m_controller,
            &AppController::secondMSD_changed,
            this,
            &StepAdjustSpinBox::onSecondMSD_changed);
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

void StepAdjustSpinBox::setAdaptiveStepType(bool enabled)
{
    QDoubleSpinBox::setStepType(enabled ?
                             QAbstractSpinBox::AdaptiveDecimalStepType :
                             QAbstractSpinBox::DefaultStepType);
}

QLineEdit *StepAdjustSpinBox::lineEditWidget() const
{
    return lineEdit();
}

// Slot for signal secondMSD_changed(bool checked) sent from MainWindow
// MenuBar Edit item
void StepAdjustSpinBox::onSecondMSD_changed(bool checked)
{
    // the context menu(s) only exist when visible
    m_secondMSD = checked;
    setAdaptiveStepType(checked);
}
