
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
#include "FrontPanelWindow.h"
#include "InstrumentFactory.h"

#include "debug.h"

/*
 * Following is a helper list of actions required to add a field to a
 * Waveform. In this example the Waveform is DC and the field is
 * 'DC Offset'. Will add field 'DC Precision' at the same time as it is the
 * only other field for Waveform DC. sdg-control should compile after most
 * of the steps shown below.
 *
 *  1)  add dcOffset field to common/ChannelState.h with a (default) value
 *      that is valid (e.g. a freqeuency of 0.0 Hz is NOT valid)
 *  2)  in app/ChannelWidget.h add: public setter void setDcOffset(double
 *      value); and a private label: QLabel *dcOffsetLabel; and a private:
 *      QDoubleSpinBox *dcOffsetSpin;
 *      In app/ChannelWidget.cpp define setDcOffset():
 *          void ChannelWidget::setDcOffset(double value)
 *          {
 *              dcOffsetSpin->blockSignals(true);
 *              dcOffsetSpin->setValue(value);
 *              dcOffsetSpin->blockSignals(false);
 *          }    // The blockSignals() are needed
 *  3)  in app/ChannelWidget.cpp ctor add:
 *          dcOffsetSpin = new QDoubleSpinBox(groupBox);
 *      follower by initial setters (see code for other Spinboxes)a, then add
 *          dcOffsetLabel = new QLabel("DC Offset:", groupBox);
 *      then expand ChannelWidget::updateControlVisibility()
 *          const bool showDC = (waveform == "DC");
 *          const bool showStandardControls = !showNoise && !showDC;
 *      and at the end of that method:
 *          dcOffsetLabel->setVisible(showDC);
 *          dcOffsetSpin->setVisible(showDC);
 *      and the layout of that field in the ChannelWidget ctor:
 *          formLayout->addRow(dcOffsetLabel, dcOffsetSpin);
 *  4)  build and run, Select Waveform DC and the DC Offset spinbox should
 *      appear. Note there is no wiring yet to/from SDG unit
 *  5)  add void dcOffsetChanged(int channel, double value); under the
 *      signals: section on ChannelWidget.h . Note this is not real C++ but
 *      the 'moc' precompiler with Qt converts it into clean C++. Moc
 *      generates the code to implement dcOffsetChanged().
 *          connect(dcOffsetSpin,
 *                  &QDoubleSpinBox::valueChanged,
 *                  this,
 *                  [this](double value)
 *                  {
 *                      emit dcOffsetChanged(channel, value);
 *                  });
 *      which connects the dcOffset field changing to that signal.
 *  6)  Add void setDcOffset(int channel, double value); to the private
 *      part of MainWindow.h . Then add the corresponding method definition
 *      to MainWindow.cpp (by comparing to the similar methods for other
 *      fields). The add: bool setDcOffset(int channel, double value);
 *      to the public part of SDG2000X.h . Then in SDG2000X.cpp define:
 *          bool SDG2000X::setDcOffset(int channel, double value)
 *      to send the SCPI command to the SDG to set the DC Offset. That
 *      is not defined in the Siglent Programming manual so more research
 *      is needed. Try:
 *        bool SDG2000X::setDcOffset(int channel, double value)
 *        {
 *            return scpi.command(
 *                QString("%1:BSWV OFST,%2")
 *                    .arg(channelPrefix(channel))
 *                    .arg(value, 0, 'g', 12));
 *        }
 *
 *  7)  In MainWindow.cpp there is a helper function named setChannelFields()
 *      In the function add this line: 'cwid.setDcOffset(ch.dcOffset);'
 *  8)  In the ctor of MainWindow.cpp add
 *          connectChannelWidgets(
 *              &ChannelWidget::dcOffsetChanged,
 *              [this](int channel, double value)
 *              {
 *                  setDcOffset(channel, value);
 *              });
 *  9)  parse DC offset from C<chan>:BSWV? response in
 *         ChannelState SDG2000X::getChannelState(int channel)
 *            } else if (key == "OFST") {
 *                value.remove("V");
 *                if (wvtp == "DC")
 *                    state.dcOffset = value.toDouble();
 *                else
 *                    state.offset = value.toDouble();
 *            }
 *  10) add new field to app/SettingsIO.cpp so new field appears in
 *      the JSON output.
 *  11) add new field to MainWindow::updateChannelWidget()
 *
 */

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

    QMenu *editMenu = menuBar()->addMenu("&Edit");

    showChannel1Action = editMenu->addAction("Show Channel 1");

    showChannel2Action = editMenu->addAction("Show Channel 2");

    connect(ch1Widget,
            &ChannelWidget::hideRequested,
            this,
            [this](int)
            {
                ch1Widget->hide();
                updateEditMenu();
            });

    connect(ch2Widget,
            &ChannelWidget::hideRequested,
            this,
            [this](int)
            {
                ch2Widget->hide();
                updateEditMenu();
            });

    connect(showChannel1Action,
            &QAction::triggered,
            this,
            [this]()
            {
                ch1Widget->show();
                updateEditMenu();
            });

    connect(showChannel2Action,
            &QAction::triggered,
            this,
            [this]()
            {
                ch2Widget->show();
                updateEditMenu();
            });

    auto *viewMenu = menuBar()->addMenu("&View");

    frontPanelAction = viewMenu->addAction("Show front panel");
    frontPanelAction->setCheckable(true);
    frontPanelAction->setEnabled(false);

    connect(frontPanelAction,
            &QAction::toggled,
            this,
            [this](bool checked)
            {
                if (checked)
                {
                    if (!frontPanelWindow)
                        createFrontPanelWindow();

                    frontPanelWindow->show();
                    frontPanelWindow->raise();
                    frontPanelWindow->activateWindow();
                    frontPanelWindow->updateScreen();
                }
                else
                {
                    if (frontPanelWindow)
                        frontPanelWindow->hide();
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

    updateEditMenu();

    resize(800, 350);
}

MainWindow::~MainWindow()
{
    sdgDebug() << "start of MainWindow destructor";
    delete frontPanelWindow;
    delete generator;
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

    ch1Widget->setControlsEnabled(false);
    ch2Widget->setControlsEnabled(false);

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
    auto ampInstru = ch.amplitude.instrumentValues();
    const QString missing = "---";

    QString common =
    QString("CH%1: %2  %3 Hz  %4,%5,%6 Vpp,Vrms,dBm  Offset %7 V  Phase %8°")
        .arg(my_chan)
        .arg(ch.waveform)
        .arg(ch.frequency, 0, 'f', 1)
        .arg(ampInstru.vpp
                 ? QString::number(*ampInstru.vpp, 'f', 3) : missing)
        .arg(ampInstru.vrms
                 ? QString::number(*ampInstru.vrms, 'f', 3) : missing)
        .arg(ampInstru.dBm
                 ? QString::number(*ampInstru.dBm, 'f', 1) : missing)
        .arg(ch.offset, 0, 'f', 2)
        .arg(ch.phase, 0, 'f', 1);

    if (ch.waveform == "RAMP")
        cwid.setStatus( QString("%1  Sym %2  Output %3")
            .arg(common)
            .arg(ch.rampSymmetry)
            .arg(ch.output ? "ON" : "OFF"));
    else if (ch.waveform == "DC")
        cwid.setStatus( QString("%1  DC_OFST %2  Output %3")
            .arg(common)
            .arg(ch.dcOffset)
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
    auto vpp = ch.amplitude.instrumentValues().vpp;

    cwid.setWaveform(ch.waveform);
    cwid.setFrequency(ch.frequency);
    cwid.setAmplitude(vpp.value_or(0.0));
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
    cwid.setDcOffset(ch.dcOffset);

// DC Precision is present in the SDG UI/firmware but is not currently
// documented by Siglent and is not returned by BSWV?. Leave the field
// in the application state/UI so it can be wired up if a future
// firmware/SCPI implementation exposes it.
    cwid.setDcPrecisionHigh(ch.dcPrecisionHigh);

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

// Connect replaces the existing pending settings with the state read from
// the SDG (or simulator). In effect, Connect performs a Refresh.
void MainWindow::connectClicked()
{
    sdgDebug() << __func__ ;
    if (!generator->connectTo(ipEdit->text()))
    {
        idEdit->setText("Connection failed: " +
                        generator->getConnectionError());

        ch1Widget->setControlsEnabled(false);
        ch2Widget->setControlsEnabled(false);

        return;
    }
    ch1Widget->setControlsEnabled(true);
    ch2Widget->setControlsEnabled(true);
    refreshButton->setEnabled(true);

    QSettings settings("sdg-control", "sdg-control");
    settings.setValue("host", ipEdit->text());

    idEdit->setText(generator->identification());

    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);

    refreshClicked();
    instrumentCombo->setEnabled(false);
    updateFrontPanelAction();
}

void MainWindow::disconnectClicked()
{
    sdgDebug() << __func__ ;
    generator->disconnect();

    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    refreshButton->setEnabled(false);

    ch1Widget->setControlsEnabled(false);
    ch2Widget->setControlsEnabled(false);

    // idEdit->clear();
    idEdit->setText("Disconnected");
    instrumentCombo->setEnabled(true);
    updateFrontPanelAction();
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

    ch1Widget->setControlsEnabled(false);
    ch2Widget->setControlsEnabled(false);

    instrumentCombo->setEnabled(true);
    updateFrontPanelAction();
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

// Currently setting Vpp
void MainWindow::setAmplitude(int channel, double value)
{
    pendingState[channel - 1].amplitude.setUserValue(
            value, SdgAmplitude::Representation::Vpp);

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

void MainWindow::setDcOffset(int channel, double value)
{
    pendingState[channel - 1].dcOffset = value;

    if (immediateMode)
        generator->applyChannelState(channel, pendingState[channel - 1]);
    else
        setDirty(true);
}

void MainWindow::setDcPrecisionHigh(int channel, bool enabled)
{
    pendingState[channel - 1].dcPrecisionHigh = enabled;

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

// This method is only invoked during Load Settings (from JSON) file
void MainWindow::updateChannelWidget(int channel, const ChannelState &state)
{
    ChannelWidget *widget = (channel == 1) ? ch1Widget : ch2Widget;

    widget->setWaveform(state.waveform);
    widget->setFrequency(state.frequency);
    widget->setAmplitude(
           state.amplitude.instrumentValues().vpp.value_or(0.0));
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
    widget->setDcOffset(state.dcOffset);
    widget->setDcPrecisionHigh(state.dcPrecisionHigh);

    widget->setOutput(state.output);
}

void MainWindow::updateEditMenu()
{
    showChannel1Action->setEnabled(!ch1Widget->isVisible());
    showChannel2Action->setEnabled(!ch2Widget->isVisible());
}

void MainWindow::createFrontPanelWindow()
{
    if (frontPanelWindow)
        return;

    frontPanelWindow = new FrontPanelWindow(generator);

    connect(frontPanelWindow,
            &QObject::destroyed,
            this,
            [this]()
            {
                frontPanelWindow = nullptr;
            });
}

void MainWindow::updateFrontPanelAction()
{
    const bool available =
        generator &&
        generator->hasFrontPanel() &&
        generator->isConnected();

    frontPanelAction->setEnabled(available);

    if (!available && frontPanelAction->isChecked())
        frontPanelAction->setChecked(false);
}
