
#pragma once

#include <QWidget>

#include "SettingsIO.h"

class QPushButton;
class QScrollArea;
class QComboBox;
class QGroupBox;
class QFormLayout;

class AppController;


class GeneralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralWidget(AppController *controller,
                           QWidget *parent = nullptr);

signals:
    void clockSourceChanged(ClockSource source);

    void hideRequested();

private:
    AppController *m_controller = nullptr;

    QPushButton *m_closeButton = nullptr;

    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContents = nullptr;

    QGroupBox *m_groupBox;
    QFormLayout *m_formLayout;

    QComboBox *m_clockSourceCombo = nullptr;
};
