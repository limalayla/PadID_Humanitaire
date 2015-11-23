#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::m_campMod(bool)
{
    quint16 nomValide(0);

    if(m_campModEnCours)
    {
        nomValide = m_nomCampValide(ui->text_campNom->text());
        if(nomValide == 0)
        {
            QSqlQuery req_modifCamps(*db());

            // ToDo : Update uniquement sur les champs différents

            req_modifCamps.prepare("UPDATE Camps SET nom_camp = :nom_camp, localisation = :localisation, nb_max = :nb_max WHERE id_camp = :id");

            req_modifCamps.bindValue(":nom_camp",     ui->text_campNom->text());
            req_modifCamps.bindValue(":localisation", ui->text_campLoc->text());
            req_modifCamps.bindValue(":nb_max",       ui->text_campPlaceMax->text());
            req_modifCamps.bindValue(":id",           m_campsIdBdD[m_curCamp]);

            if(req_modifCamps.exec())
            {
                qDebug()    << "[DEBUG] Camp modifying: successful";
                ui->liste_camp->item(m_curCamp)->setText(ui->text_campNom->text());
            }
            else qCritical() << "[ERROR] Camp modifying: Updating infos (" << req_modifCamps.lastError() << ")";
        }

        else
        {
                 if(nomValide == 1) QMessageBox::warning(this, "Nom Vide", "Le nom de camp rentré est vide, re-rentrez en un.");
            else if(nomValide == 2) QMessageBox::warning(this, "Nom Trop Long", "Le nom de camp rentré fait plus de 50 caractères, re-rentrez en un.");
            else if(nomValide == 3) QMessageBox::warning(this, "Nom Mal Formaté", "Le nom de camp rentré à un mauvais format (^[a-z](\\w|-)*$), re-rentrez en un.");
            else if(nomValide == 4) QMessageBox::warning(this, "Nom déjà pris", "Le nom de camp rentré est déjà pris, re-rentrez en un.");
        }
    }

    if(nomValide == 0)
    {
        m_campModEnCours = !m_campModEnCours;
        m_campSetEnabledInput(m_campModEnCours);

        ui->btn_campModAnnuler->setVisible(m_campModEnCours);
        ui->btn_campMod->setText(m_campModEnCours ? "Valider Modifications" : "Modifier Camp");
    }
}

void MainWin::m_campModAnnuler(bool)
{
    if(m_campModEnCours)
    {
        m_campModEnCours = false;
        m_campSetEnabledInput(false);
        ui->btn_campModAnnuler->setVisible(false);
        ui->btn_campMod->setText("Modifier Camp");

        m_vueensLoad(true);
    }
}

void MainWin::m_campSetEnabledInput(bool b)
{
    ui->text_campNom->setEnabled(b);
    ui->text_campLoc->setEnabled(b);
}

void MainWin::m_campSuppr(bool)
{
    bool campVide = false; // Si personne n'est dans le camp (Camp.nbPersonne == 0)

    if( campVide ||
        QMessageBox::question(this, "Supprimer ?", "Êtes vous sur de vouloir tuer les restant ?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes
      )
    {
        // ToDo : Récuperer l'id dans la bdd correspondant au camp à supprimer
        // Supprimer son entrée dans le tableau de camps avec l'indice m_curCamp -1
        // Requête pour le supprimer
            // /!\ Supprimer aussi les personnes et stocks correspondants

        QMessageBox::information(this, "", "Supression de " + QString::number(m_curCamp));
    }
}

void MainWin::m_vueensLoad(bool reload)
{
    QSqlQuery requeteVueEns(*db());
    QSqlQuery requeteNbPers(*db());

    requeteVueEns.prepare("SELECT nom_camp, localisation, nb_max  FROM Camps WHERE id_camp= :id_courant");
    requeteVueEns.bindValue(":id_courant", m_campsIdBdD[m_curCamp]);

    if(requeteVueEns.exec())
    {
        if(requeteVueEns.next())
        {
            ui->text_campNom->setText(requeteVueEns.value(0).toString());
            ui->text_campLoc->setText(requeteVueEns.value(1).toString());
            ui->text_campPlaceMax->setText(requeteVueEns.value(2).toString());
        }

        if(!reload)
        {
            requeteNbPers.prepare("SELECT count(*) FROM Refugie WHERE id_camp = :id_courant");
            requeteNbPers.bindValue(":id_courant", m_campsIdBdD[m_curCamp]);

            if(requeteNbPers.exec())
            {
                 if(requeteNbPers.next())
                 {
                    int nbPers= requeteNbPers.value(0).toInt();
                    ui->text_campNbPers->setText(QString::number(nbPers));
                    ui->text_campPlaceRest->setText(QString::number(ui->text_campPlaceMax->text().toInt()-nbPers));
                 }
            } else qCritical() << "[ERROR] onglet_vueensemble.cpp::vueensLoad()::requeteNbPers.exec() : " << requeteNbPers.lastError();
        }
    } else qCritical() << "[ERROR] onglet_vueensemble.cpp::vueensLoad()::requeteVueEns.exec() : " << requeteVueEns.lastError();
}
