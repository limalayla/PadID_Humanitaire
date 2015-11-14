#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>

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
        void chargementCamp(quint16);

    public slots:
        void changeCamp(QModelIndex);
        void changeOnglet(int);

    private:
        Ui::MainWin *ui;
        quint16 m_curCamp;
        quint16 m_curOnglet;

    private slots:


/* Onglet vue d'ensemble */
    public:

    public slots:

    private:
        bool m_campModEnCours;

    private slots:
        void m_campMod(bool);


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
