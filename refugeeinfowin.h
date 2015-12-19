#ifndef REFUGEEINFOWIN_H
#define REFUGEEINFOWIN_H

#include <QDialog>

#include "tools.h"
#include "database.h"

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

    explicit RefugeeInfoWin(Database* db_, QWidget *parent= 0, int idDb= -1, OpenMode openMode= readOnly, int curCamp= -1);
    ~RefugeeInfoWin();
    void fillFields(QSqlDatabase* db_);

public slots:
    void insertOrUpdateRefugee();

private:
    Ui::RefugeeInfoWin *ui;
    Database* m_db;
    OpenMode m_openMode;
    quint16 m_idDb;
    quint16 m_curCamp;
};

#endif // REFUGEEINFOWIN_H
