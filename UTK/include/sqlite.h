#ifndef SQLITE_H
#define SQLITE_H

#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/wxsqlite3.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <xlsxwriter.h>
#include <wx/wfstream.h>
#include <iostream>
#include <vector>
#include <locale>
#include <codecvt>
#include <logFile.h>
//For user name
#ifdef _WIN32
    #include <lmcons.h>
    #include <windows.h>
#elif __LINUX__
    //If linux
#endif
struct Binds{
    wxString login;
    wxString numUnit;
    Binds(){ login = ""; numUnit = "";}
    Binds(wxString lLogin, wxString lNumUnit){ login = lLogin; numUnit = lNumUnit;}
    bool operator ==(Binds& a){
        if(this->login == a.login && this->numUnit == a.numUnit) return true;
        return false;
    }
    //void operator =(Binds& a){ this->login = a.login; this->numUnit = a.numUnit;}
};
struct field
{
	field() {}
	field(wxString fName, wxString fType) { name = fName; type = fType; }
	wxString name;
	wxString type;
};
class Column
{
public:
	Column() {
		numUnit = field(wxT("№ техники"), wxT("TEXT"));
		login = field(wxT("Логин"), wxT("TEXT"));
		numHours = field(wxT("Количество часов"), wxT("INTEGER"));
		dateOut = field(wxT("Дата выдачи"), wxT("TEXT"));
		dateIn = field(wxT("Дата приёма"), wxT("TEXT"));
		timeInWork = field(wxT("Часов в работе"), wxT("INTEGER"));
		status = field(wxT("Статус"), wxT("TEXT"));
		nameUnit = field(wxT("Имя техники"), wxT("TEXT"));
		whoOutput = field(wxT("Выдал"), wxT("TEXT"));
		whoInput = field(wxT("Принял"), wxT("TEXT"));
		timeCharge = field(wxT("Время заряда"), wxT("INTEGER"));
        serialNumber = field(wxT("Порядковый номер"), wxT("TEXT"));
	}
    field numUnit, login, numHours, dateOut, dateIn, timeInWork, status, nameUnit, whoOutput, whoInput, timeCharge, serialNumber;
};
class BatteryColumn
{
public:
	BatteryColumn() {
		numUnit = field(wxT("№ техники"), wxT("TEXT"));
		dateOut = field(wxT("Дата выдачи"), wxT("TEXT"));
		dateIn = field(wxT("Дата приёма"), wxT("TEXT"));
		timeInWork = field(wxT("Время заряда/работы"), wxT("INTEGER"));
		status = field(wxT("Статус"), wxT("TEXT"));
		whoInput = field(wxT("Принял"), wxT("TEXT"));
	}
	field numUnit, login, numHours, dateOut, dateIn, timeInWork, status, nameUnit, whoInput;
};
class MyDB
{
public:
	MyDB(wxString name, std::vector<wxString> tblMachine/* = { wxT("Штабеллёры"), wxT("Электротележки"), wxT("Комплектовщики"), wxT("ЭлТележки_с_площ") }*/, std::vector<wxString> tblList/* = { wxT("списокШтабеллёры"), wxT("списокЭлектротележки"), wxT("списокКомплектовщики"), wxT("списокЭл_тел_отк_пл") }*/);
	//Смена базы
	bool renewDB(wxString name, std::vector<wxString> tblMachine/* = { wxT("Штабеллёры"), wxT("Электротележки"), wxT("Комплектовщики"), wxT("ЭлТележки_с_площ") }*/, std::vector<wxString> tblList/* = { wxT("списокШтабеллёры"), wxT("списокЭлектротележки"), wxT("списокКомплектовщики"), wxT("списокЭл_тел_отк_пл") }*/);
	bool iniTDB(wxString name);

