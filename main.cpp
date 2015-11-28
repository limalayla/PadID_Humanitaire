#include "mainwin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    if(translator.load("humanitaire_en.qm", app.applicationDirPath()))
    {
        qDebug() << "[DEBUG]main.cpp::main() : Translation succeeded !";
    }

    else
    {
        qCritical() << "[ERROR]main.cpp::main() : Translation Failed !";
    }

    app.installTranslator(&translator);

    MainWin w;
    w.show();

    return app.exec();
}
