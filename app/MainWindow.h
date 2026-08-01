#pragma once

#include <QMainWindow>
#include <QDoubleSpinBox>

#include "SDG2000X.h"
#include "ChannelWidget.h"

class QLabel;
class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void refreshClicked();

private:
    QLabel *idLabel;

    ChannelWidget *ch1Widget;
    ChannelWidget *ch2Widget;

    SDG2000X generator;
};
