#include "gridFrame.h"

const wxColour colorGood = wxColour("#A9FF85");
const wxColour colorBad = wxColour("#ff4d4d");


gridFrame::gridFrame(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name):
    GridFrameBase(parent, id, pos, size, style, name)
{
    initVar();
	CreateObject();
}
bool gridFrame::writeTable(std::vector<std::vector<wxString>> &rows, int badTime) {
	mRows = rows;
	mBadTime = badTime;
	if (writeTableHelp(mRows, mSortField, mAsc, mBadTime)) return true;
	return false;
}
bool gridFrame::writeTableHelp(vector<vector<wxString>> &rows, int sortField, bool asc, int badTime)
{
	int timeInWork = 3;		//Часов в работе
	int numUnit = 0;		//Колонка с номером техники
	int dateOut = 2;		//Дата выдачи
	//int badTime = 13;		//Время, через которое строка закрашивается в красный
	//Выбор количества строк для отрисовки
    if(rows.size() > this->GetNumberRows())
    {
        this->AppendRows(rows.size() - this->GetNumberRows());
    }
    else if(this->GetNumberRows() > rows.size())
    {
        this->DeleteRows(rows.size(), (this->GetNumberRows() - rows.size()));
    }
    int maxim = min(int(rows[0].size()), this->GetNumberCols());
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
    for(int i = 0; i < rows.size(); i++) {
		for (int g = 0; g < maxim; g++) {
			this->SetCellValue(i, g, rows[i][g]);
			//Закрашивание ячеек
			if (wxAtoi(rows[i][3]) < badTime)
				//this->SetCellBackgroundColour(i, g, wxColour(wxT("green")));
				this->SetCellBackgroundColour(i, g, colorGood);
			else
				this->SetCellBackgroundColour(i, g, colorBad);
		}
    }
    return true;
}
void gridFrame::initVar()
{
	wxString str("Дата выдачи", wxCSConv(wxT("utf8")));
    nameColmn.push_back(wxString::FromUTF8("№ техники"));
    nameColmn.push_back(wxString::FromUTF8("Логин"));
    nameColmn.push_back(str);
    nameColmn.push_back(wxString::FromUTF8("Часов в работе"));
    //nameColmn.push_back(wxT("Моточасы"));
    nameColmn.push_back(wxString::FromUTF8("Статус"));
    nameColmn.push_back(wxString::FromUTF8("Тип техники"));
	nameColmn.push_back(wxString::FromUTF8("Выдал"));
    //nameColmn.push_back("");
	mAsc = true;
	mSortField = 0;
}
