#include "mainwin.h"
#include "ui_mainwin.h"

void MainWin::refugeeSearch(bool)
{
    /*  SLOT
     *      CALLED  : When the "search" button is clicked in the search tab
     *
     *      REMARKS : /!\ May change a lot when switching to the new db
    */

    QMap<QString, QString> queryArg;
    QString query;

    ui->list_res_search->clear();
    m_searchRefugeeIdDb.clear();

    /* Get all the informations that are given */
        queryArg["base"] = "SELECT DISTINCT id_refugee, name_refugee, firstname_refugee FROM Refugees,Types,States,Country WHERE ";

        if(m_curCamp != c_AllCampIndex)                   queryArg["camp" ] = QString::number(m_campsIdDb[m_curCamp]);
        if(!ui->text_searchLName->text().isEmpty())       queryArg["lname"] = ui->text_searchLName->text();
        if(!ui->text_searchFName->text().isEmpty())       queryArg["fname"] = ui->text_searchFName->text();
        if(!ui->text_searchMisc->toPlainText().isEmpty()) queryArg["misc" ] = ui->text_searchMisc->toPlainText();

        if(!ui->text_searchBD->text().isEmpty()) queryArg["age"     ] = ui->text_searchBD->text();
        if(ui->combo_searchSex->currentIndex()      != 0) queryArg["sex"     ] = ui->combo_searchSex->currentText();
        if(ui->combo_searchHomeland->currentIndex() != 0) queryArg["homeland"] = ui->combo_searchHomeland->currentText();
        if(ui->combo_searchType->currentIndex()     != 0) queryArg["type"    ] = ui->combo_searchType->currentText();
        if(ui->combo_searchState->currentIndex()    != 0) queryArg["state"   ] = ui->combo_searchState->currentText();


    // If there's at least one (by default, contains one "base"
    if(queryArg.count() > 1)
    {
        QSqlQuery req_search(*m_db->access(10));

        /* Prepare the query (add "field = :field") when there's someting to search for */
            query += queryArg["base"];

            if(queryArg.contains("camp"   )) query += "id_camp = :id_camp AND ";
            if(queryArg.contains("lname"   )) query += "name_refugee = :lname AND ";
            if(queryArg.contains("fname"   )) query += "firstname_refugee = :fname AND ";
            if(queryArg.contains("misc"    )) query += "several_informations = :misc AND ";
            if(queryArg.contains("age"     )) query += "birth_date = :BD AND ";
            if(queryArg.contains("sex"     )) query += "sex = :sex AND ";
            if(queryArg.contains("homeland")) query += "Refugees.id_origin_country = Country.id_country AND Country.name_country = :homeland AND ";
            if(queryArg.contains("type"    )) query += "Refugees.id_type = Types.id_type AND Types.name_type = :type AND ";
            if(queryArg.contains("state"   )) query += "Refugees.id_state = States.id_state AND States.name_state = :state AND ";

            query.remove(query.length() -5, 5); // Deletes the last " AND "
            req_search.prepare(query);


        /* Bind the values to there field (if both exists) */
            if(queryArg.contains("camp"    )) req_search.bindValue(":id_camp",  queryArg["camp"   ]);
            if(queryArg.contains("lname"   )) req_search.bindValue(":lname",    queryArg["lname"   ]);
            if(queryArg.contains("fname"   )) req_search.bindValue(":fname",    queryArg["fname"   ]);
            if(queryArg.contains("misc"    )) req_search.bindValue(":misc",     queryArg["misc"    ]);
            if(queryArg.contains("age"     )) req_search.bindValue(":BD", queryArg["age"    ]);
            if(queryArg.contains("sex"     )) req_search.bindValue(":sex",      queryArg["sex"     ]);
            if(queryArg.contains("homeland")) req_search.bindValue(":homeland", queryArg["homeland"]);
            if(queryArg.contains("type"    )) req_search.bindValue(":type",     queryArg["type"    ]);
            if(queryArg.contains("state"   )) req_search.bindValue(":lname",    queryArg["state"   ]);

        qDebug() << "[DEBUG] tab_search.cpp::refugeeSearch() : query = "    << query;
        qDebug() << "[DEBUG] tab_search.cpp::refugeeSearch() : queryArg = " << queryArg;
        qDebug() << "";

        /* Then execute it, and put the results in ui->list_res_search */
            if(req_search.exec())
            {
                while(req_search.next())
                {
                    qDebug() << "[DEBUG] tab_search.cpp::refugeeSearch() :      res: " << req_search.value(0);
                    m_searchRefugeeIdDb.push_back(req_search.value(0).toInt());
                    ui->list_res_search->addItem(req_search.value(1).toString() + " " + req_search.value(2).toString());
                }

                if(ui->list_res_search->count() == 0) QMessageBox::information(this, tr("No Search Result"), tr("No Refugee were found with those parameters"));
            }
            else qWarning() << "[WARN ] tab_search.cpp::refugeeSearch() : Search Failed : " << req_search.lastError().text();
    }
}

void MainWin::searchClear()
{
    ui->text_searchLName->clear();
    ui->text_searchFName->clear();
    ui->text_searchMisc->clear();

    ui->text_searchBD->clear();
    ui->combo_searchSex->clear();
    ui->combo_searchHomeland->clear();
    ui->combo_searchType->clear();
    ui->combo_searchState->clear();

    ui->list_res_search->clear();
}

void MainWin::search_fillFields()
{

    // Get the list of the differents country from db and put it in ui->combo_searchCountry
    QSqlQuery req_countryList(*m_db->access());
    QSqlQuery req_TypeList(*m_db->access());
    QSqlQuery req_StateList(*m_db->access());
    if(req_countryList.exec("SELECT DISTINCT name_country from Country"))
    {
        while(req_countryList.next())
        {
            ui->combo_searchHomeland->addItem(req_countryList.value(0).toString());
        }
    }
    if(req_TypeList.exec("SELECT DISTINCT name_type from Types"))
    {
        while(req_TypeList.next())
        {
            ui->combo_searchType->addItem(req_TypeList.value(0).toString());
        }
    }
    if(req_StateList.exec("SELECT DISTINCT name_state from States"))
    {
        while(req_StateList.next())
        {
            ui->combo_searchState->addItem(req_StateList.value(0).toString());
        }
    }
}

void MainWin::search_refugeeSee(QModelIndex)
{
    qDebug() << "[DEBUG] tab_search.cpp::search_refugeeSee() : " << m_curSearchRefugee << " -> " << m_searchRefugeeIdDb[m_curSearchRefugee];
    openRefugeeInfo(RefugeeInfoWin::readOnly, m_searchRefugeeIdDb[m_curSearchRefugee]);
}

void MainWin::search_refugeeSet(QModelIndex index)
{
    if(index.isValid()) m_curSearchRefugee = index.row();
}
