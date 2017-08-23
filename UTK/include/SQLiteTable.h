#include "sqlite.h"

class SQLiteTable{
public:
    SQLiteTable(wxSQLite3Database* lDB, std::vector<wxString> lTblUnit, std::vector<wxString> lTblSource,
                std::vector<field> lUnitFields, std::vector<field> lSourceFields);

    bool    CreateTable(wxString lName, std::vector<field> lUnitFields);
    bool    CheckTable(wxString lName, std::vector<field> lUnitFields);
    bool 	IsHave();
    bool	InputUnit();
    bool	OutputUnit();
    bool 	AddUnit();
    bool	EditUnit();
    bool	RemoveUnit();

private:
    wxSQLite3Database* mDb;	//Указателя на БД
    std::vector<wxString>	mTblSource;	//Таблица с иходными данными
    std::vector<wxString>	mTblUnit;	//Рабочая таблица
    std::vector<field>		mUnitFields;
    std::vector<field>		mSourceFields;
};
