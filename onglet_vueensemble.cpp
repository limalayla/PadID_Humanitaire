#include "mainwin.h"
#include "ui_mainwin.h"

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

void MainWin::chargementCamp(quint16 index)
{

}
