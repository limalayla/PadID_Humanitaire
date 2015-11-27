#include "refugeeinfowin.h"
#include "ui_refugeeinfowin.h"

RefugeeInfoWin::RefugeeInfoWin(QSqlDatabase* db_, QWidget *parent, int idDb, OpenMode openMode) :
    QDialog(parent), ui(new Ui::RefugeeInfoWin),
    m_openMode(openMode)
{
    ui->setupUi(this);

    ui->text_fname->setText(QString::number(idDb));

    if(m_openMode == RefugeeInfoWin::readWrite || m_openMode == RefugeeInfoWin::creation)
    {
        ui->text_fname->setEnabled(true);
        ui->text_lname->setEnabled(true);
        ui->combo_state->setEnabled(true);
        ui->text_misc->setEnabled(true);
        ui->combo_curCamp->setEnabled(true);
    }

    else
    {
        ui->btn_ok->setVisible(false);
        ui->btn_cancel->setVisible(false);
    }

    if(m_openMode == RefugeeInfoWin::creation)
    {
        ui->combo_age->setEnabled(true);
        ui->combo_sex->setEnabled(true);
        ui->combo_country->setEnabled(true);
        ui->combo_type->setEnabled(true);
    }

    QObject::connect(this ,          SIGNAL(accepted()),    parent, SLOT(closeRefugeeInfo()));
    QObject::connect(this ,          SIGNAL(rejected()),    parent, SLOT(closeRefugeeInfo()));
    QObject::connect(ui->btn_ok ,    SIGNAL(clicked(bool)), parent, SLOT(closeRefugeeInfo()));
    QObject::connect(ui->btn_cancel, SIGNAL(clicked(bool)), parent, SLOT(closeRefugeeInfo()));
}

RefugeeInfoWin::~RefugeeInfoWin()
{
    delete ui;
}
