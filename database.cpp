#include "database.h"

Database::Database(QObject *parent, QJsonDocument configFile) :
    QObject(parent),
    m_db(NULL), m_wakedUp(false), m_configFile(configFile)
{

}

QSqlDatabase* Database::access(quint16 timeout)
{
    /*  SINGLETON
     *      WHEN CALL   : Whenever to use the database
     *      ACTIONS     : Instantiate a new db if there isn't already one from a json formated config file
     *                    Open this db for n seconds if not already open
     *                    Start a timeout timer
     *      RETURN VAL  : Database's address once initiated
    */

    // If no database instantiated
    if(m_db == NULL)
    {
        // Create a new one (silly but working way of doing it)
            QSqlDatabase tmpdb = QSqlDatabase::addDatabase("QMYSQL");
            m_db = new QSqlDatabase(tmpdb);


        // Configure it with a json config file
            if(!m_configFile.isNull())
            {
                QJsonObject configDb = m_configFile.object()["db"].toObject();

                m_db->setHostName    (configDb["host"  ].toString());
                m_db->setPort        (configDb["port"  ].toInt()   );
                m_db->setUserName    (configDb["login" ].toString());
                m_db->setPassword    (configDb["passwd"].toString());
                m_db->setDatabaseName(configDb["name"  ].toString());
                m_db->open();
            }
    }

    // If db not already open
    if(!m_db->isOpen())
    {
        // Open it (with error and debug output as well)
            qDebug() << "[DEBUG] general.cpp::db() : Connecting to " << m_db->hostName()     << ":"
                                                                     << m_db->port()         << " on "
                                                                     << m_db->databaseName() << " for "
                                                                     << timeout              << "s";

            if(m_db->open()) qDebug() << "[DEBUG] general.cpp::db() : Db successfuly opened";
            else
            {
                qCritical() << "[ERROR] general.cpp::db() : " << m_db->lastError().text();
                QMessageBox::critical(NULL, "Db Connection", "Error Connecting to Database:\n" +
                                                                  m_db->lastError().text());
            }
    }

    // Start or restart its timer
    m_timer.start(timeout * 1000);

    return m_db;
}

void Database::close()
{
    /*  FUNCTION
     *      WHEN CALL   : When the db needs to be closed (automatically or manually)
     *      ACTIONS     : Close the db and stop its timer if the db is instantiated and open
    */

    if(m_db != NULL)
    {
        if(m_db->isOpen())
        {
            m_db->close();
            m_timer.stop();
            qDebug() << "[DEBUG] database.cpp::close() : Db closed";
        }
        else qDebug() << "[DEBUG] database.cpp::close() : Db can't be closed (not opened)";
    }
    else qDebug() << "[ERROR] database.cpp::close() : Db can't be closed (not instantiated)";
}

void Database::privateclose()
{
    /*  SLOT
     *      ACTIVATION  : When the db's timer runs out
     *      ACTIONS     : Close the db and stop its timer if the db is instantiated and open
    */

    close();
}
