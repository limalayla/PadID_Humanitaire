#include "mainwin.h"
#include "ui_mainwin.h"

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
