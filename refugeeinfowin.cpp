#include "refugeeinfowin.h"
#include "ui_refugeeinfowin.h"

RefugeeInfoWin::RefugeeInfoWin(QSqlDatabase* db_, QWidget *parent, int idDb, OpenMode openMode) :
    QDialog(parent), ui(new Ui::RefugeeInfoWin),
    m_openMode(openMode)
{
    ui->setupUi(this);
	
    QSqlQuery req_refugeeinfo(*db_);
    QSqlQuery req_allcamp(*db_);
    QSqlQuery req_allSexe(*db_);
    QSqlQuery req_allCountry(*db_);
    QSqlQuery req_allType(*db_);
    QSqlQuery req_allState(*db_);


    req_allSexe.prepare("Select Distinct sexe from Refugie ");
    if(req_allSexe.exec())
    {
        while(req_allSexe.next())
        {
            ui->combo_sex->addItem(req_allSexe.value(0).toString());
        }
    }

    req_allcamp.prepare("SELECT Distinct nom_camp FROM Camps");
    if(req_allcamp.exec())
    {
        while(req_allcamp.next())
        {
            ui->combo_curCamp->addItem(req_allcamp.value(0).toString());
        }
    }

    req_allCountry.prepare("Select Distinct pays_dorigine from Refugie");
    if(req_allCountry.exec())
    {
        while(req_allCountry.next())
            ui->combo_homeland->addItem(req_allCountry.value(0).toString());
    }

    req_allType.prepare("Select Distinct type from Refugie");
    if(req_allType.exec())
    {
        while(req_allType.next())
            ui->combo_type->addItem(req_allType.value(0).toString());
    }

    for(int i=1;i<=100;i++)
        ui->combo_age->addItem(QString::number(i));

    req_allState.prepare("Select Distinct etat from Refugie");
    if(req_allState.exec())
    {
        while(req_allState.next())
            ui->combo_state->addItem(req_allState.value(0).toString());
    }

    req_refugeeinfo.prepare("Select nom, prenom,age,sexe,pays_dorigine,type,etat,divers,id_camp from Refugie where id_refugie=:idDb");
    req_refugeeinfo.bindValue(":idDb",idDb);
    if(idDb!=-1)
    {
        if(req_refugeeinfo.exec())
        {
            if(req_refugeeinfo.next())
            {
                ui->text_lname->setText(req_refugeeinfo.value(0).toString());
                ui->text_fname->setText(req_refugeeinfo.value(1).toString());
                ui->combo_age->setCurrentIndex(req_refugeeinfo.value(2).toInt()-1);

                for(int i=0; i<ui->combo_sex->count();i++)
                {
                    if(req_refugeeinfo.value(3).toString()==ui->combo_sex->currentText())
                    {
                        ui->combo_sex->setCurrentIndex(i);
                        break;
                    }
                    ui->combo_sex->setCurrentIndex(ui->combo_sex->currentIndex()+1);
                }

                for(int i=0; i<ui->combo_homeland->count();i++)
                {
                    if(req_refugeeinfo.value(4).toString()==ui->combo_homeland->currentText())
                    {
                        ui->combo_homeland->setCurrentIndex(i);
                        break;
                    }
                    ui->combo_homeland->setCurrentIndex(ui->combo_homeland->currentIndex()+1);
                }

                for(int i=0; i<ui->combo_type->count();i++)
                {
                    if(req_refugeeinfo.value(5).toString()==ui->combo_type->currentText())
                    {
                        ui->combo_type->setCurrentIndex(i);
                        break;
                    }
                    ui->combo_type->setCurrentIndex(ui->combo_type->currentIndex()+1);
                }

                for(int i=0; i<ui->combo_state->count();i++)
                {
                    if(req_refugeeinfo.value(6).toString()==ui->combo_state->currentText())
                    {
                        ui->combo_state->setCurrentIndex(i);
                        break;
                    }
                    ui->combo_state->setCurrentIndex(ui->combo_state->currentIndex()+1);
                }
                ui->text_misc->setPlainText(req_refugeeinfo.value(7).toString());
                ui->combo_curCamp->setCurrentIndex(req_refugeeinfo.value(8).toInt());

            }
        }
    }
	
    else
    {
        ui->combo_age->setCurrentIndex(-1);
        ui->combo_homeland->setCurrentIndex(-1);
        ui->combo_curCamp->setCurrentIndex(-1);
        ui->combo_sex->setCurrentIndex(-1);
        ui->combo_state->setCurrentIndex(-1);
        ui->combo_type->setCurrentIndex(-1);
    }

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
        ui->combo_homeland->setEnabled(true);
        ui->combo_type->setEnabled(true);
    }

    QObject::connect(this ,          SIGNAL(accepted()),    parent, SLOT(closeRefugeeInfo()));
    QObject::connect(this ,          SIGNAL(rejected()),    parent, SLOT(closeRefugeeInfo()));
    QObject::connect(ui->btn_ok ,    SIGNAL(clicked(bool)), parent, SLOT(OkRefugeeInfo()));
    QObject::connect(ui->btn_cancel, SIGNAL(clicked(bool)), parent, SLOT(closeRefugeeInfo()));
}

RefugeeInfoWin::~RefugeeInfoWin()
{
    delete ui;
}
