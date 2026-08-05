#pragma once

#include <QMainWindow>
#include <QSettings>

#include <array>

#include "SDG2000X.h"
#include "ChannelWidget.h"

class QLineEdit;
class QPushButton;
class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void refreshClicked();
    void connectClicked();
    void disconnectClicked();
    void connectionLost();

private:
    bool immediateMode = true;
    bool dirty = false;

    std::array<ChannelState, 2> pendingState;

    void setDirty(bool value);
    void setFrequency(int channel, double value);
    void setAmplitude(int channel, double value);
    void setOffset(int channel, double value);
    void setPhase(int channel, double value);
    void setSymmetry(int channel, double value);

    QLineEdit *ipEdit;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *refreshButton;

    QLineEdit *idEdit;

    ChannelWidget *ch1Widget;
    ChannelWidget *ch2Widget;

    SDG2000X generator;
};
