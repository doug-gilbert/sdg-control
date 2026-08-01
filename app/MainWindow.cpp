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
    auto *status = new QLabel("Instrument: Not connected", central);
    auto *refresh = new QPushButton("Refresh", central);

    layout->addWidget(title);
    layout->addWidget(status);
    layout->addWidget(refresh);

    setCentralWidget(central);

    resize(400, 200);
}
