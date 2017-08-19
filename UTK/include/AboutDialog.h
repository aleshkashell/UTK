#pragma once
#include <wx/wx.h>
class AboutDialog :
	public wxDialog
{
public:
	AboutDialog(wxWindow *parent, std::string version);
private:
	std::string progVersion;
};

