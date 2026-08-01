#pragma once

#include <QMainWindow>

#include "SDG2000X.h"

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void refreshClicked();

private:
    QLabel *statusLabel;
    SDG2000X generator;
};
