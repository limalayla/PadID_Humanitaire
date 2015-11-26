#include "mainwin.h"
#include "ui_mainwin.h"


void MainWin::managementLoad(QSqlDatabase* db_)
{
    QSqlQuery req_management (*db_);
    ui->list_manageSearch->setVisible(false);
    ui->list_manage->clear();
    req_management.prepare("Select Refugie.nom from Refugie where  :id_courant= Refugie.id_camp");
    req_management.bindValue(":id_courant", m_campsIdDb[m_curCamp]);

    if(req_management.exec())
    {
            while(req_management.next())
            {
                ui->list_manage->addItem(req_management.value(0).toString());
            }
    }
}

void MainWin::manageSearch(QString searchString)
{
    /*  SLOT
     *      ACTIVATION  : When the text inside ui->text_searchManage changes
     *      ACTIONS     : Show or hide a list of results (ui->list_ManageSearch) if the input is empty or not
     *                    Search in all ui->list_manage's labels, and if it starts with searchString, add it to the results
    */

    if(searchString.isEmpty())
        ui->list_manageSearch->setVisible(false);
    else
    {
        ui->list_manageSearch->clear();
        for(int i=0;i< ui->list_manage->count();i++)
        {
            if(ui->list_manage->item(i)->text().startsWith(searchString, Qt::CaseInsensitive))
            {
                ui->list_manageSearch->addItem(QString::number(i) + " : " + ui->list_manage->item(i)->text());
            }
        }
        ui->list_manageSearch->setVisible(true);
    }
}

void MainWin::changeManageSearch(QModelIndex index)
{
    /*  SLOT
     *      ACTIVATION  : When a person of ui->list_manageSearch is clicked
     *      ACTIONS     : Gets the searched person index (via its label, can't find another way)
     *                    Selects the corresponding person on ui->list_manage
    */
    ui->list_manage->setCurrentRow(ui->list_manageSearch->item(index.row())->text().split(" : ").at(0).toInt());
}


