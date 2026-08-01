#pragma once

#include <QMainWindow>

#include "SDG2000X.h"

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
    QLabel *ch1Label;
    QLabel *ch2Label;
    QCheckBox *ch1OutputCheck;

    SDG2000X generator;
};
