#pragma once
#include <wx\wx.h>
#include <wx\dialog.h>
class addNewRow :
	public wxDialog
{
public:
	addNewRow(wxWindow *parent);
	wxString getNum();
	wxString getName();
	void setNum(wxString numUnit);
	void setName(wxString nameUnit);
	~addNewRow();
private:
	wxTextCtrl *textNumUnit;
	wxTextCtrl *textNameUnit;
	wxButton *btnOk;
	wxButton *btnCancel;
};

