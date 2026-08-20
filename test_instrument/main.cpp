
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

#include <iostream>
#include <string>
#include <limits>

#include <unistd.h>   // for sleep()
#include <stdio.h>    // for stdout

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   testing
//     #undef HAVE_GETOPT_LONG
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  end of testing

#include "my_getopt.h"

#include "SDG2000X.h"
#include "debug.h"


static const char * version_str = "0.90 20260818";

static const struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"test", required_argument, 0, 't'},
    {"verbose", no_argument, 0, 'v'},
    {"version", no_argument, 0, 'V'},
    {0, 0, 0, 0},
};


static void usage()
{
    qDebug() << "  sdg_test   [--help] [--test=NUM] [--verbose] [--version]";
    qDebug() << "where:";
    qDebug() << "   --help | -h           print out usage message";
    qDebug() << "   --test=NUM |-t NUM    dummy option with argument";
    qDebug() << "   --verbose | -v        increase level of verbosity/debug";
    qDebug() << "   --version | -V        print version information and exit";
    qDebug() << "";

    QTextStream(stdout) << "Example of no trailing LF";
    qDebug() << ", this should follow on same line";
}

static void printChannelState(SDG2000X & generator, int channel)
{
    auto state = generator.getChannelState(channel);

    sdgDebug() << "CH" << channel << ":";
    sdgDebug() << "  Waveform:" << state.waveform;
    sdgDebug() << "  Frequency:" << state.frequency;
    sdgDebug() << "  Amplitude:"
              << state.amplitude.instrumentValues().vpp.value_or(0.0);
    sdgDebug() << "  Offset:" << state.offset;
    sdgDebug() << "  Duty:" << state.duty;
    sdgDebug() << "  Output:" << state.output;
}

static QByteArray getFPWindow(QFile & file, SDG2000X &generator)
{
    QByteArray bmp = generator.getFrontPanelImage();

    sdgDebug() << "Screen dump:" << bmp.size() << "bytes";

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(bmp);
        file.close();
        return bmp;
    } else
        return { };
}


int main(int argc, char *argv[])
{
    bool verbose_given = false;
    int vb = 0;
    char c;
    const char * device_name = nullptr;
    std::string s;

    while (1) {
        int option_index = 0;

        c = getopt_long(argc, argv, "ht:vV", long_options,
                        &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'h':
        case '?':
            usage();
            return 0;
        case 't':
            qDebug() << "test of --test= option, argument given" << optarg;
            break;
        case 'v':
            verbose_given = true;
            ++vb;
            break;
        case 'V':
            qDebug() << "Version:" << version_str;
            return 0;
        default:
            qDebug() << "unrecognised option code:" << c;
            usage();
            return 1;
        }
    }
    if (optind < argc) {
        if (NULL == device_name) {
            device_name = argv[optind];
            ++optind;
        }
        if (optind < argc) {
            for (; optind < argc; ++optind)
                qDebug() << "Unexpected extra argument:" << argv[optind];
            usage();
            return 1;
        }
    }
    if (device_name)
        qDebug() << "First command line argument:" << device_name;


    QCoreApplication app(argc, argv);

    SDG2000X generator;

    if (!generator.connectTo("192.168.48.28"))
    {
        sdgDebug() << "Connection failed";
        return 1;
    }

    if (vb)
        qDebug() << " Turn both Output ON at same time";
    generator.outputBoth(true);
    generator.waitForOperationComplete(4000);

    QFile fname_before { "before.bmp" };
    auto before = getFPWindow(fname_before, generator);

    sdgDebug() << "Toggling front-panel channel";
    generator.toggleChannelFocus();

    QFile fname_after_immed { "after_immed.bmp" };
    auto after_immed = getFPWindow(fname_after_immed, generator);

    if (before == after_immed)
        sdgDebug() << "Front-panel image UNCHANGED";
    else
        sdgDebug() << "Front-panel image CHANGED";

    sdgDebug() << generator.identification();

    generator.setWaveform(1, "SQUARE");
    generator.setFrequency(1, 2000);
    generator.setAmplitude(1, 2);
    generator.setOffset(1, 1);

    generator.setDuty(1, 90);
    generator.output(1, true);

generator.getChannelState(1);
    // sleep(3);
    sdgDebug() << "Send INVERT(true) command";
    sdgDebug() << "<< Press Return to continue >>";
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::cin.get();
    bool inverted = generator.invert(1, true);
    sdgDebug() << "INVT:" << inverted;
generator.getChannelState(1);

    sdgDebug() << "Send INVERT(false) command";
    sdgDebug() << "<< Press Return to continue >>";
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::cin.get();
    inverted = generator.invert(1, false);
    sdgDebug() << "INVT:" << inverted;
generator.getChannelState(1);

    // sleep(3);
    sdgDebug() << "Send INVERT(true) command";
    sdgDebug() << "<< Press Return to continue >>";
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::cin.get();
    inverted = generator.invert(1, true);
    sdgDebug() << "INVT:" << inverted;
generator.getChannelState(1);

    sdgDebug() << "Send OUTP(true, load50=true, polNormal=true) command";
    sdgDebug() << "<< Press Return to continue >>";
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::cin.get();

    bool ok = generator.outputLoadPol(1, true, /* load50= */ true,
                                      /* polNormal */ true);
    sdgDebug() << "outputLoadPol --> " << ok;
generator.getChannelState(1);


    sdgDebug() << "Continue with other tests";
    sdgDebug() << "<< Press Return to continue >>";
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::cin.get();

    generator.output(1, true);

    printChannelState(generator, 1);

    generator.setWaveform(2, "SINE");
    generator.setFrequency(2, 1000);
    generator.setAmplitude(2, 2);
    generator.setOffset(2, 0);
#ifdef SDG_DEBUG
sdgDebug() << "SCPI error:"
         << generator.getError();
sdgDebug() << "CH2 immediate:"
         << generator.getChannelState(2).offset;
#endif
    generator.output(2, true);

    printChannelState(generator, 2);

    return 0;
}
