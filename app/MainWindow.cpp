
#include <QApplication>
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
#include <QSettings>
#include <QFileDialog>
#include <QFrame>
#include <QStatusBar>
#include <QToolTip>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "MainWindow.h"
#include "FrontPanelWindow.h"
#include "InstrumentFactory.h"
#include "SettingsIO.h"
#include "ChannelWidget.h"
#include "QuantityEdit.h"
#include "AppController.h"
#include "cli_options.h"

#include "debug.h"


MainWindow::MainWindow(const CLI_options &cli_opts, QWidget *parent)
    : QMainWindow(parent),
      m_cli_options(cli_opts),
      m_controller(new AppController(this))
{
    setWindowTitle("SDG Control");

    // this should give us a Status Bar at the bottom of the main window
    // statusBar();

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *connectionLayout = new QHBoxLayout;

    auto *instrumentLabel = new QLabel("Instrument:", central);
    instrumentLabel->setToolTip(
        "The Instrument is a Siglent SDG2000X series function generator");

    m_instrumentCombo = new QComboBox(central);
    m_instrumentCombo->addItem("Simulator");
    m_instrumentCombo->addItem("Siglent SDG2000X");
    m_instrumentCombo->setToolTip(
        "The Simulator is a dummy backend so no real SDG2000X is needed");

    connect(m_instrumentCombo,
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
    m_ipaddrEdit = new QLineEdit(
        settings.value("host", "sdg2000x").toString(),
        central);
    m_ipaddrEdit->setObjectName("ipaddrEdit");

    m_connectButton = new QPushButton("Connect", central);
    m_connectButton->setObjectName("connectButton");
    m_connectButton->setToolTip(
        "Try to connect to the given Host/IP or the simulator");
    m_disconnectButton = new QPushButton("Disconnect", central);
    m_disconnectButton->setObjectName("disconnectButton");
    m_disconnectButton->setToolTip(
        "Disconnect from either a Host/IP or the simulator");
    m_disconnectButton->setEnabled(false);

    m_immediateCheck = new QCheckBox("Immediate updates", central);
    m_immediateCheck->setObjectName("immediateCheck");
    m_immediateCheck->setChecked(true);
    m_immediateCheck->setToolTip(
        "When Checked: changes are sent to the function generator\n"
        "when editing of each field is complete.\n"
        "When Unchecked: changes are sent when Send button is pressed.");

    m_sendButton = new QPushButton("Send", central);
    m_sendButton->setObjectName("sendButton");
    m_sendButton->hide();
    m_sendButton->setEnabled(false);
    m_sendButton->setToolTip(
        "Send all pending changes to the connected function generator.");

    auto *modeLayout = new QHBoxLayout;

    modeLayout->addWidget(m_immediateCheck);
    modeLayout->addStretch();
    modeLayout->addWidget(m_sendButton);

    layout->addLayout(modeLayout);

    connectionLayout->addWidget(instrumentLabel);
    connectionLayout->addWidget(m_instrumentCombo);
    connectionLayout->addWidget(hostLabel);
    connectionLayout->addWidget(m_ipaddrEdit);
    connectionLayout->addWidget(m_connectButton);
    connectionLayout->addWidget(m_disconnectButton);

    layout->addLayout(connectionLayout);

    m_connectionStateEdit = new QLineEdit(central);
    m_connectionStateEdit->setReadOnly(true);
    m_connectionStateEdit->setFocusPolicy(Qt::NoFocus);
    m_connectionStateEdit->setText("Not connected");

#if 0
    // QLabel can be made selectable, but QLineEdit(read-only) was chosen
    // because it provides a more obvious copyable text field.
    idLabel = new QLabel("Not connected", central);
    idLabel->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                     Qt::TextSelectableByKeyboard);
#endif

    m_ch1Widget = new ChannelWidget(m_controller, 1, central);
    m_ch2Widget = new ChannelWidget(m_controller, 2, central);

    m_refreshButton = new QPushButton("Refresh", central);
    m_refreshButton->setObjectName("refreshButton");
    m_refreshButton->setEnabled(false);
    m_refreshButton->setToolTip(
        "Read the current settings from the function generator.\n"
        "Any unsent changes are discarded.");

    layout->addWidget(m_connectionStateEdit);

    auto *channelLayout = new QHBoxLayout;

    channelLayout->addWidget(m_ch1Widget);
    channelLayout->addWidget(m_ch2Widget);

    layout->addLayout(channelLayout);

    layout->addWidget(m_refreshButton);

    auto *frame = new QFrame(this);

    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Plain);
    frame->setLineWidth(2);

    auto *frameLayout = new QVBoxLayout(frame);
    frameLayout->setContentsMargins(4, 4, 4, 4);
    frameLayout->addWidget(central);

    setCentralWidget(frame);

