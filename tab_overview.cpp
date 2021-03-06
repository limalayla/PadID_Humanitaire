#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::overviewCreation()
{
    ui->groupbox_campOther->setVisible(false);

    ui->text_campName->setValidator(new QRegExpValidator(Tools::c_rgx_alphaNumString, ui->text_campName));
    ui->text_campLoc->setValidator (new QRegExpValidator(Tools::c_rgx_alphaNumString, ui->text_campLoc));

    ui->text_campPlaceMax->setValidator(new QIntValidator(this));
    ui->text_campNbRefugee->setValidator(new QIntValidator(this));
    ui->text_campPlaceRemaining->setValidator(new QIntValidator(this));

    overviewLoad();
}

void MainWin::campMod(bool)
{
    /*  SLOT
     *      ACTIVATION  : When the "Modify Camp" button is clicked
     *      ACTIONS     : Two states : normal               -> switch to 2nd state
     *                                 modification ongoing -> Check if the new name is valid
     *                                                         If so, change it in the ui and the db
     *                                                         switch to 1st state
    */

    Tools::StringEvalCode validName(Tools::Ok);
    bool placesMaxOk = true;

    /* 2nd state (modification ongoing) */
        if(m_campModOngoing)
        {
            // Check if new name is valid
            validName = Tools::campNameValid(ui->text_campName->text(), *ui->list_camp, m_curCamp);

            // Change it in the ui and the db if so
                if(validName == Tools::Ok)
                {
                    QSqlQuery req_campMod(*m_db->access());

                    req_campMod.prepare("UPDATE Camps "
                                        "   SET name_camp   = :nameCamp , "
                                        "       id_location = (SELECT id_country FROM Country WHERE name_country = :nameCountry), "
                                        "       nb_max      = :nbMax , "
                                        "       id_center   = (SELECT id_center FROM Centers WHERE name_center = :nameCenter) "
                                        " WHERE id_camp     = :id");

                    req_campMod.bindValue(":nameCamp",    ui->text_campName->text());
                    req_campMod.bindValue(":nameCountry", ui->text_campLoc->text());
                    req_campMod.bindValue(":nbMax",       ui->text_campPlaceMax->text());
                    req_campMod.bindValue(":id",          m_campsIdDb[m_curCamp]);

                    if(req_campMod.exec())
                    {
                        qDebug() << "[DEBUG] tab_overview.cpp::campMod() : Camp modifying successful";
                        ui->list_camp->item(m_curCamp)->setText(ui->text_campName->text());
                        loadCampList();
                        m_campModOngoing = true;
                    }
                    else qWarning() << "[WARN ] tab_overview.cpp::campMod() : Camp modifying failed : " << req_campMod.lastError().text();
                }

            // Else, show the corresponding error
                else
                    Tools::dispErr(this, validName);

            /* Check if the what is entered in places max is good */
                placesMaxOk = ui->text_campPlaceMax->text().toInt() >= ui->text_campNbRefugee->text().toInt();
                if(placesMaxOk)
                {
                    // SQL
                    ui->text_campPlaceRemaining->setText(QString::number((ui->text_campPlaceMax->text().toInt() -
                                                                          ui->text_campNbRefugee->text().toInt())));
                }
                else
                {
                    qWarning() << "[WARN ] tab_overview.cpp::campMod() : places max (" << ui->text_campPlaceMax->text().toInt()
                                                                                       << ") < places occupied ("
                                                                                       << ui->text_campNbRefugee->text().toInt()
                                                                                       << ")";
                    QMessageBox::warning(this, tr("Wrong number"), tr("You entered a place number too small"));
                }
        }

    /* 1st and 2nd states */
        if(validName == Tools::Ok && placesMaxOk)
        {
            // Toggle the state and ui
                m_campModOngoing = !m_campModOngoing;
                overview_setCampModOngoing();
        }
}

