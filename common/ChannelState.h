#pragma once

#include <QString>

struct ChannelState
{
    QString waveform = "SINE";

    double frequency = 1'000.0;
    double amplitude = 1.0;
    double offset = 0.0;
    double phase = 0.0;

    double rampSymmetry = 50.0;

    double pulseWidth = 0.000'200;
    double pulseRise  = 0.000'000'008'4;
    double pulseFall  = 0.000'000'008'4;

    bool output = false;
};
