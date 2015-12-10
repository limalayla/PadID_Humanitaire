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

    // Load the overview tab consequently
        overviewLoad();

    // Deactivate "Management" and "Supplies" tabs and switch to "Overview" tab when camp "All" is select
    if(m_curCamp == c_AllCampIndex)
    {
        if(m_curTab == 2 || m_curTab == 3) m_curTab = 0;
        // ToDo: SQL
    }

    else
    {
        managementLoad(m_db->access());
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
            validName = Tools::campNameValid(ans, *ui->list_camp, Tools::c_rgx_alphaNumString, m_curCamp, 50);

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
