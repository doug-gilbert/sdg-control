#pragma once

#include <QString>

struct ChannelState
{
    QString waveform;

    double frequency = 0;
    double amplitude = 0;
    double offset = 0;
    double phase = 0;

    double symmetry = 0;

    bool output = false;
};
