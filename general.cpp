#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::changeOnglet(int index)
{
    m_curOnglet = index;
    m_campModAnnuler();
}

void MainWin::changeCamp(QModelIndex index)
{
    if(index.isValid()) m_curCamp = index.row();

    if(m_curCamp == 0)
    {
        if(m_curOnglet == 2 || m_curOnglet == 3) m_curOnglet = 0;
    }

    ui->onglets->setTabEnabled(2, m_curCamp != 0);
    ui->onglets->setTabEnabled(3, m_curCamp != 0);

    ui->groupbox_campTous->setVisible(m_curCamp == 0);
    ui->groupbox_campAutre->setVisible(m_curCamp != 0);

     m_campModAnnuler();
    QSqlQuery requeteVueEns(db);
    QSqlQuery requeteNbPers(db);
    requeteVueEns.prepare("select nom_camp, localisation, nb_max  from Camps where id_camp= :id_courant");
    requeteVueEns.bindValue(":id_courant",m_curCamp);
    requeteNbPers.prepare("select count(*) from Refugie where id_camp = :id_courant");
    requeteNbPers.bindValue(":id_courant",m_curCamp);
    if(requeteVueEns.exec())
    {
        while(requeteVueEns.next())
        {
            ui->text_campNom->setText(requeteVueEns.value(0).toString());
            ui->text_campLoc->setText(requeteVueEns.value(1).toString());
            ui->text_campPlaceMax->setText(requeteVueEns.value(2).toString());
        }
        if(requeteNbPers.exec())
        {
             while(requeteNbPers.next())
             {
                 int nbPers= requeteNbPers.value(0).toInt();
                 ui->text_campNbPers->setText(QString::number(nbPers));
                ui->text_campPlaceRest->setText(QString::number(ui->text_campPlaceMax->text().toInt()-nbPers));
             }
        }
        else
            qDebug() << "erreur: "<< requeteNbPers.lastError();

    }
    else
        qDebug() << "erreur: "<< requeteVueEns.lastError();

}

void MainWin::changeCampRech(QModelIndex index)
{
    m_curCamp = ui->liste_campRech->item(index.row())->text().split(" : ").at(0).toInt();

    ui->liste_camp->setCurrentRow(m_curCamp);
    changeCamp(QModelIndex()); // Appel à change champ avec un objet vide (vérifié là bas, c'est juste pour éviter les erreurs)
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
        ui->text_campPlaceMax->setText("");
        ui->text_campPlaceRest->setText("");
        ui->text_campNbPers->setText("");

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
            // En attendant : Remplis par une chaine vide

        /* Onglet vue d'ensemble */
            ui->text_campTotal->setText("");
            ui->text_campTotalRefugies->setText("");
            ui->text_campTotalPlaceMax->setText("");
            ui->text_campTotalPlaceRest->setText("");
    }
}

void MainWin::campAjouter(bool)
{
    bool ok, nomPris(false);
    QString ans;

    do
    {
        ans = QInputDialog::getText(this, "Nouveau Camp", "Nom du nouveau camp : ", QLineEdit::Normal, QString(), &ok);

        if(ok && !ans.isEmpty())
        {
            // On vérifie qu'il n'y a pas déjà de camp portant ce nom
                for(quint16 i= 0; i< ui->liste_camp->count() && !nomPris; i++)
                    if(ans.compare(ui->liste_camp->item(i)->text(), Qt::CaseInsensitive) == 0) nomPris = true;

           if(nomPris)
               QMessageBox::warning(this, "Nom déjà pris", "Le nom de camp rentré est déjà pris, re-rentrez en un.");
        }
        else
        {
            if(ok) QMessageBox::warning(this, "Erreur avec le nom", "Erreur lors de la récuperation du non du camp");
        }
    }
    while(ans.isEmpty() && nomPris);

    if(ok && !ans.isEmpty() && !nomPris)
    {
        // ToDo : Ajouter dans la BdD

        ui->liste_camp->addItem(ans);
        m_campsIdBdD.push_back(0 /* Id Sql*/);

        // ToDo : Initialiser champs du nouveau camp
    }
}

void MainWin::campRecherche(QString s)
{
    if(s.isEmpty()) ui->liste_campRech->setVisible(false);
    else
    {
        ui->liste_campRech->clear();
        for(quint16 i= 1; i< ui->liste_camp->count(); i++)
        {
            if(ui->liste_camp->item(i)->text().startsWith(s, Qt::CaseInsensitive))
            {
                ui->liste_campRech->addItem(QString::number(i) + " : " + ui->liste_camp->item(i)->text());
            }
        }

        ui->liste_campRech->setVisible(true);
    }
}
