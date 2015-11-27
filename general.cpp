#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::changeTab(int index)
{
    /*  SLOT
     *      ACTIVATION  : When a tab is clicked
     *      ACTIONS     : Gets the new tab index
     *                    Cancels pending operation on previous camp if any
    */

    m_curTab = index;
    campModCancel();
}

void MainWin::changeCamp(QModelIndex index)
{
    /*  SLOT
     *      ACTIVATION  : When a camp of ui->list_camp is clicked
     *      ACTIONS     : Gets the new camp index
     *                    Changes and load the UI based on the new camp
     *                    Cancels pending operation on previous camp if any
     *      WARNING     : Never use index directly, can be wrong, use m_curCamp instead
    */

    if(index.isValid()) m_curCamp = index.row();

    // Deactivating "Management" and "supplies" when camp "All" is selected
        ui->tabs->setTabEnabled(2, m_curCamp != c_AllCampIndex);
        ui->tabs->setTabEnabled(3, m_curCamp != c_AllCampIndex);

    // Switching UI : When camp "All" is selected or not
        ui->groupbox_campAll->setVisible  (m_curCamp == c_AllCampIndex);
        ui->groupbox_campOther->setVisible(m_curCamp != c_AllCampIndex);

     // Cancel pending operation on db if any
         campModCancel();

    // Deactivate "Management" and "Supplies" tabs and switch to "Overview" tab when camp "All" is select
    if(m_curCamp == c_AllCampIndex)
    {
        if(m_curTab == 2 || m_curTab == 3) m_curTab = 0;
        // ToDo: SQL
    }

    else
    {
        overviewLoad();
        managementLoad(db());
        // ToDo: Other tabs loading
    }
}

void MainWin::changeCampSearch(QModelIndex index)
{
    /*  SLOT
     *      ACTIVATION  : When a camp of ui->list_campSearch is clicked
     *      ACTIONS     : Gets the searched camp index (via its label, can't find another way)
     *                    Selects the corresponding camp on ui->list_camp
     *                    Calls the normal changeCamp()
    */

    m_curCamp = ui->list_campSearch->item(index.row())->text().split(" : ").at(0).toInt();
    ui->list_camp->setCurrentRow(m_curCamp);

    // Call the camp changing slot with a null parameter (checked there)
    changeCamp(QModelIndex());
}

void MainWin::campAdd(bool)
{
    /*  SLOT
     *      ACTIVATION  : When the ui->btn_campAdd button is clicked
     *      ACTIONS     : Opens a popup to ask the name of the new camp
     *                    Checks if that name is correct
     *                    If so, adds it to the database and to the application
    */

    bool ok;
    Tools::StringEvalCode validName;
    QString ans;

    do
    {
        ans = QInputDialog::getText(this, tr("New Camp"), tr("Camp name : "), QLineEdit::Normal, QString(), &ok);

        // If actually clicked on the "ok" button and not just exited the window or cancelled
        if(ok)
        {
            validName = Tools::campNameValid(ans, *ui->list_camp, Tools::c_regex_campName, m_curCamp, 50);

            if(validName != Tools::Ok)
                Tools::dispErr(this, validName);
        }
    }
    while(ok && validName != Tools::Ok);

    if(ok)
    {
        // ToDo : Add camp to Db

        ui->list_camp->addItem(ans);
        m_campsIdDb.push_back(0 /* Id Sql*/);

        // ToDo : Initialize all off the new camp's attributes
    }
}

void MainWin::campSearch(QString searchString)
{
    /*  SLOT
     *      ACTIVATION  : When the text inside ui->text_searchCamp changes
     *      ACTIONS     : Show or hide a list of results (ui->list_campSearch) if the input is empty or not
     *                    Search in all ui->list_camp's labels, and if it starts with searchString, add it to the results
    */

    // If the string inside ui->text_searchCamp is empty, hide the result list
    if(searchString.isEmpty()) ui->list_campSearch->setVisible(false);
    else
    {
        // Else, clear that result list
            ui->list_campSearch->clear();

        // Then search in all the camps (exept "All" and if corresponds, add it to the result list
            for(quint16 i= 1; i< ui->list_camp->count(); i++)
            {
                if(ui->list_camp->item(i)->text().startsWith(searchString, Qt::CaseInsensitive))
                {
                    ui->list_campSearch->addItem(QString::number(i) + " : " + ui->list_camp->item(i)->text());
                }
            }

        // Then show it
            ui->list_campSearch->setVisible(true);
    }
}

QSqlDatabase* MainWin::db(quint16 timeoutTimer_s)
{
    /*  SINGLETON
     *      WHEN CALL   : Whenever to use the database
     *      ACTIONS     : Instantiate a new db if there isn't already one
     *                    Open this db for n seconds if not already open
     *                    Start a timeout timer
     *      RETURN VAL  : Database's address once initiated
    */

    // If no database instantiated
    if(m_db == NULL)
    {
        // Create a new one (silly but functional way of doing it)
            QSqlDatabase tmpdb = QSqlDatabase::addDatabase("QMYSQL");
            m_db = new QSqlDatabase(tmpdb);

        // Configure it (might be in a config file in the future)
            /* Config IUT */
                m_db->setHostName("127.0.0.1");
                m_db->setPort(5555);

            /* Config NORMALE */
                //m_db->setHostName("joretapo.fr");
                //m_db->setPort(3306);

            m_db->setUserName("root");
            m_db->setPassword("toor");
            m_db->setDatabaseName("humanitaire");
    }

    // If db not already open
    if(!m_db->isOpen())
    {
        // Open it (with error and debug output as well)
            qDebug() << "[DEBUG] general.cpp::db() : Connecting to " << m_db->hostName()     << ":"
                                                                     << m_db->port()         << " on "
                                                                     << m_db->databaseName() << " for "
                                                                     << timeoutTimer_s       << "s";

            if(m_db->open()) qDebug() << "[DEBUG] general.cpp::db() : Db successfuly opened";
            else
            {
                qCritical() << "[ERROR] general.cpp::db() : " << m_db->lastError().text();
                QMessageBox::critical(this, "Db Connextion", "Error Connecting to Database:\n" +
                                                                  m_db->lastError().text());
            }
    }

    // Start or restart its timer
    m_timerdb.start(timeoutTimer_s * 1000);

    return m_db;
}

void MainWin::closedb()
{
    /*  SLOT
     *      ACTIVATION  : When the db's timer runs out
     *      ACTIONS     : Close the db and stop its timer if the db is instantiated and open
    */

    if(m_db != NULL)
    {
        if(m_db->isOpen())
        {
            m_db->close();
            m_timerdb.stop();
            qDebug() << "[DEBUG] general.cpp::closedb() : Db closed";
        }
        else qDebug() << "[DEBUG] general.cpp::closedb() : Db can't be closed (not opened)";
    }
    else qDebug() << "[ERROR] general.cpp::closedb() : Db can't be closed (not instantiated)";
}
