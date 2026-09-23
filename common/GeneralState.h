/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QString>
#include <QDebug>

#include <tuple>        // for struct GeneralDirtyState below


enum class ClockSource {
    Internal,
    External
};

struct GeneralState
{
    ClockSource g_clockSource;

};

// Trying hard to state this list of dirty flags once and only once
#define GENERAL_DIRTY_FIELDS(X) \
    X(clockSource)

struct GeneralDirtyState
{
// Generate fields, the first one should be: 'bool clockSource = false;'
#define DECLARE_FIELD(name) bool m_##name = false;
    GENERAL_DIRTY_FIELDS(DECLARE_FIELD)
#undef DECLARE_FIELD

    struct GField
    {
        const char *name;
        bool GeneralDirtyState::*member;
    };

// Make a C array of Field_s the first of which is:
#define MAKE_FIELD(name) { #name, &GeneralDirtyState::m_##name },

    inline static constexpr GField fields[] = {
        GENERAL_DIRTY_FIELDS(MAKE_FIELD)
    };

#undef MAKE_FIELD
#undef GENERAL_DIRTY_FIELDS

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

struct PendingGeneralState {
    struct GeneralState      m_generalState;
    struct GeneralDirtyState m_generalDirtyState;
};
