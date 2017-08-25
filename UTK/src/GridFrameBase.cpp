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
    if (writeTableHelp(mRows, mSortField, mAsc)) return true;
	return false;
}
bool GridFrameBase::writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField, bool asc, int badTime)
{
	return true;
}
void GridFrameBase::Update() {
    writeTableHelp(mRows, mSortField, mAsc);
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
