#include <QApplication>
#include <QCommandLineParser>

#include <vector>

#ifndef MSVC
#include <stdio.h>      // for freopen()
#include <fcntl.h>
#include <unistd.h>     // needed for dup2( ,STDERR_FILENO) and close()
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "MainWindow.h"
#include "cli_options.h"
#include "debug.h"

#ifndef MSVC	// goes down until just before main() starts
#include "my_getopt.h"

// As getopt_long() checks argv/argc first, we need a list (array) of
// options that getopt_long() should ignore. Those ignored options (and
// following argument if required) then need to be passed through to
// QtApplication() to be processed. This is that list.
static constexpr option qtOptions[] = {
    {"--debug-focus",   no_argument, nullptr, 0},
    {"-platform",       required_argument, nullptr, 0},
    {"-platformtheme",  required_argument, nullptr, 0},
    {"-plugin",         required_argument, nullptr, 0},
    {"-qmljsdebugger",  required_argument, nullptr, 0},
    {"-style",          required_argument, nullptr, 0},
    {"-stylesheet",     required_argument, nullptr, 0},
    {nullptr, 0, nullptr, 0}
};

static constexpr option getoptOptions[] = {
    {"help", no_argument, nullptr, 'h'},      // consumed by getopt_long()
    {"help-all", no_argument, nullptr, 'H'},  // forwarded to QtApp
    {"qt-help", no_argument, nullptr, 'Q'},   // translated to --help ...
    {"qt_help", no_argument, nullptr, 'Q'},   // ... then forwarded to QtApp
    {"stderr", required_argument, nullptr, 's'},
    {"verbose", no_argument, nullptr, 'v'},
    {"version", no_argument, nullptr, 'V'},
    {nullptr, 0, nullptr, 0}
};

static char help_all[] = "--help-all";


static const option *findQtOption(const char *arg)
{
    for (const option *opt = qtOptions; opt->name != nullptr; ++opt) {
        if (std::strcmp(arg, opt->name) == 0)
            return opt;
    }

    return nullptr;
}

static void usage(void)
{
    qsdgDebug() <<
        "\nUsage of options processed before Qt processes them:\n"
        " sdg-control  [--help] [--help-all] [--qt-help] "
        "[--stderr=FN]\n"
        "              [--verbose] [--version]";
    qsdgDebug() <<
        "  where:\n"
        "    --help|-h          print out usage message\n"
        "    --help-all|-H      forwards --help-all to QtApplication\n"
        "    --qt-help|-Q       sends --help to QtApplication\n"
        "    --stderr=FN|-S FN  errors/warnings that would be usually "
        "be sent\n"
        "                       to stderr, instead are appended to file FN\n"
        "    --verbose|-v       increase verbosity of output\n"
        "    --version|-V       print version string and exit\n\n"
        "sdg-control is an application that remotely controls the Siglent "
        "SDG2000X\nseries of function generators. The User Interface (UI) "
        "is based on the\nQt(6) application framework and follows Qt "
        "conventions rather than copying\nthe front panel of the instrument "
        "and duplicating its UI.\n";
}
#endif        // end of NOT MSVC

