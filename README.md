# sdg-control

A Qt6 based graphical application for controlling Siglent SDG2000X series
arbitrary waveform function generators using SCPI over TCP (Ethernet).

Copyright (c) 2026 Douglas Gilbert

SPDX-License-Identifier: BSD-2-Clause

## Supported instruments

The following Siglent SDG2000X series generators are supported:

|  Model   | Maximum frequency |
|----------|-------------------|
| SDG2042X |     40 MHz        |
| SDG2082X |     80 MHz        |
| SDG2122X |    120 MHz        |

Communication uses SCPI over TCP/IP on TCP port 5025.

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
    - Symmetry [RAMP only]
- Refresh instrument state
- Provide two modes: Immediate or Send (non-immediate)
    - Immediate: transmit field contents to SDG after each field edit is complete
    - Send: Send button appears, only send fields when Send button pressed
- Introduce some tooltips
- Connection diagnostics with descriptive error messages
- Lost connection detection and reporting
- Cmake/cpack can build 'deb' and/or 'rpm' packages on Linux
- Save settings to JSON (if requested) and load settings ability as well
- Add a simulator so GUI can be exercised without a SDG2000X present

## Design philosophy

The graphical user interface of this application does _not_ emulate the
front panel of the Siglent SDG2000X series of function generators. Instead,
it is built using native Qt6 widgets and layouts.

> **Note:** Recent SDG2000X firmware includes a webserver that presents a
> virtual front panel. That might be ideal for remote control by mobile
> phones, as they have screen sizes similar to this generator.

Desktop systems, laptops and tablets provide considerably more screen space
than the generator's front-panel display. This space allows related controls
to be grouped together and additional information to be shown simultaneously.

Another benefit of the 'non-emulation' model is that the user interface
is not tied to a particular instrument model. Most modern function generators
share a common set of capabilities (such as waveform selection, frequency,
amplitude, offset, and phase control) even though the details and front
panel placement differ between manufacturers and product families.

By separating the user interface from the layout of any specific front
panel, the application can more readily be extended to support additional
instruments in the future.

## Requirements

- Qt 6
- CMake
- C++17 compatible compiler

Using Ubuntu (a Debian based Linux distribution) these installs are
suggested:
```sh
    sudo apt install \
        qt6-base-dev \
        qt6-base-dev-tools \
        cmake \
        build-essential
```

Tested on Linux.

## Building

```sh
mkdir build
cd build
cmake ..
cmake --build .
```
or in debug mode (by setting SDG_DEBUG=ON):

```sh
cmake -B build -DSDG_DEBUG=ON
cmake --build build -v
cd build
cpack .
app/sdg-control
```

The second last line above builds a 'deb' and/or a 'rpm' package. The last
line executes the app that has just been built.

Note that both cmake build examples above do an "out-of-tree" build. All
build artifacts, including the executable and any packages generated, are
placed under the build sub-directory leaving the rest of the sdg-control
directory structure "clean". Doing
```sh
cd <top_level_source_directory>
rm -rf build
```
will result in a completely clean package structure. It is also a good way
to make sure any cached values from previous builds are removed. And that
is useful before a new build.

## Test utility

The repository also contains a small command-line test program
(`test_scpi`) that can be used to verify SCPI communication with an
instrument independently of the GUI. That test program is not built
by default, to build it uncomment the following line in the top level
CMakeLists.txt file:
    # add_subdirectory(test_scpi)

## License

This project is licensed under the BSD 2-Clause License.

See the LICENSE file for details.

## Credits

The author's existing open source packages use a command line interface
(CLI). With programming assistance from ChatGPT this package uses a
graphical user interface (GUI) based on the Qt6 toolkit.

## Related projects

| Project                    | Language     | Notes                       |
| -------------------------- | ------------ | --------------------------- |
| canxin121/sdg2000x_control | Rust, Qt     | SDG2000X controller         |
| penfold42/SDGRemote        | Python, HTML | Remote front panel          |
| TestController             | C++          | Multi-instrument controller |
| tinylabs/SDG2000X          | Python       | Waveform upload             |

Any similar projects reported to the author will be added here.

## Status

This project is in early active development.

The current focus is implementing reliable control of the SDG2000X
series. The user interface and supported functionality will continue to
evolve.


Last updated: 2026-08-08
