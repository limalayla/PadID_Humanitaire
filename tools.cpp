#include "tools.h"

const QRegExp Tools::c_rgx_campName = QRegExp("^[a-z](\\w|-)*$", Qt::CaseInsensitive);
const QRegExp Tools::c_rgx_alphaNumString = QRegExp("^\\w(\\w| |-|_)*\\w&", Qt::CaseInsensitive);

Tools::Tools(QObject *parent) : QObject(parent)
{}

Tools::StringEvalCode Tools::campNameValid(const QString& s, const QListWidget& list, quint16 curCamp, quint16 sizeMax)
{
    /*  TOOL
     *      USE         : To check if a string is valid (to a certain patern wich should be precised)
     *      ACTIONS     : Check multiple points and returns a corresponding value
     *      RETURN VAL  : An enum : String Evaluation Code (StringEvalCode)
    */

    if(s.isEmpty()) return Tools::EmptyStr;
    if(s.length() >= sizeMax) return Tools::TooLong;

    for(quint16 i= 0; i< list.count(); i++)
        if(s.compare(list.item(i)->text(), Qt::CaseInsensitive) == 0 && i != curCamp)
            return Tools::AlreadyTake;

    return Tools::Ok;
}

void Tools::dispErr(QWidget* src, const StringEvalCode& code)
{
         if(code == Tools::EmptyStr)    QMessageBox::warning(src, tr("Empty Name"), tr("You entered an empty name for a camp, please enter one again."));
    else if(code == Tools::TooLong)     QMessageBox::warning(src, tr("Name Too Long"), tr("This name is too long, please enter one again.."));
    else if(code == Tools::BadFormat)   QMessageBox::warning(src, tr("Incorrect Name"), tr("This name is incorrect. A camp name may start with a letter and can only be made of letters, dashes and spaces. Please enter one again."));
    else if(code == Tools::AlreadyTake) QMessageBox::warning(src, tr("Name Already Taken"), tr("This camp name is already taken, please enter one again."));
}

QJsonDocument* Tools::jsonFromFile(const QString& path, bool* ok)
{
    QString pathToJson(path + ((path.endsWith(".json")) ? "" : ".json"));
    QFile file(pathToJson);
    QJsonDocument* jsonDoc = new QJsonDocument();
    QJsonParseError err;

    qDebug() << "[DEBUG] tools.cpp::jsonFromFile() : opening " << pathToJson;
    if(ok != NULL) *ok = true;

    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "[ERROR] tools.cpp::jsonFromFile() : file " << file.fileName() << " cannot be opened (" << file.errorString() << ")";
        QMessageBox::critical(NULL, tr("Can't open File"), tr("File ") + file.fileName() + tr(" cannot be opened") + " (" + file.errorString() + ")");

        if(ok != NULL) *ok = false;
        return NULL;
    }

    *jsonDoc = QJsonDocument::fromJson(file.readAll(), &err);

    if(err.error != QJsonParseError::NoError)
    {
        qCritical() << "[ERROR] tools.cpp::jsonFromFile() : error parsing " << pathToJson << " (#" << err.error << ") at line " << err.offset << " : " << err.errorString();
        QMessageBox::critical(NULL, tr("Error Parsing"), tr("Error parsing ") + pathToJson + " (#" + err.error + " )" + tr("at line ") + err.offset + " : " + err.errorString());

        if(ok != NULL) *ok = false;
        return NULL;
    }

    return jsonDoc;
}
