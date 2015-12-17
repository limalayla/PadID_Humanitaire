#include "mainwin.h"
#include "ui_mainwin.h"


void MainWin::suppliesLoad(QSqlDatabase& db)
{    

    QSqlQuery req_supplies(db);
    QSqlQuery req_AmountSupplies(db);
    QSqlQuery req_AmountInCenter(db);
    int i=1; // put at 0 when we delete the example tab
    ui->tabs_supplies->clear();
 /*   for(int i= 0; i< m_pointeur.length(); i++)
    {
      for(int j= 0; j< m_pointeur[i].length(); j++)
      {
        delete m_pointeur[i][j];
      }
      m_pointeur[i].clear();
    }
    m_pointeur.clear();*/
    req_supplies.prepare("SELECT DISTINCT name_category from Supplies_categories ORDER BY id_category");
    req_AmountSupplies.prepare("Select quantity, name_supply "
                                                " from Stock_camps, Supplies "
                                                "where Stock_camps.id_camp = :id_courant AND "
                                                 " Stock_camps.id_supplies = Supplies.id_supplies AND "
                                                " Supplies.id_category = :id_category ");
    req_AmountSupplies.bindValue(":id_courant", m_campsIdDb[m_curCamp]);

    req_AmountInCenter.prepare("Select quantity from Stock_centers, Camps, Supplies"
                                                    " where Stock_centers.id_center = Camps.id_center AND "
                                                    " Stock_centers.id_supplies = Supplies.id_supplies AND "
                                                    " Supplies.id_category = :id_category AND "
                                                    " Camps.id_camp = :id_courant"
                                                    " ORDER BY Supplies.id_supplies");
    req_AmountInCenter.bindValue(":id_courant", m_campsIdDb[m_curCamp]);

    if(req_supplies.exec())
    {
        while(req_supplies.next())
        {
            m_pointeurLabel.append(QVector<QLabel*>());
            m_pointeur.append(QVector<QLineEdit*>());
            req_AmountSupplies.bindValue(":id_category",i);
             req_AmountInCenter.bindValue(":id_category",i);
            QGridLayout *layout = new QGridLayout();
            QScrollArea *Area= new QScrollArea();
            QWidget *tab = new QWidget();
            //QLabel *Type;
           // QLineEdit *AmountType, *Command, *AmountInCenter;

            //Widget for every tabs
            QLabel *NomTab = new QLabel(req_supplies.value(0).toString());
            QLabel *Amount = new QLabel(tr("Amount"));
            QLabel *LabelCommand = new QLabel(tr("Amount to command"));
            QPushButton *ButtonValidate = new QPushButton(tr("Confirm"));
            QLabel *LabelAmountInCenter = new QLabel(tr("Amount in Center"));

            layout->addWidget(NomTab,0,0);
            layout->addWidget(Amount,0,1);
            layout->addWidget(LabelAmountInCenter,0,2);
            layout->addWidget(LabelCommand,0,3);

            int y=1,x=0;
            if(req_AmountSupplies.exec())
            {
                while(req_AmountSupplies.next())
                {

                    m_pointeurLabel[i].push_back(new QLabel(req_AmountSupplies.value(1).toString()));
                    m_pointeur[i].push_back(new QLineEdit(req_AmountSupplies.value(0).toString()));
                    m_pointeur[i].push_back(new QLineEdit());
                    m_pointeur[i][x]->setEnabled(false);

                    layout->addWidget(m_pointeurLabel[i][x],y,0);
                    layout->addWidget(m_pointeur[i][x],y,1);
                    layout->addWidget(m_pointeur[i][x+1],y,3);

                    if(req_AmountInCenter.exec())
                    {
                        while(req_AmountInCenter.next())
                        {
                            m_pointeur[i].push_back(new QLineEdit(req_AmountInCenter.value(0).toString()));
                            m_pointeur[i][x+2]->setEnabled(false);
                            layout->addWidget(m_pointeur[i][x+2],y,2);
                        }
                    }
                    else
                        qDebug() << req_AmountInCenter.lastError();
                    y++;
                    x+=3;
                }
            }
            else
                qDebug() << req_AmountSupplies.lastError();
            layout->addWidget(ButtonValidate,y,4);
            tab->setLayout(layout);
            Area->setWidget(tab);
            ui->tabs_supplies->addTab(Area,req_supplies.value(0).toString());
            ui->tabs_supplies->setCurrentIndex(i);
            i++;
            QObject::connect(ButtonValidate,     SIGNAL(clicked(bool)),              this, SLOT(CommandStock(bool)));
        }

    }
    else
        qDebug() << req_supplies.lastError();
    ui->tabs_supplies->setCurrentIndex(0);

}

void MainWin::CommandStock(bool)
{
    qDebug()<< m_pointeur[1][1]->text();
    bool ok=true;
    int curTabs=ui->tabs_supplies->currentIndex()+1;
    for(int i=0;i<m_pointeur[curTabs].length();i+=3)
    {
        if(m_pointeur[curTabs][i+1]->text().toInt()>m_pointeur[curTabs][i+2]->text().toInt())
        {
            ok=false;
            QMessageBox::warning(this, tr("Error"), tr("The center can't assure the command"));
        }
     }

    if(ok)
        {
               for(int i=0;i<m_pointeur[curTabs].length();i+=3)
               {
                   QSqlQuery updt_supplies(*m_db->access());
                   QSqlQuery updt_Center(*m_db->access());
                   updt_supplies.prepare("Update Stock_camps set "
                                                     " quantity = :new_quantity "
                                                     " where id_camp = :id_courant AND "
                                                     " id_supplies = (Select  id_supplies from Supplies where name_supply = :Type)");
                   updt_supplies.bindValue(":id_courant", m_campsIdDb[m_curCamp]);
                   updt_supplies.bindValue(":Type",m_pointeurLabel[curTabs][i]->text());
                   updt_supplies.bindValue(":new_quantity", m_pointeur[curTabs][i]->text().toInt()+m_pointeur[curTabs][i+1]->text().toInt());

                   if(updt_supplies.exec())
                       qDebug() << updt_supplies.lastQuery();
                   else
                       qDebug() << updt_supplies.lastError();

                   updt_Center.prepare("Update Stock_centers SET"
                                                    " quantity = :new_quantity "
                                                    " where id_supplies = (Select  id_supplies from Supplies where name_supply = :Type) AND"
                                                    " id_center = (SELECT id_center from Camps Where id_camp = :id_courant)   ");
                   updt_Center.bindValue(":id_courant", m_campsIdDb[m_curCamp]);
                   updt_Center.bindValue(":Type",m_pointeurLabel[curTabs][i]->text());
                   updt_Center.bindValue(":new_quantity", m_pointeur[curTabs][i+2]->text().toInt()-m_pointeur[curTabs][i+1]->text().toInt());

                   if(updt_Center.exec())
                       qDebug() << updt_Center.lastQuery();
                   else
                       qDebug() << updt_Center.lastError();
               }
               suppliesLoad(*m_db->access());
        }
}


