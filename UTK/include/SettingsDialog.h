#pragma once
/*Окно настроек
	Выбор базы данных
*/

#include <wx/wx.h>
#include "utkGuiFrame.h"

class SettingsDialog :
	public wxDialog
{
public:
	SettingsDialog(wxWindow *parent);

	void OnBtnView(wxCommandEvent &event);
	wxString getPathToDB();
	int getBadTime();
private:
	wxTextCtrl *txtPathToDB;
	wxTextCtrl *txtTimeToColor;
};

