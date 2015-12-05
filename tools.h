#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QRegExp>
#include <QListWidget>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QtGlobal>
#include <QRegExp>

class Tools : public QObject
{
    Q_OBJECT

public:
    explicit Tools(QObject *parent = 0);

    static const QRegExp c_rgx_campName;
    static const QRegExp c_rgx_alphaNumString;

    enum StringEvalCode
    {
        Ok          = 0,
        EmptyStr    = 1,
        TooLong     = 2,
        BadFormat   = 3,
        AlreadyTake = 4
    };

    static StringEvalCode campNameValid(const QString& s,
                                        const QListWidget&   list,
                                        const QRegExp& regex,
                                        quint16 curCamp,
                                        quint16 sizeMax= 50);

    static void dispErr(QWidget* source, const StringEvalCode& code);
    static QJsonDocument* jsonFromFile(const QString& path, bool* ok = NULL);
};

#endif // TOOLS_H
