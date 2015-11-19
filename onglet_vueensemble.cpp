#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::m_campMod(bool)
{
    if(m_campModEnCours)
    {
        QSqlQuery req_modifCamps(db);

        // ToDo : Update uniquement sur les champs différents

        req_modifCamps.prepare("UPDATE Camps SET nom_camp = :nom_camp, localisation = :localisation, nb_max = :nb_max WHERE id_camp = :id");

        req_modifCamps.bindValue(":nom_camp",     ui->text_campNom->text());
        req_modifCamps.bindValue(":localisation", ui->text_campLoc->text());
        req_modifCamps.bindValue(":nb_max",       ui->text_campPlaceMax->text());
        req_modifCamps.bindValue(":id",           m_curCamp);

        if(req_modifCamps.exec())
        {
            qDebug() << "[DEBUG] Modification de camp réussie";
        }
        else qDebug() << "[ERROR] Modification camps: Insertion valeurs (" << req_modifCamps.lastError() << ")";
    }

    m_campModEnCours = !m_campModEnCours;
    m_campSetEnabledInput(m_campModEnCours);

    ui->btn_campModAnnuler->setVisible(m_campModEnCours);
    ui->btn_campMod->setText(m_campModEnCours ? "Valider Modifications" : "Modifier Camp");
}

void MainWin::m_campModAnnuler(bool)
{
    if(m_campModEnCours)
    {
        m_campModEnCours = false;
        m_campSetEnabledInput(false);
        ui->btn_campModAnnuler->setVisible(false);
        ui->btn_campMod->setText("Modifier Camp");
        // ToDo : Remplacer dans les QLineEdit la valeur dans la BdD correspondante pour écraser les trucs modifiés
        // En attendant : on vide juste les inputs
            ui->text_campNom->setText("");
            ui->text_campLoc->setText("");
            ui->text_campNbPers->setText("");
            ui->text_campPlaceMax->setText("");
            ui->text_campPlaceRest->setText("");
    }
}

void MainWin::m_campSetEnabledInput(bool b)
{
    ui->text_campNom->setEnabled(b);
    ui->text_campLoc->setEnabled(b);
    ui->text_campNbPers->setEnabled(b);
    ui->text_campPlaceMax->setEnabled(b);
    ui->text_campPlaceRest->setEnabled(b);
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
