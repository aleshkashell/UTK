﻿#include "sqlite.h"

MyDB::MyDB(wxString name, std::vector<wxString> tblMachine, std::vector<wxString> tblList) //: tblSource(tblList), tblWork(tblMachine)
{
	tblSource = tblList;
	tblWork = tblMachine;
    tblRepair = wxT("Ремонт");
    tblBindUnits = wxT("Связанный список");
    tblBattery = wxT("Аккумуляторы_тек");
    tblHistoryBattery = wxT("Аккумуляторы");
    loginRemont = wxT("remont");
	initVar();
    errMsg = wxT("error is not touch");
	if (!iniTDB(name)) {
		errMsg = wxT("Инициализирована некорректная БД");
		myFunc::writeLog(errMsg);
		dbIsCorrect = false;
	}
	else dbIsCorrect = true;
	
}
bool MyDB::createAndCheckAllTable(){
    for(auto name: tblSource) if(!createTable(name, mSrcFields)) return false;
    for(auto name: tblWork) if(!createTable(name, fields)) return false;
    if(!createTable(tblRepair, fields)) return false;
    if(!createTable(tblBattery, mBatteryFields)) return false;
    if(!createTable(tblHistoryBattery, mHistoryBatteryFields)) return false;
    if(!createTable(tblBindUnits, mBindFields)) return false;
    return true;
}
bool MyDB::iniTDB(wxString name)
{
	if (db.IsOpen())
		db.Close();
	errMsg = "error is not touch";
	try {
		db.Open(name);
		if (!db.IsOpen()) {
			std::cerr << "Database is not open" << std::endl;
            return false;
		}
        //Проверка, что нужные таблицы существуют, а также проверка на необходимые поля
        createAndCheckAllTable();
	}
	catch (wxSQLite3Exception &e)
	{
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}

	return true;
}
bool MyDB::renewDB(wxString name, std::vector<wxString> tblMachine, std::vector<wxString> tblList)
{
	if (db.IsOpen()) db.Close();
	errMsg = "error is not touch";
	db.Open(name);
	if (!db.IsOpen()) {
		errMsg = wxT("Невозможно открыть БД");
		myFunc::writeLog(wxT("Database is not open"));
		dbIsCorrect = false;
		return false;
	}
	//Проверка, что нужные таблицы существуют
    if(!createAndCheckAllTable()){
        myFunc::writeLog(wxT("Create table false"));
		dbIsCorrect = false;
		return false;
	}
	dbIsCorrect = true;
	return true;
}
bool MyDB::outputUnit(wxString login, wxString numUnit)
{
	//wxString numUnitStr = wxString::Format(wxT("%i"), numUnit);
	try
	{
		//Для каждой рабочей таблицы
		for (auto tableName : tblWork) {
			wxString sqlRequest = wxT("select count(ID) from ") + tableName + wxT(" where \"") + column.numUnit.name + wxT("\" = \"") + numUnit +
				wxT("\" and \"") + column.status.name + wxT("\" = \"в работе\";");
			//Проверка, что техника не выдана.
			if (db.ExecuteScalar(sqlRequest) != 0)
			{
				errMsg = wxT("Техника с таким номером уже выдана");
				return false;
			}
		}
		//Для таблицы ремонта
        wxString sqlRequestRem = wxT("select count(ID) from \"") + tblRepair + wxT("\" where \"") + column.numUnit.name + wxT("\" = \"") + numUnit +
			wxT("\" and \"") + column.status.name + wxT("\" = \"в ремонте\";");
		//Проверка, что техника не выдана.
		if (db.ExecuteScalar(sqlRequestRem) != 0)
		{
			errMsg = wxT("Техника находится в ремонте");
			return false;
		}
		//Проверка, что техника существует
		bool find = false;
		wxString workTableName;
		wxString sourceTableName;
		for (auto i = 0; i < tblSource.size(); i++) {
			wxString sqlRequest = wxT("select count(ID) from ") + tblSource[i] + wxT(" where \"") + column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\";");
			if (db.ExecuteScalar(sqlRequest) > 0) {
				find = true;
				workTableName = tblWork[i];
				sourceTableName = tblSource[i];
			}
		}
		if (!find) {
			errMsg = wxT("Техники с таким номером не существует.");
			return false;
		}
		if (login == loginRemont) {
			//Запись в таблицу ремонт
			sendToRepair(numUnit, sourceTableName);
			return true;
		}
		else {
			//Выдача INSERT INTO machine (numUnit, login, dateOut, status, nameUnit) VALUES (numUnitStr, login, getNowDate(), \"в работе\", (SELECT nameUnit FROM listOfTech WHERE nameUnit = numUnitStr));
			wxString sqlRequest = wxT("INSERT INTO ") + workTableName + wxT("(\"") + column.numUnit.name + wxT("\", \"") + column.login.name + wxT("\", \"")
				+ column.dateOut.name + wxT("\", \"") + column.status.name + wxT("\", \"") + column.nameUnit.name + wxT("\", \"")
				+ column.whoOutput.name + wxT("\") VALUES (\"")
				+ numUnit + wxT("\", \"") + login + wxT("\", \"") + getNowDate() + wxT("\", \"в работе\", (SELECT \"") + column.nameUnit.name
				+ wxT("\" FROM ") + sourceTableName + wxT(" WHERE \"") + column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\"), \"")
				+ userName + wxT("\");");
			db.ExecuteUpdate(sqlRequest);
			return true;
		}
	}
	catch(wxSQLite3Exception &e)
	{
		errMsg =  e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
}
bool MyDB::outputUnit(wxString lSN){
    auto answer = getBinds(lSN);
    if(answer == errorBind) return false;
    return outputUnit(answer.login, answer.numUnit);
}
bool MyDB::outputBattery(wxString numUnit) {
	try {
		wxString sqlRequest = wxT("UPDATE \"") + tblBattery + wxT("\" SET \"") + column.dateOut.name + wxT("\" = \"") + getNowDate() +
			wxT("\", \"") + column.status.name + wxT("\" = \"В работе") +
			wxT("\", \"") + column.whoOutput.name + wxT("\" = \"") + getUserName() + wxT("\" WHERE \"") +
			column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\"");
		db.ExecuteUpdate(sqlRequest);
		return true;
	}
	catch (wxSQLite3Exception &e)
	{
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
}
wxString MyDB::getStatusBattery(wxString numUnit) {
	try {
		wxString sqlRequest = wxT("SELECT \"") + column.status.name + wxT("\" FROM \"") + tblBattery +
			wxT("\" WHERE \"") + column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\"");
		auto result = db.ExecuteQuery(sqlRequest);
		if (result.NextRow()) {
			return result.GetAsString(4);	//Номер столбца со статусом
		}
		else {
			return wxT("Error");
		}
	}
	catch (wxSQLite3Exception &e)
	{
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		return wxT("error SQL");
	}
}
bool MyDB::sendToRepair(wxString numUnit, wxString srcTblName)
{
	//Выдача INSERT INTO machine (numUnit, login, dateOut, status, nameUnit) VALUES (numUnitStr, login, getNowDate(), \"в работе\", (SELECT nameUnit FROM listOfTech WHERE nameUnit = numUnitStr));
	wxString sqlRequest = wxT("INSERT INTO ") + tblRepair + wxT("(\"") + column.numUnit.name + wxT("\", \"") + column.login.name + wxT("\", \"")
		+ column.dateOut.name + wxT("\", \"") + column.status.name + wxT("\", \"") + column.nameUnit.name + wxT("\", \"")
		+ column.whoOutput.name + wxT("\") VALUES (\"")
		+ numUnit + wxT("\", \"") + loginRemont + wxT("\", \"") + getNowDate() + wxT("\", \"в ремонте\", (SELECT \"") + column.nameUnit.name
		+ wxT("\" FROM ") + srcTblName + wxT(" WHERE \"") + column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\"), \"")
		+ userName + wxT("\");");
	db.ExecuteUpdate(sqlRequest);
	return true;
}
wxString MyDB::getNowDate()
{
	return wxDateTime::Now().FormatISOCombined(' ');
}
wxString MyDB::diffTime(wxString dateFirst, wxString dateLast)
{
	wxDateTime dtFirst;
	wxDateTime dtLast;
	wxString::const_iterator end;
	dtFirst.ParseDateTime(dateFirst, &end);
	dtLast.ParseDateTime(dateLast, &end);
	wxTimeSpan ts = dtLast - dtFirst;
	return wxString::Format(wxT("%i"), ts.GetHours());
}
wxString MyDB::getUserName()
{
#ifdef __WIN32
    wchar_t buffer[UNLEN + 1];
	DWORD len = UNLEN + 1;
	GetUserName(buffer, &len);
    return wxString(buffer);
#elif __LINUX__
    return wxString(wxT("Пользователь Linux"));
#elif _WINDOWS
    wchar_t buffer[UNLEN + 1];
    DWORD len = UNLEN + 1;
    GetUserName(buffer, &len);
    return wxString(buffer);
#endif
    return wxString(wxT("Unknown system"));
}
Binds MyDB::getBinds(wxString lSN){
    if(!isHaveBindSN(lSN)) return errorBind;
    wxString sqlRequest = wxT("SELECT \"") + column.login.name + wxT("\", \"") + column.numUnit.name +
            wxT("\" FROM \"") + tblBindUnits + wxT("\" WHERE \"") + column.serialNumber.name + wxT("\" = \"") +
            lSN + wxT("\"");
    try{
        auto result = db.ExecuteQuery(sqlRequest);
        result.NextRow();
        return Binds(result.GetAsString(0), result.GetAsString(1));
    }
    catch(wxSQLite3Exception &e)
    {
        errMsg =  e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        return errorBind;
    }

}
wxString MyDB::getBindLogin(wxString lSN){
    if(!isHaveBindSN(lSN)) return errorTxtBind;
    wxString sqlRequest = wxT("SELECT \"") + column.login.name +
            wxT("\" FROM \"") + tblBindUnits + wxT("\" WHERE \"") + column.serialNumber.name + wxT("\" = \"") +
            lSN + wxT("\"");
    try{
        auto result = db.ExecuteQuery(sqlRequest);
        result.NextRow();
        return result.GetAsString(0);
    }
    catch(wxSQLite3Exception &e)
    {
        errMsg =  e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        return errorTxtBind;
    }
}

