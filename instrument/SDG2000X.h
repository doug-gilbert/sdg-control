/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QString>

#include <optional>

#include "ScpiConnection.h"
#include "Instrument.h"


class SDG2000X : public Instrument
{
    Q_OBJECT

public:
    explicit SDG2000X(QObject *parent = nullptr);

    ~SDG2000X() override;

    bool connectTo(const QString& ip) override;

    void disconnect() override;

    bool isConnected() const override;

    QString identification() override;

    QString getConnectionError() const override;

    bool waitForOperationComplete(int timeout_ms) override;

    ChannelState getChannelState(int channel) override;
    GeneralState getGeneralState() override;

#if 0
    ChannelState getSweepState(int channel);
#endif

    bool applyChannelState(int channel,
                           const ChannelState& state,
                           const ChannelDirtyState& dirty) override;

    bool applyGeneralState(const GeneralState& g_state,
                           const GeneralDirtyState& dirty) override;

    // Siglent front-panel screen capture and virtual CH1/CH2 button press
    QByteArray getFrontPanelImage() override;
    bool toggleChannelFocus() override;

    bool hasFrontPanel() const override;

    // reset and set default values on the unit
    bool reset() override;

    // Helper for applyChannelState()
    bool applyVoltageHelper(int channel,
                            const ChannelState& state,
                            const ChannelDirtyState& dirty);

    bool setSdgWaveform(int channel, const QString& waveform);
    bool setSdgFrequency(int channel, double hz);
    bool setSdgAmplitude(int channel, const AmplitudeState &amplitude);
    bool setSdgOffset(int channel, double volts);
    bool setSdgVHigh(int channel, double volts);
    bool setSdgVLow(int channel, double volts);
    bool setSdgPhase(int channel, double degrees);
    bool setSdgRampSymmetry(int channel, double percent);
    bool setSdgPulseWidth(int channel, double seconds);
    bool setSdgPulseRise(int channel, double seconds);
    bool setSdgPulseFall(int channel, double seconds);
    bool setSdgNoiseBandset(int channel, bool enabled);
    bool setSdgNoiseStdev(int channel, double stdev);
    bool setSdgNoiseMean(int channel, double mean);
    bool setSdgNoiseBandwidth(int channel, double freq);
    bool setSdgDcOffset(int channel, double value);
    bool setSdgDcPrecisionHigh(int channel, bool enabled);
    bool setSdgDuty(int channel, double percent);
    bool setInvert(int channel, bool enable);
    bool setSdgOutputLoadPol(int channel, const OutputState &oState);
    bool setSdgExternalOutput(int channel, bool externalOutput);

    bool setSdgOutputBoth(bool enabled);

    bool setSdgClockSource(ClockSource cs);
    bool setSdgOverVoltageProtection(OverVoltageProtection ovp);
    bool setSdgMode(SdgMode mode);

    bool clearErrors();

    std::optional<OutputState> getOutputState(int channel);

    QString getError();

private:

    ScpiConnection m_scpi;

    QString channelPrefix(int channel);
};
