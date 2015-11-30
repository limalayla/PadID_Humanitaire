#include "mainwin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    /* Opening the config file */
    bool ok;
    QJsonDocument *configFile = Tools::jsonFromFile(app.applicationDirPath() + "/config.json", &ok);

    if(ok)
    {
        QTranslator translator;
        QString translation = app.applicationDirPath() + "/humanitaire_" + configFile->object()["lang"].toString() + ".qm";

        if(!QFile::exists(translation)) qWarning() << translation << " does not exist";

        if(translator.load(translation))
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
