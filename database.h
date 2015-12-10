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
    QTimer m_accessTimer;
    QTimer m_errorTimer;
    QJsonDocument m_configFile;
    bool m_dispError;

private slots:
    void privateclose();
    void doDispErr();
};

#endif // DATABASE_H
