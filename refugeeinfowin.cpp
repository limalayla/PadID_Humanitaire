#include "refugeeinfowin.h"
#include "ui_refugeeinfowin.h"

RefugeeInfoWin::RefugeeInfoWin(Database* db_, QWidget *parent, int idDb,  OpenMode openMode) :
    QDialog(parent), ui(new Ui::RefugeeInfoWin), m_db(db_),
    m_openMode(openMode), m_idDb(-1)
{
    ui->setupUi(this);

    ui->text_fname->setValidator(new QRegExpValidator(Tools::c_rgx_alphaNumString, ui->text_fname));
    ui->text_lname->setValidator(new QRegExpValidator(Tools::c_rgx_alphaNumString, ui->text_lname));

    fillFields(m_db->access());

    if(m_openMode != creation)
    {
        QSqlQuery req_refugeeinfo(*m_db->access());
        m_idDb = idDb;

        req_refugeeinfo.prepare("SELECT "
                                "Refugees.name_refugee,"
                                "Refugees.firstname_refugee,"
                                "Refugees.sex, "
                                "Refugees.birth_date, "
                                "Country.name_country, "
                                "Types.name_type, "
                                "States.name_state,"
                                "Camps.name_camp, "
                                "Refugees.several_informations "
                                "FROM "
                                "Refugees, "
                                "Country, "
                                "Types, "
                                "States,"
                                "Camps "
                                "WHERE "
                                "id_refugee = :idDb "
                                "and "
                                "Refugees.id_camp = Camps.id_camp "
                                "and "
                                "Refugees.id_origin_country = Country.id_country "
                                "and "
                                "Refugees.id_type = Types.id_type "
                                "and "
                                "Refugees.id_state = States.id_state");

        req_refugeeinfo.bindValue(":idDb", m_idDb);

        if(req_refugeeinfo.exec())
        {
            if(req_refugeeinfo.next())
            {
                ui->label_id->setText(" #" + QString::number(idDb));
                ui->text_lname->setText(req_refugeeinfo.value(0).toString());
                ui->text_fname->setText(req_refugeeinfo.value(1).toString());
                ui->combo_sex->setCurrentIndex(ui->combo_sex->findText(req_refugeeinfo.value(2).toString()));
                ui->text_birthDate->setText(req_refugeeinfo.value(3).toString());
                ui->combo_homeland->setCurrentIndex(ui->combo_homeland->findText(req_refugeeinfo.value(4).toString()));
                ui->combo_type->setCurrentIndex(ui->combo_type->findText(req_refugeeinfo.value(5).toString()));
                ui->combo_state->setCurrentIndex(ui->combo_state->findText(req_refugeeinfo.value(6).toString()));
                ui->combo_curCamp->setCurrentIndex(ui->combo_curCamp->findText(req_refugeeinfo.value(7).toString()));
                ui->text_misc->setPlainText(req_refugeeinfo.value(8).toString());

            }
        }
        else
            qDebug() << req_refugeeinfo.lastError();
    }
    else
    {
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
        ui->text_birthDate->setEnabled(true);
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
    bool ok(true);

    if(ui->text_fname->text().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Please enter the first name!"));
    }
    if(ok && ui->text_lname->text().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Please enter the last name!"));
    }
    if(ok && ui->text_birthDate->text().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Please enter the age!"));
    }
    if(ok && ui->combo_sex->currentText().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this ,tr("Error"), tr("Please enter the sex!"));
    }
    if(ok && ui->combo_homeland->currentText().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Please enter the homeland!"));
    }
    if(ok && ui->combo_type->currentText().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Please enter the type!"));
    }
    if(ok && ui->combo_state->currentText().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Please enter the state!"));
    }
    if(ok && ui->combo_curCamp->currentText().isEmpty())
    {
        ok = false;
        QMessageBox::warning(this, tr("Error"), tr("Please enter the current camp!"));
    }

    if(ok)
    {
        QSqlQuery req_campFull("SELECT count(Refugees.id_refugee), Camps.nb_max "
                               "FROM Refugees, Camps"
                               "WHERE Camps.id_camp = Refugees.id_camp AND Camps.name_camp = :campname", *m_db->access());
        req_campFull.bindValue(":campname", ui->combo_curCamp->currentText());
        if(req_campFull.exec())
        {
            req_campFull.next();
            if(req_campFull.value(1).toInt() >= req_campFull.value(2).toInt())
            {
                ok = false;
                QMessageBox::warning(this, tr("Error"), tr("This camp is already full! Try another one."));
            }
        }
    }

    if(ok)
    {
        if(m_openMode == creation)
        {
            StartRequest="INSERT INTO Refugees ( name_refugee, firstname_refugee, sex, birth_date, id_origin_country, id_type, id_state,  id_camp, several_informations) ";
            MidRequest = "Value(name_refugee = :newname , firstname_refugee = :newfname , sex= :newSexe, birth_date= :newBd, "
                         "id_origin_country=(select id_country from Country where name_country=:newCountry),  "
                         "id_type=(select id_type from Types where name_type= :newtype),  "
                         "id_state= (select id_state from States where name_state= :newState) ,  "
                         "id_camp =(select id_camp from Camps where name_camp= :newCamp),  "
                         "several_informations = :newMisc )";


        }
        else
        {
            StartRequest = "Update Refugees set "
                           " name_refugee= :newname ,"
                           " firstname_refugee = :newfname ,"
                           " Refugees.id_state= (select id_state from States where name_state=:newState) ,"
                           " Refugees.id_camp = (select id_camp from Camps where name_camp=:newId_camp)"
                           ", several_informations = :newMisc ";
            MidRequest = " where id_refugee = :newid";
        }

        AddorUpdateRefugee.prepare( StartRequest + MidRequest);

        AddorUpdateRefugee.bindValue(":newid",      m_idDb);
        AddorUpdateRefugee.bindValue(":newname",    ui->text_lname->text());
        AddorUpdateRefugee.bindValue(":newfname",   ui->text_fname->text());
        AddorUpdateRefugee.bindValue(":newBd",      ui->text_birthDate->text());
        AddorUpdateRefugee.bindValue(":newSexe",    ui->combo_sex->currentText());
        AddorUpdateRefugee.bindValue(":newCountry", ui->combo_homeland->currentText());
        AddorUpdateRefugee.bindValue(":newtype",    ui->combo_type->currentText());
        AddorUpdateRefugee.bindValue(":newState",   ui->combo_state->currentText());
        AddorUpdateRefugee.bindValue(":newMisc",    ui->text_misc->toPlainText());
        AddorUpdateRefugee.bindValue(":newId_camp", ui->combo_curCamp->currentText());

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


    if(req_allSexe.exec("SELECT DISTINCT sex FROM Refugees"))
    {
        while(req_allSexe.next())
            ui->combo_sex->addItem(req_allSexe.value(0).toString());
    }

    if(req_allcamp.exec("SELECT DISTINCT name_camp FROM Camps"))
    {
        while(req_allcamp.next())
            ui->combo_curCamp->addItem(req_allcamp.value(0).toString());
    }

    if(req_allCountry.exec("SELECT DISTINCT name_country FROM Country"))
    {
        while(req_allCountry.next())
            ui->combo_homeland->addItem(req_allCountry.value(0).toString());
    }

    if(req_allType.exec("SELECT DISTINCT name_type FROM Types"))
    {
        while(req_allType.next())
            ui->combo_type->addItem(req_allType.value(0).toString());
    }

    if(req_allState.exec("SELECT DISTINCT name_state FROM States"))
    {
        while(req_allState.next())
            ui->combo_state->addItem(req_allState.value(0).toString());
    }
}

