
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include <getopt.h>

#include "SDG2000X.h"
#include "debug.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SDG2000X generator;

    if (!generator.connectTo("192.168.48.28"))
    {
        sdgDebug() << "Connection failed";
        return 1;
    }

    QByteArray bmp = generator.getFrontPanelImage();

    sdgDebug() << "Screen dump:" << bmp.size() << "bytes";

    QFile file("sdg-screen.bmp");

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(bmp);
        file.close();
    }
exit(0);

    sdgDebug() << "Toggling front-panel channel";
    generator.toggleChannelFocus();

    sdgDebug() << generator.identification();

    generator.setWaveform(1, "SQUARE");
    generator.setFrequency(1, 2000);
    generator.setAmplitude(1, 4);
    generator.setOffset(1, 0);

    generator.output(1, true);

    auto ch1 = generator.getChannelState(1);

    sdgDebug() << "CH1:";
    sdgDebug() << "Waveform:" << ch1.waveform;
    sdgDebug() << "Frequency:" << ch1.frequency;
    sdgDebug() << "Amplitude:" <<
                 ch1.amplitude.instrumentValues().vpp.value_or(0.0);

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

    auto ch2 = generator.getChannelState(2);

    sdgDebug() << "CH2:";
    sdgDebug() << "Waveform:" << ch2.waveform;
    sdgDebug() << "Frequency:" << ch2.frequency;
    sdgDebug() << "Amplitude:" <<
                 ch2.amplitude.instrumentValues().vpp.value_or(0.0);
    sdgDebug() << "Offset:" << ch2.offset;
    sdgDebug() << "Output:" << generator.getOutputState(2);

    return 0;
}
