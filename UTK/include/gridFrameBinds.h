#pragma once
#include "GridFrameBase.h"

class gridFrameBinds:
        public GridFrameBase
{
public:
    gridFrameBinds(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString &name = wxGridNameStr);
    void initVar();
    bool writeTable(std::vector<std::vector<wxString>> &rows, int badTime = 13) override;
    bool writeTableHelp(std::vector<std::vector<wxString>> &rows, int sortField, bool asc, int badTime = 13) override;
    void Update();
};
