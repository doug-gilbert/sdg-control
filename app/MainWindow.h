#pragma once

#include <QMainWindow>

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
};
