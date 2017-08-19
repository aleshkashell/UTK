#include "BatteryPanel.h"

const long BatteryPanel::ID_BTNOK = wxNewId();
const long BatteryPanel::ID_TXTNUM = wxNewId();

BatteryPanel::BatteryPanel(wxNotebook* parent) : wxPanel(parent){
	//Шрифты
	wxFont btnFont(16, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Arial"), wxFONTENCODING_DEFAULT);
	wxFont txtFont(26, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString, wxFONTENCODING_DEFAULT);

	//Сайзеры
	wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hBox = new wxBoxSizer(wxHORIZONTAL);

	//Создание контролов
	mLblNum = new wxStaticText(this, wxID_ANY, wxT("Номер батареи"));
	mBtnOk = new wxButton(this, ID_BTNOK, wxString::FromUTF8("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator);
	mTxtNumber = new wxTextCtrl(this, ID_TXTNUM, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	//Установка шрифтов
	mLblNum->SetFont(txtFont);
	mBtnOk->SetFont(btnFont);
	mTxtNumber->SetFont(txtFont);
	//Размещение на форме
	hBox->Add(-1, -1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);
	hBox->Add(mBtnOk, 2, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);
	vBox->Add(-1, -1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);
	vBox->Add(mLblNum, 0, wxALL | wxEXPAND, 5);
	vBox->Add(mTxtNumber, 0, wxALL | wxEXPAND, 5);
	vBox->Add(-1, -1, 2, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	vBox->Add(hBox, 0, wxALL | wxEXPAND, 5);
	this->SetSizerAndFit(vBox);
	SetMinSize(wxSize(-1, -1));
	//События
	//Connect(ID_TXTNUM, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&BatteryPanel::OnEnterTXTNum);
}
void BatteryPanel::OnEnterTXTNum(wxCommandEvent & event){
	wxCommandEvent eventt(wxEVT_BUTTON, ID_BTNOK);
}
void BatteryPanel::Clear(){
	mTxtNumber->Clear();
	mTxtNumber->SetFocus();
}
wxString BatteryPanel::GetTxtNum()
{
	return mTxtNumber->GetValue();
}
