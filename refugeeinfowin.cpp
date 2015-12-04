#include "refugeeinfowin.h"
#include "ui_refugeeinfowin.h"

RefugeeInfoWin::RefugeeInfoWin(Database* db_, QWidget *parent, int idDb,  OpenMode openMode) :
    QDialog(parent), ui(new Ui::RefugeeInfoWin), m_db(db_),
    m_openMode(openMode), m_idDb(-1)
{
    ui->setupUi(this);

    fillFields(m_db->access());

    if(m_openMode != creation)
    {
        QSqlQuery req_refugeeinfo(*m_db->access());
        m_idDb = idDb;

        req_refugeeinfo.prepare("SELECT nom, prenom, age, sexe, pays_dorigine, type, etat, divers, nom_camp FROM Refugie, Camps WHERE id_refugie = :idDb and Refugie.id_camp = Camps.id_camp");
        req_refugeeinfo.bindValue(":idDb", m_idDb);

        if(req_refugeeinfo.exec())
        {
            if(req_refugeeinfo.next())
            {
                ui->text_lname->setText(req_refugeeinfo.value(0).toString() + " #" + QString::number(idDb));
                ui->text_fname->setText(req_refugeeinfo.value(1).toString());
                ui->combo_age->setCurrentIndex(req_refugeeinfo.value(2).toInt() - 1);

                ui->combo_sex->setCurrentIndex(ui->combo_sex->findText(req_refugeeinfo.value(3).toString()));
                ui->combo_type->setCurrentIndex(ui->combo_type->findText(req_refugeeinfo.value(5).toString()));
                ui->combo_state->setCurrentIndex(ui->combo_state->findText(req_refugeeinfo.value(6).toString()));
                ui->combo_homeland->setCurrentIndex(ui->combo_homeland->findText(req_refugeeinfo.value(4).toString()));

                ui->text_misc->setPlainText(req_refugeeinfo.value(7).toString());
                ui->combo_curCamp->setCurrentIndex(ui->combo_curCamp->findText(req_refugeeinfo.value(8).toString()));
            }
        }
        else
            qDebug() << req_refugeeinfo.lastError();
    }
    else
    {
        ui->combo_age->setCurrentIndex(-1);
        ui->combo_homeland->setCurrentIndex(-1);
        ui->combo_curCamp->setCurrentIndex(-1); //Put on the good position at the beginning of the window
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
    QObject::connect(ui->btn_ok,     SIGNAL(clicked(bool)), this,   SLOT(insertOrUpdateRefugee()));
    QObject::connect(ui->btn_ok ,    SIGNAL(clicked(bool)), parent, SLOT(OkRefugeeInfo()));
    QObject::connect(ui->btn_cancel, SIGNAL(clicked(bool)), parent, SLOT(closeRefugeeInfo()));
}

void RefugeeInfoWin::insertOrUpdateRefugee()
{
    QSqlQuery AddorUpdateRefugee;
    QString StartRequest,MidRequest;
    bool ok=true;
    if(m_openMode == creation)
    {
        StartRequest="Insert into Refugie(nom,prenom,age,sexe,pays_dorigine,type,etat,divers,id_camp) ";
        MidRequest = "Values ( :newname, :newfname , :newage , :newSexe , :newPays , :newtype , :newState , :newDivers , :newId_camp )";

    }
    else
    {
        StartRequest = "Update Refugie set nom= :newname , prenom = :newfname , etat= :newState, id_camp = :newId_camp, divers = :newDivers";
        MidRequest = " where id_refugie = :newid";
    }

    AddorUpdateRefugee.prepare( StartRequest + MidRequest);

    AddorUpdateRefugee.bindValue(":newid",      m_idDb);
    AddorUpdateRefugee.bindValue(":newname",    ui->text_lname->text());
    AddorUpdateRefugee.bindValue(":newfname",   ui->text_fname->text());
    AddorUpdateRefugee.bindValue(":newage",     ui->combo_age->currentText().toInt());
    AddorUpdateRefugee.bindValue(":newSexe",    ui->combo_sex->currentText());
    AddorUpdateRefugee.bindValue(":newPays",    ui->combo_homeland->currentText());
    AddorUpdateRefugee.bindValue(":newtype",    ui->combo_type->currentText());
    AddorUpdateRefugee.bindValue(":newState",   ui->combo_state->currentText());
    AddorUpdateRefugee.bindValue(":newDivers",  ui->text_misc->toPlainText());
    AddorUpdateRefugee.bindValue(":newId_camp", ui->combo_curCamp->currentIndex());

    if(ui->text_fname->text()=="")
    {
        ok=false;
        QMessageBox::critical(this,tr("Error"),tr("Please enter the firstname"));
    }
    if(ui->text_lname->text()=="" && ok)
    {
        ok=false;
        QMessageBox::critical(this,tr("Error"),tr("Please enter the surname"));
    }
    if(ui->combo_age->currentText()=="" && ok)
    {
        ok=false;
        QMessageBox::critical(this,tr("Error"),tr("Please enter the age"));
    }
    if(ui->combo_sex->currentText()=="" && ok)
    {
        ok=false;
         QMessageBox::critical(this,tr("Error"),tr("Please enter the sex"));
    }
    if(ui->combo_homeland->currentText()=="" && ok)
    {
        ok=false;
         QMessageBox::critical(this,tr("Error"),tr("Please enter the homeland"));
    }
    if(ui->combo_type->currentText()=="" && ok)
    {
        ok=false;
         QMessageBox::critical(this,tr("Error"),tr("Please enter the type"));
    }
    if(ui->combo_state->currentText()=="" && ok)
    {
        ok=false;
         QMessageBox::critical(this,tr("Error"),tr("Please enter the state"));
    }
    if(ui->combo_curCamp->currentText()=="" && ok)
    {
        ok=false;
        QMessageBox::critical(this,tr("Error"),tr("Please enter the current Camp"));
    }

    if(ok)
    {
        if(AddorUpdateRefugee.exec())
            qDebug() << "[DEBUG] refugeeinfowin.cpp::addOrUpdateRefugee() : Refugee Insertion Successful : " + m_idDb;
        else
            qDebug() << "[ERROR] refugeeinfowin.cpp::addOrUpdateRefugee() : Insertion Failed (" + AddorUpdateRefugee.lastError().text() + ")";
    }
}

RefugeeInfoWin::~RefugeeInfoWin()
{
    delete ui;
}

void RefugeeInfoWin::fillFields(QSqlDatabase* db_)
{
    /* List of sql querys needed */
        QSqlQuery req_allcamp(*db_);
        QSqlQuery req_allSexe(*db_);
        QSqlQuery req_allCountry(*db_);
        QSqlQuery req_allType(*db_);
        QSqlQuery req_allState(*db_);


    if(req_allSexe.exec("SELECT DISTINCT sexe FROM Refugie"))
    {
        while(req_allSexe.next())
            ui->combo_sex->addItem(req_allSexe.value(0).toString());
    }

    if(req_allcamp.exec("SELECT DISTINCT nom_camp FROM Camps"))
    {
        while(req_allcamp.next())
            ui->combo_curCamp->addItem(req_allcamp.value(0).toString());
    }

    if(req_allCountry.exec("SELECT DISTINCT pays_dorigine FROM Refugie"))
    {
        while(req_allCountry.next())
            ui->combo_homeland->addItem(req_allCountry.value(0).toString());
    }

    if(req_allType.exec("SELECT DISTINCT type FROM Refugie"))
    {
        while(req_allType.next())
            ui->combo_type->addItem(req_allType.value(0).toString());
    }

    for(quint16 i= 1; i<= 100; i++)
        ui->combo_age->addItem(QString::number(i));

    if(req_allState.exec("SELECT DISTINCT etat FROM Refugie"))
    {
        while(req_allState.next())
            ui->combo_state->addItem(req_allState.value(0).toString());
    }
}

