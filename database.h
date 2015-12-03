#ifndef DATABASE_H
#define DATABASE_H

#include <QTimer>
#include <QObject>
#include <QSqlDatabase>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);
    QSqlDatabase* access(quint16 timeout= 300);

signals:

public slots:

private:
    QSqlDatabase* m_db;
    bool m_wakedUp;
    QTimer m_timer;

private slots:
    void close();
};

#endif // DATABASE_H
