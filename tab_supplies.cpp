#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::suppliesInit(QSqlDatabase& db)
{
    QSqlQuery req_supplies(db);
    req_supplies.prepare("SELECT DISTINCT name_category from Supplies_categories ORDER BY id_category");

    if(req_supplies.exec())
    {
        while(req_supplies.next())
        {
            ui->tabs_supplies->addTab(new QWidget(ui->tabs_supplies),req_supplies.value(0).toString());
        }
    }
    else
        qDebug() << req_supplies.lastError();

}


void MainWin::suppliesLoad(QSqlDatabase& db)
{    

    QSqlQuery req_supplies(db);
    QSqlQuery req_AmountSupplies(db);
    int i=1; // put at 0 when we delete the example tab
    ui->tabs_supplies->clear();
    req_supplies.prepare("SELECT DISTINCT name_category from Supplies_categories ORDER BY id_category");
    req_AmountSupplies.prepare("Select quantity, name_supply "
                                                " from Stock_camps, Supplies "
                                                "where Stock_camps.id_camp = :id_courant AND "
                                                 " Stock_camps.id_supplies = Supplies.id_supplies AND "
                                                " Supplies.id_category = :id_category ");
    req_AmountSupplies.bindValue(":id_courant", m_campsIdDb[m_curCamp]);

    if(req_supplies.exec())
    {
        while(req_supplies.next())
        {
            req_AmountSupplies.bindValue(":id_category",i);
            QGridLayout *layout = new QGridLayout();
            QScrollArea *Area= new QScrollArea();
            QWidget *tab = new QWidget();
            QLabel *Type;
            QLineEdit *AmountType, *Command;

            //Widget for every tabs
            QLabel *NomTab = new QLabel(req_supplies.value(0).toString());
            QLabel *Amount = new QLabel(tr("Amount"));
            QLabel *LabelCommand = new QLabel(tr("Amount to command"));
            QPushButton *ButtonValidate = new QPushButton(tr("Confirm"));

            layout->addWidget(NomTab,0,0);
            layout->addWidget(Amount,0,1);
            layout->addWidget(LabelCommand,0,2);
            int y=1;
            if(req_AmountSupplies.exec())
            {
                while(req_AmountSupplies.next())
                {
                    Type = new QLabel(req_AmountSupplies.value(1).toString());
                    AmountType = new QLineEdit(req_AmountSupplies.value(0).toString());
                    Command = new QLineEdit();
                    AmountType->setEnabled(false);

                    layout->addWidget(Type,y,0);
                    layout->addWidget(AmountType,y,1);
                    layout->addWidget(Command,y,2);
                    y++;
                }
            }
            else
                qDebug() << req_AmountSupplies.lastError();
            layout->addWidget(ButtonValidate,y,3);
            tab->setLayout(layout);
            Area->setWidget(tab);
            ui->tabs_supplies->addTab(Area,req_supplies.value(0).toString());
            ui->tabs_supplies->setCurrentIndex(i);
            i++;
            QObject::connect(ButtonValidate,     SIGNAL(clicked(bool)),              this, SLOT(CommandeStock(bool)));
        }

    }
    else
        qDebug() << req_supplies.lastError();
    ui->tabs_supplies->setCurrentIndex(0);

}
/*
void MainWin::CommandeStock(bool)
{

}
*/
