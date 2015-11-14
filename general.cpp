#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::changeOnglet(int index)
{
    m_curOnglet = index;
    m_campModAnnuler();
}

void MainWin::changeCamp(QModelIndex index)
{
    m_curCamp = index.row();

    if(m_curCamp == 0)
    {
        if(m_curOnglet == 2 || m_curOnglet == 3) m_curOnglet = 0;
    }

    ui->onglets->setTabEnabled(2, m_curCamp != 0);
    ui->onglets->setTabEnabled(3, m_curCamp != 0);

    ui->btn_campMod->setVisible(m_curCamp != 0);
    ui->btn_campSuppr->setVisible(m_curCamp != 0);

    m_campModAnnuler();
}

void MainWin::campChargement(quint16)
{
    if(m_curCamp != 0)
    {
        // ToDo : Récuperer l'id dans la bdd correspondant au camp à charger
        // Todo : remplir tout les champs suivant
            // En attendant : Remplis par une chaine vide

        /* Onglet vue d'ensemble */
            ui->text_campNom->setText("");
            ui->text_campLoc->setText("");
            ui->text_campNbPers->setText("");
            ui->text_campPlaceMax->setText("");
            ui->text_campPlaceRest->setText("");

        /* Onglet stocks */
            // Stocks actuels
            ui->text_actEau->setText("");
            ui->text_actNour->setText("");
            ui->text_actFour->setText("");
            ui->text_actMed->setText("");

            // Stocks disponibles (reserve)
            ui->text_dispoEau->setText("");
            ui->text_dispoNour->setText("");
            ui->text_dispoFour->setText("");
            ui->text_dispoMed->setText("");

            // Stocks actuels
            ui->text_requEau->setText("");
            ui->text_requNour->setText("");
            ui->text_requFour->setText("");
            ui->text_requMed->setText("");
    }

    else
    {
        // ToDo : Récuperer l'ensemble des infos necessaires
        // Todo : remplir tout les champs suivant
            // En attendant : Remplis par une chaine vide (quand les champs seront crées)

        /* Onglet vue d'ensemble */
            // Total camps
            // Total personnes
            // Total places max
            // Total places restantes
    }
}

void MainWin::campAjouter(bool)
{
    // Ajout dans la BdD
    // Ajout dans le tableau m_campsIdBdD
    // Initialiser champs du nouveau camp
}

void MainWin::campRecherche(QString s)
{
    if(s.isEmpty()) ui->liste_campRech->setVisible(false);
    else
    {
        //ui->liste_campRech->clear();
        foreach(QListWidgetItem* item, ui->liste_camp->findItems(s, Qt::MatchStartsWith))
        {
            QMessageBox::information(this, "", item->text());
            ui->liste_campRech->addItem(item->text());
        }

        ui->liste_campRech->setVisible(true);
    }
}
