
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
#include <QComboBox>
#include <QStringList>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "MainWindow.h"
#include "InstrumentFactory.h"

#include "debug.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SDG Control");

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *connectionLayout = new QHBoxLayout;

    auto *instrumentLabel = new QLabel("Instrument:", central);
    instrumentLabel->setToolTip(
        "The Instrument is a Siglent SDG2000X series function generator");

    instrumentCombo = new QComboBox(central);
    instrumentCombo->addItem("Simulator");
    instrumentCombo->addItem("Siglent SDG2000X");
    instrumentCombo->setToolTip(
        "The Simulator is a dummy backend so no real SDG2000X is needed");

    connect(instrumentCombo,
            &QComboBox::currentIndexChanged,
            this,
            [this](int index)
            {
                switch (index)
                {
                case 0:
                    setInstrument(InstrumentType::Simulator);
                    break;

                case 1:
                    setInstrument(InstrumentType::SDG2000X);
                    break;
                }
            });

    auto *hostLabel = new QLabel("Host / IP:", central);

    QSettings settings("sdg-control", "sdg-control");
    ipEdit = new QLineEdit(
        settings.value("host", "sdg2000x").toString(),
        central);

    connectButton = new QPushButton("Connect", central);
    connectButton->setToolTip(
        "Try to connect to the given Host/IP or the simulator");
    disconnectButton = new QPushButton("Disconnect", central);
    disconnectButton->setToolTip(
        "Disconnect from either a Host/IP or the simulator");
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

    connectionLayout->addWidget(instrumentLabel);
    connectionLayout->addWidget(instrumentCombo);
    connectionLayout->addWidget(hostLabel);
    connectionLayout->addWidget(ipEdit);
    connectionLayout->addWidget(connectButton);
    connectionLayout->addWidget(disconnectButton);

    layout->addLayout(connectionLayout);

    idEdit = new QLineEdit(central);
    idEdit->setReadOnly(true);
    idEdit->setFocusPolicy(Qt::NoFocus);
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

    setInstrument(InstrumentType::Simulator);

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

    // Define a Lambda function to ease the tedium of doing a connect for
    // each channel
    auto connectChannelWidgets =
        [this](auto signal, auto setter)
        {
            connect(ch1Widget, signal, this, setter);
            connect(ch2Widget, signal, this, setter);
        };

    connectChannelWidgets(
        &ChannelWidget::waveformChanged,
        [this](int channel, const QString &waveform)
        {
            setWaveform(channel, waveform);
        });

    connectChannelWidgets(
        &ChannelWidget::frequencyChanged,
        [this](int channel, double value)
        {
            setFrequency(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::amplitudeChanged,
        [this](int channel, double value)
        {
            setAmplitude(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::offsetChanged,
        [this](int channel, double value)
        {
            setOffset(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::phaseChanged,
        [this](int channel, double value)
        {
            setPhase(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::rampSymmetryChanged,
        [this](int channel, double value)
        {
            setRampSymmetry(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::pulseWidthChanged,
        [this](int channel, double value)
        {
            setPulseWidth(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::pulseRiseChanged,
        [this](int channel, double value)
        {
            setPulseRise(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::pulseFallChanged,
        [this](int channel, double value)
        {
            setPulseFall(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::noiseBandsetChanged,
        [this](int channel, bool enabled)
        {
            setNoiseBandset(channel, enabled);
        });

    connectChannelWidgets(
        &ChannelWidget::noiseStdevChanged,
        [this](int channel, double value)
        {
            setNoiseStdev(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::noiseMeanChanged,
        [this](int channel, double value)
        {
            setNoiseMean(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::noiseBandwidthChanged,
        [this](int channel, double value)
        {
            setNoiseBandwidth(channel, value);
        });


    connectChannelWidgets(
        &ChannelWidget::outputChanged,
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

    auto *fileMenu = menuBar()->addMenu("&File");

    auto *loadAction = fileMenu->addAction("&Load settings");
    auto *saveAction = fileMenu->addAction("&Save settings");
    auto *quitAction = fileMenu->addAction("&Quit");

    connect(loadAction,
            &QAction::triggered,
            this,
            &MainWindow::loadSettings);

    connect(saveAction,
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

    resize(800, 350);
}

MainWindow::~MainWindow()
{
    sdgDebug() << "MainWindow destructor";
}

void MainWindow::setInstrument(InstrumentType type)
{
    if (generator)
    {
        if (generator->isConnected())
            generator->disconnect();

        delete generator;
    }

    generator = createInstrument(type, this);

    connect(generator,
            &Instrument::disconnected,
            this,
            &MainWindow::connectionLost);

    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    refreshButton->setEnabled(false);

    ch1Widget->setEnabled(false);
    ch2Widget->setEnabled(false);

    pendingState = {};
    setDirty(false);

    idEdit->setText("Not connected");
}

QString MainWindow::displayIdentification(const QString &idn) const
{
    QStringList fields = idn.split(',');

    if (fields.size() >= 4)
        fields[2] = "<hidden serial>";

    return fields.join(',');
}

#ifdef SDG_DEVELOPER_UI
static void setChannelStatus(int my_chan, ChannelWidget & cwid,
                             const ChannelState &ch)
{
#if 1      /* The Qt way versus the C way ... */
    QString common =
    QString("CH%1: %2  %3 Hz  %4 V  Offset %5 V  Phase %6°")
        .arg(my_chan)
        .arg(ch.waveform)
        .arg(ch.frequency, 0, 'f', 1)
        .arg(ch.amplitude, 0, 'f', 3)
        .arg(ch.offset, 0, 'f', 2)
        .arg(ch.phase, 0, 'f', 1);
#else
    char b[128];

    snprintf(b, sizeof(b),
             "CH%d: %s  %.1f Hz  %.3f V  Offset %.2f V  Phase %.1f°",
             my_chan, ch.waveform.toUtf8().constData(), ch.frequency,
             ch.amplitude, ch.offset, ch.phase);
    QString common(b);
#endif

    if (ch.waveform == "RAMP")
        cwid.setStatus( QString("%1  Sym %2  Output %3")
            .arg(common)
            .arg(ch.rampSymmetry)
            .arg(ch.output ? "ON" : "OFF"));
    else if (ch.waveform == "PULSE")
        cwid.setStatus( QString("%1  Width %2  Rise %3  Fall %4  Output %5")
            .arg(common)
            .arg(ch.pulseWidth)
            .arg(ch.pulseRise)
            .arg(ch.pulseFall)
            .arg(ch.output ? "ON" : "OFF"));
    else if (my_chan == 1 && ch.waveform == "SINE")
        cwid.setStatus( QString("%1  Output %2  %3")
            .arg(common)
            .arg(ch.output ? "ON" : "OFF")
            .arg(BUILD_TIME));
    else
        cwid.setStatus( QString("%1  Output %2")
            .arg(common)
            .arg(ch.output ? "ON" : "OFF"));
}
#endif

static void setChannelFields(int my_chan, ChannelWidget & cwid,
                             const ChannelState & ch)
{
    cwid.setWaveform(ch.waveform);
    cwid.setFrequency(ch.frequency);
    cwid.setAmplitude(ch.amplitude);
    cwid.setOffset(ch.offset);
    cwid.setPhase(ch.phase);
    cwid.setRampSymmetry(ch.rampSymmetry);
    cwid.setPulseWidth(ch.pulseWidth);
    cwid.setPulseRise(ch.pulseRise);
    cwid.setPulseFall(ch.pulseFall);
    cwid.setNoiseBandset(ch.noiseBandset);
    cwid.setNoiseStdev(ch.noiseStdev);
    cwid.setNoiseMean(ch.noiseMean);
    cwid.setNoiseBandwidth(ch.noiseBandwidth);

    cwid.setOutput(ch.output);

#ifdef SDG_DEVELOPER_UI
    setChannelStatus(my_chan, cwid, ch);
#else
    Q_UNUSED(my_chan);
#endif
}

static inline bool haveIOError(const ChannelState & ch)
{
    return (ch.waveform == "READ TIMEOUT" || ch.waveform == "WRITE ERROR");
}

void MainWindow::refreshClicked()
{
    sdgDebug() << "Refresh clicked";

    constexpr const char *ioErrorMsg = "WRITE or READ error";

    if (!generator->isConnected())
    {
        idEdit->setText("Not connected");
        return;
    }

    idEdit->setText(displayIdentification(generator->identification()));

    auto ch1 = generator->getChannelState(1);

    if (haveIOError(ch1))
    {
        sdgDebug() << __func__ << ":ch1: " << ioErrorMsg;
        return;
    }
    setChannelFields(1, *ch1Widget, ch1);

    auto ch2 = generator->getChannelState(2);

    if (haveIOError(ch2))
    {
        sdgDebug() << __func__ << ":ch2: " << ioErrorMsg;
        return;
    }
    setChannelFields(2, *ch2Widget, ch2);

    pendingState[0] = ch1;
    pendingState[1] = ch2;

    setDirty(false);
}

void MainWindow::connectClicked()
{
    sdgDebug() << __func__ ;
    if (!generator->connectTo(ipEdit->text()))
    {
        idEdit->setText("Connection failed: " +
                        generator->getConnectionError());

        ch1Widget->setEnabled(false);
        ch2Widget->setEnabled(false);

        return;
    }
    ch1Widget->setEnabled(true);
    ch2Widget->setEnabled(true);
    refreshButton->setEnabled(true);

    QSettings settings("sdg-control", "sdg-control");
    settings.setValue("host", ipEdit->text());

    idEdit->setText(generator->identification());

    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);

    refreshClicked();
    instrumentCombo->setEnabled(false);
}

void MainWindow::disconnectClicked()
{
    sdgDebug() << __func__ ;
    generator->disconnect();

    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    refreshButton->setEnabled(false);

    ch1Widget->setEnabled(false);
    ch2Widget->setEnabled(false);

    // idEdit->clear();
    idEdit->setText("Disconnected");
    instrumentCombo->setEnabled(true);
}

void MainWindow::sendClicked()
{
    bool ok = true;

    sdgDebug() << __func__;

    if (!generator->isConnected())
        return;

    for (int channel = 1; channel <= 2; channel++)
    {
        const auto &state = pendingState.at(channel - 1);
        ok &= generator->applyChannelState(channel, state);
    }

    if (ok)
        setDirty(false);
    else
        sdgDebug() << __func__
                   << "setting of at least one field failed";
    // sdgDebug() << "focusWidget:" << focusWidget();
}

void MainWindow::connectionLost()
{
    sdgDebug() << __func__;

    idEdit->setText("Connection lost");

    disconnectButton->setEnabled(false);
    connectButton->setEnabled(true);
    refreshButton->setEnabled(false);

    ch1Widget->setEnabled(false);
    ch2Widget->setEnabled(false);

    instrumentCombo->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    sdgDebug() << __func__;

    if (generator->isConnected())
    {
        generator->disconnect();
    }
    event->accept();
}

void MainWindow::setWaveform(int channel, const QString & waveform)
{
    pendingState[channel - 1].waveform = waveform;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);

}

void MainWindow::setFrequency(int channel, double value)
{
    pendingState[channel - 1].frequency = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setAmplitude(int channel, double value)
{
    pendingState[channel - 1].amplitude = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setOffset(int channel, double value)
{
    pendingState[channel - 1].offset = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setPhase(int channel, double value)
{
    pendingState[channel - 1].phase = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setRampSymmetry(int channel, double value)
{
    pendingState[channel - 1].rampSymmetry = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setPulseWidth(int channel, double value)
{
    pendingState[channel - 1].pulseWidth = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setPulseRise(int channel, double value)
{
    pendingState[channel - 1].pulseRise = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setPulseFall(int channel, double value)
{
    pendingState[channel - 1].pulseFall = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setNoiseBandset(int channel, bool enabled)
{
    pendingState[channel - 1].noiseBandset = enabled;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setNoiseStdev(int channel, double value)
{
    pendingState[channel - 1].noiseStdev = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setNoiseMean(int channel, double value)
{
    pendingState[channel - 1].noiseMean = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setNoiseBandwidth(int channel, double value)
{
    pendingState[channel - 1].noiseBandwidth = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setOutput(int channel, bool enabled)
{
    pendingState[channel - 1].output = enabled;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
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

    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Load Settings"),
        QString(),
        tr("JSON settings (*.json)")
    );

    if (fileName.isEmpty())
        return;

    std::array<ChannelState, 2> loadedState;
    QString error;

    if (!SettingsIO::load(fileName, loadedState, &error))
    {
        sdgDebug() << "Load failed:" << error;
        return;
    }

    pendingState = loadedState;

    updateWidgetsFromState();

    setDirty(true);
}

void MainWindow::saveSettings()
{
    sdgDebug() << __func__;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Settings"),
        QString(),
        tr("JSON settings (*.json)")
    );

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".json", Qt::CaseInsensitive))
        fileName += ".json";

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
    widget->setRampSymmetry(state.rampSymmetry);
    widget->setPulseWidth(state.pulseWidth);
    widget->setPulseRise(state.pulseRise);
    widget->setPulseFall(state.pulseFall);
    widget->setNoiseBandset(state.noiseBandset);
    widget->setNoiseStdev(state.noiseStdev);
    widget->setNoiseMean(state.noiseMean);
    widget->setNoiseBandwidth(state.noiseBandwidth);

    widget->setOutput(state.output);
}
