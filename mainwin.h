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

/* Géneral */
    public:
        explicit MainWin(QWidget *parent = 0);
        ~MainWin();
        void campChargement(quint16);
        QSqlDatabase* db();

    public slots:
        void changeCamp(QModelIndex);
        void changeCampRech(QModelIndex);
        void changeOnglet(int);
        void campAjouter(bool);
        void campRecherche(QString);

    private:
        Ui::MainWin *ui;
        QSqlDatabase* m_db;
        quint16 m_curCamp;
        quint16 m_curOnglet;
        void initEvenement();
        quint16 m_nomCampValide(const QString&);
        QVector<int> m_campsIdBdD;
        QTimer m_timerdb;

    private slots:
        void closedb();

/* Onglet vue d'ensemble */
    public:

    public slots:

    private:
        bool m_campModEnCours;
        void m_campSetEnabledInput(bool);
        void m_vueensLoad(bool= false);

    private slots:
        void m_campMod(bool);
        void m_campModAnnuler(bool =false);
        void m_campSuppr(bool);


/* Onglet recherche */
    public:

    public slots:

    private slots:

/* Onglet gestion humaine */
    public:

    public slots:

    private slots:


 /* Onglet stocks */
    public:

    public slots:

    private slots:
};

#endif // MAINWIN_H
