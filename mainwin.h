#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QVector>
#include <QInputDialog>
#include <QGridLayout>
#include <QMap>
#include <QDate>
#include <QProgressDialog>

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

        enum appLanguages
        {
            En = 0,
            Fr = 1
        };

    public slots:
        void changeCamp        (QModelIndex);
        void changeCampSearch  (QModelIndex);
        void changeCenter      (QModelIndex);
        void changeCenterSearch(QModelIndex);

        void changeTab(int);

        void campAdd  (bool);
        void centerAdd(bool);

        void campSearch  (QString);
        void centerSearch(QString);

        void loadCampList  (bool=false);
        void loadCenterList(bool=false);

        void overview_setCampModOngoing();

        void gen_translateEn(bool);
        void gen_translateFr(bool);

    private:
        Ui::MainWin *ui;
        quint16 m_curCamp;
        quint16 m_curCenter;
        quint16 m_curTab;
        QVector<int> m_campsIdDb;
        QVector<int> m_centerIdDb;

        void initSlots();
        void gen_translate(appLanguages);

/* Overview Tab */
    public:

    public slots:

    private:
        bool m_campModOngoing;
        void campSetEnabledInput(bool);
        void overviewCreation();
        void overviewLoad(bool= false);
        void overviewLoad(QSqlDatabase* db, bool= false);
        void overview_centerLoad(QSqlDatabase* db);

    private slots:
        void campMod(bool);
        void campModCancel(bool =false);
        void campDel(bool);


/* Search Tab */
    public:
        void searchClear();
        void search_fillFields();

    public slots:

    private:
        quint16 m_curSearchRefugee;
        QVector<int> m_searchRefugeeIdDb;

    private slots:
        void refugeeSearch(bool);
        void search_refugeeSet(QModelIndex);
        void search_refugeeSee(QModelIndex);

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
        void commandStock(bool);

    private:
         QVector<QVector<QLineEdit*> > m_suppliesText;
         QVector<QVector<QLabel*> >    m_suppliesLabel;
};

#endif // MAINWIN_H
