/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

struct CLI_options {
    bool debugFocus = false;
    int verbosity = 0;
    int version = 0;
    const char * stderr_fn = nullptr;
};
