#pragma once
#include <wx/wx.h>
#include <wx/valtext.h>
#include <wx/notebook.h>
#include <string>

class BatteryPanel : public wxPanel
{
public:
	BatteryPanel(wxNotebook* parent);
	void OnEnterTXTNum(wxCommandEvent& event);
	//void OnBtnOK(wxCommandEvent& event);
	void Clear();
	//get функции
	wxString GetTxtNum();
	//ID комплектующих
	static long const ID_TXTNUM;
	static long const ID_BTNOK;

private:
	wxStaticText *mLblNum;
	wxTextCtrl *mTxtNumber;
	wxButton *mBtnOk;
};

