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

public:
    explicit MainWin(QWidget *parent = 0);
    ~MainWin();

public slots:
    void changeCamp(QModelIndex);
    void changeOnglet(int);

private:
    Ui::MainWin *ui;
    quint16 m_curCamp;
    quint16 m_curOnglet;
    bool m_campModEnCours;

private slots:
    void m_campMod(bool);
};

#endif // MAINWIN_H
