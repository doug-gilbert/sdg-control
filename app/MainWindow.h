#pragma once

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>

#include <array>

#include "SettingsIO.h"
#include "Instrument.h"
#include "ChannelWidget.h"

class QLineEdit;
class QPushButton;
class QCloseEvent;
class QCheckBox;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

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
    bool immediateMode = true;
    bool dirty = false;

    std::array<ChannelState, 2> pendingState;

    void setDirty(bool value);

    void updateWidgetsFromState();
    void updateChannelWidget(int channel, const ChannelState &state);

    void setWaveform(int channel, const QString & waveform);
    void setFrequency(int channel, double value);
    void setAmplitude(int channel, double value);
    void setOffset(int channel, double value);
    void setPhase(int channel, double value);
    void setSymmetry(int channel, double value);
    void setOutput(int channel, bool enabled);

    QCheckBox *immediateCheck;
    QPushButton *sendButton;

    QLineEdit *ipEdit;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *refreshButton;

    QLineEdit *idEdit;

    ChannelWidget *ch1Widget;
    ChannelWidget *ch2Widget;

    Instrument *generator;
};
