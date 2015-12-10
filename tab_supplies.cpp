#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::suppliesInit(QSqlDatabase*)
{
    QSqlQuery req_supplies (*m_db->access());
    int i=1; // put at 0 when we delete the example tab
    req_supplies.prepare("SELECT nom_fourniture, truc, Amount FROM Fourniture");

    if(req_supplies.exec())
    {
        while(req_supplies.next())
        {
            QGridLayout *layout = new QGridLayout;
            layout->setColumnStretch(0,0);
            layout->setColumnStretch(1,1);
            layout->setSpacing(0);

            ui->tabs_supplies->addTab(new QWidget(ui->tabs_supplies),req_supplies.value(0).toString());
            ui->tabs_supplies->setCurrentIndex(i);

            //Widget for every tabs
            QLabel *NomTab = new QLabel(req_supplies.value(0).toString());
            QLabel *Amount = new QLabel(tr("Amount"));
            QPushButton *ButtonValidate = new QPushButton(tr("Confirm"));

            QCheckBox *Type = new QCheckBox(req_supplies.value(1).toString());
            QLineEdit *AmountType = new QLineEdit(req_supplies.value(2).toString());

            /*
             *while(req_typeSupplies.next())
             * {
             *      QCheckBox *Type = new QCheckBox(req_Typesupplies.value(0).toString());
             *      QLineEdit *AmountType = new QLineEdit(req_typeSupplies.value(1).toString());
             *
             * layout->addWidget(Type,y,0);
             * layout->addWidget(AmountType,y,2);
             *
             * } */


            QSqlQuery req_typeSupplies(*m_db->access());

            layout->addWidget(NomTab,0,0);
            layout->addWidget(Amount,0,1);
            layout->addWidget(Type,1,0);
            layout->addWidget(AmountType,1,1);
            layout->addWidget(ButtonValidate,2,2);
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
