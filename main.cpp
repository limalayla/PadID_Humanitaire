#include "mainwin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);



    /* Opening the config file */
    bool ok;
    QJsonDocument *configFile = Tools::jsonFromFile(app.applicationDirPath() + "/config.json", &ok);
    QTranslator translator;

    if(ok)
    {
        QString translation = "humanitaire_" + configFile->object()["lang"].toString() + ".qm";

        if(!QFile::exists(translation)) qWarning() << translation << " does not exist";

        if(translator.load(translation, app.applicationDirPath() + "/lang"))
        {
            qDebug() << "[DEBUG]main.cpp::main() : Translation succeeded for " << translation << "!";
            app.installTranslator(&translator);
        }

        else
        {
            qWarning() << "[WARNI]main.cpp::main() : Translation Failed for " << translation << "!";
        }
    }

    else
    {
        qCritical() << "[ERROR] Couldn't open the configuration file";
    }

    MainWin w(0, ok ? *configFile : QJsonDocument());
    w.show();

    return app.exec();
}
