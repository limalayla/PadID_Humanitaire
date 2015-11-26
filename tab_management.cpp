#include "mainwin.h"
#include "ui_mainwin.h"


void MainWin::managementLoad(QSqlDatabase* db_)
{
    QSqlQuery req_management (*db_);

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
