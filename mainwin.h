#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QVector>
#include <QInputDialog>
#include <QtSql>
#include <QDebug>
#include <QtGlobal>
#include <QRegExp>

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

/* General */
    public:
        explicit MainWin(QWidget *parent = 0);
                ~MainWin();
        QSqlDatabase* db(quint16 timeoutTimer_s= 5*60);

        static const quint8 c_AllCampIndex = 0;

    public slots:
        void changeCamp      (QModelIndex);
        void changeCampSearch(QModelIndex);
        void changeTab(int);
        void campAdd(bool);
        void campSearch(QString);

    private:
        Ui::MainWin *ui;
        QSqlDatabase* m_db;
        quint16 m_curCamp;
        quint16 m_curTab;
        QVector<int> m_campsIdDb;
        QTimer m_timerdb;

        void    initSlots();
        quint16 campNameValid(const QString&);

    private slots:
        void closedb();

/* Overview Tab */
    public:

    public slots:

    private:
        bool m_campModOngoing;
        void campSetEnabledInput(bool);
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

    public slots:

    private slots:
        void managementLoad(QSqlDatabase* db);


 /* Supplies Tab */
    public:

    public slots:

    private slots:
};

#endif // MAINWIN_H
