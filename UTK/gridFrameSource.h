/*–едактирование таблицы с исходными данными*/
#pragma once
#include <wx\grid.h>
#include <logFile.h>
#include <vector>
#include "GridFrameBase.h"
class gridFrameSource :
	public GridFrameBase
{
public:
	gridFrameSource(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString &name = wxGridNameStr);

	void initVar();
	bool writeTable(std::vector<std::vector<wxString>> &rows);
	bool writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField, int asc);
	void Update();
};

