#include "mainwin.h"
#include "ui_mainwin.h"

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
            validName = Tools::campNameValid(ui->text_campName->text(), *ui->list_camp, Tools::c_regex_campName, m_curCamp);

            // Change it in the ui and the db if so
                if(validName == Tools::Ok)
                {
                    QSqlQuery req_campMod(*m_db->access());

                    // ToDo : Update only differents fields

                    req_campMod.prepare("UPDATE Camps SET nom_camp = :nom_camp, localisation = :localisation, nb_max = :nb_max WHERE id_camp = :id");

                    req_campMod.bindValue(":nom_camp",     ui->text_campName->text());
                    req_campMod.bindValue(":localisation", ui->text_campLoc->text());
                    req_campMod.bindValue(":nb_max",       ui->text_campPlaceMax->text());
                    req_campMod.bindValue(":id",           m_campsIdDb[m_curCamp]);

                    if(req_campMod.exec())
                    {
                        qDebug()    << "[DEBUG] Camp modifying: successful";
                        ui->list_camp->item(m_curCamp)->setText(ui->text_campName->text());
                    }
                    else qCritical() << "[ERROR] Camp modifying: Updating infos (" << req_campMod.lastError() << ")";
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
                    qDebug() << "[WARN ] tab_overview.cpp::campMod() : places max (" << ui->text_campPlaceMax->text().toInt()
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
                campSetEnabledInput(m_campModOngoing);

                ui->btn_campModCancel->setVisible(m_campModOngoing);
                ui->btn_campMod->setText(m_campModOngoing ? tr("Validate Changes") : tr("Modify Camp"));
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
        campSetEnabledInput(false);
        ui->btn_campModCancel->setVisible(false);
        ui->btn_campMod->setText(tr("Modify Camp"));

        overviewLoad(m_db->access(), true);
    }
}

void MainWin::campSetEnabledInput(bool b)
{
    ui->text_campName->setEnabled(b);
    ui->text_campLoc->setEnabled(b);
    ui->text_campPlaceMax->setEnabled(b);
}

void MainWin::campDel(bool)
{
    bool campEmpty = false; // Si personne n'est dans le camp (Camp.nbPersonne == 0)
    bool success(true);

    if( campEmpty ||
        QMessageBox::question(this, tr("Delete ?"), tr("Are you sure you want to delete the camp ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes
      )
    {
        QSqlQuery req_delcamp_camp(*m_db->access());
        QSqlQuery req_delcamp_pers(*m_db->access());

        req_delcamp_camp.prepare("DELETE FROM Camps  WHERE id_camp= :id2Del");
        req_delcamp_camp.bindValue(":id2Del", m_campsIdDb[m_curCamp]);

        req_delcamp_pers.prepare("DELETE FROM Refugie  WHERE id_camp= :id2Del");
        req_delcamp_pers.bindValue(":id2Del", m_campsIdDb[m_curCamp]);

        if(req_delcamp_camp.exec())
        {
            if(req_delcamp_pers.exec())
            {
                qDebug() << "[DEBUG] onglet_overview.cpp::campDel() : Deletion successful";
                loadCampList();
                m_curCamp--;
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

void MainWin::overviewLoad(QSqlDatabase* db_, bool reload)
{
    QSqlQuery req_overview (*db_);
    QSqlQuery req_nbRefugee(*db_);

    req_overview.prepare("SELECT nom_camp, localisation, nb_max  FROM Camps WHERE id_camp= :id_courant");
    req_overview.bindValue(":id_courant", m_campsIdDb[m_curCamp]);

    if(req_overview.exec())
    {
        if(req_overview.next())
        {
            ui->text_campName->setText    (req_overview.value(0).toString());
            ui->text_campLoc->setText     (req_overview.value(1).toString());
            ui->text_campPlaceMax->setText(req_overview.value(2).toString());
        }

        if(!reload)
        {
            req_nbRefugee.prepare("SELECT count(*) FROM Refugie WHERE id_camp = :id_courant");
            req_nbRefugee.bindValue(":id_courant", m_campsIdDb[m_curCamp]);

            if(req_nbRefugee.exec())
            {
                 if(req_nbRefugee.next())
                 {
                    int nbRefugee= req_nbRefugee.value(0).toInt();
                    ui->text_campNbRefugee->setText(QString::number(nbRefugee));
                    ui->text_campPlaceRemaining->setText(QString::number(ui->text_campPlaceMax->text().toInt()-nbRefugee));
                 }
            } else qCritical() << "[ERROR] onglet_overview.cpp::overviewLoad()::req_nbRefugee.exec() : " << req_nbRefugee.lastError();
        }
    } else qCritical() << "[ERROR] onglet_overview.cpp::overviewLoad()::req_overview.exec() : " << req_overview.lastError();
}
