#pragma once
/*���� ��������
	����� ���� ������
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

