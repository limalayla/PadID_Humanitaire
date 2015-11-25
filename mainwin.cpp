#include "mainwin.h"
#include "ui_mainwin.h"


MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWin), m_db(NULL),
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
                ui->list_camp->item(0)->setFont(QFont("Arial", 11));
                ui->list_camp->item(0)->setTextAlignment(Qt::AlignHCenter);
                m_campsIdDb.push_back(-1);

            // Get the camp list from database
                QSqlQuery req_listCamp(*db());
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

        /* Overview Tab */
            ui->groupbox_campOther->setVisible(false);

        /* Search Tab */
            for(quint8 i= 1; i<= 100; i++) ui->combo_searchAge->addItem(QString::number(i));

            // Get the list of the differents country from db and put it in ui->combo_searchCountry
            QSqlQuery req_countryList(*db());
            if(req_countryList.exec("SELECT nom_pays FROM Pays"))
            {
                while(req_countryList.next())
                {
                    QVariant res_countryList(req_countryList.value(0));

                    ui->combo_searchCountry->addItem(res_countryList.toString());
                }
            }
    /* Initiating the signals - slots */
        initSlots();
}

MainWin::~MainWin()
{
    delete ui;
    closedb();
}

void MainWin::initSlots()
{
    /* General */
        QObject::connect(ui->btn_campAdd,     SIGNAL(clicked(bool)),        this, SLOT(campAdd(bool)));
        QObject::connect(ui->tabs,            SIGNAL(currentChanged(int)),  this, SLOT(changeTab(int)));
        QObject::connect(ui->list_camp,       SIGNAL(clicked(QModelIndex)), this, SLOT(changeCamp(QModelIndex)));
        QObject::connect(ui->text_searchCamp, SIGNAL(textChanged(QString)), this, SLOT(campSearch(QString)));
        QObject::connect(ui->list_campSearch, SIGNAL(clicked(QModelIndex)), this, SLOT(changeCampSearch(QModelIndex)));
        QObject::connect(&m_timerdb,          SIGNAL(timeout()),            this, SLOT(closedb()));
        QObject::connect(ui->actionExit,    SIGNAL(toggled(bool)),        this, SLOT(close()));


   /* Overview Tab */
        QObject::connect(ui->btn_campMod,       SIGNAL(clicked(bool)), this, SLOT(campMod(bool)));
        QObject::connect(ui->btn_campModCancel, SIGNAL(clicked(bool)), this, SLOT(campModCancel(bool)));
        QObject::connect(ui->btn_campDel,       SIGNAL(clicked(bool)), this, SLOT(campDel(bool)));

}