	//Выдача техники
    bool outputUnit(wxString login, wxString numUnit);
    bool outputUnit(wxString lSN);
    //Отправка техники в ремонт
    bool sendToRepair(wxString numUnit, wxString srcTblName);
    bool sendToRepair(wxString lSN);
	//Показать весь список техники с заголовками
	std::vector<std::vector<wxString>> getAll(wxString tableName, bool onlyInWork = false);
	//Без заголовков
	std::vector<std::vector<wxString>> getAllNeed(bool onlyInWork = false);
	std::vector<std::vector<wxString>> getAllNeedRem();
	std::vector<std::vector<wxString>> getBatteryList();
    Binds getBinds(wxString lSN);
    wxString getBindLogin(wxString lSN);
	//Выдача батареи
	bool outputBattery(wxString numUnit);
	//Приём батареи
	bool inputBattery(wxString numUnit, wxString numHours);
	//Статус батареи
	wxString getStatusBattery(wxString numUnit);
	//bool doSomething(wxString numUnit, int numHours, wxString login);

	void initVar();

	//Приём техники
	bool inputUnit(wxString numUnit, int numHours);
    bool inputUnit(wxString lSN);
	//Получить ошибку
	wxString getErrMsg();
	//Запись результатов в файл
	//bool exportXLSX(wxString path);
	bool exportXLSX(wxString path);
	bool exportCSV(wxString path);

	//Создание таблицы 
    bool createTable(wxString tableName, std::vector<field> lFields);
    bool createAndCheckAllTable();
	//Показать список имеющейся техники
	std::vector<std::vector<wxString>> getSourceUnit(int numTable);
	//Удалить технику
	bool removeUnit(wxString numUnit, int numTable);
	bool removeBattery(wxString numUnit);
    bool removeBind(wxString lSN);
	//Добавить технику
	bool addUnit(wxString numUnit, wxString nameUnit, int numTable);
    bool addBattery(wxString numUnit, wxString nameUnit);
    bool addBind(wxString lSN, wxString lLogin);
	//Редактировать технику
	bool editUnit(wxString oldNum, wxString oldName, wxString numUnit, wxString nameUnit, int numTable);
    bool editBattery(wxString oldNum, wxString oldName, wxString numUnit, wxString nameUnit);
    bool editBind(wxString oldSN, wxString oldLogin, wxString lSN, wxString lLogin);
	//Проверка что техника не существует
	bool isHave(wxString numUnit);
    bool isHaveBindSN(wxString lSN);
    bool isHaveBindLogin(wxString lLogin);
	bool isHaveBattery(wxString numUnit);

	//Проверка корректности названия столбцов
    bool checkTable(wxString tableName, std::vector<field> lFields);

	bool getDbIsCorrect();
	//Оператор присваивания
	//MyDB& operator=(MyDB& v);
	//Получить имена таблиц
	std::vector<wxString> getSourceTableName() { return tblSource; }
	//Вспомагательные функции
	wxString getNowDate();
	wxString diffTime(wxString dateFirst, wxString dateLast);
	wxString getUserName();


private:
	void WriteList(lxw_workbook  *workbook, wxString tableName, std::vector<double> sizeColumnExcelTable);
	wxSQLite3Database db;	//База данных для работы
	std::vector<wxString> tblSource;		//Таблица с исходными данными
	std::vector<wxString> tblWork;		//Основна таблица учёта
	wxString tblRepair;			//Таблица ремонта
	wxString tblBattery;		//Рабочая таблица для аккумуляторов
	wxString tblHistoryBattery;	//История использования
    wxString tblBindUnits;		//Таблица для связи логинов и техники
	wxString loginRemont;		//Логин для ремонта
	wxString errMsg;
	std::vector<wxString> sortWork;	//Вывод всех\в работе
    std::vector<field> fields;	//Поля основной таблицы
    std::vector<field> mSrcFields;
    std::vector<field> mBindFields;
	std::vector<field> mBatteryFields;
	std::vector<field> mHistoryBatteryFields;
	Column column;
	bool dbIsCorrect;
    wxString userName;
    Binds errorBind;	//Маркер ошибки
    wxString errorTxtBind;


};

#endif	//SQLITE_H