std::vector<std::vector<wxString>> MyDB::getAllNeed(bool onlyInWork)
{
	try
	{
		std::vector<wxString> unionRequest;
		//Делаем массив запросов к разным таблицам
		for (auto tableName : tblWork) {
			unionRequest.push_back(wxT("SELECT \"") + column.numUnit.name + wxT("\", \"") + column.login.name + wxT("\", \"") + \
				column.dateOut.name + wxT("\", \"") + column.timeInWork.name + wxT("\", \"") + column.status.name + wxT("\", \"") + \
				column.nameUnit.name + wxT("\", \"") + column.whoOutput.name + wxT("\" FROM ") + tableName + sortWork[onlyInWork]);
		}
		wxString sqlRequest = unionRequest[0];
		//И объединяем в один запрос
		for (int i = 1; i < unionRequest.size(); i++) {
			sqlRequest += wxT(" UNION ") + unionRequest[i];
		}
		/*wxString sqlRequest = wxT("SELECT \"") + column.numUnit.name + wxT("\", \"") + column.login.name + wxT("\", \"") + \
			column.dateOut.name + wxT("\", \"") + column.timeInWork.name + wxT("\", \"") + column.status.name + wxT("\", \"") + \
			column.nameUnit.name + wxT("\" FROM ") + tblWork[0] + sortWork[onlyInWork];*/
		wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
		int iter = 0;
		std::vector<std::vector<wxString>> answer;
		while(result.NextRow())
		{
			answer.push_back(std::vector<wxString>(result.GetColumnCount()));
			for(int i = 0; i < result.GetColumnCount(); i++)
			{
				answer[iter][i] = result.GetAsString(i);
			}
			iter++;
		}
		return answer;
	}
	catch(wxSQLite3Exception &e)
	{
		errMsg =  e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
	}
}
std::vector<std::vector<wxString>> MyDB::getAllNeedRem() {
	try
	{
		wxString sqlRequest = wxT("SELECT \"") + column.numUnit.name + wxT("\", \"") + column.login.name + wxT("\", \"") + \
			column.dateOut.name + wxT("\", \"") + column.timeInWork.name + wxT("\", \"") + column.status.name + wxT("\", \"") + \
			column.nameUnit.name + wxT("\", \"") + column.whoOutput.name + wxT("\" FROM ") + tblRepair + wxT(" WHERE \"") + \
			column.status.name + wxT("\" = \"в ремонте\";");
		/*wxString sqlRequest = wxT("SELECT \"") + column.numUnit.name + wxT("\", \"") + column.login.name + wxT("\", \"") + \
		column.dateOut.name + wxT("\", \"") + column.timeInWork.name + wxT("\", \"") + column.status.name + wxT("\", \"") + \
		column.nameUnit.name + wxT("\" FROM ") + tblWork[0] + sortWork[onlyInWork];*/
		wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
		int iter = 0;
		std::vector<std::vector<wxString>> answer;
		while (result.NextRow())
		{
			answer.push_back(std::vector<wxString>(result.GetColumnCount()));
			for (int i = 0; i < result.GetColumnCount(); i++)
			{
				answer[iter][i] = result.GetAsString(i);
			}
			iter++;
		}
		return answer;
	}
	catch (wxSQLite3Exception &e)
	{
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
	}
}
std::vector<std::vector<wxString>> MyDB::getBatteryList() {
	try {
		wxString sqlRequest = wxT("SELECT * FROM ") + tblBattery;
		wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
		int iter = 0;
		std::vector<std::vector<wxString>> answer;
		while (result.NextRow())
		{
			answer.push_back(std::vector<wxString>(result.GetColumnCount()));
			for (int i = 0; i < result.GetColumnCount()-1; i++)	//Первый элемент ID
			{
				answer[iter][i] = result.GetAsString(i+1);
			}
			iter++;
		}
		return answer;
	}
	catch (wxSQLite3Exception &e)
	{
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
	}
}
void MyDB::initVar()
{
    errorBind = Binds(wxT("NOTHING"), wxT("NOTHING"));
    errorTxtBind = wxT("NOTHING");
	userName = getUserName();
	sortWork.push_back("");
	sortWork.push_back(wxT(" WHERE \"") + column.status.name + wxT("\" = \"в работе\""));
	
	fields.push_back(column.numUnit);
	fields.push_back(column.login);
	fields.push_back(column.numHours);
	fields.push_back(column.dateOut);
	fields.push_back(column.dateIn);
	fields.push_back(column.timeInWork);
	fields.push_back(column.status);
	fields.push_back(column.nameUnit);
	fields.push_back(column.whoOutput);
    mSrcFields.push_back(column.numUnit);
    mSrcFields.push_back(column.nameUnit);
    mBindFields.push_back(column.serialNumber);
    mBindFields.push_back(column.login);
    //mBindFields.push_back(column.numUnit);
	mBatteryFields.push_back(column.numUnit);
	mBatteryFields.push_back(column.status);
	mBatteryFields.push_back(column.dateIn);
	mBatteryFields.push_back(column.dateOut);
	mBatteryFields.push_back(column.timeCharge);
	mBatteryFields.push_back(column.timeInWork);
	mBatteryFields.push_back(column.whoInput);
	mBatteryFields.push_back(column.whoOutput);
	mBatteryFields.push_back(column.nameUnit);
	mHistoryBatteryFields.push_back(column.numUnit);
	mHistoryBatteryFields.push_back(column.status);
	mHistoryBatteryFields.push_back(column.dateIn);
	mHistoryBatteryFields.push_back(column.dateOut);
	mHistoryBatteryFields.push_back(column.timeCharge);
	mHistoryBatteryFields.push_back(column.timeInWork);
	mHistoryBatteryFields.push_back(column.whoInput);
	mHistoryBatteryFields.push_back(column.whoOutput);
	mHistoryBatteryFields.push_back(column.nameUnit);
}
std::vector<std::vector<wxString>> MyDB::getAll(wxString tableName, bool onlyInWork)
{
	try
	{
		wxString sqlRequest = wxT("SELECT * FROM \"") + tableName + wxT("\";");
		wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
		int iter = 0;
		std::vector<std::vector<wxString>> answer;
		answer.push_back(std::vector<wxString>(result.GetColumnCount()));
		for(int i = 0; i < result.GetColumnCount(); i++)
			answer[iter][i] = result.GetColumnName(i);
		iter++;
		while(result.NextRow())
		{
			answer.push_back(std::vector<wxString>(result.GetColumnCount()));
			for(int i = 0; i < result.GetColumnCount(); i++)
			{
				answer[iter][i] = result.GetAsString(i);
			}
			iter++;
		}
		return answer;
	}
	catch(wxSQLite3Exception &e)
	{
		errMsg =  e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
	}
}
bool MyDB::inputUnit(wxString numUnit, int numHours)
{
	wxString numUnitStr = wxString::Format(wxT("%i"), numUnit);
	wxString numHoursStr = wxString::Format(wxT("%i"), numHours);
	try
	{
		//Проверка присутствия техники среди выданных
		bool find = false;
		wxString workTableName;
		for (auto tableName : tblWork) {
			wxString sqlRequest = wxT("select count(*) from ") + tableName + wxT(" where \"") + column.numUnit.name + wxT("\" = \"") + numUnit + \
				wxT("\" and \"") + column.status.name + wxT("\" = \"в работе\";");
			if (db.ExecuteScalar(sqlRequest) > 0) {
				find = true;
				workTableName = tableName;
				break;
			}
		}
		wxString sqlRequestRem = wxT("select count(*) from ") + tblRepair + wxT(" where \"") + column.numUnit.name + wxT("\" = \"") + numUnit + \
			wxT("\" and \"") + column.status.name + wxT("\" = \"в ремонте\";");
		if (db.ExecuteScalar(sqlRequestRem) > 0) {
			find = true;
			workTableName = tblRepair;
		}
		if(!find){
			errMsg = wxT("Техника с номером ") + numUnit + wxT(" отсутствует среди выданных.");
			myFunc::writeLog(errMsg);
			return false;
		}
		//Получение времени выдачи
		//SELECT "Дата выдачи" FROM machine WHERE numUnit = numUnitStr AND status = "в работе"
		wxString sqlRequest = wxT("SELECT \"") + column.dateOut.name + wxT("\" FROM ") + workTableName + wxT(" WHERE \"") + column.numUnit.name + wxT("\" = \"") + numUnit + \
			wxT("\" AND \"") + column.status.name;
		if(workTableName == tblRepair) sqlRequest += wxT("\" = \"в ремонте\";");
		else sqlRequest += wxT("\" = \"в работе\";");
		wxString timeOutputStr;
		wxString timeNow = wxDateTime::Now().FormatISOCombined(' ');
		wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
		result.NextRow();
		timeOutputStr = result.GetAsString(0);
		wxString hoursInWorkStr = diffTime(timeOutputStr, timeNow);
		//Приём техники
		sqlRequest = wxT("UPDATE ") + workTableName + wxT(" SET \"") + column.dateIn.name + wxT("\" = \"") + timeNow +
			wxT("\", \"") + column.timeInWork.name + wxT("\" = \"") + hoursInWorkStr +
			wxT("\", \"") + column.numHours.name + wxT("\" = \"") + numHoursStr +
			wxT("\",  \"") + column.status.name + wxT("\" = \"Сдана\" WHERE  \"") + column.numUnit.name + wxT("\" = \"") + numUnit +
			wxT("\" and  \"") + column.status.name;
		if(workTableName == tblRepair) sqlRequest += wxT("\" = \"в ремонте\";");
		else sqlRequest += wxT("\" = \"в работе\";");
		db.ExecuteUpdate(sqlRequest);		
		return true;
	}
	catch(wxSQLite3Exception &e)
	{
		errMsg =  e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
}
bool MyDB::inputUnit(wxString lSN){
    auto answer = getBinds(lSN);
    if(answer == errorBind) return false;
    return inputUnit(answer.numUnit, -1);
}

bool MyDB::inputBattery(wxString numUnit, wxString numHours) {
	//Копируем строку в вектор
	wxSQLite3ResultSet result;
	std::vector<wxString> line;
	wxString sqlRequest = wxT("SELECT * FROM \"") + tblBattery + wxT("\" WHERE \"") +
		column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\"");
	try	{
		result = db.ExecuteQuery(sqlRequest);
		result.NextRow();
		for (auto i = 1; i < result.GetColumnCount(); ++i) {
			line.push_back(result.GetAsString(i));
		}
	}
	catch (wxSQLite3Exception &e){
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
	//Добавляем данные
	line[1] = "Принято";
	line[5] = numHours;
	line[4] = diffTime(line[2], line[3]);
	//Записываем в историю
	try {
		sqlRequest = wxT("INSERT INTO \"") + tblHistoryBattery + wxT("\" (\"") + column.numUnit.name;
		for (auto i = 1; i < mHistoryBatteryFields.size(); i++) {
			sqlRequest += wxT("\", \"") + mHistoryBatteryFields[i].name;
		}
		sqlRequest += wxT("\") VALUES (\"") + line[0];
		for (auto i = 1; i < line.size(); i++) {
			sqlRequest += wxT("\", \"") + line[i];
		}
		sqlRequest += wxT("\")");
		db.ExecuteQuery(sqlRequest);
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
	//Обновляем данные в основной таблице
	try {
		sqlRequest = wxT("UPDATE \"") + tblBattery + wxT("\" SET \"") + 
			column.status.name + wxT("\" = \"Заряжается\", \"") +
			column.dateIn.name + wxT("\" = \"") + getNowDate() + wxT("\", \"") +
			column.dateOut.name + wxT("\" = \"\", \"") + 
			column.timeCharge.name + wxT("\" = \"\", \"") +
			column.timeInWork.name + wxT("\" = \"\", \"") +
			column.whoInput.name + wxT("\" = \"") + getUserName() + wxT("\", \"") +
			column.whoOutput.name + wxT("\" = \"\"") +
			wxT(" WHERE \"") + column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\"");
		db.ExecuteUpdate(sqlRequest);
		return true;
	}
	catch (wxSQLite3Exception &e)
	{
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
}
wxString MyDB::getErrMsg()
{
	return errMsg;
}
bool MyDB::exportCSV(wxString path)
{
	std::vector<std::vector<wxString>> result;
	wxFile file;
	if(!wxFile::Exists(path))
		file.Create(path, true);
	file.Open(path, wxFile::write);
	if(!file.IsOpened())
	{
		std::cout << "Файл не открыт." << std::endl;
		std::cout << path << std::endl;
		return false;
	}
	const char *bom = "\xef\xbb\xbf";
	file.Write(bom, 3);
	for (auto tableName : tblWork) {
		result = getAll(tableName);
		for (auto row : result)
		{
			for (auto word : row)
			{
				file.Write(word + wxT(";"));
			}
			file.Write(wxTextFile::GetEOL());
		}
	}
	file.Close();
	return true;
}
bool MyDB::exportXLSX(wxString path) {
	// Create a new workbook
	lxw_workbook  *workbook = workbook_new(path.ToUTF8());
	std::vector<double> sizeColumnExcelTable = { 6, 15, 27, 16.38, 18.38, 18.38, 13.38, 8.38, 55.0 };
	std::vector<double> sizeColumnUPS = { 6, 15, 15, 18.38, 18.38, 13.38, 13.38, 27, 27, 55.0 };
	for (auto tblName : tblWork) {
		WriteList(workbook, tblName, sizeColumnExcelTable);
	}
	WriteList(workbook, tblRepair, sizeColumnExcelTable);
	WriteList(workbook, tblHistoryBattery, sizeColumnUPS);
	WriteList(workbook, tblBattery, sizeColumnUPS);
	/*//Для каждой таблицы в рабочей БД
	for (auto tblName : tblWork) {
		//Создаём лист
		lxw_worksheet *worksheet = workbook_add_worksheet(workbook, tblName.ToUTF8());
		auto result = getAll(tblName);
		for (auto row = 0; row < result.size(); row++) {
			for (auto column = 0; column < result[row].size(); column++) {
				worksheet_write_string(worksheet, row, column, result[row][column].ToUTF8(), format);
			}
		}
		for (auto iter = 0; iter < sizeColumnExcelTable.size(); iter++)
			worksheet_set_column(worksheet, iter, iter, sizeColumnExcelTable[iter], NULL);			
	}
	//Таблица ремонта
	{
		lxw_worksheet *worksheet = workbook_add_worksheet(workbook, tblRepair.ToUTF8());
		auto result = getAll(tblRepair);
		for (auto row = 0; row < result.size(); row++) {
			for (auto column = 0; column < result[row].size(); column++) {
				worksheet_write_string(worksheet, row, column, result[row][column].ToUTF8(), format);
			}
		}
		for (auto iter = 0; iter < sizeColumnExcelTable.size(); iter++)
			worksheet_set_column(worksheet, iter, iter, sizeColumnExcelTable[iter], NULL);
	}
	//Таблица истории аккумуляторов
	{
		lxw_worksheet *worksheet = workbook_add_worksheet(workbook, tblHistoryBattery.ToUTF8());
		auto result = getAll(tblHistoryBattery);
		for (auto row = 0; row < result.size(); row++) {
			for (auto column = 0; column < result[row].size(); column++) {
				worksheet_write_string(worksheet, row, column, result[row][column].ToUTF8(), format);
			}
		}
		for (auto iter = 0; iter < sizeColumnExcelTable.size(); iter++)
			worksheet_set_column(worksheet, iter, iter, sizeColumnExcelTable[iter], NULL);
	}*/
	auto error = workbook_close(workbook);
	switch (error)
	{
	case LXW_NO_ERROR:
		return true;
	case LXW_ERROR_MEMORY_MALLOC_FAILED:
		errMsg = "LXW_ERROR_MEMORY_MALLOC_FAILED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_CREATING_XLSX_FILE:
		errMsg = "LXW_ERROR_CREATING_XLSX_FILE";
		myFunc::writeLog(errMsg); 
		break;
	case LXW_ERROR_CREATING_TMPFILE:
		errMsg = "LXW_ERROR_CREATING_TMPFILE";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_ZIP_FILE_OPERATION:
		errMsg = "LXW_ERROR_ZIP_FILE_OPERATION";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_ZIP_FILE_ADD:
		errMsg = "LXW_ERROR_ZIP_FILE_ADD";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_ZIP_CLOSE:
		errMsg = "LXW_ERROR_ZIP_CLOSE";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_NULL_PARAMETER_IGNORED:
		errMsg = "LXW_ERROR_NULL_PARAMETER_IGNORED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_PARAMETER_VALIDATION:
		errMsg = "LXW_ERROR_PARAMETER_VALIDATION";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_SHEETNAME_LENGTH_EXCEEDED:
		errMsg = "LXW_ERROR_SHEETNAME_LENGTH_EXCEEDED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_INVALID_SHEETNAME_CHARACTER:
		errMsg = "LXW_ERROR_INVALID_SHEETNAME_CHARACTER";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_SHEETNAME_ALREADY_USED:
		errMsg = "LXW_ERROR_SHEETNAME_ALREADY_USED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_128_STRING_LENGTH_EXCEEDED:
		errMsg = "LXW_ERROR_128_STRING_LENGTH_EXCEEDED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_255_STRING_LENGTH_EXCEEDED:
		errMsg = "LXW_ERROR_255_STRING_LENGTH_EXCEEDED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_MAX_STRING_LENGTH_EXCEEDED:
		errMsg = "LXW_ERROR_MAX_STRING_LENGTH_EXCEEDED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_SHARED_STRING_INDEX_NOT_FOUND:
		errMsg = "LXW_ERROR_SHARED_STRING_INDEX_NOT_FOUND";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_WORKSHEET_INDEX_OUT_OF_RANGE:
		errMsg = "LXW_ERROR_WORKSHEET_INDEX_OUT_OF_RANGE";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_WORKSHEET_MAX_NUMBER_URLS_EXCEEDED:
		errMsg = "LXW_ERROR_WORKSHEET_MAX_NUMBER_URLS_EXCEEDED";
		myFunc::writeLog(errMsg);
		break;
	case LXW_ERROR_IMAGE_DIMENSIONS:
		errMsg = "LXW_ERROR_IMAGE_DIMENSIONS";
		myFunc::writeLog(errMsg);
		break;
	case LXW_MAX_ERRNO:
		errMsg = "LXW_MAX_ERRNO";
		myFunc::writeLog(errMsg);
		break;
	default:
		errMsg = "unknown error";
		break;
	}
	return false;
}
void MyDB::WriteList(lxw_workbook  *workbook, wxString tableName, std::vector<double> sizeColumnExcelTable) {
	//Создаём формат
	lxw_format *format = workbook_add_format(workbook);
	format_set_border(format, LXW_BORDER_THIN);
	//Создаём лист
	lxw_worksheet *worksheet = workbook_add_worksheet(workbook, tableName.ToUTF8());
	auto result = getAll(tableName);
	for (auto row = 0; row < result.size(); row++) {
		for (auto column = 0; column < result[row].size(); column++) {
			worksheet_write_string(worksheet, row, column, result[row][column].ToUTF8(), format);
		}
	}
	//Применяем размеры столбцов
	for (auto iter = 0; iter < sizeColumnExcelTable.size(); iter++)
		worksheet_set_column(worksheet, iter, iter, sizeColumnExcelTable[iter], NULL);
}
bool MyDB::createTable(wxString tableName, std::vector<field> lFields){
    //Если таблица существует
    if(db.TableExists(tableName)){
        if(checkTable(tableName, lFields)){
            return true;
        }
        else{
            return false;
        }
    }
    wxString request = wxT("CREATE TABLE \"") + tableName + wxT("\" (ID INTEGER primary key");
    for (auto x : lFields) {
        request += wxT(", \"") + x.name + wxT("\" ") + x.type;
    }
    request += wxT(")");
    try {
        db.ExecuteUpdate(request);
    }
    catch (wxSQLite3Exception &e) {
        errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        myFunc::writeLog(wxT("Таблица: ") + tableName + wxT("; Req = \"") + request + wxT("\""));
        return false;
    }
    return true;

}
std::vector<std::vector<wxString>> MyDB::getSourceUnit(int numTable)
{
	if (numTable < tblSource.size()) {
        wxString request = wxT("SELECT * FROM \"") + tblSource[numTable] + wxT("\"");
		wxSQLite3ResultSet result;
		std::vector<std::vector<wxString>> answer;
		try {
			result = db.ExecuteQuery(request);
			int iter = 0;
			while (result.NextRow()) {
				answer.push_back(std::vector<wxString>(result.GetColumnCount()));
				for (auto i = 0; i < result.GetColumnCount(); i++) {
					answer[iter][i] = result.GetAsString(i);
				}
				iter++;
			}
			return answer;
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
		}
    }
    else if(numTable == tblSource.size()){
        wxString request = wxT("SELECT * FROM \"") + tblBattery + wxT("\"");
		wxSQLite3ResultSet result;
//		int numColumn = 3;	//Количество колонок для загрузки (ID, имя, имя техники)
		std::vector<std::vector<wxString>> answer;
		try {
			result = db.ExecuteQuery(request);
			while (result.NextRow()) {
				answer.push_back(std::vector<wxString>{
					result.GetAsString(0),result.GetAsString(1), result.GetAsString(9)
				});
			}
			return answer;
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
		}
    }
    else {
        wxString request = wxT("SELECT * FROM \"") + tblBindUnits + wxT("\"");
        wxSQLite3ResultSet result;
        std::vector<std::vector<wxString>> answer;
        try {
            result = db.ExecuteQuery(request);
            while (result.NextRow()) {
                answer.push_back(std::vector<wxString>{
                    //0 - ID
                    result.GetAsString(1), result.GetAsString(2)
                });
            }
            return answer;
        }
        catch (wxSQLite3Exception &e) {
            errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
            myFunc::writeLog(errMsg);
        }

    }
}
bool MyDB::removeUnit(wxString numUnit, int numTable)
{
	if (numTable < tblSource.size()) {
		wxString sqlRequest = wxT("DELETE FROM ") + tblSource[numTable] + wxT(" WHERE  \"") + column.numUnit.name + wxT("\" =\"") + numUnit + wxT("\";");
		try {
			db.ExecuteUpdate(sqlRequest);
			return true;
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
			return false;
		}
	}
	else {
		return removeBattery(numUnit);
	}
}
bool MyDB::removeBind(wxString lSN){
    wxString sqlRequest = wxT("DELETE FROM \"") + tblBindUnits + wxT("\" WHERE \"") + column.serialNumber.name + wxT("\" = \"") + lSN + wxT("\"");
    try {
        db.ExecuteUpdate(sqlRequest);
        return true;
    }
    catch (wxSQLite3Exception &e) {
        errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        return false;
    }
}

bool MyDB::removeBattery(wxString numUnit) {
	wxString sqlRequest = wxT("DELETE FROM ") + tblBattery + wxT(" WHERE  \"") + column.numUnit.name + wxT("\" =\"") + numUnit + wxT("\";");
	try {
		db.ExecuteUpdate(sqlRequest);
		return true;
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
}
bool MyDB::addUnit(wxString numUnit, wxString nameUnit, int numTable)
{
	if (numTable < tblSource.size()) {
		if (isHave(numUnit)) return false;
		try {
            wxString sqlRequest = wxT("INSERT INTO ") + tblSource[numTable] + wxT("(\"") + column.numUnit.name + wxT("\", \"") +
				column.nameUnit.name + wxT("\") VALUES(\"") + numUnit + wxT("\", \"") + nameUnit + wxT("\");");
			db.ExecuteUpdate(sqlRequest);
			return true;
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
			return false;
		}
	}
	else {
		return addBattery(numUnit, nameUnit);
	}
}
bool MyDB::addBattery(wxString numUnit, wxString nameUnit) {
	if (isHaveBattery(numUnit)) return false;
	try {
		wxString sqlRequest = wxT("INSERT INTO ") + tblBattery + wxT("(\"") + column.numUnit.name + wxT("\", \"") +
			column.nameUnit.name + wxT("\", \"") + column.dateIn.name + wxT("\", \"") + column.status.name + wxT("\") VALUES(\"") + 
			numUnit + wxT("\", \"") + nameUnit + wxT("\", \"") + getNowDate() + wxT("\", \"Заряжается\");");
		db.ExecuteUpdate(sqlRequest);
		return true;
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
}
bool MyDB::addBind(wxString lSN, wxString lLogin){
    if(isHaveBindSN(lSN) || isHaveBindLogin(lLogin)) return false;
    try{
        wxString sqlRequest = wxT("INSERT INTO \"") + tblBindUnits + wxT("\" (\"") + column.serialNumber.name + wxT("\", \"") +
                column.login.name + wxT("\") VALUES(\"") +
                lSN + wxT("\", \"") + lLogin + wxT("\");");
        db.ExecuteUpdate(sqlRequest);
        return true;
    }
    catch(wxSQLite3Exception &e){
        errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        return false;
    }
}
bool MyDB::editBind(wxString oldSN, wxString oldLogin, wxString lSN, wxString lLogin){
    if(oldSN == lSN){
        if(oldLogin != lLogin)
            if(isHaveBindLogin(lLogin)) return false;
        wxString sqlRequest = wxT("UPDATE \"") + tblBindUnits + wxT("\" SET \"") + column.serialNumber.name + wxT("\" = \"") +
            lSN + wxT("\", \"") + column.login.name + wxT("\" = \"") + lLogin + wxT("\" WHERE \"") + column.serialNumber.name + wxT("\" = \"") + oldSN + wxT("\"");
        try{
            db.ExecuteUpdate(sqlRequest);
            return true;
        }
        catch(wxSQLite3Exception &e){
            errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
            myFunc::writeLog(errMsg);
            return false;
        }
    }
    if(oldLogin == lLogin){
        if(isHaveBindSN(lSN)) return false;
        wxString sqlRequest = wxT("UPDATE \"") + tblBindUnits + wxT("\" SET \"") + column.serialNumber.name + wxT("\" = \"") +
            lSN + wxT("\", \"") + column.login.name + wxT("\" = \"") + lLogin + wxT("\", \"") + wxT("\" WHERE \"") + column.serialNumber.name + wxT("\" = \"") + oldSN + wxT("\"");
        try{
            db.ExecuteUpdate(sqlRequest);
            return true;
        }
        catch(wxSQLite3Exception &e){
            errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
            myFunc::writeLog(errMsg);
            return false;
        }
    }
}

bool MyDB::editUnit(wxString oldNum, wxString oldName, wxString numUnit, wxString nameUnit, int numTable) {
	if (oldNum == numUnit && oldName == nameUnit) return true;
	if (numTable < tblSource.size()) {
		if (oldNum == numUnit) {
			wxString sqlRequest = wxT("UPDATE ") + tblSource[numTable] + wxT(" SET \"") + column.nameUnit.name + wxT("\" = \"")
				+ nameUnit + wxT("\" WHERE \"") + column.numUnit.name + wxT("\" = \"") + oldNum + wxT("\"");
			try {
				db.ExecuteUpdate(sqlRequest);
				return true;
			}
			catch (wxSQLite3Exception &e) {
				errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
				myFunc::writeLog(errMsg);
				return false;
			}
		}
		else {
			if (isHave(numUnit)) {
				errMsg = wxT("Техника с таким номером уже используется. \nВыберите другой номер.");
				return false;
			}
			wxString sqlRequest = wxT("UPDATE ") + tblSource[numTable] + wxT(" SET \"") + column.nameUnit.name + wxT("\" = \"")
				+ nameUnit + wxT("\", \"") + column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\" WHERE \"")
				+ column.numUnit.name + wxT("\" = \"") + oldNum + wxT("\"");
			try {
				db.ExecuteUpdate(sqlRequest);
				return true;
			}
			catch (wxSQLite3Exception &e) {
				errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
				myFunc::writeLog(errMsg);
				return false;
			}
		}
	}
	else {
		return editBattery(oldNum, oldName, numUnit, nameUnit);
	}
}
bool MyDB::editBattery(wxString oldNum, wxString oldName, wxString numUnit, wxString nameUnit) {
	if (oldNum == numUnit) {
		wxString sqlRequest = wxT("UPDATE ") + tblBattery + wxT(" SET \"") + column.nameUnit.name + wxT("\" = \"")
			+ nameUnit + wxT("\" WHERE \"") + column.numUnit.name + wxT("\" = \"") + oldNum + wxT("\"");
		try {
			db.ExecuteUpdate(sqlRequest);
			return true;
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
			return false;
		}
	}
	else {
		if (isHaveBattery(numUnit)) {
			errMsg = wxT("Техника с таким номером уже используется. \nВыберите другой номер.");
			return false;
		}
		wxString sqlRequest = wxT("UPDATE ") + tblBattery + wxT(" SET \"") + column.nameUnit.name + wxT("\" = \"")
			+ nameUnit + wxT("\", \"") + column.numUnit.name + wxT("\" = \"") + numUnit + wxT("\" WHERE \"")
			+ column.numUnit.name + wxT("\" = \"") + oldNum + wxT("\"");
		try {
			db.ExecuteUpdate(sqlRequest);
			return true;
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
			return false;
		}
	}
}
bool MyDB::isHave(wxString numUnit)
{
	//wxString numUnitStr = wxString::Format(wxT("%i"), numUnit);

	try {
		for (auto tableName : tblSource) {
			wxString sqlRequest = wxT("select count(*) from ") + tableName + wxT(" WHERE  \"") + column.numUnit.name + wxT("\" = \"") +
				numUnit + wxT("\";");
			if (db.ExecuteScalar(sqlRequest) > 0)
			{
				errMsg = wxT("Техника с таким номером уже имеется в таблице: " + tableName);
				return true;
			}
		}
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return true;
	}
	return false;
}
bool MyDB::isHaveBindSN(wxString lSN){
    try {
        wxString sqlRequest = wxT("SELECT COUNT(*) FROM \"") + tblBindUnits + wxT("\" WHERE \"") + column.serialNumber.name +
                wxT("\" = \"") + lSN + wxT("\";");
        if(db.ExecuteScalar(sqlRequest) > 0){
            errMsg = wxT("Связь с П/Н \"") + lSN + wxT("\" уже имеется");
            return true;
        }
    }
    catch (wxSQLite3Exception &e) {
        errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        return true;
    }
    return false;
}
bool MyDB::isHaveBindLogin(wxString lLogin){
    try {
        wxString sqlRequest = wxT("SELECT COUNT(*) FROM \"") + tblBindUnits + wxT("\" WHERE \"") + column.login.name +
                wxT("\" = \"") + lLogin + wxT("\";");
        if(db.ExecuteScalar(sqlRequest) > 0){
            errMsg = wxT("Связь с логином \"") + lLogin + wxT("\" уже имеется");
            return true;
        }
    }
    catch (wxSQLite3Exception &e) {
        errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(errMsg);
        return true;
    }
    return false;
}
bool MyDB::isHaveBattery(wxString numUnit) {
	try {
		wxString sqlRequest = wxT("select count(*) from ") + tblBattery + wxT(" WHERE  \"") + column.numUnit.name + wxT("\" = \"") +
			numUnit + wxT("\";");
		if (db.ExecuteScalar(sqlRequest) > 0)
		{
			errMsg = wxT("Техника с таким номером уже имеется в таблице: " + tblBattery);
			return true;
		}
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return true;
	}
	return false;
}
bool MyDB::checkTable(wxString tableName, std::vector<field> lFields){
    wxString sqlRequest = wxT("SELECT * FROM \"") + tableName + wxT("\";");
    wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
    //Сохраняем заголовки
    std::vector<wxString> answer;
    for (int i = 0; i < result.GetColumnCount(); i++) {
        answer.push_back(result.GetColumnName(i));
    }
    //Сверяем нужные заголовки
    std::vector<std::vector<wxString>::iterator> vec;
    for(auto x: lFields){
        vec.push_back(std::find_if(answer.begin(), answer.end(), [x](auto element){return element == x.name;}));
    }
    for(auto it : vec){
        if(it == answer.end()){
            errMsg = wxT("Таблица <") + tableName + wxT("> не корректна. Выберите другую БД");
            return false;
        }
    }
    return true;
}
bool MyDB::getDbIsCorrect()
{
	return dbIsCorrect;
}
