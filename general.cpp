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
        managementLoad(m_db->access());
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
