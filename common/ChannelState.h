#pragma once

#include <QString>

#include "SdgAmplitude.h"

struct ChannelState
{
    QString waveform = "SINE";

    double frequency = 1'000.0;
    SdgAmplitude amplitude;
    double offset = 0.0;
    double phase = 0.0;

    double duty = 50.0;       // percentage, 0..100

    double rampSymmetry = 50.0;

    double pulseWidth = 0.000'200;
    double pulseRise  = 0.000'000'008'4;
    double pulseFall  = 0.000'000'008'4;

    bool noiseBandset = false;    // noiseBandwidth active when value true
    double noiseStdev = 0.002;
    double noiseMean = 0.0;
    double noiseBandwidth = 1'000'000.0;

    double dcOffset = 0.0;
    bool dcPrecisionHigh = true;  // false implies low precision

    bool output = false;
};
