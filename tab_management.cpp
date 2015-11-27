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


void MainWin::refugeeAdd(bool)
{
    openRefugeeInfo(RefugeeInfoWin::creation);
}

void MainWin::refugeeMod(bool)
{
    openRefugeeInfo(RefugeeInfoWin::readWrite);
}

void MainWin::refugeeDel(bool)
{
    if(QMessageBox::question(this, tr("Are you sure ?"), tr("Delete this refugee ?")) == QMessageBox::Yes)
    {
        /* SQL */
    }
}

void MainWin::openRefugeeInfo(RefugeeInfoWin::OpenMode openMode)
{
    if(m_refugeeInfoWin == NULL)
    {
        m_refugeeInfoWin = new RefugeeInfoWin(db(),this, -1,openMode);
    }

    if(m_refugeeInfoWin->isVisible())
    {
        QMessageBox::critical(this, "Refugee Window Already Open", "The Refugee Window is already open !");
    }

    else
    {
        m_refugeeInfoWin->show();
    }
}

void MainWin::closeRefugeeInfo()
{
    if(m_refugeeInfoWin != NULL)
    {
        delete m_refugeeInfoWin;
        m_refugeeInfoWin = NULL;
    }
}

void MainWin::OkRefugeeInfo()
{
    /*QSqlQuery AddorUpdateRefugee;

    AddorUpdateRefugee.prepare("Insert into Refugie (id_refugie,nom,prenom,age,sexe,pays_dorigine,type,etat,divers,id_camp "
                               +"Values"
                               +"on Duplicate Key nom= :newname , prenom = :newfname, etat= :newState,id_camp = :newId_camp");*/
    if(m_refugeeInfoWin != NULL)
    {
        delete m_refugeeInfoWin;
        m_refugeeInfoWin = NULL;
    }
}
