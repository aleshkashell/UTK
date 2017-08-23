#include "SQLiteTable.h"

SQLiteTable::SQLiteTable(wxSQLite3Database *lDB, std::vector<wxString> lTblUnit, std::vector<wxString> lTblSource, std::vector<field> lUnitFields, std::vector<field> lSourceFields){
    mDb = lDB;
    mTblUnit = lTblUnit;
    mTblSource = lTblSource;
    mUnitFields = lUnitFields;
    mSourceFields = lSourceFields;
    CreateTables();
}
bool SQLiteTable::CreateTables(){
    for(auto table: mTblUnit){
        if(!CreateTable(table, mUnitFields)){
            return false;
        }
    }
    for(auto table: mTblSource){
        if(!CreateTable(table, mSourceFields)){
            return false;
        }
    }
}
bool SQLiteTable::CreateTable(wxString lName, std::vector<field> lUnitFields){
    //Проверяем, существует ли таблица
    if(mDb->TableExists(lName)){
        //Если существует проверяем правильность полей
        if(CheckTable(lName, lUnitFields)) return true;
        return false;
    }
    //Создаём новую таблицу
    wxString request = wxT("CREATE TABLE ") + lName + wxT("(ID INTEGER primary key");
    for (auto x : lUnitFields) {
            request += wxT(", \"") + x.name + wxT("\" ") + x.type;
        }
    request += wxT(")");
    try {
        mDb.ExecuteUpdate(request);
    }
    catch (wxSQLite3Exception &e) {
        errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        myFunc::writeLog(wxT("Таблица: ") + lName + wxT("; Req = \"") + request + wxT("\""));
        return false;
    }
    return true;
}
bool SQLiteTable::CheckTable(wxString lName, std::vector<field> lUnitFields){
    //Делаем запрос в таблицу
    wxString sqlRequest = wxT("SELECT * FROM \"") + lName + wxT("\";");
    wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
    //Сохраняем заголовки
    std::vector<wxString> answer;
    for (int i = 0; i < result.GetColumnCount(); i++) {
        answer.push_back(result.GetColumnName(i));
    }
    //Сверяем нужные заголовки
    std::vector<std::vector<wxString>::iterator> vec;
    for(auto name: lUnitFields){
        vec.push_back(std::find_if(answer.begin(), answer.end(), [name](auto element){return element == name.name;}));
    }
    for (auto it : vec) {
        if (it == answer.end()) {
            errMsg = wxT("Таблица <") + lName + wxT("> не корректна. Выберите другую БД");
            return false;
        }
    }
    return true;
}
