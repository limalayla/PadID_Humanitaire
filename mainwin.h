#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QListWidgetItem>

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
    void changeOnglet(QListWidgetItem*,QListWidgetItem*);

private:
    Ui::MainWin *ui;
};

#endif // MAINWIN_H
