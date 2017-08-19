#include "sqlite.h"

MyDB::MyDB(wxString name, std::vector<wxString> tblMachine, std::vector<wxString> tblList) //: tblSource(tblList), tblWork(tblMachine)
{
	tblSource = tblList;
	tblWork = tblMachine;
    tblRepair = wxT("Ремонт");
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
bool MyDB::iniTDB(wxString name)
{
	if (db.IsOpen())
		db.Close();
	errMsg = "error is not touch";
	try {
		db.Open(name);
		if (!db.IsOpen()) {
			std::cerr << "Database is not open" << std::endl;
		}
		//Проверка, что нужные таблицы существуют
		if (!createSourceTable() || !createWorkTable() || !createRepairTable() || !createBatteryTable() || !createHistoryBatteryTable()) {
			return false;
		}
	}
	catch (wxSQLite3Exception &e)
	{
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
		return false;
	}
	//Проверка, что в таблице имеются нужные поля
	for (auto tableName : tblSource) {
		if (!checkSource(tableName)) {
			errMsg = "Error in source table: " + tableName;
			myFunc::writeLog(errMsg);
			return false;
		}
	}
	for (auto tableName : tblWork) {
		if (!checkWork(tableName)) {
			errMsg = "Error in work table: " + tableName;
			myFunc::writeLog(errMsg);
			return false;
		}
	}
	if (!checkRepair(tblRepair)) {
		errMsg = "Error in repair table: " + tblRepair;
		myFunc::writeLog(errMsg);
		return false;
	}
	if (!checkBattery(tblBattery)) {
		errMsg = "Error in repair table: " + tblBattery;
		myFunc::writeLog(errMsg);
		return false;
	}
	if (!checkHistoryBattery(tblHistoryBattery)) {
		errMsg = "Error in repair table: " + tblHistoryBattery;
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
	if (!createSourceTable()) {
		//errMsg = wxT("Source table is not create");
		myFunc::writeLog(wxT("Source table is not create"));
		dbIsCorrect = false;
		return false;
	}
	if (!createWorkTable()) {
		//errMsg = 
		myFunc::writeLog(wxT("Work table is not create"));
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
		wxString sqlRequestRem = wxT("select count(ID) from ") + tblRepair + wxT(" where \"") + column.numUnit.name + wxT("\" = \"") + numUnit +
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
		myFunc::writeLog(errMsg);
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
#endif
    return wxString(wxT("Unknown system"));
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
/*
bool MyDB::exportXLSX(wxString path)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;	//Для конвертации русских символов
	std::vector<std::vector<wxString>> result;
	//result = getAll();

	xlnt::workbook wb;
	xlnt::worksheet ws2 = wb.active_sheet();
	for (auto tableName : tblWork) {
		xlnt::worksheet ws = wb.create_sheet();
		ws.title(conv1.to_bytes(tableName.ToStdWstring()));

		///Настройка ширины колонн
		std::vector<int> colWidth = { 4, 11, 20, 17, 19, 19, 14, 9, 30 };
		auto width = ws.column_properties(1).width;
		auto style = ws.column_properties(1).style;
		auto custom = ws.column_properties(1).custom;
		int iter = 1;
		for (auto len : colWidth) {
			xlnt::column_properties props1;
			props1.width = len;
			props1.style = style;
			props1.custom = custom;
			ws.add_column_properties(iter, props1);
			iter++;
		}
		///
		result = getAll(tableName);
		for (auto row : result)
		{
			std::vector<std::string> row_ansi;
			for (auto word : row)
			{
				std::wstring str = word.ToStdWstring();
				std::string str_ansi = conv1.to_bytes(str);
				row_ansi.push_back(str_ansi);
			}
			ws.append(row_ansi);
		}
		///Рамки ячеек
		xlnt::border myBorder;
		xlnt::border::border_property proper;
		//Стиль линии
		proper.style(xlnt::border_style::thin);
		//Цвет линии
		proper.color(xlnt::color::black());
		//Стиль устанавливается для каждой границы
		myBorder = myBorder.side(xlnt::border_side::end, proper);
		myBorder = myBorder.side(xlnt::border_side::bottom, proper);
		myBorder = myBorder.side(xlnt::border_side::start, proper);
		myBorder = myBorder.side(xlnt::border_side::top, proper);
		//Установка рамок для ячеек
		auto numRow = ws.highest_row();
		auto numCol = ws.highest_column();

		for (auto r = 1; r <= numRow; r++) {
			for (auto c = 1; c <= numCol; c++) {
				ws.cell(c, r).border(myBorder);
			}
		}
	}
	wb.remove_sheet(ws2);
	

	
	std::wstring str = path.ToStdWstring();
	std::string path_ansi = conv1.to_bytes(str);
	try {
		wb.save(path_ansi);
	}
	catch (std::exception &e) {
		errMsg = e.what();
		myFunc::writeLog(errMsg);
		return false;
	}
	return true;
}*/
bool MyDB::createSourceTable()
{
	for (auto sourceTable : tblSource) {
		if (db.TableExists(sourceTable)) {
			if (checkSource(sourceTable)) continue;
			else return false;
		}
		wxString request = wxT("CREATE TABLE ") + sourceTable + wxT("(ID INTEGER primary key, \"") +
			column.numUnit.name + wxT("\" INTEGER UNIQUE, \"") + column.nameUnit.name + wxT("\" TEXT)");
		try {
			db.ExecuteUpdate(request);
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
            myFunc::writeLog(wxT("Таблица: ") + sourceTable + wxT("; Req = \"") + request + wxT("\""));
            return false;
		}
	}
	return true;
}
bool MyDB::createWorkTable() {
	for (auto workTable : tblWork) {
		//Таблица существует
		if (db.TableExists(workTable)) {
			if (checkWork(workTable)) continue;	//Проверяем корректность и продолжаем
			else return false;
		}
		//Делаем новую
		wxString request = wxT("CREATE TABLE ") + workTable + wxT("(ID INTEGER primary key");
		for (auto x : fields) {
			request += wxT(", \"") + x.name + wxT("\" ") + x.type;
		}
		request += wxT(")");
		try {
			db.ExecuteUpdate(request);
		}
		catch (wxSQLite3Exception &e) {
			errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
			myFunc::writeLog(errMsg);
            myFunc::writeLog(wxT("Таблица: ") + workTable + wxT("; Req = \"") + request + wxT("\""));
			return false;
		}
	}
	return true;
}
bool MyDB::createRepairTable() {
	if (db.TableExists(tblRepair)) {
		if (checkWork(tblRepair)) return true;	//Проверяем корректность
		else return false;
	}
	//Если таблицы не существует
	//Делаем новую
	wxString request = wxT("CREATE TABLE ") + tblRepair + wxT("(ID INTEGER primary key");
	for (auto x : fields) {
		request += wxT(", \"") + x.name + wxT("\" ") + x.type;
	}
	request += wxT(")");
	try {
		db.ExecuteUpdate(request);
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
        myFunc::writeLog(wxT("Таблица: ") + tblRepair + wxT("; Req = \"") + request + wxT("\""));
        return false;
	}
	return true;
}
bool MyDB::createBatteryTable() {
	if (db.TableExists(tblBattery)) {
		if (checkBattery(tblBattery)) return true;
		else return false;
	}
	wxString request = wxT("CREATE TABLE ") + tblBattery + wxT("(ID INTEGER primary key");
	for (auto x : mBatteryFields) {
		request += wxT(", \"") + x.name + wxT("\" ") + x.type;
	}
	request += wxT(")");
	try {
		db.ExecuteUpdate(request);
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
        myFunc::writeLog(wxT("Таблица: ") + tblBattery + wxT("; Req = \"") + request + wxT("\""));
        myFunc::writeLog(errMsg);
		return false;
	}
	return true;
}
bool MyDB::createHistoryBatteryTable() {
	if (db.TableExists(tblHistoryBattery)) {
		if (checkHistoryBattery(tblHistoryBattery)) return true;
		else return false;
	}
	wxString request = wxT("CREATE TABLE ") + tblHistoryBattery + wxT("(ID INTEGER primary key");
	for (auto x : mHistoryBatteryFields) {
		request += wxT(", \"") + x.name + wxT("\" ") + x.type;
	}
	request += wxT(")");
	try {
		db.ExecuteUpdate(request);
	}
	catch (wxSQLite3Exception &e) {
		errMsg = e.GetErrorCode() + wxT(":") + e.GetMessage();
		myFunc::writeLog(errMsg);
        myFunc::writeLog(wxT("Таблица: ") + tblHistoryBattery + wxT("; Req = \"") + request + wxT("\""));
        return false;
	}
	return true;
}
std::vector<std::vector<wxString>> MyDB::getSourceUnit(int numTable)
{
	if (numTable < tblSource.size()) {
		wxString request = wxT("SELECT * FROM ") + tblSource[numTable];
		wxSQLite3ResultSet result;
		std::vector<std::vector<wxString>> answer;
		try {
			result = db.ExecuteQuery(request);
			int iter = 0;
			while (result.NextRow()) {
				answer.push_back(std::vector<wxString>(result.GetColumnCount()));
				for (auto i = 0; i < result.GetColumnCount(); i++) {
					answer[iter][i] = result.GetAsString(i);
					std::cerr << iter << answer[iter][i] << std::endl;
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
	else {
		wxString request = wxT("SELECT * FROM ") + tblBattery;
		wxSQLite3ResultSet result;
		int numColumn = 3;	//Количество колонок для загрузки (ID, имя, имя техники)
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
bool MyDB::checkSource(wxString tableName) {
	//Делаем запрос в таблицу
	wxString sqlRequest = wxT("SELECT * FROM \"") + tableName + wxT("\";");
	wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
	//Сохраняем заголовки
	std::vector<wxString> answer;
	for (int i = 0; i < result.GetColumnCount(); i++) {
		answer.push_back(result.GetColumnName(i));
	}
	//Сверяем нужные заголовки
	auto tmpFields = column;		//Для захвата в лямбда
	std::vector<std::vector<wxString>::iterator> vec;
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.nameUnit.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.numUnit.name; }));
	//auto it = std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.nameUnit.name; });
	for (auto it : vec) {
		if (it == answer.end()) {
			errMsg = wxT("Таблица <") + tableName + wxT("> с исходными данными не корректна");
			return false;
		}
	}
	return true;
}
bool MyDB::checkWork(wxString tableName) {
	//Делаем запрос в таблицу
	wxString sqlRequest = wxT("SELECT * FROM \"") + tableName + wxT("\";");
	wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
	//Сохраняем заголовки
	std::vector<wxString> answer;
	for (int i = 0; i < result.GetColumnCount(); i++) {
		answer.push_back(result.GetColumnName(i));
	}
	//Сверяем нужные заголовки
	auto tmpFields = column;		//Для захвата в лямбда
	std::vector<std::vector<wxString>::iterator> vec;
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.numUnit.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.login.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.numHours.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.dateOut.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.dateIn.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.timeInWork.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.status.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.nameUnit.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.whoOutput.name; }));

	for (auto it : vec) {
		if (it == answer.end()) {
			errMsg = wxT("Рабочая таблица <") + tableName + wxT("> не корректна. Выберите другую БД");
			return false;
		}
	}
	return true;
}
bool MyDB::checkRepair(wxString tableName) {
	//Делаем запрос в таблицу
	wxString sqlRequest = wxT("SELECT * FROM \"") + tableName + wxT("\";");
	wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
	//Сохраняем заголовки
	std::vector<wxString> answer;
	for (int i = 0; i < result.GetColumnCount(); i++) {
		answer.push_back(result.GetColumnName(i));
	}
	//Сверяем нужные заголовки
	auto tmpFields = column;		//Для захвата в лямбда
	std::vector<std::vector<wxString>::iterator> vec;
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.numUnit.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.login.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.numHours.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.dateOut.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.dateIn.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.timeInWork.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.status.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.nameUnit.name; }));
	vec.push_back(std::find_if(answer.begin(), answer.end(), [tmpFields](auto element) {return element == tmpFields.whoOutput.name; }));

	for (auto it : vec) {
		if (it == answer.end()) {
			errMsg = wxT("Таблица <") + tableName + wxT("> не корректна. Выберите другую БД");
			return false;
		}
	}
	return true;
}
bool MyDB::checkBattery(wxString tableName) {
	//Делаем запрос в таблицу
	wxString sqlRequest = wxT("SELECT * FROM \"") + tableName + wxT("\";");
	wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
	//Сохраняем заголовки
	std::vector<wxString> answer;
	for (int i = 0; i < result.GetColumnCount(); i++) {
		answer.push_back(result.GetColumnName(i));
	}
	//Сверяем нужные заголовки
	std::vector<std::vector<wxString>::iterator> vec;
	for (auto x : mBatteryFields) {
		vec.push_back(std::find_if(answer.begin(), answer.end(), [x](auto element) {return element == x.name; }));
	}
	for (auto it : vec) {
		if (it == answer.end()) {
			errMsg = wxT("Таблица <") + tableName + wxT("> не корректна. Выберите другую БД");
			return false;
		}
	}
	return true;
}
bool MyDB::checkHistoryBattery(wxString tableName) {
	//Делаем запрос в таблицу
	wxString sqlRequest = wxT("SELECT * FROM \"") + tableName + wxT("\";");
	wxSQLite3ResultSet result = db.ExecuteQuery(sqlRequest);
	//Сохраняем заголовки
	std::vector<wxString> answer;
	for (int i = 0; i < result.GetColumnCount(); i++) {
		answer.push_back(result.GetColumnName(i));
	}
	//Сверяем нужные заголовки
	std::vector<std::vector<wxString>::iterator> vec;
	for (auto x : mHistoryBatteryFields) {
		vec.push_back(std::find_if(answer.begin(), answer.end(), [x](auto element) {return element == x.name; }));
	}
	for (auto it : vec) {
		if (it == answer.end()) {
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
