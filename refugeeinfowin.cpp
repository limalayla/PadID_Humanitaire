#include "refugeeinfowin.h"
#include "ui_refugeeinfowin.h"

RefugeeInfoWin::RefugeeInfoWin(QSqlDatabase* db_,QWidget *parent,int idDb, RefugeeInfoWin::OpenMode openMode) :
    QDialog(parent), ui(new Ui::RefugeeInfoWin),
    m_openMode(openMode)
{
    ui->setupUi(this);
    if(idDb==-1)
    {

    }
    idDb=2;
    if(m_openMode == RefugeeInfoWin::readWrite || m_openMode == RefugeeInfoWin::creation)
    {
        ui->text_fname->setEnabled(true);
        ui->text_lname->setEnabled(true);
        ui->combo_state->setEnabled(true);
        ui->text_misc->setEnabled(true);
        ui->combo_curCamp->setEnabled(true);
    }

    QSqlQuery req_refudgeeinfo(*db_);
    req_refudgeeinfo.prepare("Select nom, prenom, age, sexe,pays_dorigine,type,etat,divers from Refugie where id_refugie=:idDb");
    req_refudgeeinfo.bindValue(":idDb",idDb);
    if(req_refudgeeinfo.exec())
    {
        if(req_refudgeeinfo.next())
        {
            ui->text_lname->setText(req_refudgeeinfo.value(0).toString());
            ui->text_fname->setText(req_refudgeeinfo.value(1).toString());
            ui->combo_age->addItem(req_refudgeeinfo.value(2).toString());
            ui->combo_sex->addItem(req_refudgeeinfo.value(3).toString());
            ui->combo_country->addItem(req_refudgeeinfo.value(4).toString());
            ui->combo_type->addItem(req_refudgeeinfo.value(5).toString());
            ui->combo_state->addItem(req_refudgeeinfo.value(6).toString());
            ui->text_misc->setPlainText(req_refudgeeinfo.value(7).toString());
        }

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
