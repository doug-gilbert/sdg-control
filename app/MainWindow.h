#pragma once

#include <QMainWindow>

#include <array>

#include "InstrumentFactory.h"

class QLineEdit;
class QPushButton;
class QCloseEvent;
class QCheckBox;
class QComboBox;
class QAction;

class ChannelWidget;
class FrontPanelWindow;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool myDebugFocus, QWidget *parent = nullptr);

    ~MainWindow();

    bool isAmplitudeWidget(QWidget *widget) const;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void refreshClicked();
    void connectClicked();
    void disconnectClicked();
    void connectionLost();
    void sendClicked();
    void loadSettings();
    void saveSettings();

private:
    bool debugFocus = false;

    bool immediateMode = true;
    bool dirty = false;

    FrontPanelWindow *frontPanelWindow = nullptr;
    QAction *frontPanelAction = nullptr;

    std::array<ChannelState, 2> pendingState;

    QCheckBox *immediateCheck;
    QPushButton *sendButton;

    QLineEdit *ipEdit;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *refreshButton;
    QComboBox *instrumentCombo;

    QLineEdit *idEdit;

    QAction *resetAction;

    QAction *showChannel1Action;
    QAction *showChannel2Action;

    ChannelWidget *ch1Widget;
    ChannelWidget *ch2Widget;

    Instrument *generator = nullptr;

    void createFrontPanelWindow();
    void updateFrontPanelAction();

    void setInstrument(InstrumentType type);
    void setDirty(bool value);

    void updateWidgetsFromState();
    void updateChannelWidget(int channel, const ChannelState &state);

    void setWaveform(int channel, const QString & waveform);
    void setFrequency(int channel, double value);
    void setAmplitude(int channel, double value);
    void setAmplitudeRepresentation(int channel,
            SdgAmplitude::Representation representation);
    void setOffset(int channel, double value);
    void setPhase(int channel, double value);
    void setDuty(int channel, double value);
    void setRampSymmetry(int channel, double value);
    void setPulseWidth(int channel, double value);
    void setPulseRise(int channel, double value);
    void setPulseFall(int channel, double value);
    void setNoiseBandset(int channel, bool enabled);
    void setNoiseStdev(int channel, double value);
    void setNoiseMean(int channel, double value);
    void setNoiseBandwidth(int channel, double value);
    void setDcOffset(int channel, double value);
    void setDcPrecisionHigh(int channel, bool enabled);

    void setOutput(int channel, bool enabled);

    void createMenuBar();

    QString displayIdentification(const QString &idn) const;
};
