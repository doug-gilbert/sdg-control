
#include <QCoreApplication>
#include <QDebug>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "SDG2000X.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SDG2000X generator;

    if (!generator.connectTo("192.168.48.28"))
    {
        qDebug() << "Connection failed";
        return 1;
    }

    qDebug() << generator.identification();

    generator.setWaveform(1, "SQUARE");
    generator.setFrequency(1, 2000);
    generator.setAmplitude(1, 4);
    generator.setOffset(1, 0);

    generator.output(1, true);

    auto ch1 = generator.getChannelState(1);

    qDebug() << "CH1:";
    qDebug() << "Waveform:" << ch1.waveform;
    qDebug() << "Frequency:" << ch1.frequency;
    qDebug() << "Amplitude:" <<
                 ch1.amplitude.instrumentValues().vpp.value_or(0.0);


    generator.setWaveform(2, "SINE");
    generator.setFrequency(2, 1000);
    generator.setAmplitude(2, 2);
    generator.setOffset(2, 0);
#ifdef SDG_DEBUG
qDebug() << "SCPI error:"
         << generator.getError();
qDebug() << "CH2 immediate:"
         << generator.getChannelState(2).offset;
#endif
    generator.output(2, true);
    
    auto ch2 = generator.getChannelState(2);
    
    qDebug() << "CH2:";
    qDebug() << "Waveform:" << ch2.waveform;
    qDebug() << "Frequency:" << ch2.frequency;
    qDebug() << "Amplitude:" <<
                 ch2.amplitude.instrumentValues().vpp.value_or(0.0);
    qDebug() << "Offset:" << ch2.offset;
    qDebug() << "Output:" << generator.getOutputState(2);

    return 0;
}
