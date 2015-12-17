#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::suppliesLoad(QSqlDatabase* db)
{
    QSqlQuery req_supplies(*db);
    QSqlQuery req_amountSupplies(*db);
    QSqlQuery req_amountInCenter(*db);

    int curTab=1; // put at 0 when we delete the example tab
    int x, y;

    ui->tabs_supplies->clear();

    /* Clear the previous tabs */
       /* for(int i= 0; i< m_suppliesLabel.size(); i++)
        {
            for(int j= 0; j< m_suppliesLabel[i].size(); j++)
            {
                delete m_suppliesLabel[i][j];
            }
            m_suppliesLabel[i].clear();
        }

        m_suppliesLabel.clear();*/


    req_supplies.prepare("SELECT DISTINCT name_category FROM Supplies_categories ORDER BY id_category");


    req_amountSupplies.prepare("SELECT quantity, name_supply"
                               "  FROM Stock_camps, Supplies"
                               " WHERE Stock_camps.id_camp     = :id"
                               "   AND Stock_camps.id_supplies = Supplies.id_supplies"
                               "   AND Supplies.id_category    = :id_category");

    req_amountSupplies.bindValue(":id", m_campsIdDb[m_curCamp]);


    req_amountInCenter.prepare("SELECT quantity "
                               "  FROM Stock_centers, Camps, Supplies "
                               " WHERE Stock_centers.id_center   = Camps.id_center"
                               "   AND Stock_centers.id_supplies = Supplies.id_supplies"
                               "   AND Supplies.id_category      = :id_category"
                               "   AND Camps.id_camp             = :id "
                               "ORDER BY Supplies.id_supplies");

    req_amountInCenter.bindValue(":id", m_campsIdDb[m_curCamp]);

    if(req_supplies.exec())
    {
        while(req_supplies.next())
        {
            m_suppliesLabel.push_back(QVector<QLabel*>());
            m_suppliesText.push_back (QVector<QLineEdit*>());

            req_amountSupplies.bindValue(":id_category", curTab);
            req_amountInCenter.bindValue(":id_category", curTab);

            QGridLayout *layout = new QGridLayout();
            QScrollArea *area   = new QScrollArea();
            QWidget     *tab    = new QWidget();

            /* Widget for every tabs */
                QLabel *nomTab               = new QLabel(req_supplies.value(0).toString());
                QLabel *amount               = new QLabel(tr("Amount"));
                QLabel *label_command        = new QLabel(tr("Amount to command"));
                QPushButton *btn_validate    = new QPushButton(tr("Confirm"));
                QLabel *label_amountInCenter = new QLabel(tr("Amount in Center"));

            /* Add them to the layout */
                layout->addWidget(nomTab,               0, 0);
                layout->addWidget(amount,               0, 1);
                layout->addWidget(label_amountInCenter, 0, 2);
                layout->addWidget(label_command,        0, 3);


            x = 0;
            y = 1;

            if(req_amountSupplies.exec())
            {
                while(req_amountSupplies.next())
                {
                    m_suppliesLabel[curTab].push_back(new QLabel(req_amountSupplies.value(1).toString()));

                    QLineEdit* tmp = new QLineEdit(req_amountSupplies.value(0).toString());
                    tmp->setValidator(new QIntValidator(0, 99999, tmp));
                    m_suppliesText[curTab].push_back(tmp);

                    tmp = new QLineEdit();
                    tmp->setValidator(new QIntValidator(0, 99999, tmp));
                    m_suppliesText[curTab].push_back(tmp);

                    m_suppliesText [curTab][x]->setEnabled(false);

                    layout->addWidget(m_suppliesLabel[curTab][  x], y, 0);
                    layout->addWidget(m_suppliesText [curTab][  x], y, 1);
                    layout->addWidget(m_suppliesText [curTab][x+1], y, 3);

                    if(req_amountInCenter.exec())
                    {
                        while(req_amountInCenter.next())
                        {
                            m_suppliesText[curTab].push_back(new QLineEdit(req_amountInCenter.value(0).toString()));
                            m_suppliesText[curTab][x+2]->setEnabled(false);
                            layout->addWidget(m_suppliesText[curTab][x+2], y, 2);
                        }
                    }
                    else qDebug() << req_amountInCenter.lastError().text();

                    y++;
                    x += 3;
                }
            }
            else qDebug() << req_amountSupplies.lastError().text();

            layout->addWidget(btn_validate, y, 4);
            tab->setLayout(layout);

            area->setWidget(tab);

            ui->tabs_supplies->addTab(area, req_supplies.value(0).toString());
            ui->tabs_supplies->setCurrentIndex(curTab);

            curTab++;

            QObject::connect(btn_validate, SIGNAL(clicked(bool)), this, SLOT(commandStock(bool)));
        }

    }
    else
        qDebug() << req_supplies.lastError();
    ui->tabs_supplies->setCurrentIndex(0);

}

void MainWin::commandStock(bool)
{
    qDebug() << m_suppliesText[1][1]->text();

    bool ok(true);
    int curTab(ui->tabs_supplies->currentIndex() + 1);

    for(int i= 0; i< m_suppliesText[curTab].size(); i+=3)
    {
        if(m_suppliesText[curTab][i+1]->text().toInt() > m_suppliesText[curTab][i+2]->text().toInt())
        {
            ok = false;
            QMessageBox::warning(this, tr("Error"), tr("The center can't assure the command"));
        }
     }

    if(ok)
    {
       for(int i= 0; i< m_suppliesText[curTab].size(); i+=3)
       {
           QSqlQuery updt_supplies(*m_db->access());
           QSqlQuery updt_center  (*m_db->access());

           updt_supplies.prepare("UPDATE Stock_camps SET quantity = :new_quantity"
                                 " WHERE id_camp     = :id"
                                 "   AND id_supplies = (SELECT id_supplies FROM Supplies WHERE name_supply = :type)");

           updt_supplies.bindValue(":id",           m_campsIdDb[m_curCamp]);
           updt_supplies.bindValue(":type",         m_suppliesLabel[curTab][  i]->text());
           updt_supplies.bindValue(":new_quantity", m_suppliesText [curTab][  i]->text().toInt() +
                                                    m_suppliesText [curTab][i+1]->text().toInt());

           if(updt_supplies.exec())
               qDebug()   << updt_supplies.lastQuery();
           else
               qWarning() << updt_supplies.lastError().text();

           updt_center.prepare("UPDATE Stock_centers SET quantity = :new_quantity"
                               " WHERE id_supplies = (SELECT id_supplies FROM Supplies WHERE name_supply = :type)"
                               "   AND id_center   = (SELECT id_center   FROM Camps    WHERE id_camp = :id)");

           updt_center.bindValue(":id",           m_campsIdDb[m_curCamp]);
           updt_center.bindValue(":type",         m_suppliesLabel[curTab][  i]->text());
           updt_center.bindValue(":new_quantity", m_suppliesText [curTab][i+2]->text().toInt() -
                                                  m_suppliesText [curTab][i+1]->text().toInt());

           if(updt_center.exec())
               qDebug()   << updt_center.lastQuery();
           else
               qWarning() << updt_center.lastError().text();
       }

       suppliesLoad(m_db->access());
    }
}


