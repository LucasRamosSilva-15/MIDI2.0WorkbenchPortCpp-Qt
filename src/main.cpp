#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QSettings>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QCoreApplication::setOrganizationName("MidiUmpAnalyzer");
    QCoreApplication::setApplicationName("MidiUmpAnalyzer");
    
    QSettings settings;
    QString lang = settings.value("Language", "system").toString();
    
    QTranslator translator;
    if (lang == "en" || (lang == "system" && QLocale::system().language() == QLocale::English)) {
        if (translator.load(":/i18n/MidiUmpAnalyzer_en_US.qm")) {
            app.installTranslator(&translator);
        }
    }
    
    MainWindow w;
    w.show();
    
    return app.exec();
}