int main(int argc, char *argv[])
{
    CLI_options cli_options;

#ifndef MSVC      // all ther way down to and including QAplication
    // Arguments that will be given to getopt_long().
    std::vector<char *> appArgs;
    appArgs.push_back(argv[0]);  // argv[0] is name of app

    // Arguments that will eventually be given to QApplication.
    std::vector<char *> qtArgs;
    qtArgs.push_back(argv[0]);

    // Separate Qt arguments from application arguments.
    for (int k = 1; k < argc; ++k) {
        const option *qtOption = findQtOption(argv[k]);

        if (qtOption != nullptr) {
            // Preserve the Qt option.
            qtArgs.push_back(argv[k]);
            // Preserve its argument, if it has one.
            if (qtOption->has_arg == required_argument && k + 1 < argc)
                qtArgs.push_back(argv[++k]);
        } else // Everything else goes to our command-line parser.
            appArgs.push_back(argv[k]);
    }

    appArgs.push_back(nullptr);

    // Parse application options with getopt_long() first.
    const int appArgc = static_cast<int>(appArgs.size()) - 1;
    char help_b[16];
    int c;
    bool addHelp2Qt = false;
    char * addHelpAll2Qt = nullptr;

    opterr = 0; // Prevent getopt_long() from printing its own diagnostics.
    optind = 1; // getopt_long() uses these globals.

    while ((c = getopt_long(appArgc, appArgs.data(), "hHQs:vV",
                            getoptOptions, nullptr)) != -1) {
        switch (c) {
        case 'h':
            usage();
            return 0;
        case 'H':
            addHelpAll2Qt = help_all;
            // addHelpAll2Qt = appArgs.data()[optind - 1];   // nearly works
            // It fails when the short option ('-H') is used.
            break;
        case 'Q':
            addHelp2Qt = true;
            break;
        case 's':
            cli_options.stderr_fn = optarg;
            break;
        case 'v':
            ++cli_options.verbosity;
            break;
        case 'V':
            ++cli_options.version;
            break;
        case '?':
            // Unknown option.
            qsdgDebug() << "getopt_long parsing: unknown option: "
                        << appArgs.data()[optind - 1];
            return 1;
        default:
            qsdgDebug() << "getopt_long parsing: fall-through option: "
                        << appArgs.data()[optind - 1];
            return 1;
        }
    }

    if (cli_options.stderr_fn) {
        int fd = open(cli_options.stderr_fn, O_WRONLY | O_CREAT | O_APPEND,
                      0644);

        sdgDebug() << "About to redirect stderr to file name:"
                   << cli_options.stderr_fn;
        if (fd != -1) {
            dup2(fd, STDERR_FILENO);
            close(fd);
        } else {
            int err = errno;

            sdgDebug() << "Unable to open:" << cli_options.stderr_fn
                       << ", errno=" << err;
        }
    }
    if (addHelp2Qt) {
        strncpy(help_b, "--help", sizeof(help_b) - 1);
        qtArgs.push_back(help_b);
    }
    if (addHelpAll2Qt)
        qtArgs.push_back(addHelpAll2Qt);

    qtArgs.push_back(nullptr);

    // Anything left after getopt_long() is a positional argument.
    // Pass those to QApplication as well.
    for (int k = optind; k < appArgc; ++k)
        qtArgs.push_back(appArgs[k]);

    if (cli_options.verbosity > 3) {
        qsdgDebug() << "shell generated argv/argc contains:";
        for (int k = 0; k < argc; ++k)
            qsdgDebug() << "    " << k << argv[k] ;
        qsdgDebug() << "";    // blank line
        qsdgDebug() << "getoptArgs vector contains:";
        for (int k = 0; k < (int)appArgs.size(); ++k)
            qsdgDebug() << "    " << k << appArgs[k] ;
        qsdgDebug() << "";    // blank line
        qsdgDebug() << "qtArgs vector contains:";
        for (int k = 0; k < (int)qtArgs.size(); ++k)
            qsdgDebug() << "    " << k << qtArgs[k] ;
        sdgDebug() << "<<< start of app timestamp";
    }

    // Now construct QApplication using ONLY the arguments Qt should see.
    int qtArgc = static_cast<int>(qtArgs.size()) - 1;
    QApplication app(qtArgc, qtArgs.data());
#else
    QApplication app(argc, argv);
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription("SDG2000X control");
    parser.addHelpOption();

    QCommandLineOption debugFocusOption(
        "debug-focus",
        "Log widget focus changes.");

    parser.addOption(debugFocusOption);

    parser.process(app);

    cli_options.debugFocus = parser.isSet(debugFocusOption);

    MainWindow window(cli_options);
    window.show();

    return app.exec();
}
