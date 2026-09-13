/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QObject>


// An instance of this class is owned by the MainWindow (assumed to be a
// singleton). A pointer to that instance is passed to all spawned widgets
// that need to receive signals from the MainWindow.
class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr)
        : QObject(parent) { }

    // Methods to send signals to spawned widgets
    void secondMSD_inform(bool checked)
    {
        emit(secondMSD_changed(checked));
    }

signals:
    // List of signals emitted by above methods
    void secondMSD_changed(bool checked);

private:
    // Could hold some state here.

};
