
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
#ifdef SDG_DEBUG
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

    immediateCheck = new QCheckBox("Immediate updates", central);
    immediateCheck->setChecked(true);
    immediateCheck->setToolTip(
        "When checked, changes are sent to the function generator "
        "as soon as editing is complete.\n"
        "When unchecked, changes are sent only when Send is pressed.");

    sendButton = new QPushButton("Send", central);
    sendButton->hide();
    sendButton->setEnabled(false);
    sendButton->setToolTip(
        "Send all pending changes to the connected function generator.");

    auto *modeLayout = new QHBoxLayout;

    modeLayout->addWidget(immediateCheck);
    modeLayout->addStretch();
    modeLayout->addWidget(sendButton);

    layout->addLayout(modeLayout);

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
    refreshButton->setToolTip(
        "Read the current settings from the function generator.\n"
        "Any unsent changes are discarded.");

    layout->addWidget(idEdit);

    auto *channelLayout = new QHBoxLayout;

    channelLayout->addWidget(ch1Widget);
    channelLayout->addWidget(ch2Widget);

    layout->addLayout(channelLayout);

    layout->addWidget(refreshButton);

    setCentralWidget(central);

    connect(immediateCheck,
            &QCheckBox::toggled,
            this,
            [this](bool checked)
            {
                immediateMode = checked;

                if (checked)
                {
                    sendButton->hide();
                    setDirty(false);
                }
                else
                {
                    sendButton->show();
                    sendButton->setEnabled(dirty);
                }
            });

    connect(ch1Widget,
            &ChannelWidget::waveformChanged,
            this,
            [this](int channel, const QString &waveform)
            {
                setWaveform(channel, waveform);
            });

    connect(ch2Widget,
            &ChannelWidget::waveformChanged,
            this,
            [this](int channel, const QString &waveform)
            {
                setWaveform(channel, waveform);
            });

    connect(ch1Widget,
            &ChannelWidget::frequencyChanged,
            this,
            [this](int channel, double value)
            {
                setFrequency(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::frequencyChanged,
            this,
            [this](int channel, double value)
            {
                setFrequency(channel, value);
            });

    connect(ch1Widget,
            &ChannelWidget::amplitudeChanged,
            this,
            [this](int channel, double value)
            {
                setAmplitude(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::amplitudeChanged,
            this,
            [this](int channel, double value)
            {
                setAmplitude(channel, value);
            });

    connect(ch1Widget,
            &ChannelWidget::offsetChanged,
            this,
            [this](int channel, double value)
            {
                setOffset(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::offsetChanged,
            this,
            [this](int channel, double value)
            {
                setOffset(channel, value);
            });

    connect(ch1Widget,
            &ChannelWidget::phaseChanged,
            this,
            [this](int channel, double value)
            {
                setPhase(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::phaseChanged,
            this,
            [this](int channel, double value)
            {
                setPhase(channel, value);
            });

    connect(ch1Widget,
            &ChannelWidget::symmetryChanged,
            this,
            [this](int channel, double value)
            {
                setSymmetry(channel, value);
            });

    connect(ch2Widget,
            &ChannelWidget::symmetryChanged,
            this,
            [this](int channel, double value)
            {
                setSymmetry(channel, value);
            });


    connect(ch1Widget,
            &ChannelWidget::outputChanged,
            this,
            [this](int channel, bool enabled)
            {
                setOutput(channel, enabled);
            });

    connect(ch2Widget,
            &ChannelWidget::outputChanged,
            this,
            [this](int channel, bool enabled)
            {
                setOutput(channel, enabled);
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

    connect(sendButton,
            &QPushButton::clicked,
            this,
            &MainWindow::sendClicked);


    connect(&generator,
            &SDG2000X::disconnected,
            this,
            &MainWindow::connectionLost);

    auto *fileMenu = menuBar()->addMenu("&File");

    auto *connectAction = fileMenu->addAction("&Load_settings");
    auto *disconnectAction = fileMenu->addAction("&Save_settings");
    auto *quitAction = fileMenu->addAction("&Quit");

    connect(connectAction,
            &QAction::triggered,
            this,
            &MainWindow::loadSettings);

    connect(disconnectAction,
            &QAction::triggered,
            this,
            &MainWindow::saveSettings);

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

MainWindow::~MainWindow()
{
    sdgDebug() << "MainWindow destructor";
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

    ch1Widget->setWaveform(ch1.waveform);
    ch1Widget->setFrequency(ch1.frequency);
    ch1Widget->setAmplitude(ch1.amplitude);
    ch1Widget->setOffset(ch1.offset);
    ch1Widget->setPhase(ch1.phase);
    ch1Widget->setSymmetry(ch1.symmetry);
    ch1Widget->setOutput(ch1.output);

    ch1Widget->setStatus(
        QString("CH1: %1  %2 Hz  %3 V  Offset %4 V  Phase %5° Sym %6 "
                "Output %7")
            .arg(ch1.waveform)
            .arg(ch1.frequency)
            .arg(ch1.amplitude)
            .arg(ch1.offset)
            .arg(ch1.phase)
            .arg(ch1.symmetry)
            .arg(ch1.output ? "ON" : "OFF"));

    auto ch2 = generator.getChannelState(2);

    if (ch2.waveform == "READ TIMEOUT" ||
        ch2.waveform == "WRITE ERROR")
    {
        sdgDebug() << __func__ << ":ch2: WRITE or READ error";
        return;
    }

    ch2Widget->setWaveform(ch2.waveform);
    ch2Widget->setFrequency(ch2.frequency);
    ch2Widget->setAmplitude(ch2.amplitude);
    ch2Widget->setOffset(ch2.offset);
    ch2Widget->setPhase(ch2.phase);
    ch2Widget->setSymmetry(ch2.symmetry);
    ch2Widget->setOutput(ch2.output);

    ch2Widget->setStatus(
        QString("CH2: %1  %2 Hz  %3 V  Offset %4 V  Phase %5° Sym %6%% "
                "Output %7")
            .arg(ch2.waveform)
            .arg(ch2.frequency)
            .arg(ch2.amplitude)
            .arg(ch2.offset)
            .arg(ch2.phase)
            .arg(ch2.symmetry)
            .arg(ch2.output ? "ON" : "OFF"));

    pendingState[0] = ch1;
    pendingState[1] = ch2;

    setDirty(false);
}

void MainWindow::connectClicked()
{
    sdgDebug() << __func__ ;
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
    sdgDebug() << __func__ ;
    generator.disconnect();

    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    refreshButton->setEnabled(false);

    ch1Widget->setEnabled(false);
    ch2Widget->setEnabled(false);

    // idEdit->clear();
    idEdit->setText("Disconnected");
}

void MainWindow::sendClicked()
{
    bool ok = true;

    sdgDebug() << __func__;

    if (!generator.isConnected())
        return;

    sendButton->setEnabled(false);

    for (int channel = 1; channel <= 2; channel++)
    {
        const auto &state = pendingState.at(channel - 1);
        ok &= generator.applyChannelState(channel, state);
    }

    if (ok)
        setDirty(false);
    else {
        sdgDebug() << __func__ << "setting of at least one field failed";
        sendButton->setEnabled(true);
    }
}

void MainWindow::connectionLost()
{

    idEdit->setText("Connection lost");

    // Update UI state only
    disconnectButton->setEnabled(false);

    connectButton->setEnabled(true);
}

// Investigated delayed busy cursor after exit under Ubuntu GNOME.
// Application exits cleanly. No remaining process or socket. Likely
// desktop startup notification/state issue. Deferred until packaging.
void MainWindow::closeEvent(QCloseEvent *event)
{
    sdgDebug() << __func__;

    if (generator.isConnected())
    {
        generator.disconnect();
    }
    event->accept();
}

void MainWindow::setWaveform(int channel, const QString & waveform)
{
    if (immediateMode)
        generator.setWaveform(channel, waveform);
    else
    {
        pendingState[channel].waveform = waveform;
        setDirty(true);
    }
}

void MainWindow::setFrequency(int channel, double value)
{
    if (immediateMode)
        generator.setFrequency(channel, value);
    else
    {
        pendingState[channel].frequency = value;
        setDirty(true);
    }
}

void MainWindow::setAmplitude(int channel, double value)
{
    if (immediateMode)
        generator.setAmplitude(channel, value);
    else
    {
        pendingState[channel - 1].amplitude = value;
        setDirty(true);
    }
}

void MainWindow::setOffset(int channel, double value)
{
    if (immediateMode)
        generator.setOffset(channel, value);
    else
    {
        pendingState[channel - 1].offset = value;
        setDirty(true);
    }
}

void MainWindow::setPhase(int channel, double value)
{
    if (immediateMode)
        generator.setPhase(channel, value);
    else
    {
        pendingState[channel - 1].phase = value;
        setDirty(true);
    }
}

void MainWindow::setSymmetry(int channel, double value)
{
    if (immediateMode)
        generator.setSymmetry(channel, value);
    else
    {
        pendingState[channel - 1].symmetry = value;
        setDirty(true);
    }
}

void MainWindow::setOutput(int channel, bool enabled)
{
    if (immediateMode)
        generator.output(channel, enabled);
    else
    {
        pendingState[channel - 1].output = enabled;
        setDirty(true);
    }
}

void MainWindow::setDirty(bool value)
{
    dirty = value;

    if (!immediateMode)
        sendButton->setEnabled(dirty);
}

void MainWindow::loadSettings()
{
    sdgDebug() << __func__;
}

void MainWindow::saveSettings()
{
    sdgDebug() << __func__;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Settings"),
        QString(),
        tr("Settings (*.json)")
    );

    if (fileName.isEmpty())
        return;

    SettingsIO::save(fileName, pendingState);
}

void MainWindow::updateWidgetsFromState()
{
    updateChannelWidget(1, pendingState.at(0));
    updateChannelWidget(2, pendingState.at(1));
}

void MainWindow::updateChannelWidget(int channel, const ChannelState &state)
{
    ChannelWidget *widget = (channel == 1) ? ch1Widget : ch2Widget;

    widget->setWaveform(state.waveform);
    widget->setFrequency(state.frequency);
    widget->setAmplitude(state.amplitude);
    widget->setOffset(state.offset);
    widget->setPhase(state.phase);
    widget->setSymmetry(state.symmetry);
    widget->setOutput(state.output);
}
