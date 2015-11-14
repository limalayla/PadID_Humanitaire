#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent), m_curCamp(0), m_curOnglet(0), m_campModEnCours(false),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);

    /* Connexion à la BdD */


    /* Préparation de l'interface */
        /* Géneral */
            ui->onglets->setCurrentIndex(0);
            ui->onglets->setTabEnabled(2, false);
            ui->onglets->setTabEnabled(3, false);

            // Personnalisation de l'item "Tous" de liste_camp
                ui->liste_camp->item(0)->setFont(QFont("Arial", 11));
                ui->liste_camp->item(0)->setTextAlignment(Qt::AlignHCenter);

            // ToDo : Récuperer dans BdD liste camps et les mettres dans ui->liste_camp

        /* Onglet recherche */
            for(quint8 i= 1; i<= 100; i++) ui->combo_rechAge->addItem(QString::number(i));
            // ToDo : Récuperer dans une table les pays d'origine et les mettres dans ui->combo_rechPaysOrigine

    /* Liaison des evenements */
        // Géneral
            QObject::connect(ui->onglets,    SIGNAL(currentChanged(int)),  this, SLOT(changeOnglet(int)));
            QObject::connect(ui->liste_camp, SIGNAL(clicked(QModelIndex)), this, SLOT(changeCamp(QModelIndex)));

       // Onglet vue d'ensemble
            QObject::connect(ui->btn_campMod, SIGNAL(clicked(bool)), this, SLOT(m_campMod(bool)));
}

MainWin::~MainWin()
{
    delete ui;
}
