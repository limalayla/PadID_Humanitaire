#include "tools.h"

const QRegExp Tools::c_regex_campName = QRegExp("^[a-z](\\w|-)*$", Qt::CaseInsensitive);

Tools::Tools(QObject *parent) : QObject(parent)
{}

Tools::StringEvalCode Tools::campNameValid(const QString& s, const QListWidget& list, const QRegExp& regex, quint16 curCamp, quint16 sizeMax)
{
    /*  TOOL
     *      USE         : To check if a string is valid (to a certain patern wich should be precised)
     *      ACTIONS     : Check multiple points and returns a corresponding value
     *      RETURN VAL  : An enum : String Evaluation Code (StringEvalCode)
    */

    if(s.isEmpty()) return Tools::EmptyStr;
    if(s.length() >= sizeMax) return Tools::TooLong;

    if(!regex.exactMatch(s))  return Tools::BadFormat;

    for(quint16 i= 0; i< list.count(); i++)
        if(s.compare(list.item(i)->text(), Qt::CaseInsensitive) == 0 && i != curCamp)
            return Tools::AlreadyTake;

    return Tools::Ok;
}

void Tools::dispErr(QWidget* src, const StringEvalCode& code)
{
         if(code == Tools::EmptyStr)    QMessageBox::warning(src, tr("Empty name"), tr("You entered an empty name for a camp, please enter one again."));
    else if(code == Tools::TooLong)     QMessageBox::warning(src, tr("Name too long"), tr("This name is too long ( more than 50 character), please enter one again.."));
    else if(code == Tools::BadFormat)   QMessageBox::warning(src, tr("Incorrect name"), tr("You enter an incorrect name: (^[a-z](\\w|-)*$), please enter one again."));
    else if(code == Tools::AlreadyTake) QMessageBox::warning(src, tr("Name already taken"), tr("The name of the camp is already taken, please enter one again."));
}
