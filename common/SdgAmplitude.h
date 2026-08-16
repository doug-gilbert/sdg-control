
#pragma once

#include <optional>


class SdgAmplitude
{
public:
    enum class Representation
    {
        Vpp,
        Vrms,
        dBm
    };

    struct InstrumentValues
    {
        std::optional<double> vpp;
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
