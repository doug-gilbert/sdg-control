# sdg-control

A Qt6 graphical application for controlling Siglent SDG2000X series
arbitrary waveform generators using SCPI over Ethernet.

Copyright (c) 2026 Douglas Gilbert

SPDX-License-Identifier: BSD-2-Clause

## Supported instruments

The following Siglent SDG2000X series generators are supported:

| Model | Maximum frequency |
|-------|------------------:|
| SDG2042X | 40 MHz |
| SDG2082X | 80 MHz |
| SDG2122X | 120 MHz |

Communication uses SCPI over TCP/IP on port 5025.

## Features

Current features include:

- Connect to an SDG2000X by hostname or IP address
- Persistent storage of the last hostname/IP using QSettings
- Read instrument identification
- Read and modify both channels
    - Output enable
    - Waveform
    - Frequency
    - Amplitude
    - Offset
    - Phase
- Refresh instrument state
- Connection diagnostics with descriptive error messages

## Requirements

- Qt 6
- CMake
- C++17 compatible compiler

Tested on Linux.

## Building

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## License

This project is licensed under the BSD 2-Clause License.

See the LICENSE file for details.

## Related projects

Several other open-source projects support the Siglent SDG2000X series:

- canxin121/sdg2000x_control
  - Rust
  - Qt GUI
  - SDG2000X controller

- penfold42/SDGRemote
  - HTML and Python
  - Mimics the SDG front panel and adds additional functionality

- TestController
  - General-purpose instrument controller
  - Supports many instruments including the SDG2000X family

- tinylabs/SDG2000X
  - Python
  - Uploads arbitrary waveforms to SDG2000X instruments

## Status

This project is under active development.


Last updated: 2026-08-02
