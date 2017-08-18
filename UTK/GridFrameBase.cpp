#include "GridFrameBase.h"

GridFrameBase::GridFrameBase(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
	:wxGrid(parent, id, pos, size, style, name) {
	mAsc = true;
	mSortField = 0;
	colorGood = wxColour("#A9FF85");
	colorBad = wxColour("#ff4d4d");
}
void GridFrameBase::CreateObject() {
	this->CreateGrid(1, nameColmn.size());
	this->SetDefaultRowSize(20, true);
	this->SetDefaultColSize(140, true);
	for (auto i = 0; i < nameColmn.size(); i++)
	{
		this->SetColLabelValue(i, nameColmn[i]);
	}
	this->EnableEditing(false);
	this->DisableDragRowSize();
	//this->DisableDragColSize();
	this->SetSelectionMode(wxGridSelectRows);
	this->SetColSize(0, 120);
	this->SetColSize(5, 200);
	Connect(wxEVT_GRID_COL_SORT, (wxObjectEventFunction)&GridFrameBase::OnSortClickColumn);
}
void GridFrameBase::DrawCellHighlight(wxDC & dc, const wxGridCellAttr * attr) { return; }
wxString GridFrameBase::diffTime(wxString dateFirst, wxString dateLast) {
	wxDateTime dtFirst;
	wxDateTime dtLast;
	wxString::const_iterator end;
	dtFirst.ParseDateTime(dateFirst, &end);
	dtLast.ParseDateTime(dateLast, &end);
	wxTimeSpan ts = dtLast - dtFirst;
	return wxString::Format(wxT("%i"), ts.GetHours());
}
bool GridFrameBase::writeTable(std::vector<std::vector<wxString>> &rows, int badTime) {
	mRows = rows;
	mBadTime = badTime;
	if (writeTableHelp(mRows, mSortField, mAsc, mBadTime)) return true;
	return false;
}
bool GridFrameBase::writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField, bool asc, int badTime)
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
			if (wxAtoi(rows[i][3]) < badTime)
				//this->SetCellBackgroundColour(i, g, wxColour(wxT("green")));
				this->SetCellBackgroundColour(i, g, colorGood);
			else
				this->SetCellBackgroundColour(i, g, colorBad);
		}
	}
	return true;
}
void GridFrameBase::Update() {
	writeTableHelp(mRows, mSortField, mAsc, mBadTime);
}
void GridFrameBase::OnSortClickColumn(wxGridEvent & event) {
	int column = event.GetCol();
	if (column == mSortField) {
		if (mAsc) mAsc = false;
		else mAsc = true;
	}
	else {
		mSortField = column;
		mAsc = true;
	}
	Update();
}
