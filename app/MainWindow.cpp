#include "MainWindow.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SDG Control");

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *connectionLayout = new QHBoxLayout;

    auto *hostLabel = new QLabel("Host / IP:", central);

    ipEdit = new QLineEdit("sdg2000x", central);
    QSettings settings("sdg-control", "sdg-control");
    ipEdit->setText(settings.value("host", "sdg2000x").toString());

    connectButton = new QPushButton("Connect", central);

    connectionLayout->addWidget(hostLabel);
    connectionLayout->addWidget(ipEdit);
    connectionLayout->addWidget(connectButton);

    layout->addLayout(connectionLayout);

    idEdit = new QLineEdit(central);
    idEdit->setReadOnly(true);
    idEdit->setText("Not connected");

#if 0
    idLabel = new QLabel("Not connected", central);
    idLabel->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                     Qt::TextSelectableByKeyboard);
#endif

    ch1Widget = new ChannelWidget(1, central);
    ch2Widget = new ChannelWidget(2, central);

    auto *refresh = new QPushButton("Refresh", central);

    layout->addWidget(idEdit);

    layout->addWidget(ch1Widget);
    layout->addWidget(ch2Widget);

    layout->addWidget(refresh);

    setCentralWidget(central);

    connect(refresh,
            &QPushButton::clicked,
            this,
            &MainWindow::refreshClicked);

    connect(ch1Widget,
            &ChannelWidget::outputChanged,
            this,
            [this](int channel, bool enabled)
            {
                generator.output(channel, enabled);
            });

    connect(ch2Widget,
            &ChannelWidget::outputChanged,
            this,
            [this](int channel, bool enabled)
            {
                generator.output(channel, enabled);
            });

    connect(ch1Widget,
            &ChannelWidget::frequencyChanged,
            this,
            [this](int channel, double value)
            {
                generator.setFrequency(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::frequencyChanged,
            this,
            [this](int channel, double value)
            {
                generator.setFrequency(channel, value);
            });

    connect(ch1Widget,
            &ChannelWidget::amplitudeChanged,
            this,
            [this](int channel, double value)
            {
                generator.setAmplitude(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::amplitudeChanged,
            this,
            [this](int channel, double value)
            {
                generator.setAmplitude(channel, value);
            });

    connect(ch1Widget,
            &ChannelWidget::offsetChanged,
            this,
            [this](int channel, double value)
            {
                generator.setOffset(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::offsetChanged,
            this,
            [this](int channel, double value)
            {
                generator.setOffset(channel, value);
            });

    connect(ch1Widget,
            &ChannelWidget::waveformChanged,
            this,
            [this](int channel, const QString &waveform)
            {
                generator.setWaveform(channel, waveform);
            });

    connect(ch2Widget,
            &ChannelWidget::waveformChanged,
            this,
            [this](int channel, const QString &waveform)
            {
                generator.setWaveform(channel, waveform);
            });

    connect(connectButton,
            &QPushButton::clicked,
            this,
            &MainWindow::connectClicked);

#if 0
    if (!generator.connectTo("192.168.48.28"))
    {
        idEdit->setText("Connection failed");
        return;
    }
#endif

    resize(500, 250);
}

void MainWindow::refreshClicked()
{
    idEdit->setText(generator.identification());

    auto ch1 = generator.getChannelState(1);
    bool ch1Output = generator.getOutputState(1);

    ch1Widget->setWaveform(ch1.waveform);
    ch1Widget->setFrequency(ch1.frequency);
    ch1Widget->setAmplitude(ch1.amplitude);
    ch1Widget->setOffset(ch1.offset);
    ch1Widget->setOutput(ch1Output);

    ch1Widget->setStatus(
        QString("CH1: %1  %2 Hz  %3 V  Offset %4 V  Output %5")
            .arg(ch1.waveform)
            .arg(ch1.frequency)
            .arg(ch1.amplitude)
            .arg(ch1.offset)
            .arg(ch1Output ? "ON" : "OFF"));

    auto ch2 = generator.getChannelState(2);
    bool ch2Output = generator.getOutputState(2);

    ch2Widget->setWaveform(ch2.waveform);
    ch2Widget->setFrequency(ch2.frequency);
    ch2Widget->setAmplitude(ch2.amplitude);
    ch2Widget->setOffset(ch2.offset);
    ch2Widget->setOutput(ch2Output);

    ch2Widget->setStatus(
        QString("CH2: %1  %2 Hz  %3 V  Offset %4 V  Output %5")
            .arg(ch2.waveform)
            .arg(ch2.frequency)
            .arg(ch2.amplitude)
            .arg(ch2.offset)
            .arg(ch2Output ? "ON" : "OFF"));
}

void MainWindow::connectClicked()
{
    QSettings settings("sdg-control", "sdg-control");

    settings.setValue("host", ipEdit->text());
    if (!generator.connectTo(ipEdit->text()))
    {
        idEdit->setText("Connection failed: " +
                        generator.getConnectionError());
        return;
    }

    idEdit->setText(generator.identification());

    refreshClicked();
}
