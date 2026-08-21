#include <QApplication>
#include <QCommandLineParser>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "MainWindow.h"

#include "debug.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("SDG2000X control");
    parser.addHelpOption();

    QCommandLineOption debugFocusOption(
        "debug-focus",
        "Log widget focus changes.");

    parser.addOption(debugFocusOption);

    parser.process(app);

    const bool debugFocus = parser.isSet(debugFocusOption);

    sdgDebug() << "debugFocus=" << debugFocus
               << "not implemented yet";

    MainWindow window;
    window.show();

    return app.exec();
}
