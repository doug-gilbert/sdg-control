
#pragma once

#include <QWidget>

class QLabel;
class QPushButton;
class QCloseEvent;

class Instrument;


class FrontPanelWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FrontPanelWindow(Instrument *a_instrument,
                              QWidget *parent = nullptr);

public:
    void setInstrumentConnected(bool connected);

public slots:
    void updateScreen();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Instrument *instrument = nullptr;

    QLabel *screenLabel;
    QPushButton *updateButton;
    QPushButton *toggleButton;
};