void MainWin::campModCancel(bool)
{
    /*  SLOT
     *      ACTIVATION  : When the "Cancel Modifications" button is clicked
     *      ACTIONS     : Cancels pending modification (if any)
     *                    Reset the ui and fields to the normal state
     *      REMARKS     : Can be called without a signal
    */

    if(m_campModOngoing)
    {
        m_campModOngoing = false;
        overview_setCampModOngoing();
        overviewLoad(m_db->access(), true);
    }
}

void MainWin::campDel(bool)
{
    bool campEmpty = ui->text_campNbRefugee->text().toInt() == 0; // Si personne n'est dans le camp
    bool success(true);

    qDebug() << "[DEBUG] onglet_overview.cpp::campDel() : inside da function";
    if(!campEmpty)
    {
        QMessageBox::warning(this, tr("Deletion Forbidden"), tr("You cannot delete this camp, there is still refugee(s) in it."));
    }

    else
    {
        QSqlQuery req_delcamp_camp(*m_db->access());
        QSqlQuery req_delcamp_pers(*m_db->access());

        req_delcamp_camp.prepare("DELETE FROM Camps  WHERE id_camp= :id2Del");
        req_delcamp_camp.bindValue(":id2Del", m_campsIdDb[m_curCamp]);

        req_delcamp_pers.prepare("DELETE FROM Refugees  WHERE id_camp= :id2Del");
        req_delcamp_pers.bindValue(":id2Del", m_campsIdDb[m_curCamp]);

        if(req_delcamp_camp.exec())
        {
            if(req_delcamp_pers.exec())
            {
                qDebug() << "[DEBUG] onglet_overview.cpp::campDel() : Deletion successful";
                m_curCamp--;
                loadCampList();
                changeCamp(QModelIndex());
            }
            else success = false;
        }
        else success = false;

        if(!success)
        {
            qWarning() << "[WARN ] onglet_overview.cpp::campDel()) : Deletion failed" << req_delcamp_camp.lastError().text();
            QMessageBox::warning(this, tr("Error on Camp Deletion"), tr("Camp Deletion Failed") + req_delcamp_camp.lastError().text());
        }

        // /!\ Also delete the correpsonding stocks and so
    }
}
void MainWin::overviewLoad(bool reload)
{
    overviewLoad(m_db->access(), reload);
}

