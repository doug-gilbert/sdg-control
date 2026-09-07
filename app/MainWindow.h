
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
class AppController;
class FrontPanelWindow;
class CLI_options;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const CLI_options &cli_opts,
                        QWidget *parent = nullptr);

    ~MainWindow();

    bool isAmplitudeWidget(QWidget *widget) const;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:  // 'slots' Qt6 keyword not needed with modern connects()s
    void refreshClicked();
    void connectClicked();
    void disconnectClicked();
    void connectionLost();
    void sendClicked();
    void loadSettings();
    void saveSettings();

private:
    bool m_debugFocus = false;

    bool m_immediateMode = true;
    bool m_dirty = false;

    FrontPanelWindow *m_frontPanelWindow = nullptr;
    QAction *m_frontPanelAction = nullptr;

    // Channel is either 1 or 2. Take care when indexing this array with
    // the channel number (i.e. need to use 'channel - 1' as the index
    // since array indexing is origin 0.
    std::array<CombinedChannelState, 2> m_pendingState;

    QCheckBox *m_immediateCheck;
    QPushButton *m_sendButton;

    QLineEdit *m_ipaddrEdit;
    QPushButton *m_connectButton;
    QPushButton *m_disconnectButton;
    QPushButton *m_refreshButton;
    QComboBox *m_instrumentCombo;

    QLineEdit *m_connectionStateEdit;

    QAction *m_resetAction;
    QAction *m_adaptiveDecimalStepAction;

    QAction *m_showChannel1Action;
    QAction *m_showChannel2Action;

    ChannelWidget *m_ch1Widget;
    ChannelWidget *m_ch2Widget;

    Instrument *m_generator = nullptr;

    AppController *m_controller = nullptr;

    const CLI_options &m_cli_options;

    void createFrontPanelWindow();
    void updateFrontPanelAction();

    void setInstrument(InstrumentType type);
    void setDirty(bool value);
    bool isDirty() const;

    void updateWidgetsFromState();
    void updateChannelWidget(int channel, const ChannelState &state);

    void setWaveform(int channel, const QString & waveform);
    void setFrequency(int channel, double value);
    void setAmplitude(int channel, double value,
                      const QString &representation);
    void setOffset(int channel, double value, const QString &representation);
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

    void clearDirty(int channel);  // temporary, need per QuantityEdit call

    void createMenuBar();

    QString displayIdentification(const QString &idn) const;

    // Access functions for components of m_pendingState. As long as an
    // instance of this class exists, the returned pointer will be valid.
    ChannelState *pendingChannelState(int channel);
    ChannelDirtyState *pendingChannelDirtyState(int channel);
};
