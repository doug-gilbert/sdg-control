
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

public slots:
    void updateScreen();
#if 0
    void toggleChannel();
#endif

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Instrument *instrument = nullptr;

    QLabel *screenLabel;
    QPushButton *updateButton;
    QPushButton *toggleButton;
};
