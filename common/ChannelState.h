#pragma once

#include <QString>
#include <QDebug>

#include <tuple>        // for struct ChannelDirtyState below


// This header (ChannelState.h) currently has no corresponding
// implementation/body (i.e. there is no ChannelState.cpp yet). The
// structs/classes in this header are (in appearance order):
//       ValueRepresentation
//       AmplitudeState
//       ChannelState
//       ChannelDirtyState
//       CombinedChannelState
//
// Generally speaking, the above list is ordered from the lowest-level
// abstraction to the highest level abstraction. Currently the MainWindow
// singleton holds one instance of struct CombinedChannelState per channel.


struct ValueRepresentation
{
    double value;
    QString representation;
};

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
    // userRepresentation. Normalised in the sense that for any voltages the
    // Unit is Volts (never milliVolts). This follows how SCPI communicates
    // between this app and a SDG2000X series device.
    ValueRepresentation valueRepresentation() const
    {
        if (userRepresentation.length() > 0)
        {
            if ((userRepresentation == "Vpp" ||
                 userRepresentation == "mVpp") && v_ppValid)
                return ValueRepresentation {v_pp, "Vpp"};
            if ((userRepresentation == "Vrms" ||
                      userRepresentation == "mVrms") && v_rmsValid)
                return ValueRepresentation {v_rms, "Vrms"};
            if (userRepresentation == "dBm" && dBmValid)
                return ValueRepresentation {dBm, "dBm"};
            return ValueRepresentation {0.000'01, {}};
        }
        if (v_ppValid)     // fall backs, not ideal
            return ValueRepresentation {v_pp, "Vpp"};
        if (v_rmsValid)
            return ValueRepresentation {v_rms, "Vrms"};
        if (dBmValid)
            return ValueRepresentation {dBm, "dBm"};
        // things are not good, don't make it worse with an Amplitude of 0.0
        return ValueRepresentation {0.000'01, {}};
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

// Trying hard to state this list of dirty flags once and only once
#define CHANNEL_DIRTY_FIELDS(X) \
    X(waveform)                 \
    X(frequency)                \
    X(amplitude)                \
    X(offset)                   \
    X(phase)                    \
    X(duty)                     \
    X(rampSymmetry)             \
    X(pulseWidth)               \
    X(pulseRise)                \
    X(pulseFall)                \
    X(noiseBandset)             \
    X(noiseStdev)               \
    X(noiseMean)                \
    X(noiseBandwidth)           \
    X(dcOffset)                 \
    X(dcPrecisionHigh)          \
    X(output)

struct ChannelDirtyState
{
// Generate fields, the first one should be: 'bool m_waveform = fale;'
#define DECLARE_FIELD(name) bool m_##name = false;
    CHANNEL_DIRTY_FIELDS(DECLARE_FIELD)
#undef DECLARE_FIELD

    struct Field
    {
        const char *name;
        bool ChannelDirtyState::*member;
    };

// Make a C array of Field_s the first of which is:
//      { waveform, &ChannelDirtyState::, waveform},
#define MAKE_FIELD(name) { #name, &ChannelDirtyState::m_##name },

    inline static constexpr Field fields[] = {
        CHANNEL_DIRTY_FIELDS(MAKE_FIELD)
    };

#undef MAKE_FIELD

    void setAll()
    {
        for (const auto &field : fields)
            this->*field.member = true;
    }

    void clearAll()
    {
        for (const auto &field : fields)
            this->*field.member = false;
    }

    bool areAnySet() const
    {
        for (const auto &field : fields)
            if (this->*field.member)
                return true;
        return false;
    }

    bool areAnyClear() const
    {
        for (const auto &field : fields)
            if (!(this->*field.member))
                return true;
        return false;
    }

    QString debugStr() const
    {
        QString str;
        int k = 0;

        for (const auto &field : fields)
        {
            str += field.name;
            str += '=';
            str += QString::number(this->*field.member);
            if (++k == 5)
            {
                str += "\n ";
                k = 0;
            }
            else
                str += ' ';
        }
        if (k > 0)
            str += '\n';
        return str;
    }
};

struct CombinedChannelState {
    struct ChannelState      m_channelState;
    struct ChannelDirtyState m_channelDirtyState;
};
