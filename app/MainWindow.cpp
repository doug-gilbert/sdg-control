#include "MainWindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SDG Control");

    auto *label = new QLabel("SDG Control Application", this);

    setCentralWidget(label);

    resize(400, 200);
}
