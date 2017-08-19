#include "gridFrameSource.h"

gridFrameSource::gridFrameSource(wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString & name):
	GridFrameBase(parent, id, pos, size, style, name)
{
	initVar();
	CreateObject();
}
void gridFrameSource::initVar()
{
	nameColmn.push_back(wxT("Номер техники"));
	nameColmn.push_back(wxT("Вид техники"));
}
bool gridFrameSource::writeTable(std::vector<std::vector<wxString>> &rows){
	mRows = rows;
	if (writeTableHelp(mRows, mSortField, mAsc)) return true;
	return false;
}
bool gridFrameSource::writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField, int asc) {
	//Выбор количества строк для отрисовки
	if (rows.size() > this->GetNumberRows()){
		this->AppendRows(rows.size() - this->GetNumberRows());
	}
	else if (this->GetNumberRows() > rows.size()){
		this->DeleteRows(rows.size(), (this->GetNumberRows() - rows.size()));
	}
	int maxim = std::min(int(rows[0].size()), this->GetNumberCols());	
	//Сортировка
	if (sortField != 1) {	//Не равно полю с номером
		if (asc)
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> &a, std::vector<wxString> &b) { return a[sortField] < b[sortField]; });
		else
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> &a, std::vector<wxString> &b) { return a[sortField] > b[sortField]; });
	}
	else {
		if (asc)
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> &a, std::vector<wxString> &b) { return wxAtoi(a[sortField]) < wxAtoi(b[sortField]); });
		else
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> &a, std::vector<wxString> &b) { return wxAtoi(a[sortField]) > wxAtoi(b[sortField]); });
	}
	//Заполнение таблицы
	for (int i = 0; i < rows.size(); i++) {
		for (int g = 0; g < maxim; g++)
			this->SetCellValue(i, g, rows[i][g+1]);
	}
	return true;
}
void gridFrameSource::Update() {
	writeTableHelp(mRows, mSortField, mAsc);
}