#if 0
    setStyleSheet(R"(
        QMainWindow {
            background: #dedfdd;
        }

        QMenuBar {
            background: #dedfdd;
            border-bottom: 1px solid #aeb0ad;
        }

        QStatusBar {
            background: #dedfdd;
            border-top: 1px solid #aeb0ad;
        }
    )");

    central->setStyleSheet("background: #f7f7f4;");

    // central->setStyleSheet("background-color: #f5f5f2;");
#endif

    setInstrument(InstrumentType::Simulator);

    connect(m_immediateCheck,
            &QCheckBox::toggled,
            this,
            [this](bool checked)
            {
                m_immediateMode = checked;

                if (checked)
                {
                    m_sendButton->hide();
                    sdgDebug() << "enter Immediate mode";
                    setSettingsDirty(false);     // legacy global flag
                }
                else
                {
                    m_sendButton->show();
                    sdgDebug() << "enter Send mode";
                    m_sendButton->setEnabled(m_settingsDirty);
                }
            });

    // Define a Lambda function to ease the tedium of doing a connect for
    // each channel
    auto connectChannelWidgets =
        [this](auto signal, auto setter)
        {
            connect(m_ch1Widget, signal, this, setter);
            connect(m_ch2Widget, signal, this, setter);
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
        &ChannelWidget::periodChanged,
        [this](int channel, double value)
        {
            setPeriod(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::amplitudeChanged,
        [this](int channel,
               double value,
               const QString &representation)
        {
            setAmplitude(channel, value, representation);
        });

    connectChannelWidgets(
        &ChannelWidget::offsetChanged,
        [this](int channel,
               double value,
               const QString &representation)
        {
            setOffset(channel, value, representation);
        });

    connectChannelWidgets(
        &ChannelWidget::phaseChanged,
        [this](int channel, double value)
        {
            setPhase(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::dutyChanged,
        [this](int channel, double value)
        {
            setDuty(channel, value);
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
        &ChannelWidget::dcOffsetChanged,
        [this](int channel, double value)
        {
            setDcOffset(channel, value);
        });

    connectChannelWidgets(
        &ChannelWidget::dcPrecisionHighChanged,
        [this](int channel, bool enabled)
        {
            setDcPrecisionHigh(channel, enabled);
        });


    connectChannelWidgets(
        &ChannelWidget::outputChanged,
        [this](int channel, bool enabled)
        {
            setOutput(channel, enabled);
        });

    connect(m_connectButton,
            &QPushButton::clicked,
            this,
            &MainWindow::connectClicked);

    connect(m_disconnectButton,
            &QPushButton::clicked,
            this,
            &MainWindow::disconnectClicked);

    connect(m_refreshButton,
            &QPushButton::clicked,
            this,
            &MainWindow::refreshClicked);

    connect(m_sendButton,
            &QPushButton::clicked,
            this,
            &MainWindow::sendClicked);

    createMenuBar();

    resize(800, 350);
}

// Offloaded work from the ctor
void MainWindow::createMenuBar()
{
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

    QMenu *editMenu = menuBar()->addMenu("&Edit");

    m_resetAction = editMenu->addAction("Reset and set defaults");
    m_adaptiveDecimalStepAction = editMenu->addAction("Spin second MS digit");
    m_adaptiveDecimalStepAction->setCheckable(true);
    m_adaptiveDecimalStepAction->setChecked(false);
    m_adaptiveDecimalStepAction->setEnabled(false);
    // m_adaptiveDecimalStepAction->setStatusTip(<str>); // but no Status bar
    m_adaptiveDecimalStepAction->setToolTip(
        "In Adaptive decimal step mode the\n"
        "second Most Significant Digit spins");

    connect(m_resetAction,
            &QAction::triggered,
            this,
            [this]()
            {
                sdgDebug() << "Reset requested";
                if (m_generator && m_generator->reset())
                    refreshClicked();
            });

    connect(m_adaptiveDecimalStepAction,
            &QAction::toggled,
            this,
            [this](bool checked)
            {
                m_controller->secondMSD_inform(checked);
            });

    auto *viewMenu = menuBar()->addMenu("&View");

    m_showChannel1Action = viewMenu->addAction("Show Channel 1");
    m_showChannel1Action->setCheckable(true);
    // Assume at startup, both Channels are visible
    m_showChannel1Action->setChecked(true);
    // At this point the widgets have not yet been shown, so isVisible()
    // cannot be used to initialize these actions.
    // Needs a connect(viewMenu, QMenu::hovered, ...) for toolTip to work
    m_showChannel1Action->setToolTip(
        "Remove Channel 1 from this UI leaving more\n"
        "screen 'real estate' for Channel 2");

    m_showChannel2Action = viewMenu->addAction("Show Channel 2");
    m_showChannel2Action->setCheckable(true);
    m_showChannel2Action->setChecked(true);
    m_showChannel2Action->setToolTip(
        "Remove Channel 2 from this UI leaving more\n"
        "screen 'real estate' for Channel 1");

    m_frontPanelAction = viewMenu->addAction("Show front panel");
    m_frontPanelAction->setCheckable(true);
    m_frontPanelAction->setEnabled(false);
    m_frontPanelAction->setToolTip(
        "Fetch SDG2000X's screen as a bmp and render it");

    connect(m_ch1Widget,
            &ChannelWidget::hideRequested,
            this,
            [this](int)
            {
                m_ch1Widget->hide();
                m_showChannel1Action->setChecked(false);
            });

    connect(m_ch2Widget,
            &ChannelWidget::hideRequested,
            this,
            [this](int)
            {
                m_ch2Widget->hide();
                m_showChannel2Action->setChecked(false);
            });

    connect(m_showChannel1Action,
            &QAction::toggled,
            this,
            [this](bool checked)
            {
                m_ch1Widget->setVisible(checked);
            });

    connect(m_showChannel2Action,
            &QAction::toggled,
            this,
            [this](bool checked)
            {
                m_ch2Widget->setVisible(checked);
            });

    connect(m_frontPanelAction,
            &QAction::toggled,
            this,
            [this](bool checked)
            {
                if (checked)
                {
                    if (!m_frontPanelWindow)
                        createFrontPanelWindow();

                    m_frontPanelWindow->show();
                    m_frontPanelWindow->raise();
                    m_frontPanelWindow->activateWindow();
                    m_frontPanelWindow->updateScreen();
                }
                else
                {
                    if (m_frontPanelWindow)
                        m_frontPanelWindow->hide();
                }
            });

    // This connect() is to display the 'Show Channel 1/2' toolTips
    connect(viewMenu,
            &QMenu::hovered,
            this,
            [](QAction *action)
            {
                if (action && !action->toolTip().isEmpty())
                {
                    QToolTip::showText(
                        QCursor::pos(),
                        action->toolTip());
                }
                else
                {
                    QToolTip::hideText();
                }
            });

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
}

MainWindow::~MainWindow()
{
    sdgDebug() << Q_FUNC_INFO << "starting";
    delete m_frontPanelWindow;
    delete m_generator;
}

ChannelState *MainWindow::pendingChannelState(int channel)
{
    // channel argument should be 1 or 2, if otherwise use channel 2
    return &m_pendingState[(channel == 1) ? 0 : 1].m_channelState;
}

ChannelDirtyState *MainWindow::pendingChannelDirtyState(int channel)
{
    return &m_pendingState[(channel == 1) ? 0 : 1].m_channelDirtyState;
}

void MainWindow::setInstrument(InstrumentType type)
{
    if (m_generator)
    {
        if (m_generator->isConnected())
            m_generator->disconnect();

        delete m_generator;
    }

    m_generator = createInstrument(type, this);

    connect(m_generator,
            &Instrument::disconnected,
            this,
            &MainWindow::connectionLost);

    m_connectButton->setEnabled(true);
    m_disconnectButton->setEnabled(false);
    m_refreshButton->setEnabled(false);

    m_ch1Widget->setControlsEnabled(false);
    m_ch2Widget->setControlsEnabled(false);

    m_pendingState = {};
    setSettingsDirty(false);     // legacy global flag

    m_connectionStateEdit->setText("Not connected");
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
    auto amp = ch.amplitude;
    const QString missing = "---";

    QString common =
    QString("CH%1: %2  %3 Hz  %4,%5,%6 Vpp,Vrms,dBm  Offset %7 V  Phase %8°")
        .arg(my_chan)
        .arg(ch.waveform)
        .arg(ch.frequency, 0, 'f', 1)
        .arg(amp.v_ppValid
                 ? QString::number(amp.getVpp(), 'f', 3) : missing)
        .arg(amp.v_rmsValid
                 ? QString::number(amp.getVrms(), 'f', 3) : missing)
        .arg(amp.dBmValid
                 ? QString::number(amp.get_dBm(), 'f', 1) : missing)
        .arg(ch.offset, 0, 'f', 2)
        .arg(ch.phase, 0, 'f', 1);

    if (ch.waveform == "RAMP")
        cwid.setUiStatus( QString("%1  Sym %2  Output %3")
            .arg(common)
            .arg(ch.rampSymmetry)
            .arg(ch.output.enabled ? "ON" : "OFF"));
    else if (ch.waveform == "DC")
        cwid.setUiStatus( QString("%1  DC_OFST %2  Output %3")
            .arg(common)
            .arg(ch.dcOffset)
            .arg(ch.output.enabled ? "ON" : "OFF"));
    else if (ch.waveform == "PULSE")
        cwid.setUiStatus( QString("%1  Width %2  Rise %3  Fall %4  Output %5")
            .arg(common)
            .arg(ch.pulseWidth)
            .arg(ch.pulseRise)
            .arg(ch.pulseFall)
            .arg(ch.output.enabled ? "ON" : "OFF"));
    else if (ch.waveform == "SQUARE")
        cwid.setUiStatus( QString("%1  Duty %2 Output %3")
            .arg(common)
            .arg(ch.duty)
            .arg(ch.output.enabled ? "ON" : "OFF"));
    else if (my_chan == 1 && ch.waveform == "SINE")
        cwid.setUiStatus( QString("%1  Output %2  %3")
            .arg(common)
            .arg(ch.output.enabled ? "ON" : "OFF")
            .arg(BUILD_TIME));
    else
        cwid.setUiStatus( QString("%1  Output %2")
            .arg(common)
            .arg(ch.output.enabled ? "ON" : "OFF"));
}
#endif

// This function is called when the Refresh button is pressed
static void setChannelFields(int my_chan, ChannelWidget & cwid,
                             const ChannelState & ch)
{
    cwid.setUiWaveform(ch.waveform);
    cwid.setUiFrequency(ch.frequency);
    cwid.setUiAmplitude(ch.amplitude);
    cwid.setUiOffset(ch.offset);
    cwid.setUiPhase(ch.phase);
    cwid.setUiDuty(ch.duty);
    cwid.setUiRampSymmetry(ch.rampSymmetry);
    cwid.setUiPulseWidth(ch.pulseWidth);
    cwid.setUiPulseRise(ch.pulseRise);
    cwid.setUiPulseFall(ch.pulseFall);
    cwid.setUiNoiseBandset(ch.noiseBandset);
    cwid.setUiNoiseStdev(ch.noiseStdev);
    cwid.setUiNoiseMean(ch.noiseMean);
    cwid.setUiNoiseBandwidth(ch.noiseBandwidth);
    cwid.setUiDcOffset(ch.dcOffset);

// DC Precision is present in the SDG UI/firmware but is not currently
// documented by Siglent and is not returned by BSWV?. Leave the field
// in the application state/UI so it can be wired up if a future
// firmware/SCPI implementation exposes it.
    cwid.setUiDcPrecisionHigh(ch.dcPrecisionHigh);

    cwid.setUiOutput(ch.output);

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

    if (!m_generator->isConnected())
    {
        m_connectionStateEdit->setText("Not connected");
        return;
    }

    const bool wasDirty = areSettingsDirty();

    m_connectionStateEdit->setText(
               displayIdentification(m_generator->identification()));

    auto ch1 = m_generator->getChannelState(1);

    if (haveIOError(ch1))
    {
        sdgDebug() << Q_FUNC_INFO << ":ch1: " << ioErrorMsg;
        return;
    }
    // Need to carry over userRepresentation from existing UI state
    ch1.amplitude.userRepresentation =
        pendingChannelState(1)->amplitude.userRepresentation;

    setChannelFields(1, *m_ch1Widget, ch1);

    auto ch2 = m_generator->getChannelState(2);

    if (haveIOError(ch2))
    {
        sdgDebug() << Q_FUNC_INFO << ":ch2: " << ioErrorMsg;
        return;
    }
    // Need to carry over userRepresentation from existing UI state
    ch2.amplitude.userRepresentation =
        pendingChannelState(2)->amplitude.userRepresentation;

    setChannelFields(2, *m_ch2Widget, ch2);

    *pendingChannelState(1) = ch1;
    *pendingChannelState(2) = ch2;

    if (wasDirty)
    {
        sdgDebug() << Q_FUNC_INFO << ">>> Refresh overwrote user data";
    }
    setSettingsDirty(false);     // legacy global flag
}

// Connect replaces the existing pending settings with the state read from
// the SDG (or simulator). In effect, Connect performs a Refresh.
void MainWindow::connectClicked()
{
    sdgDebug() << Q_FUNC_INFO ;
    if (!m_generator->connectTo(m_ipaddrEdit->text()))
    {
        m_connectionStateEdit->setText("Connection failed: " +
                                       m_generator->getConnectionError());

        m_ch1Widget->setControlsEnabled(false);
        m_ch2Widget->setControlsEnabled(false);

        return;
    }
    m_ch1Widget->setControlsEnabled(true);
    m_ch2Widget->setControlsEnabled(true);
    m_refreshButton->setEnabled(true);

    QSettings settings("sdg-control", "sdg-control");
    settings.setValue("host", m_ipaddrEdit->text());

    m_connectionStateEdit->setText(m_generator->identification());

    m_connectButton->setEnabled(false);
    m_disconnectButton->setEnabled(true);

    m_adaptiveDecimalStepAction->setEnabled(true);

    refreshClicked();
    m_instrumentCombo->setEnabled(false);
    updateFrontPanelAction();
}

void MainWindow::disconnectClicked()
{
    sdgDebug() << Q_FUNC_INFO ;
    m_generator->disconnect();

    m_connectButton->setEnabled(true);
    m_disconnectButton->setEnabled(false);
    m_refreshButton->setEnabled(false);

    m_adaptiveDecimalStepAction->setEnabled(false);

    m_ch1Widget->setControlsEnabled(false);
    m_ch2Widget->setControlsEnabled(false);

    // m_connectionStateEdit->clear();
    m_connectionStateEdit->setText("Disconnected");
    m_instrumentCombo->setEnabled(true);
    updateFrontPanelAction();
}

void MainWindow::sendClicked()
{
    bool ok = true;

    sdgDebug() << Q_FUNC_INFO;

    if (!m_generator->isConnected())
        return;

    for (int channel = 1; channel <= 2; channel++)
    {
        const auto state = pendingChannelState(channel);
        ChannelWidget *widget = (channel == 1) ? m_ch1Widget : m_ch2Widget;

sdgDebug() << Q_FUNC_INFO << "pending_dirty:\n"
           << pendingChannelDirtyState(channel)->debugStr();
        bool local_ok = m_generator->applyChannelState(channel, *state);
        if (local_ok)
            widget->clearAllDirty();
        ok &= local_ok;
    }

    if (ok)
        setSettingsDirty(false);        // global dirty flag, to be replaced
    else
        sdgDebug() << Q_FUNC_INFO
                   << "setting of at least one field failed";
    // sdgDebug() << "focusWidget:" << focusWidget();
}

void MainWindow::connectionLost()
{
    sdgDebug() << Q_FUNC_INFO;

    m_connectionStateEdit->setText("Connection lost");

    m_disconnectButton->setEnabled(false);
    m_connectButton->setEnabled(true);
    m_refreshButton->setEnabled(false);

    m_ch1Widget->setControlsEnabled(false);
    m_ch2Widget->setControlsEnabled(false);

    m_instrumentCombo->setEnabled(true);

    updateFrontPanelAction();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    sdgDebug() << Q_FUNC_INFO << "starting";

    if (m_generator && m_generator->isConnected())
    {
        m_generator->disconnect();
    }
    if (m_frontPanelWindow)
    {
        // __PRETTY_FUNCTION__ is a GCC extension, best not to use
        // sdgDebug() << __PRETTY_FUNCTION__ << "closing frontPanelWindow";
        sdgDebug() << "closing frontPanelWindow";
        m_frontPanelWindow->close();
    }
    event->accept();
    sdgDebug() << Q_FUNC_INFO << "finishing";
}

// This is a temporary solution. Want to move to something like
//    void clearDirty(int channel, String objectName) where objectName
// belongs to a sub-class of QuantityEdit.
void MainWindow::clearDirty(int channel)
{
    ChannelWidget *widget = (channel == 1) ? m_ch1Widget : m_ch2Widget;

    widget->clearAllDirty();
}

void MainWindow::setWaveform(int channel, const QString & waveform)
{
    auto pendingState = pendingChannelState(channel);
    auto dirtyState = pendingChannelDirtyState(channel);

    pendingState->waveform = waveform;
    dirtyState->m_waveform = true;
    if (m_immediateMode)
    {
        // applyChannelState() needs to pass dirtyState as well
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);        // temporary hack, need to revisit
    }
    else
        setSettingsDirty(true);     // global dirty flag, to be replaced
}

void MainWindow::setFrequency(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);
    auto dirtyState = pendingChannelDirtyState(channel);

    pendingState->frequency = value;
    dirtyState->m_frequency = true;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

// Note that the Period field exists on the UI side (calling this method
// when changes to it are completed) but on the instrument side it is
// represented by its reciprocal placed in the Frequency holder.
void MainWindow::setPeriod(int channel, double value)
{
    if (value > 0.0)
        setFrequency(channel, 1.0 / value);
    else
        sdgDebug() << Q_FUNC_INFO << "<< WILD period=" << value
                   << " chan=" << channel << " >>";
}

void MainWindow::setAmplitude(int channel, double value,
                              const QString &representation)
{
    auto pendingState = pendingChannelState(channel);
    AmplitudeState & ampState = pendingState->amplitude;
    auto dirtyState = pendingChannelDirtyState(channel);

    sdgDebug() << Q_FUNC_INFO << "value =" << value
               << "representation =" << representation;

    ampState.v_ppValid = false;
    ampState.v_rmsValid = false;
    ampState.dBmValid = false;

    if (representation == "Vpp")
    {
        ampState.setAmplitudeVpp(value);
        ampState.v_ppValid = true;
    }
    else if (representation == "mVpp")
    {
        ampState.setAmplitudeVpp(value * 0.001);
        ampState.v_ppValid = true;
    }
    else if (representation == "Vrms")
    {
        ampState.setAmplitudeVrms(value);
        ampState.v_rmsValid = true;
    }
    else if (representation == "mVrms")
    {
        ampState.setAmplitudeVrms(value * 0.001);
        ampState.v_rmsValid = true;
    }
    else if (representation == "dBm")
    {
        ampState.setAmplitude_dBm(value);
        ampState.dBmValid = true;
    }
    else
    {
        sdgDebug() << objectName() << ">>>> BAD representation:"
                   << representation;
        return;
    }
    ampState.userRepresentation = representation;

    dirtyState->m_amplitude = true;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        ChannelWidget *widget = (channel == 1) ? m_ch1Widget : m_ch2Widget;

        if (widget) {
            QuantityEdit *qe = widget->amplitudeEdit();

            if (qe) {
                qe->clearDirty();
                return;
            }
        }
        // fall-through: clear all dirty flags in a channel
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setOffset(int channel, double value,
                           const QString &representation)
{
    auto pendingState = pendingChannelState(channel);
    auto dirtyState = pendingChannelDirtyState(channel);
    double volts = value;

    // Normalize to the unit of Volts
    if (representation == "mVdc")
        volts *= 0.001;

    pendingState->offset = volts;

    sdgDebug()
        << "setOffset:"
        << "value =" << value
        << "representation =" << representation
        << "volts =" << volts;

    dirtyState->m_offset = true;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setPhase(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);
    auto dirtyState = pendingChannelDirtyState(channel);

    pendingState->phase = value;
    dirtyState->m_phase = true;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setDuty(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);
    auto dirtyState = pendingChannelDirtyState(channel);

    pendingState->duty = value;
    dirtyState->m_duty = true;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setRampSymmetry(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->rampSymmetry = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setPulseWidth(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->pulseWidth = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setPulseRise(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->pulseRise = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setPulseFall(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->pulseFall = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setNoiseBandset(int channel, bool enabled)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->noiseBandset = enabled;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setNoiseStdev(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->noiseStdev = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setNoiseMean(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->noiseMean = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setNoiseBandwidth(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->noiseBandwidth = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setDcOffset(int channel, double value)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->dcOffset = value;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setDcPrecisionHigh(int channel, bool enabled)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->dcPrecisionHigh = enabled;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setOutput(int channel, bool enabled)
{
    auto pendingState = pendingChannelState(channel);

    pendingState->output.enabled = enabled;
    if (m_immediateMode)
    {
        m_generator->applyChannelState(channel, *pendingState);
        clearDirty(channel);
    }
    else
        setSettingsDirty(true);
}

void MainWindow::setSettingsDirty(bool value)
{
    m_settingsDirty = value;    // global dirty flag to be replaced

    if (!m_immediateMode)
        m_sendButton->setEnabled(m_settingsDirty);
}

bool MainWindow::areSettingsDirty() const
{
    return m_settingsDirty;
}

void MainWindow::loadSettings()
{
    sdgDebug() << Q_FUNC_INFO;

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

    *pendingChannelState(1) = loadedState[0];
    pendingChannelDirtyState(1)->setAll();
    *pendingChannelState(2) = loadedState[1];
    pendingChannelDirtyState(2)->setAll();
    // m_pendingState = loadedState;

    updateWidgetsFromState();

    setSettingsDirty(true);
}

void MainWindow::saveSettings()
{
    sdgDebug() << Q_FUNC_INFO;

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

    std::array<ChannelState, 2> stateToSave;

    stateToSave[0] = *pendingChannelState(1);
    stateToSave[1] = *pendingChannelState(2);

    SettingsIO::save(fileName, stateToSave);
}

void MainWindow::updateWidgetsFromState()
{
    updateChannelWidget(1, *pendingChannelState(1));
    updateChannelWidget(2, *pendingChannelState(2));
}

// This method is only invoked during Load Settings (from JSON) file
void MainWindow::updateChannelWidget(int channel, const ChannelState &state)
{
    ChannelWidget *widget = (channel == 1) ? m_ch1Widget : m_ch2Widget;

    // accept the default second argument od each set*State() method, so
    // that is makeDirty = true.
    // This could be made more efficient by checking the prior value of
    // each field and onlt setting dirty if the new value is different.
    widget->setUiWaveform(state.waveform);
    widget->setUiFrequency(state.frequency);
    widget->setUiAmplitude(state.amplitude);
    widget->setUiOffset(state.offset);
    widget->setUiPhase(state.phase);
    widget->setUiDuty(state.duty);
    widget->setUiRampSymmetry(state.rampSymmetry);
    widget->setUiPulseWidth(state.pulseWidth);
    widget->setUiPulseRise(state.pulseRise);
    widget->setUiPulseFall(state.pulseFall);
    widget->setUiNoiseBandset(state.noiseBandset);
    widget->setUiNoiseStdev(state.noiseStdev);
    widget->setUiNoiseMean(state.noiseMean);
    widget->setUiNoiseBandwidth(state.noiseBandwidth);
    widget->setUiDcOffset(state.dcOffset);
    widget->setUiDcPrecisionHigh(state.dcPrecisionHigh);

    widget->setUiOutput(state.output);
}

void MainWindow::createFrontPanelWindow()
{
    if (m_frontPanelWindow)
        return;

    m_frontPanelWindow = new FrontPanelWindow(m_generator);

    connect(m_frontPanelWindow,
            &FrontPanelWindow::windowClosed,
            this,
            [this]()
            {
                m_frontPanelAction->setChecked(false);
                updateFrontPanelAction();
            });

    connect(m_frontPanelWindow,
            &QObject::destroyed,
            this,
            [this]()
            {
                m_frontPanelWindow = nullptr;
            });
}

void MainWindow::updateFrontPanelAction()
{
    const bool connected =
        m_generator &&
        m_generator->hasFrontPanel() &&
        m_generator->isConnected();

    const bool canHide =
        m_frontPanelWindow &&
        m_frontPanelAction->isChecked();

    m_frontPanelAction->setEnabled(connected || canHide);

    if (m_frontPanelWindow)
        m_frontPanelWindow->setInstrumentConnected(connected);
}
