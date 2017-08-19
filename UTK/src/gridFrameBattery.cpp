#include "gridFrameBattery.h"


gridFrameBattery::gridFrameBattery(wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString & name)
:GridFrameBase(parent, id, pos, size, style, name) {
	initVar();
	CreateObject();
}
bool gridFrameBattery::writeTableHelp(std::vector<std::vector<wxString>>& rows, int sortField, bool asc, int badTime)
{
	int timeInWork = 3;		//Часов в работе
	int numUnit = 0;		//Колонка с номером техники
	int dateOut = 2;		//Дата выдачи
							//int badTime = 13;		//Время, через которое строка закрашивается в красный
							//Выбор количества строк для отрисовки
	if (rows.size() > this->GetNumberRows())
	{
		this->AppendRows(rows.size() - this->GetNumberRows());
	}
	else if (this->GetNumberRows() > rows.size())
	{
		this->DeleteRows(rows.size(), (this->GetNumberRows() - rows.size()));
	}
	//wxMessageBox(wxString::Format(wxT("%i"),this->GetNumberCols()));
	int maxim = std::min(int(rows[0].size()), this->GetNumberCols());
	//Вычисление времени в работе, если техника ещё не сдана
	for (auto &row : rows)
	{
		if (row[timeInWork] == "") row[timeInWork] = diffTime(row[dateOut], wxDateTime::Now().FormatISOCombined(' '));
	}
	//Сортировка данных по полю
	if (sortField != timeInWork && sortField != numUnit) {
		if (asc)
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return a[sortField] < b[sortField]; });
		else
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return a[sortField] > b[sortField]; });
	}
	else {
		if (asc)
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return wxAtoi(a[sortField]) < wxAtoi(b[sortField]); });
		else
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return wxAtoi(a[sortField]) > wxAtoi(b[sortField]); });
	}
	//Заполнение таблицы
	for (int i = 0; i < rows.size(); i++) {
		for (int g = 0; g < maxim; g++) {
			this->SetCellValue(i, g, rows[i][g]);
			//Закрашивание ячеек
			if (rows[i][1] != wxT("В работе"))
				//this->SetCellBackgroundColour(i, g, wxColour(wxT("green")));
				this->SetCellBackgroundColour(i, g, colorGood);
			else
				this->SetCellBackgroundColour(i, g, colorBad);
		}
	}
	return true;
}
void gridFrameBattery::initVar(){
	wxString str("Дата выдачи", wxCSConv(wxT("utf8")));
	nameColmn.push_back(wxT("№ батареи"));
	nameColmn.push_back(wxT("Статус"));
	nameColmn.push_back(wxT("Дата приёма/выдачи"));
	nameColmn.push_back(wxT("Время заряда/работы"));
	nameColmn.push_back(wxT("Принял/выдал"));
}
bool gridFrameBattery::writeTable(std::vector<std::vector<wxString>>& rows, int badTime){
	mRows.clear();
	for (auto line : rows) {
		std::vector<wxString> tmpLine;
		if(line[1] == "В работе"){
			tmpLine.push_back(line[0]);
			tmpLine.push_back(line[1]);
			tmpLine.push_back(line[3]);
			tmpLine.push_back(line[4]);
			tmpLine.push_back(line[7]);
		}
		else {
			tmpLine.push_back(line[0]);
			tmpLine.push_back(line[1]);
			tmpLine.push_back(line[2]);
			tmpLine.push_back(line[4]);
			tmpLine.push_back(line[6]);
		}
		mRows.push_back(tmpLine);
	}
	mBadTime = badTime;
	if (writeTableHelp(mRows, mSortField, mAsc, mBadTime)) return true;
	return false;
}