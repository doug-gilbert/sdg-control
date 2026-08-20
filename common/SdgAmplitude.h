
#pragma once

#include <optional>
#include <QDebug>


class SdgAmplitude
{
public:
    enum class Representation
    {
        Vpp,
        mVpp,
        Vrms,
        mVrms,
        dBm
    };

    struct InstrumentValues
    {
        std::optional<double> vpp = 1.0;   // don't want an Amplitude of 0 V
        std::optional<double> vrms;
        std::optional<double> dBm;
    };

    const InstrumentValues &instrumentValues() const
    {
        return m_instrumentValues;
    }

    void setInstrumentVpp(double value)
    {
        m_instrumentValues.vpp = value;
    }

    void setInstrumentVrms(double value)
    {
        m_instrumentValues.vrms = value;
    }

    void setInstrumentdBm(double value)
    {
        m_instrumentValues.dBm = value;
    }

    Representation userRepresentation() const
    {
        return m_userRepresentation;
    }

    double userValue() const
    {
        return m_userValue;
    }

    void setUserValue(double value, Representation representation)
    {
        m_userValue = value;
        m_userRepresentation = representation;
    }

private:
    InstrumentValues m_instrumentValues;

    double m_userValue = 1.0;
    Representation m_userRepresentation = Representation::Vpp;
};

inline QDebug operator<<(QDebug debug, const SdgAmplitude &amplitude)
{
    const auto &iv = amplitude.instrumentValues();

    debug.noquote()
        << "SdgAmplitude{"
        << "userValue=" << amplitude.userValue()
        << ", userRepresentation=";

    switch (amplitude.userRepresentation())
    {
    case SdgAmplitude::Representation::Vpp:
        debug << "Vpp";
        break;

    case SdgAmplitude::Representation::Vrms:
        debug << "Vrms";
        break;

    case SdgAmplitude::Representation::dBm:
        debug << "dBm";
        break;

    case SdgAmplitude::Representation::mVpp:
        debug << "mVpp";
        break;

    case SdgAmplitude::Representation::mVrms:
        debug << "mVrms";
        break;
    }

    debug << ", instrumentVpp="
          << (iv.vpp ? QString::number(*iv.vpp) : "<none>")
          << ", instrumentVrms="
          << (iv.vrms ? QString::number(*iv.vrms) : "<none>")
          << ", instrumentdBm="
          << (iv.dBm ? QString::number(*iv.dBm) : "<none>")
          << "}";

    return debug;
}
