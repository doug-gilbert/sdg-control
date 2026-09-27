/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QWidget>

class QLabel;
class QPushButton;
class QCloseEvent;

class Instrument;


/// A instance of this class renders a BMP image of the SDG2000X front
/// panel screen in the ctor and when the Update button is  pressed.
class FrontPanelWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FrontPanelWindow(Instrument *a_instrument,
                              QWidget *parent = nullptr);

    ~FrontPanelWindow();

    void setInstrumentConnected(bool connected);

// public slots:
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
