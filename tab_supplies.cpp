#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::suppliesInit(QSqlDatabase*)
{
    QSqlQuery req_supplies (*m_db->access());
    int i=1; // put at 0 when we delete the example tab
    req_supplies.prepare("Select nom_fourniture from Fourniture");

    if(req_supplies.exec())
    {
        while(req_supplies.next())
                {
                        ui->tabs_supplies->addTab(new QWidget(ui->tabs_supplies),req_supplies.value(0).toString());
                ui->tabs_supplies->setCurrentIndex(i);
                    QLabel *NomTab = new QLabel(req_supplies.value(0).toString());
                    QLabel *Amount = new QLabel("Amount");


                    QGridLayout *layout = new QGridLayout;
                    layout->addWidget(NomTab,0,0);
                    layout->addWidget(Amount,0,2);
                    ui->tabs_supplies->currentWidget()->setLayout(layout);



                    i++;
                }
    //For every tab create put the same configuration in there like quantity,....*/
    }
    else
        qDebug() << req_supplies.lastError();

}


void MainWin::suppliesLoad(QSqlDatabase*)
{
    /* In function of the maquette */
    //....
    //....
}
