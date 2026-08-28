#pragma once

#include <QString>
#include <QDebug>


enum class OutputLoad
{
    Ohm50,
    HighZ
};

struct OutputState
{
    bool enabled = false;
    OutputLoad load = OutputLoad::Ohm50;
};

inline bool is_mV(const QString & rep)
{
    return rep.startsWith("mV");
}

struct AmplitudeState
{
    void setAmplitudeVpp(double value) { v_pp = value; v_ppValid = true; }
    void setAmplitudeVrms(double value) { v_rms = value; v_rmsValid = true; }
    void setAmplitude_dBm(double value) { dBm = value; dBmValid = true; }

    double getVpp() const { return v_pp; }
    double getVrms() const { return v_rms; }
    double get_dBm() const { return dBm; }

    // Returns a normalised value usually corresponding to
    // userRepresentation. Normalised is the sense that for any voltages the
    // Unit is Volts (never milliVolts).
    double getMainValue() const
    {
        if (userRepresentation.length() > 0)
        {
            if ((userRepresentation == "Vpp" ||
                 userRepresentation == "mVpp") && v_ppValid)
                return v_pp;
            if ((userRepresentation == "Vrms" ||
                 userRepresentation == "mVrms") && v_rmsValid)
                return v_rms;
            if (userRepresentation == "dBm" && dBmValid)
                return dBm;
        }
        if (v_ppValid)     // fall backs, not ideal
            return v_pp;
        else if (v_rmsValid)
            return v_rms;
        else if (dBmValid)
            return dBm;
        return 0.000'01; // things are not good, don't make it worse with 0.0
    }

    QString userRepresentation;   // the Unit specified by user in the UI

    bool v_ppValid = false;
    bool v_rmsValid = false;
    bool dBmValid = false;

protected:

    double v_pp = 1.0;        // 0.0 blows up dBm (and Period)
    double v_rms = 0.353'6;   // correct for a SINE wave, Vpp=1
    double dBm = 3.979'4;     // correct for a SINE wave, Vpp=1, Rload=50
};

struct ChannelState
{
    QString waveform = "SINE";

    double frequency = 1'000.0;
    AmplitudeState amplitude;
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

    OutputState output;
};

inline QDebug operator<<(QDebug debug, const OutputState &output)
{
    debug << "OutputState{"
          << " enabled=" << output.enabled
          << " load="
          << (output.load == OutputLoad::Ohm50 ? "Ohm50" : "HighZ")
          << " }";
    return debug;
}
