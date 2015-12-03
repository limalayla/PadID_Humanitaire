#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::managementLoad(QSqlDatabase* db_)
{
    QSqlQuery req_management (*db_);
    ui->list_manageSearch->setVisible(false);
    ui->list_manage->clear();
    m_refugeeIdDb.clear();
    req_management.prepare("SELECT Refugie.nom,Refugie.id_refugie FROM Refugie WHERE Refugie.id_camp = :id_courant");
    req_management.bindValue(":id_courant", m_campsIdDb[m_curCamp]);

    if(req_management.exec())
    {
        while(req_management.next())
        {
            ui->list_manage->addItem(req_management.value(0).toString());
            m_refugeeIdDb.push_back(req_management.value(1).toInt());
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

    m_curRefugee = ui->list_manageSearch->item(index.row())->text().split(" : ").at(0).toInt();
    ui->list_manage->setCurrentRow(m_curRefugee);

    refugeeSet(QModelIndex());
}


void MainWin::refugeeSet(QModelIndex index)
{
    if(index.isValid()) m_curRefugee = index.row();
}

void MainWin::refugeeSee(QModelIndex)
{
    openRefugeeInfo(RefugeeInfoWin::readOnly, m_refugeeIdDb[m_curRefugee]);
}

void MainWin::refugeeAdd(bool)
{
    openRefugeeInfo(RefugeeInfoWin::creation, -1);
}

void MainWin::refugeeMod(bool)
{
    openRefugeeInfo(RefugeeInfoWin::readWrite, m_refugeeIdDb[m_curRefugee]);
}

void MainWin::refugeeDel(bool)
{
    if(QMessageBox::question(this, tr("Are you sure ?"), tr("Delete this refugee ?")) == QMessageBox::Yes)
    {
        QSqlQuery RemoveRefugee;
        RemoveRefugee.prepare("DELETE FROM Refugie where id_refugie = :idDelete");
        RemoveRefugee.bindValue(":idDelete", m_refugeeIdDb[m_curRefugee]);
        if(RemoveRefugee.exec())
            qDebug() << "Delete Successful";
        else
            qDebug() << RemoveRefugee.lastError();
        managementLoad(m_db->access());
    }
}

void MainWin::openRefugeeInfo(RefugeeInfoWin::OpenMode openMode, int idRefugeeDb)
{
    if(m_refugeeInfoWin == NULL)
    {
        m_refugeeInfoWin = new RefugeeInfoWin(m_db, this, idRefugeeDb, openMode);
    }

    if(m_refugeeInfoWin->isVisible())
    {
        QMessageBox::critical(this, tr("Refugee Window Already Open"), tr("The Refugee Window is already open !"));
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
    managementLoad(m_db->access());
}

void MainWin::OkRefugeeInfo()
{

    if(m_refugeeInfoWin != NULL)
    {
        delete m_refugeeInfoWin;
        m_refugeeInfoWin = NULL;
    }
    managementLoad(m_db->access());
}
