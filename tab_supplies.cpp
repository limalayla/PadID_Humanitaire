#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::suppliesInit(QSqlDatabase*)
{
    QSqlQuery req_supplies (*m_db->access());
    req_supplies.prepare("Select nom_fourniture from Fourniture");

    if(req_supplies.exec())
    {
        while(req_supplies.next())
                ui->tabs_supplies->addTab(new QWidget(ui->tabs_supplies),req_supplies.value(0).toString());
    }
    //For every tab create put the same configuration in there like quantity,....*/
}


void MainWin::suppliesLoad(QSqlDatabase*)
{
    /* In function of the maquette */
    //....
    //....
}