void MainWin::overviewLoad(QSqlDatabase* db, bool reload)
{
    if(m_curCamp == c_AllCampIndex)
    {
        QSqlQuery req_summary(*db);
        QSqlQuery req_TotalPlaceMax(*db);
        QSqlQuery req_TotalRefugiee(*db);
        bool ok(true);

        /* Get the camps count*/
            if(req_summary.exec("Select COUNT(*) from Camps"))
            {
                req_summary.next();
                ui->text_campCount->setText(req_summary.value(0).toString());
                //ui->text_campTotalPlaceMax->setText(req_summary.value(0).toString());
            }
            else ok = false;
            if(req_TotalPlaceMax.exec("Select SUM(nb_max) from Camps"))
            {
                req_TotalPlaceMax.next();
                ui->text_campTotalPlaceMax->setText(req_TotalPlaceMax.value(0).toString());
            }
            else qWarning() << "[WARN ] tab_overview.cpp::overviewLoad() Camps summary loading failed : " << req_TotalPlaceMax.lastError().text();

            if(req_TotalRefugiee.exec("SELECT count(*) FROM Refugees"))
            {
                req_TotalRefugiee.next();
               ui->text_campTotalRefugeeCount->setText(req_TotalRefugiee.value(0).toString());
            }
            else qWarning() << "[WARN ] tab_overview.cpp::overviewLoad() Camps summary loading failed : " << req_TotalRefugiee.lastError().text();
            int Remaining = req_TotalPlaceMax.value(0).toInt()- req_TotalRefugiee.value(0).toInt();
            ui->text_campTotalRemainingPlaces->setText(QString::number(Remaining));

        if(!ok)
        {
            qWarning() << "[WARN ] tab_overview.cpp::overviewLoad() Camps summary loading failed : " << req_summary.lastError().text();
            QMessageBox::warning(this, tr("Error loading camps summary"), tr("Failed to load the camps summary : ") + req_summary.lastError().text());
        }
    }

    else
    {
        QSqlQuery req_overview (*db);
        QSqlQuery req_nbRefugee(*db);

        req_overview.prepare("SELECT name_camp, name_country, nb_max, name_center"
                             "  FROM Camps, Country, Centers "
                             " WHERE id_camp           = :id "
                             "   AND Camps.id_location = Country.id_country "
                             "   AND Camps.id_center   = Centers.id_center");

        req_overview.bindValue(":id", m_campsIdDb[m_curCamp]);

        if(req_overview.exec())
        {
            if(req_overview.next())
            {
                ui->text_campName->setText    (req_overview.value(0).toString());
                ui->text_campLoc->setText     (req_overview.value(1).toString());
                ui->text_campPlaceMax->setText(req_overview.value(2).toString());
                ui->text_center->setText      (req_overview.value(3).toString());
            }

            if(!reload)
            {
                req_nbRefugee.prepare("SELECT count(*) FROM Refugees, States WHERE id_camp = :id  AND Refugees.id_state = States.id_state AND name_state <> 'Death' ");
                req_nbRefugee.bindValue(":id", m_campsIdDb[m_curCamp]);

                if(req_nbRefugee.exec())
                {
                     if(req_nbRefugee.next())
                     {
                        int nbRefugee= req_nbRefugee.value(0).toInt();
                        ui->text_campNbRefugee->setText(QString::number(nbRefugee));
                        ui->text_campPlaceRemaining->setText(QString::number(ui->text_campPlaceMax->text().toInt()-nbRefugee));
                     }
                } else qWarning() << "[WARN ] onglet_overview.cpp::overviewLoad()::req_nbRefugee.exec() : " << req_nbRefugee.lastError().text();
            }
        } else qWarning() << "[WARN ] onglet_overview.cpp::overviewLoad()::req_overview.exec() : " << req_overview.lastError().text();
    }
}

void MainWin::overview_setCampModOngoing()
{
    campSetEnabledInput(m_campModOngoing);

    ui->btn_campModCancel->setVisible(m_campModOngoing);
    ui->btn_campMod->setText(m_campModOngoing ? tr("Validate Changes") : tr("Modify Camp"));
}

void MainWin::campSetEnabledInput(bool b)
{
    ui->text_campName->setEnabled(b);
    ui->text_campLoc->setEnabled(b);
    ui->text_campPlaceMax->setEnabled(b);
}

void MainWin::overview_centerLoad(QSqlDatabase* db)
{
    ui->list_centerCamps->clear();

    QSqlQuery req_centerName (*db);
    QSqlQuery req_centerCamps(*db);

    req_centerName.prepare("SELECT name_center FROM Centers WHERE id_center = :id");
    req_centerName.bindValue(":id", m_centerIdDb[m_curCenter]);

    if(req_centerName.exec())
    {
        if(req_centerName.next()) ui->text_centerName->setText(req_centerName.value(0).toString());

        req_centerCamps.prepare("SELECT name_camp FROM Camps WHERE id_center = :id");
        req_centerCamps.bindValue(":id", m_centerIdDb[m_curCenter]);

        if(req_centerCamps.exec())
        {
             while(req_centerCamps.next())
             {
                 ui->list_centerCamps->addItem(req_centerCamps.value(0).toString());
             }
        } else qWarning() << "[WARN ] onglet_overview.cpp::overview_centerLoad()::req_centerCamps.exec() : " << req_centerCamps.lastError().text();
    } else qWarning() << "[WARN ] onglet_overview.cpp::overview_centerLoad()::req_centerName.exec() : " << req_centerName.lastError().text();
}
