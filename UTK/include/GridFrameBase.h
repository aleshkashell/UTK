#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <vector>
#include <string>
#include <algorithm>	//sort

class GridFrameBase:
	public wxGrid
{
public:
	GridFrameBase(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString &name = wxGridNameStr);
	void DrawCellHighlight(wxDC& dc, const wxGridCellAttr *attr);
    virtual bool writeTable(std::vector<std::vector<wxString>> &rows);
	//Разница между датами в часах
	wxString diffTime(wxString dateFirst, wxString dateLast);
	void Update();
	void OnSortClickColumn(wxGridEvent &event);
	std::vector<std::vector<wxString>> GetRows() { return mRows; }
    virtual bool writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField = 0, bool asc = true);
	virtual void initVar() {};


protected:
	void CreateObject();
	std::vector<wxString> nameColmn;
	bool mAsc;
	int mSortField;
	std::vector<std::vector<wxString>> mRows;
	int mBadTime;

	wxColour colorGood;
	wxColour colorBad;
};

