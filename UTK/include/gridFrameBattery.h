#pragma once
#include <wx/wx.h>
#include <vector>
#include <string>
#include <algorithm>	//sort
#include "GridFrameBase.h"

class gridFrameBattery :
	public GridFrameBase
{
public:
	gridFrameBattery(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString &name = wxGridNameStr);
	bool writeTable(std::vector<std::vector<wxString>> &rows, int badTime = 13) override;
private:
	bool writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField = 0, bool asc = true, int badTime = 13) override;
	void initVar();
};

