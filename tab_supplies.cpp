#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::suppliesInit(QSqlDatabase& db)
{
    QSqlQuery req_supplies(db);
    QSqlQuery req_AmountSupplies(db);
    int i=1; // put at 0 when we delete the example tab

    req_supplies.prepare("SELECT DISTINCT name_category from Supplies_categories");
    req_AmountSupplies.prepare("Select quantity, name_supply "
                                                " from Stock_camps, Supplies "
                                                "where Stock_camp.id_camp = :idcourant AND "
                                                 " Stock_camp.id_supplies = Supplies.id_supplies");
    req_AmountSupplies.bindValue(":idcourant", m_curCamp);

    if(req_supplies.exec())
    {
        while(req_supplies.next())
        {
            QGridLayout *layout = new QGridLayout;
            QCheckBox *Type;
             QLineEdit *AmountType ;
            layout->setColumnStretch(0,0);
            layout->setColumnStretch(1,1);
            layout->setSpacing(0);

            ui->tabs_supplies->addTab(new QWidget(ui->tabs_supplies),req_supplies.value(0).toString());
            ui->tabs_supplies->setCurrentIndex(i);

            //Widget for every tabs
            QLabel *NomTab = new QLabel(req_supplies.value(0).toString());
            QLabel *Amount = new QLabel(tr("Amount"));
            QPushButton *ButtonValidate = new QPushButton(tr("Confirm"));
            if(req_AmountSupplies.exec())
            {
                if(req_AmountSupplies.next())
                {
                    Type = new QCheckBox(req_AmountSupplies.value(1).toString());
                    AmountType = new QLineEdit(req_AmountSupplies.value(0).toString());

                    layout->addWidget(NomTab,0,0);
                    layout->addWidget(Amount,0,1);
                    layout->addWidget(Type,1,0);
                    layout->addWidget(AmountType,1,1);
                    layout->addWidget(ButtonValidate,2,2);
                    ui->tabs_supplies->currentWidget()->setLayout(layout);
                }
            }

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

            i++;
        }
    //For every tab create put the same configuration in there like quantity,....*/
    }
    else
        qDebug() << req_supplies.lastError();

}


void MainWin::suppliesLoad(QSqlDatabase&)
{
    /* In function of the maquette */
    //....
    //....
}
