#include "extradition.h"

const long extradition::ID_TXTLOGIN = wxNewId();
const long extradition::ID_TXTNUMTS = wxNewId();
const long extradition::ID_BTNOK = wxNewId();
const long extradition::ID_BTNCLEAR = wxNewId();

extradition::extradition(wxNotebook *parent) : wxPanel(parent)
{
    //Шрифты
    wxFont btnFont(16,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
    wxFont txtFont(26,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);

    //Сайзеры
    wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hBox = new wxBoxSizer(wxHORIZONTAL);

    //Создание контролов
    lblLogin = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Логин получателя"));
    lblNumTS = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Номер техники"));
    txtLogin = new wxTextCtrl(this, ID_TXTLOGIN, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_RICH);
    txtNumTS = new wxTextCtrl(this, ID_TXTNUMTS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    btnOk = new wxButton(this, ID_BTNOK, wxString::FromUTF8("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator);
    btnClear = new wxButton(this, ID_BTNCLEAR, wxString::FromUTF8("Очистить"));

    //Установка шрифтов
    lblLogin->SetFont(txtFont);
    lblNumTS->SetFont(txtFont);
    txtLogin->SetFont(txtFont);
    txtNumTS->SetFont(txtFont);
    btnOk->SetFont(btnFont);
    btnClear->SetFont(btnFont);

    //Размещение на форме
    hBox->Add(-1,-1,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(btnOk, 2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(btnClear, 2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(-1,-1,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    vBox->Add(lblLogin, 0, wxALL|wxEXPAND, 5);
    vBox->Add(txtLogin, 0, wxALL|wxEXPAND, 5);
    vBox->Add(lblNumTS, 0, wxALL|wxEXPAND, 5);
    vBox->Add(txtNumTS, 0, wxALL|wxEXPAND, 5);
    vBox->Add(-1,-1,2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    vBox->Add(hBox, 0, wxALL|wxEXPAND, 5);

    /*this->SetSizer(vBox);
    vBox->Fit(this);*/

	this->SetSizerAndFit(vBox);	

	SetMinSize(wxSize(-1, -1));
    //События
    //Connect(ID_BTNOK, wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&extradition::OnBtnOK);
    Connect(ID_BTNCLEAR, wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&extradition::OnBtnClear);
    Connect(ID_TXTLOGIN, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&extradition::OnEnterTXTLogin);
    //Connect(ID_TXTNUMTS, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&extradition::OnEnterTXTNumTS);
    txtLogin->SetFocus();



}
void extradition::OnEnterTXTLogin(wxCommandEvent &event)
{
    txtNumTS->Clear();
    txtNumTS->SetFocus();
}
void extradition::OnEnterTXTNumTS(wxCommandEvent &event)
{
    wxCommandEvent eventt(wxEVT_BUTTON, ID_BTNOK);
    //OnBtnOK(eventt);
}
void extradition::OnBtnOK(wxCommandEvent &event)
{
    wxMessageBox(wxT("Нажата клавиша OK"));
    ClearAll();
}
void extradition::OnBtnClear(wxCommandEvent &event)
{
    ClearAll();
}
void extradition::ClearAll()
{
    txtNumTS->Clear();
    txtLogin->Clear();
    txtLogin->SetFocus();
}
wxString extradition::getTxtLogin()
{
    return txtLogin->GetValue();
}
wxString extradition::getTxtNumTS()
{
	return txtNumTS->GetValue();
    //return wxAtoi(txtNumTS->GetValue());
}
