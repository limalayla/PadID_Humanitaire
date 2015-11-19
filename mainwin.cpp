#include "mainwin.h"
#include "ui_mainwin.h"


MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWin),
    m_curCamp(0), m_curOnglet(0), m_campModEnCours(false)

{
    ui->setupUi(this);

    /* Connexion à la BdD */
        db = QSqlDatabase::addDatabase("QMYSQL");
        //db.setHostName("127.0.0.1");      //CONFIG IUT
        // db.setPort(5555);          //CONFIG IUT
        db.setHostName("joretapo.fr");  //CONFIG NORMALE
        db.setPort(3306);        //CONFIG NORMALE
        db.setUserName("root");
        db.setPassword("toor");
        db.setDatabaseName("humanitaire");
        qDebug() << "connexion en cours";
            if(db.open())
                qDebug() << "connexion reussi";
            else
                qDebug() << "La connexion a échouée, désolé" ;


    /* Préparation de l'interface */
        /* Géneral */
            ui->onglets->setCurrentIndex(0);
            ui->onglets->setTabEnabled(2, false);
            ui->onglets->setTabEnabled(3, false);
            ui->liste_campRech->setVisible(false);

            // Personnalisation de l'item "Tous" de liste_camp
                ui->liste_camp->item(0)->setFont(QFont("Arial", 11));
                ui->liste_camp->item(0)->setTextAlignment(Qt::AlignHCenter);

            //Récupere dans BdD liste camps et les mettres dans ui->liste_camp et m_campsIdBdD
                        QSqlQuery requeteListeCamps(db);
                        if(requeteListeCamps.exec("select id_camp,nom_camp from Camps"))
                        {
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
            QSqlQuery requeteListePays(db);
            if(requeteListePays.exec("select nom_pays from Pays"))
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
}

void MainWin::initEvenement()
{
    /* Géneral */
        QObject::connect(ui->btn_campAjout,  SIGNAL(clicked(bool)),        this, SLOT(campAjouter(bool)));
        QObject::connect(ui->onglets,        SIGNAL(currentChanged(int)),  this, SLOT(changeOnglet(int)));
        QObject::connect(ui->liste_camp,     SIGNAL(clicked(QModelIndex)), this, SLOT(changeCamp(QModelIndex)));
        QObject::connect(ui->text_rechCamp,  SIGNAL(textChanged(QString)), this, SLOT(campRecherche(QString)));
        QObject::connect(ui->liste_campRech, SIGNAL(clicked(QModelIndex)), this, SLOT(changeCampRech(QModelIndex)));

   /* Onglet vue d'ensemble */
        QObject::connect(ui->btn_campMod,        SIGNAL(clicked(bool)), this, SLOT(m_campMod(bool)));
        QObject::connect(ui->btn_campModAnnuler, SIGNAL(clicked(bool)), this, SLOT(m_campModAnnuler(bool)));
        QObject::connect(ui->btn_campSuppr,      SIGNAL(clicked(bool)), this, SLOT(m_campSuppr(bool)));
}
