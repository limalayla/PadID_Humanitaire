#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QVector>
#include <QInputDialog>

#include "refugeeinfowin.h"
#include "database.h"
#include "tools.h"

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

/* General */
    public:
        explicit MainWin(QWidget *parent, QJsonDocument configFile);
                ~MainWin();

        RefugeeInfoWin *m_refugeeInfoWin;
        QJsonDocument m_configFile;
        Database* m_db;

        static const quint8 c_AllCampIndex = 0;

    public slots:
        void changeCamp      (QModelIndex);
        void changeCampSearch(QModelIndex);
        void changeTab(int);
        void campAdd(bool);
        void campSearch(QString);
        void loadCampList(bool=false);

    private:
        Ui::MainWin *ui;
        quint16 m_curCamp;
        quint16 m_curTab;
        QVector<int> m_campsIdDb;

        void initSlots();

/* Overview Tab */
    public:

    public slots:

    private:
        bool m_campModOngoing;
        void campSetEnabledInput(bool);
        void overviewCreation();
        void overviewLoad(bool= false);
        void overviewLoad(QSqlDatabase* db, bool= false);

    private slots:
        void campMod(bool);
        void campModCancel(bool =false);
        void campDel(bool);


/* Search Tab */
    public:

    public slots:

    private slots:

/* Management Tab */
    public:

    private:


    public slots:
        void closeRefugeeInfo();
        void OkRefugeeInfo();

    private:
        void openRefugeeInfo(RefugeeInfoWin::OpenMode openMode, int idRefugeeDb);
        quint16 m_curRefugee;
        QVector<int> m_refugeeIdDb;

    private slots:
        void managementLoad(QSqlDatabase* db);
        void manageSearch(QString searchString);
        void changeManageSearch(QModelIndex index);
        void refugeeAdd(bool);
        void refugeeMod(bool);
        void refugeeDel(bool);

        void refugeeSet(QModelIndex);
        void refugeeSee(QModelIndex);

 /* Supplies Tab */
    public:

    public slots:

    private slots:
        void suppliesLoad(QSqlDatabase* db);
        void suppliesInit(QSqlDatabase * db);
};

#endif // MAINWIN_H
