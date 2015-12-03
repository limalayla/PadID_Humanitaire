#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent, QJsonDocument configFile) :
    QMainWindow(parent), m_refugeeInfoWin(NULL), m_configFile(configFile),
    m_db(new Database(this, configFile)), ui(new Ui::MainWin),
    m_curCamp(0), m_curTab(0), m_campModOngoing(false)
{
    /*  CONSTRUCTOR
     *          Called once at the beginning of the program
    */

    ui->setupUi(this);

    /* Preparing the UI */
        /* General */
            ui->tabs->setCurrentIndex(0);
            ui->tabs->setTabEnabled(2, false);
            ui->tabs->setTabEnabled(3, false);
            ui->list_campSearch->setVisible(false);
            ui->btn_campModCancel->setVisible(false);

            // Customize the ui->list_camp's "All" item
				

            // Get the camp list from database
			Updatelist_camp();

        /* Overview Tab */
            ui->groupbox_campOther->setVisible(false);

            ui->text_campPlaceMax->setValidator(new QIntValidator(this));
            ui->text_campNbRefugee->setValidator(new QIntValidator(this));
            ui->text_campPlaceRemaining->setValidator(new QIntValidator(this));

        /* Search Tab */
            for(quint8 i= 1; i<= 100; i++) ui->combo_searchAge->addItem(QString::number(i));

            // Get the list of the differents country from db and put it in ui->combo_searchCountry
            QSqlQuery req_countryList(*m_db->access());
            if(req_countryList.exec("SELECT nom_pays FROM Pays"))
            {
                while(req_countryList.next())
                {
                    QVariant res_countryList(req_countryList.value(0));

                    ui->combo_searchHomeland->addItem(res_countryList.toString());
                }
            }

         /* Supplies Tab */
            suppliesInit(m_db->access());
            ui->tabs_supplies->setVisible(true);

         /* Initiating the signals - slots */
            initSlots();
}

MainWin::~MainWin()
{
    delete ui;
    m_db->close();
}

void MainWin::initSlots()
{
    /* General */
        QObject::connect(ui->btn_campAdd,       SIGNAL(clicked(bool)),        this, SLOT(campAdd(bool)));
        QObject::connect(ui->tabs,              SIGNAL(currentChanged(int)),  this, SLOT(changeTab(int)));
        QObject::connect(ui->list_camp,         SIGNAL(clicked(QModelIndex)), this, SLOT(changeCamp(QModelIndex)));
        QObject::connect(ui->text_searchCamp,   SIGNAL(textChanged(QString)), this, SLOT(campSearch(QString)));
        QObject::connect(ui->list_campSearch,   SIGNAL(clicked(QModelIndex)), this, SLOT(changeCampSearch(QModelIndex)));
        QObject::connect(ui->actionExit,        SIGNAL(triggered(bool)),      this, SLOT(close()));

   /* Overview Tab */
        QObject::connect(ui->btn_campMod,       SIGNAL(clicked(bool)), this, SLOT(campMod(bool)));
        QObject::connect(ui->btn_campModCancel, SIGNAL(clicked(bool)), this, SLOT(campModCancel(bool)));
        QObject::connect(ui->btn_campDel,       SIGNAL(clicked(bool)), this, SLOT(campDel(bool)));

   /* Manage Tab */
        QObject::connect(ui->btn_manageAdd,     SIGNAL(clicked(bool)),              this, SLOT(refugeeAdd(bool)));
        QObject::connect(ui->btn_manageMod,     SIGNAL(clicked(bool)),              this, SLOT(refugeeMod(bool)));
        QObject::connect(ui->btn_manageDel,     SIGNAL(clicked(bool)),              this, SLOT(refugeeDel(bool)));
        QObject::connect(ui->list_manage,       SIGNAL(clicked(QModelIndex)),       this, SLOT(refugeeSet(QModelIndex)));
        QObject::connect(ui->list_manage,       SIGNAL(doubleClicked(QModelIndex)), this, SLOT(refugeeSee(QModelIndex)));
        QObject::connect(ui->text_searchManage, SIGNAL(textChanged(QString)),       this, SLOT(manageSearch(QString)));
        QObject::connect(ui->list_manageSearch, SIGNAL(clicked(QModelIndex)),       this, SLOT(changeManageSearch(QModelIndex)));
}

void MainWin::Updatelist_camp()
{
	/*
	 *
	 *
	 *
	*/
	
	// Clear the already existing camp list
    ui->list_camp->clear();
	
	// (Re)create the first item "All" (wich have a specific meaning : see a summary of all camps)
	
	ui->list_camp->item(0)->setFont(QFont("Arial", 11));
	ui->list_camp->item(0)->setTextAlignment(Qt::AlignHCenter);
	m_campsIdDb.push_back(-1);
    QSqlQuery req_listCamp(*m_db->access());
    if(req_listCamp.exec("SELECT id_camp, nom_camp FROM Camps"))
    {
        while(req_listCamp.next())
        {
            QVariant res_campID  (req_listCamp.value(0));
            QVariant res_campName(req_listCamp.value(1));

            m_campsIdDb.push_back(res_campID.toInt());
            ui->list_camp->addItem(res_campName.toString());
        }
    }
}
