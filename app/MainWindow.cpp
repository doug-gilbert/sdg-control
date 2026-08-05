
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QCloseEvent>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "MainWindow.h"
#include "debug.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SDG Control");

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *connectionLayout = new QHBoxLayout;

    auto *hostLabel = new QLabel("Host / IP:", central);

    QSettings settings("sdg-control", "sdg-control");
    ipEdit = new QLineEdit(
        settings.value("host", "sdg2000x").toString(),
        central);

    connectButton = new QPushButton("Connect", central);
    disconnectButton = new QPushButton("Disconnect", central);
    disconnectButton->setEnabled(false);

    connectionLayout->addWidget(hostLabel);
    connectionLayout->addWidget(ipEdit);
    connectionLayout->addWidget(connectButton);
    connectionLayout->addWidget(disconnectButton);

    layout->addLayout(connectionLayout);

    idEdit = new QLineEdit(central);
    idEdit->setReadOnly(true);
    idEdit->setText("Not connected");

#if 0
    // QLabel can be made selectable, but QLineEdit(read-only) was chosen
    // because it provides a more obvious copyable text field.
    idLabel = new QLabel("Not connected", central);
    idLabel->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                     Qt::TextSelectableByKeyboard);
#endif

    ch1Widget = new ChannelWidget(1, central);
    ch2Widget = new ChannelWidget(2, central);

    refreshButton = new QPushButton("Refresh", central);
    refreshButton->setEnabled(false);

    layout->addWidget(idEdit);

    auto *channelLayout = new QHBoxLayout;

    channelLayout->addWidget(ch1Widget);
    channelLayout->addWidget(ch2Widget);

    layout->addLayout(channelLayout);

    layout->addWidget(refreshButton);

    setCentralWidget(central);

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

    connect(ch1Widget,
            &ChannelWidget::phaseChanged,
            this,
            [this](int channel, double phase)
            {
                generator.setPhase(channel, phase);
            });

    connect(ch2Widget,
            &ChannelWidget::phaseChanged,
            this,
            [this](int channel, double phase)
            {
                generator.setPhase(channel, phase);
            });

    connect(connectButton,
            &QPushButton::clicked,
            this,
            &MainWindow::connectClicked);

    connect(disconnectButton,
            &QPushButton::clicked,
            this,
            &MainWindow::disconnectClicked);

    connect(refreshButton,
            &QPushButton::clicked,
            this,
            &MainWindow::refreshClicked);

    connect(&generator,
            &SDG2000X::disconnected,
            this,
            &MainWindow::connectionLost);

    auto *fileMenu = menuBar()->addMenu("&File");

    auto *quitAction = fileMenu->addAction("&Quit");

    connect(quitAction,
            &QAction::triggered,
            this,
            &QWidget::close);

    auto *helpMenu = menuBar()->addMenu("&Help");

    auto *aboutAction = helpMenu->addAction("&About SDG Control");

    connect(aboutAction,
            &QAction::triggered,
            this,
            [this]()
            {
                QString text;

                text += QString("SDG Control\n\n");
                text += QString("Version: %1\n")
                            .arg(SDG_CONTROL_VERSION);
                text += QString("Build time: %1")
                            .arg(BUILD_TIME);

                QMessageBox::about(this,
                                   "About SDG Control",
                                   text);
            });

    ch1Widget->setEnabled(false);
    ch2Widget->setEnabled(false);

    resize(800, 350);
}

void MainWindow::refreshClicked()
{
    sdgDebug() << "Refresh clicked";

    if (!generator.isConnected())
    {
        idEdit->setText("Not connected");
        return;
    }

    idEdit->setText(generator.identification());

    auto ch1 = generator.getChannelState(1);

    if (ch1.waveform == "READ TIMEOUT" ||
        ch1.waveform == "WRITE ERROR")
    {
        sdgDebug() << __func__ << ":ch1: WRITE or READ error";
        return;
    }

    bool ch1Output = generator.getOutputState(1);

    ch1Widget->setWaveform(ch1.waveform);
    ch1Widget->setFrequency(ch1.frequency);
    ch1Widget->setAmplitude(ch1.amplitude);
    ch1Widget->setOffset(ch1.offset);
    ch1Widget->setOutput(ch1Output);
    ch1Widget->setPhase(ch1.phase);

    ch1Widget->setStatus(
        QString("CH1: %1  %2 Hz  %3 V  Offset %4 V  Phase %5° Output %6")
            .arg(ch1.waveform)
            .arg(ch1.frequency)
            .arg(ch1.amplitude)
            .arg(ch1.offset)
            .arg(ch1.phase)
            .arg(ch1Output ? "ON" : "OFF"));

    auto ch2 = generator.getChannelState(2);

    if (ch2.waveform == "READ TIMEOUT" ||
        ch2.waveform == "WRITE ERROR")
    {
        sdgDebug() << __func__ << ":ch2: WRITE or READ error";
        return;
    }

    bool ch2Output = generator.getOutputState(2);

    ch2Widget->setWaveform(ch2.waveform);
    ch2Widget->setFrequency(ch2.frequency);
    ch2Widget->setAmplitude(ch2.amplitude);
    ch2Widget->setOffset(ch2.offset);
    ch2Widget->setOutput(ch2Output);
    ch2Widget->setPhase(ch2.phase);

    ch2Widget->setStatus(
        QString("CH2: %1  %2 Hz  %3 V  Offset %4 V  Phase %5° Output %6")
            .arg(ch2.waveform)
            .arg(ch2.frequency)
            .arg(ch2.amplitude)
            .arg(ch2.offset)
            .arg(ch2.phase)
            .arg(ch2Output ? "ON" : "OFF"));
}

void MainWindow::connectClicked()
{
    if (!generator.connectTo(ipEdit->text()))
    {
        idEdit->setText("Connection failed: " +
                        generator.getConnectionError());

        ch1Widget->setEnabled(false);
        ch2Widget->setEnabled(false);

        return;
    }
    ch1Widget->setEnabled(true);
    ch2Widget->setEnabled(true);
    refreshButton->setEnabled(true);

    QSettings settings("sdg-control", "sdg-control");
    settings.setValue("host", ipEdit->text());

    idEdit->setText(generator.identification());

    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);

    refreshClicked();
}

void MainWindow::disconnectClicked()
{
    generator.disconnect();

    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    refreshButton->setEnabled(false);

    ch1Widget->setEnabled(false);
    ch2Widget->setEnabled(false);

    // idEdit->clear();
    idEdit->setText("Disconnected");
}

void MainWindow::connectionLost()
{
    disconnectClicked();
    idEdit->setText("Connection lost");
}

// Investigated delayed busy cursor after exit under Ubuntu GNOME.
// Application exits cleanly. No remaining process or socket. Likely
// desktop startup notification/state issue. Deferred until packaging.
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (generator.isConnected())
    {
        generator.disconnect();
    }
    // sdgDebug() << "closeEvent: accepting";
    event->accept();
}
