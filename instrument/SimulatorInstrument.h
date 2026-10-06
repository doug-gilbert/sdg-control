/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Instrument.h"

#include <array>

class SimulatorInstrument : public Instrument
{
public:
    explicit SimulatorInstrument(QObject *parent = nullptr);

    bool connectTo(const QString &host) override;
    void disconnect() override;
    bool isConnected() const override;

    QString identification() override;

    QString getConnectionError() const override;

    ChannelState getChannelState(int channel) override;

    GeneralState getGeneralState() override;

    bool applyChannelState(int channel,
                           const ChannelState &state,
                           const ChannelDirtyState& dirty) override;

    bool applyGeneralState(const GeneralState &state,
                           const GeneralDirtyState& dirty) override;

private:
    std::array<ChannelState, 2> channelState;

    GeneralState generalState;

    bool connected = false;
};
