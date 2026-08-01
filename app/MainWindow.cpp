#include "MainWindow.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SDG Control");

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *title = new QLabel("SDG Control", central);

    statusLabel = new QLabel("Instrument: Not connected", central);

    auto *refresh = new QPushButton("Refresh", central);

    layout->addWidget(title);
    layout->addWidget(statusLabel);
    layout->addWidget(refresh);

    setCentralWidget(central);

    connect(refresh,
            &QPushButton::clicked,
            this,
            &MainWindow::refreshClicked);

    resize(400, 200);
}


void MainWindow::refreshClicked()
{
    statusLabel->setText("Refresh pressed");
}
