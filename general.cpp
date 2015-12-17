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
        ui->tabs->setTabEnabled(1, true);
        ui->tabs->setTabEnabled(2, m_curCamp != c_AllCampIndex);
        ui->tabs->setTabEnabled(3, m_curCamp != c_AllCampIndex);

    // Switching UI : When camp "All" is selected or not
        ui->groupbox_campAll->setVisible  (m_curCamp == c_AllCampIndex);
        ui->groupbox_campOther->setVisible(m_curCamp != c_AllCampIndex);
        ui->groupbox_center->setVisible(false);

    // Cancel pending operation on db if any
        campModCancel();

    // Load the overview tab consequently
        overviewLoad();

    // Deactivate "Management" and "Supplies" tabs and switch to "Overview" tab when camp "All" is select
    if(m_curCamp == c_AllCampIndex)
    {
        if(m_curTab == 2 || m_curTab == 3) m_curTab = 0;
    }

    else
    {
        managementLoad(m_db->access());
        suppliesLoad  (m_db->access());
        // ToDo: Other tabs loading
    }

    // Clears the results of the search tab
        ui->list_res_search->clear();
        m_searchRefugeeIdDb.clear();
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
        ans = QInputDialog::getText(this, tr("New Camp"), tr("Please enter the name you want for the new camp:"), QLineEdit::Normal, QString(), &ok);

        // If actually clicked on the "ok" button and not just exited the window or cancelled
        if(ok)
        {
            validName = Tools::campNameValid(ans, *ui->list_camp, m_curCamp, 50);

            if(validName != Tools::Ok)
                Tools::dispErr(this, validName);
        }
    }
    while(ok && validName != Tools::Ok);

    if(ok)
    {
        // Add camp to Db
        QSqlQuery req_AddCamp(*m_db->access());

        req_AddCamp.prepare("INSERT INTO Camps (name_camp,nb_max,id_location,id_center) Values (':newCampName',0,0,0)");
        req_AddCamp.bindValue(":newCampName", ans);

        if(req_AddCamp.exec())
        {
            loadCampList();
            qDebug() <<  "[DEBUG] general.cpp::campAdd() : Insert Successful ("  + ans + ")";
        }
        else
        {
            qWarning() << "[WARN ] general.cpp::campAdd() : Insert Failed" << req_AddCamp.lastError().text();
            QMessageBox::warning(this, tr("Error Camp Add"), tr("Error inserting this new camp : ") + req_AddCamp.lastError().text());
        }

        // ToDo : Initialize all of the new camp's attributes
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

void MainWin::loadCampList(bool)
{
    /*  SLOT
     *      Get a fresh list containing all the camps in the database
     *      Can be used for creation or update of the list
     *      m_idDb is an int vector used to make the link between the position of the camp in the list and its id in the db
    */

    int newCampIDDb, prevCampIDDb = (m_curCamp == 0) ? -1 : m_campsIdDb[m_curCamp];

    // Clear the already existing camp list
        ui->list_camp->clear();

        m_campsIdDb.clear();

    // (Re)create the first item "All" (wich have a specific meaning : see a summary of all camps)
        ui->list_camp->addItem(tr("All"));
        ui->list_camp->item(0)->setFont(QFont("Arial", 11));
        ui->list_camp->item(0)->setTextAlignment(Qt::AlignHCenter);
        m_campsIdDb.push_back(-1);

    // Get the list of camps from the database
        QSqlQuery req_listCamp(*m_db->access());
        if(req_listCamp.exec("SELECT id_camp, name_camp FROM Camps ORDER BY name_camp ASC"))
        {
            while(req_listCamp.next())
            {
                m_campsIdDb.push_back(req_listCamp.value(0).toInt());       // ID
                ui->list_camp->addItem(req_listCamp.value(1).toString());   // Camp Name
            }
        }
        else
        {
            qWarning() << "[WARN ] general.cpp::loadCampList() : Query Failed" << req_listCamp.lastError().text();
            QMessageBox::warning(this, tr("Error Loading Camps"), tr("Error while getting the camps list : ") + req_listCamp.lastError().text());
        }

    // To select the good camp in the list, search his db's id and if it doesn't exist, place the camp on "All"
        if((newCampIDDb = m_campsIdDb.indexOf(prevCampIDDb)) == -1) m_curCamp = 0;
        else m_curCamp = newCampIDDb;

        ui->list_camp->setCurrentRow(m_curCamp);
        changeCamp(QModelIndex());
}

void MainWin::loadCenterList(bool)
{
    /*  SLOT
     *      Get a fresh list containing all the stocks in the database
     *      Can be used for creation or update of the list
     *      m_idDb is an int vector used to make the link between the position of the stock in the list and its id in the db
    */

    int newCenterIDDb, prevCenterIDDb = (m_curCenter == 0) ? -1 : m_centerIdDb[m_curCenter];

    // Clear the already existing camp list
        ui->list_center->clear();

        m_centerIdDb.clear();

    // Get the list of centers from the database
        QSqlQuery req_listCenter(*m_db->access());
        if(req_listCenter.exec("SELECT id_center, name_center FROM Centers ORDER BY name_center ASC"))
        {
            while(req_listCenter.next())
            {
                m_centerIdDb.push_back(req_listCenter.value(0).toInt());       // ID
                ui->list_center->addItem(req_listCenter.value(1).toString());   // Camp Name
            }
        }
        else
        {
            qWarning() << "[WARN ] general.cpp::loadCenterList() : Query Failed" << req_listCenter.lastError().text();
            QMessageBox::warning(this, tr("Error Loading Centers"), tr("Error while getting the center list : ") + req_listCenter.lastError().text());
        }

    // To select the good center in the list, search his db's id and if it doesn't exist, place onthe first
        if((newCenterIDDb = m_centerIdDb.indexOf(prevCenterIDDb)) == -1) m_curCenter = 0;
        else m_curCenter = newCenterIDDb;

        ui->list_center->setCurrentRow(m_curCenter);
        changeCenter(QModelIndex());
}

void MainWin::changeCenter(QModelIndex index)
{
    /*  SLOT
     *      ACTIVATION  : When a center of ui->list_center is clicked
     *      ACTIONS     : Gets the new center index
     *                    Changes and load the UI based on this center
     *                    Cancels pending operation on previous camp or center if any
     *      WARNING     : Never use index directly, can be wrong, use m_curCamp instead
    */

    if(index.isValid()) m_curCenter = index.row();

    // Deactivate "search", "Management" and "supplies" tabs
        ui->tabs->setTabEnabled(1, false);
        ui->tabs->setTabEnabled(2, false);
        ui->tabs->setTabEnabled(3, false);

    // Switching UI
        ui->groupbox_campAll->setVisible  (false);
        ui->groupbox_campOther->setVisible(false);
        ui->groupbox_center->setVisible   (true );

    // Cancel pending operation on db if any
        campModCancel();

    // Load the overview tab consequently
        overview_centerLoad(m_db->access());
}

void MainWin::changeCenterSearch(QModelIndex index)
{
    /*  SLOT
     *      ACTIVATION  : When a Center of ui->list_centerSearch is clicked
     *      ACTIONS     : Gets the searched center index (via its label, can't find another way)
     *                    Selects the corresponding camp on ui->list_center
     *                    Calls the normal changeCenter()
    */

    m_curCenter = ui->list_centerSearch->item(index.row())->text().split(" : ").at(0).toInt();
    ui->list_camp->setCurrentRow(m_curCenter);

    // Call the center changing slot with a null parameter (checked there)
    changeCenter(QModelIndex());
}

void MainWin::centerSearch(QString searchString)
{
    /*  SLOT
     *      ACTIVATION  : When the text inside ui->text_searchCenter changes
     *      ACTIONS     : Show or hide a list of results (ui->list_centerSearch) if the input is empty or not
     *                    Search in all ui->list_center's labels, and if it starts with searchString, add it to the results
    */

    // If the string inside ui->text_searchCenter is empty, hide the result list
    if(searchString.isEmpty()) ui->list_centerSearch->setVisible(false);
    else
    {
        // Else, clear that result list
            ui->list_centerSearch->clear();

        // Then search in all the camps (exept "All" and if corresponds, add it to the result list
            for(quint16 i= 1; i< ui->list_center->count(); i++)
            {
                if(ui->list_center->item(i)->text().startsWith(searchString, Qt::CaseInsensitive))
                {
                    ui->list_centerSearch->addItem(QString::number(i) + " : " + ui->list_center->item(i)->text());
                }
            }

        // Then show it
            ui->list_centerSearch->setVisible(true);
    }
}

void MainWin::centerAdd(bool)
{
    /*  SLOT
     *      ACTIVATION  : When the ui->btn_centerAdd button is clicked
     *      ACTIONS     : Opens a popup to ask the name of the new center
     *                    Checks if that name is correct
     *                    If so, adds it to the database and to the application
    */

    bool ok;
    Tools::StringEvalCode validName;
    QString ans;

    do
    {
        ans = QInputDialog::getText(this, tr("New Center"), tr("Please enter the name you want for the new center:"), QLineEdit::Normal, QString(), &ok);

        // If actually clicked on the "ok" button and not just exited the window or cancelled
        if(ok)
        {
            validName = Tools::campNameValid(ans, *ui->list_center, m_curCenter, 50);

            if(validName != Tools::Ok)
                Tools::dispErr(this, validName);
        }
    }
    while(ok && validName != Tools::Ok);

    if(ok)
    {
        // Add center to Db
        QSqlQuery req_addCenter(*m_db->access());

        req_addCenter.prepare("INSERT INTO Center (name_camp,nb_max,id_location,id_center) VALUES (:newCenterName, 0, 0, 0)");
        req_addCenter.bindValue(":newCenterName", ans);

        if(req_addCenter.exec())
        {
            loadCenterList();
            qDebug() <<  "[DEBUG] general.cpp::campCenter() : Insert Successful ("  + ans + ")";
        }
        else
        {
            qWarning() << "[WARN ] general.cpp::campCenter() : Insert Failed" << req_addCenter.lastError().text();
            QMessageBox::warning(this, tr("Error Center Add"), tr("Error inserting this new center : ") + req_addCenter.lastError().text());
        }
    }
}

void MainWin::gen_translate(appLanguages l)
{
    bool ok(false);
    QString lang;
    switch (l)
    {
        case En : {
            lang = "en";
            break;
        }

        case Fr : {
            lang = "fr";
            break;
        }
    }

    if(!m_configFile.isNull() && !m_configFile.isEmpty())
    {
        QFile configFile(qApp->applicationDirPath() + "/config.json");

        if(configFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        {
            QJsonObject newConfile;
            newConfile["db"] = m_configFile.object().value("db");
            newConfile["lang"] = lang;

            configFile.write(QJsonDocument(newConfile).toJson());
            configFile.close();
            ok = true;
        }
    }

    if(ok) QMessageBox::information(this, tr("Language Changing"), tr("Please quit and restart the application to change the language."));
    else QMessageBox::warning(this, tr("Language Changing Failure"), tr("Cannot change the language, check your configuration file."));
}

void MainWin::gen_translateEn(bool)
{
    gen_translate(En);
}

void MainWin::gen_translateFr(bool)
{
    gen_translate(Fr);
}
