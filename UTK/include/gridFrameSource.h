/*–едактирование таблицы с исходными данными*/
#pragma once
#include <wx/grid.h>
#include <logFile.h>
#include <vector>
#include "GridFrameBase.h"
class gridFrameSource :
	public GridFrameBase
{
public:
	gridFrameSource(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString &name = wxGridNameStr);

    void initVar() override;
    bool writeTable(std::vector<std::vector<wxString>> &rows) override;
    bool writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField, bool asc) override;
    void Update() override;
};

