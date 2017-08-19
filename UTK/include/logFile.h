#pragma once

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/textfile.h>
namespace myFunc {
	wxString getNowDate();
	void writeLog(wxString text);
    void logToStd(wxString text);
    void logToCerr(wxString text);
};
