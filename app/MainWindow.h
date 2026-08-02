#pragma once

#include <QMainWindow>
#include <QSettings>

#include "SDG2000X.h"
#include "ChannelWidget.h"

class QLineEdit;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void refreshClicked();
    void connectClicked();

private:
    QLineEdit *ipEdit;
    QPushButton *connectButton;

    QLineEdit *idEdit;

    ChannelWidget *ch1Widget;
    ChannelWidget *ch2Widget;

    SDG2000X generator;
};
