#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent), m_curCamp(0), m_curOnglet(0), m_campModEnCours(false),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);
    ui->onglets->setCurrentIndex(0);

    /* Connexion à la BdD */

    /* Liaison des evenements */
        // Géneraux
            QObject::connect(ui->onglets,    SIGNAL(currentChanged(int)),  this, SLOT(changeOnglet(int)));
            QObject::connect(ui->liste_camp, SIGNAL(clicked(QModelIndex)), this, SLOT(changeCamp(QModelIndex)));

       // Onglet vue d'ensemble
            QObject::connect(ui->btn_campMod, SIGNAL(clicked(bool)), this, SLOT(m_campMod(bool)));
}

MainWin::~MainWin()
{
    delete ui;
}

void MainWin::changeOnglet(int index)
{
    m_curOnglet = index;
}

void MainWin::changeCamp(QModelIndex index)
{
    m_curCamp = index.row();

    if(m_curCamp == 0)  // Camp principal et permanent
    {
        if(m_curOnglet == 2 || m_curOnglet == 3) m_curOnglet = 0;
        ui->onglets->setTabEnabled(2, false);
        ui->onglets->setTabEnabled(3, false);
    }

    else
    {
        ui->onglets->setTabEnabled(2, true);
        ui->onglets->setTabEnabled(3, true);
    }
}

void MainWin::m_campMod(bool b)
{
    m_campModEnCours = !m_campModEnCours;

    ui->text_campNom->setEnabled(m_campModEnCours);
    ui->text_campLoc->setEnabled(m_campModEnCours);
    ui->text_campNbPers->setEnabled(m_campModEnCours);
    ui->text_campPlaceMax->setEnabled(m_campModEnCours);
    ui->text_campPlaceRest->setEnabled(m_campModEnCours);

    ui->btn_campMod->setText(m_campModEnCours ? "Valider Modifications" : "Modifier Camp");
}
