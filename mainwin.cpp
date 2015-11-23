#include "mainwin.h"
#include "ui_mainwin.h"


MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWin), m_db(NULL),
    m_curCamp(0), m_curOnglet(0), m_campModEnCours(false)
{
    ui->setupUi(this);

    /* Connexion à la BdD */
        db();

    /* Préparation de l'interface */
        /* Géneral */
            ui->onglets->setCurrentIndex(0);
            ui->onglets->setTabEnabled(2, false);
            ui->onglets->setTabEnabled(3, false);
            ui->liste_campRech->setVisible(false);
            ui->btn_campModAnnuler->setVisible(false);

            // Personnalisation de l'item "Tous" de liste_camp
                ui->liste_camp->item(0)->setFont(QFont("Arial", 11));
                ui->liste_camp->item(0)->setTextAlignment(Qt::AlignHCenter);

            //Récupere dans BdD liste camps et les mettres dans ui->liste_camp et m_campsIdBdD
                QSqlQuery requeteListeCamps(*db());
                if(requeteListeCamps.exec("SELECT id_camp, nom_camp FROM Camps"))
                {
                    // On push une première fois pour le camp spécial "Tous"
                    m_campsIdBdD.push_back(-1);
                    while(requeteListeCamps.next())
                    {
                        QVariant resultatNomCamps = requeteListeCamps.value(1);
                        QVariant resultatIdCamps = requeteListeCamps.value(0);
                        ui->liste_camp->addItem(resultatNomCamps.toString());
                        m_campsIdBdD.push_back(resultatIdCamps.toInt());
                    }
                }

        /* Onglet vue d'ensemble */
            ui->groupbox_campAutre->setVisible(false);

        /* Onglet recherche */
            for(quint8 i= 1; i<= 100; i++) ui->combo_rechAge->addItem(QString::number(i));

            //Récupere dans une table les pays d'origine et les mettres dans ui->combo_rechPaysOrigine
            QSqlQuery requeteListePays(*db());
            if(requeteListePays.exec("SELECT nom_pays FROM Pays"))
            {
                while(requeteListePays.next())
                {
                    QVariant resultatNomPays = requeteListePays.value(0);
                    ui->combo_rechPaysOrigine->addItem(resultatNomPays.toString());
                }
            }
    /* Liaison des evenements */
        initEvenement();
}

MainWin::~MainWin()
{
    delete ui;
    if(m_db != NULL) delete m_db;
}

void MainWin::initEvenement()
{
    /* Géneral */
        QObject::connect(ui->btn_campAjout,     SIGNAL(clicked(bool)),        this, SLOT(campAjouter(bool)));
        QObject::connect(ui->onglets,           SIGNAL(currentChanged(int)),  this, SLOT(changeOnglet(int)));
        QObject::connect(ui->liste_camp,        SIGNAL(clicked(QModelIndex)), this, SLOT(changeCamp(QModelIndex)));
        QObject::connect(ui->text_rechCamp,     SIGNAL(textChanged(QString)), this, SLOT(campRecherche(QString)));
        QObject::connect(ui->liste_campRech,    SIGNAL(clicked(QModelIndex)), this, SLOT(changeCampRech(QModelIndex)));


   /* Onglet vue d'ensemble */
        QObject::connect(ui->btn_campMod,        SIGNAL(clicked(bool)), this, SLOT(m_campMod(bool)));
        QObject::connect(ui->btn_campModAnnuler, SIGNAL(clicked(bool)), this, SLOT(m_campModAnnuler(bool)));
        QObject::connect(ui->btn_campSuppr,      SIGNAL(clicked(bool)), this, SLOT(m_campSuppr(bool)));

}
