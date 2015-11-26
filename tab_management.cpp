#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::refugeeAdd(bool)
{
    openRefugeeInfo(RefugeeInfoWin::creation);
}

void MainWin::refugeeMod(bool)
{
    openRefugeeInfo(RefugeeInfoWin::readWrite);
}

void MainWin::refugeeDel(bool)
{
    if(QMessageBox::question(this, tr("Are you sure ?"), tr("Delete this refugee ?")) == QMessageBox::Yes)
    {
        /* SQL */
    }
}

void MainWin::openRefugeeInfo(RefugeeInfoWin::OpenMode openMode)
{
    if(m_refugeeInfoWin == NULL)
    {
        m_refugeeInfoWin = new RefugeeInfoWin(this, openMode);
    }

    if(m_refugeeInfoWin->isVisible())
    {
        QMessageBox::critical(this, "Refugee Window Already Open", "The Refugee Window is already open !");
    }

    else
    {
        m_refugeeInfoWin->show();
    }
}

void MainWin::closeRefugeeInfo()
{
    if(m_refugeeInfoWin != NULL)
    {
        delete m_refugeeInfoWin;
        m_refugeeInfoWin = NULL;
    }
}
