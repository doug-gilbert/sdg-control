#include "MainWindow.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QCheckBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SDG Control");

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    idLabel = new QLabel("Instrument: Not connected", central);
    ch1Label = new QLabel("CH1: --", central);
    ch2Label = new QLabel("CH2: --", central);

    ch1OutputCheck = new QCheckBox("CH1 Output", central);

    auto *refresh = new QPushButton("Refresh", central);

    layout->addWidget(idLabel);
    layout->addWidget(ch1Label);
    layout->addWidget(ch2Label);
    layout->addWidget(ch1OutputCheck);
    layout->addWidget(refresh);

    setCentralWidget(central);

    connect(refresh,
            &QPushButton::clicked,
            this,
            &MainWindow::refreshClicked);

    connect(ch1OutputCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                generator.output(1, enabled);

                bool actual = generator.getOutputState(1);

                ch1OutputCheck->blockSignals(true);
                ch1OutputCheck->setChecked(actual);
                ch1OutputCheck->blockSignals(false);
            });

    if (!generator.connectTo("192.168.48.28"))
    {
        idLabel->setText("Connection failed");
        return;
    }

    resize(500, 250);
}


void MainWindow::refreshClicked()
{
    idLabel->setText(generator.identification());

    auto ch1 = generator.getChannelState(1);

    ch1Label->setText(
        QString("CH1: %1  %2 Hz  %3 V  Offset %4 V  Output %5")
            .arg(ch1.waveform)
            .arg(ch1.frequency)
            .arg(ch1.amplitude)
            .arg(ch1.offset)
            .arg(generator.getOutputState(1) ? "ON" : "OFF"));

    ch1OutputCheck->blockSignals(true);
    ch1OutputCheck->setChecked(generator.getOutputState(1));
    ch1OutputCheck->blockSignals(false);

    auto ch2 = generator.getChannelState(2);

    ch2Label->setText(
        QString("CH2: %1  %2 Hz  %3 V  Offset %4 V  Output %5")
            .arg(ch2.waveform)
            .arg(ch2.frequency)
            .arg(ch2.amplitude)
            .arg(ch2.offset)
            .arg(generator.getOutputState(2) ? "ON" : "OFF"));
}
