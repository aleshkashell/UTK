#ifndef GRIDFRAME_H
#define GRIDFRAME_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <vector>
#include <string>
#include <algorithm>	//sort
#include "GridFrameBase.h"


using namespace std;

class gridFrame : public GridFrameBase
{
    public:
        gridFrame(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxWANTS_CHARS, const wxString &name=wxGridNameStr);
		bool writeTable(std::vector<std::vector<wxString>> &rows, int badTime = 13);
		bool writeTableHelp(vector<vector<wxString>> &rows, int sortField = 0, bool asc = true, int badTime = 13);
        void initVar();
};

#endif // GRIDFRAME_H
