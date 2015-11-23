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

    campChargement(m_curCamp);
    if(m_curCamp == 0)
    {
        if(m_curOnglet == 2 || m_curOnglet == 3) m_curOnglet = 0;
        /* SQL */
    }

    else
    {
        m_vueensLoad();
    }

    // Désactivation des onglets "Gestion Humaine" et "Stock" quand "Tous" est séléctionné
        ui->onglets->setTabEnabled(2, m_curCamp != 0);
        ui->onglets->setTabEnabled(3, m_curCamp != 0);

    // Changement d'interface -> interface différente quand "Tous" est séléctionné
        ui->groupbox_campTous->setVisible(m_curCamp == 0);
        ui->groupbox_campAutre->setVisible(m_curCamp != 0);

     // Annulation des changements sur les camps s'il y en a
         m_campModAnnuler();
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
    bool ok;
    quint16 nomValide;
    QString ans;

    do
    {
        ans = QInputDialog::getText(this, "Nouveau Camp", "Nom du nouveau camp : ", QLineEdit::Normal, QString(), &ok);

        if(ok)
        {
            nomValide = m_nomCampValide(ans);

                 if(nomValide == 1) QMessageBox::warning(this, "Nom Vide", "Le nom de camp rentré est vide, re-rentrez en un.");
            else if(nomValide == 2) QMessageBox::warning(this, "Nom Trop Long", "Le nom de camp rentré fait plus de 50 caractères, re-rentrez en un.");
            else if(nomValide == 3) QMessageBox::warning(this, "Nom Mal Formaté", "Le nom de camp rentré à un mauvais format (^[a-z](\\w|-)*$), re-rentrez en un.");
            else if(nomValide == 4) QMessageBox::warning(this, "Nom déjà pris", "Le nom de camp rentré est déjà pris, re-rentrez en un.");
        }
    }
    while(ok && nomValide != 0);

    if(ok)
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

QSqlDatabase* MainWin::db()
{
    if(m_db == NULL)
    {
        QSqlDatabase tmpdb = QSqlDatabase::addDatabase("QMYSQL");
        m_db = new QSqlDatabase(tmpdb);

        /* Config IUT */
            //m_db->setHostName("127.0.0.1");
            //m_db->setPort(5555);

        /* Config NORMALE */
            m_db->setHostName("joretapo.fr");
            m_db->setPort(3306);

        m_db->setUserName("root");
        m_db->setPassword("toor");
        m_db->setDatabaseName("humanitaire");
        qDebug() << "Connexion en cours";

        if(m_db->open()) qDebug() << "Connexion réussie";
        else
        {
            qDebug() << "Connexion échouée" ;
            QMessageBox::critical(this, "Connexion à la BdD", "Erreur de connexion à la base de données :\n" +
                                                              m_db->lastError().text());
        }
    }

    return m_db;
}

/*
 * Valeurs de Retour :
 *  0 -> Ok
 *  1 -> Chaine Vide
 *  2 -> Nom Trop Long (> 50)
 *  3 -> Mauvais Format
 *  4 -> Nom Déjà Pris
*/

quint16 MainWin::m_nomCampValide(const QString& s)
{
    if(s.isEmpty()) return 1;
    if(s.length() >= 50) return 2;

    if(!QRegExp("^[a-z](\\w|-)*$", Qt::CaseInsensitive).exactMatch(s)) return 3;

    for(quint16 i= 0; i< ui->liste_camp->count(); i++)
        if(s.compare(ui->liste_camp->item(i)->text(), Qt::CaseInsensitive) == 0 && i != m_curCamp) return 4;

    return 0;
}
