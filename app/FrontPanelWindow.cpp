
#include <QByteArray>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "FrontPanelWindow.h"
#include "Instrument.h"


FrontPanelWindow::FrontPanelWindow(Instrument *a_instrument, QWidget *parent)
    : QWidget(parent),
      instrument(a_instrument)
{
    setWindowTitle("SDG Front Panel");
    setWindowFlags(Qt::Window);
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_DeleteOnClose, false);

    screenLabel = new QLabel(this);
    screenLabel->setAlignment(Qt::AlignCenter);

    updateButton = new QPushButton("Update", this);
    toggleButton = new QPushButton("Toggle channel", this);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(toggleButton);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(screenLabel);
    layout->addLayout(buttonLayout);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    adjustSize();

    connect(updateButton,
            &QPushButton::clicked,
            this,
            &FrontPanelWindow::updateScreen);

    connect(toggleButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                if (!instrument)
                    return;

                if (instrument->toggleChannelFocus() &&
                    instrument->waitForOperationComplete(5000))
                {
                    updateScreen();
                }
            });
}

void FrontPanelWindow::updateScreen()
{
    if (!instrument)
        return;

    QByteArray bmp = instrument->getFrontPanelImage();

    if (bmp.isEmpty())
        return;

    QPixmap pixmap;

    if (pixmap.loadFromData(bmp, "BMP"))
    {
        screenLabel->setPixmap(pixmap);
        screenLabel->setMinimumSize(pixmap.size());
        adjustSize();
    }
    else
    {
        screenLabel->setText("Unable to read SDG screen");
    }
}

void FrontPanelWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}
