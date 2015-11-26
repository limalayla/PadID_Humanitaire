#ifndef REFUGEEINFOWIN_H
#define REFUGEEINFOWIN_H

#include <QDialog>

namespace Ui {
class RefugeeInfoWin;
}

class RefugeeInfoWin : public QDialog
{
    Q_OBJECT

public:
    enum OpenMode
    {
        readOnly  = 0,
        readWrite = 1,
        creation  = 2
    };

    explicit RefugeeInfoWin(QWidget *parent = 0, OpenMode openMode = readOnly);
    ~RefugeeInfoWin();

private:
    Ui::RefugeeInfoWin *ui;
    OpenMode m_openMode;
};

#endif // REFUGEEINFOWIN_H
