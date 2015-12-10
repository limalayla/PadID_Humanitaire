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

            // Get the camp list from database
            ui->list_camp->clear();
            loadCampList();
            ui->list_manage->setSelectionMode(QAbstractItemView::ExtendedSelection);

        // Overview Tab
            overviewCreation();

        /* Search Tab */
            search_fillFields();

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
        QObject::connect(ui->btn_campAdd,       SIGNAL(clicked(bool)),              this, SLOT(campAdd(bool)));
        QObject::connect(ui->btn_campRefresh,   SIGNAL(clicked(bool)),              this, SLOT(loadCampList(bool)));
        QObject::connect(ui->tabs,              SIGNAL(currentChanged(int)),        this, SLOT(changeTab(int)));
        QObject::connect(ui->list_camp,         SIGNAL(clicked(QModelIndex)),       this, SLOT(changeCamp(QModelIndex)));
        QObject::connect(ui->text_searchCamp,   SIGNAL(textChanged(QString)),       this, SLOT(campSearch(QString)));
        QObject::connect(ui->list_campSearch,   SIGNAL(clicked(QModelIndex)),       this, SLOT(changeCampSearch(QModelIndex)));

        QObject::connect(ui->actionExit,        SIGNAL(triggered(bool)),            this, SLOT(close()));
        QObject::connect(ui->actionEnglish,     SIGNAL(triggered(bool)),            this, SLOT(gen_translateEn(bool)));
        QObject::connect(ui->actionFrench,      SIGNAL(triggered(bool)),            this, SLOT(gen_translateFr(bool)));

   /* Overview Tab */
        QObject::connect(ui->btn_campMod,       SIGNAL(clicked(bool)),              this, SLOT(campMod(bool)));
        QObject::connect(ui->btn_campModCancel, SIGNAL(clicked(bool)),              this, SLOT(campModCancel(bool)));
        QObject::connect(ui->btn_campDel,       SIGNAL(clicked(bool)),              this, SLOT(campDel(bool)));

    /* Search Tab */
        QObject::connect(ui->btn_search,        SIGNAL(clicked(bool)),              this, SLOT(refugeeSearch(bool)));
        QObject::connect(ui->list_res_search,   SIGNAL(clicked(QModelIndex)),       this, SLOT(search_refugeeSet(QModelIndex)));
        QObject::connect(ui->list_res_search,   SIGNAL(doubleClicked(QModelIndex)), this, SLOT(search_refugeeSee(QModelIndex)));

   /* Manage Tab */
        QObject::connect(ui->btn_manageAdd,     SIGNAL(clicked(bool)),              this, SLOT(refugeeAdd(bool)));
        QObject::connect(ui->btn_manageMod,     SIGNAL(clicked(bool)),              this, SLOT(refugeeMod(bool)));
        QObject::connect(ui->btn_manageDel,     SIGNAL(clicked(bool)),              this, SLOT(refugeeDel(bool)));
        QObject::connect(ui->list_manage,       SIGNAL(clicked(QModelIndex)),       this, SLOT(refugeeSet(QModelIndex)));
        QObject::connect(ui->list_manage,       SIGNAL(doubleClicked(QModelIndex)), this, SLOT(refugeeSee(QModelIndex)));
        QObject::connect(ui->text_searchManage, SIGNAL(textChanged(QString)),       this, SLOT(manageSearch(QString)));
        QObject::connect(ui->list_manageSearch, SIGNAL(clicked(QModelIndex)),       this, SLOT(changeManageSearch(QModelIndex)));

    /* Supplies Tab */
}
