#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);
    ui->list_camps->setItemSelected(ui->list_camps->item(0), true);
    ui->tab_gestHum->setEnabled(false);
    ui->tab_stock->setEnabled(false);
    QObject::connect(ui->list_camps, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(changeOnglet(QListWidgetItem*,QListWidgetItem*)));
}

MainWin::~MainWin()
{
    delete ui;
}

void MainWin::changeOnglet(QListWidgetItem* cur,QListWidgetItem* prec)
{

    if(cur->text() != "Tous")
    {
        ui->tab_gestHum->setEnabled(true);
        ui->tab_stock->setEnabled(true);

    }
    else
    {
        ui->tab_gestHum->setEnabled(false);
        ui->tab_stock->setEnabled(false);
    }
}
