#ifndef DATABASE_H
#define DATABASE_H

#include <QTimer>
#include <QObject>

#include <QtSql>
#include <QSqlError>
#include <QSqlDatabase>

#include "tools.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent, QJsonDocument configFile);
    QSqlDatabase* access(quint16 timeout= 300);

signals:

public slots:
    void close();

private:
    QSqlDatabase* m_db;
    bool m_wakedUp;
    QTimer m_timer;
    QJsonDocument m_configFile;

private slots:
    void privateclose();
};

#endif // DATABASE_H
