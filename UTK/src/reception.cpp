#include "reception.h"

const long reception::ID_TXTNUMHOURS = wxNewId();
const long reception::ID_TXTNUMTS = wxNewId();
const long reception::ID_BTNOK = wxNewId();
const long reception::ID_BTNCLEAR = wxNewId();

reception::reception(wxNotebook *parent) : wxPanel(parent)
{
    //Шрифты
    wxFont btnFont(16,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
    wxFont txtFont(26,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);

    //Сайзеры
    wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hBox = new wxBoxSizer(wxHORIZONTAL);

    //Создание контролов
    lblNumHours = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Количество моточасов"));
    lblNumTS = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Номер техники"));
    txtNumHours = new wxTextCtrl(this, ID_TXTNUMHOURS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    txtNumTS = new wxTextCtrl(this, ID_TXTNUMTS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    btnOk = new wxButton(this, ID_BTNOK, wxString::FromUTF8("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator);
    btnClear = new wxButton(this, ID_BTNCLEAR, wxString::FromUTF8("Очистить"));

    //Установка шрифтов
    lblNumHours->SetFont(txtFont);
    lblNumTS->SetFont(txtFont);
    txtNumHours->SetFont(txtFont);
    txtNumTS->SetFont(txtFont);
    btnOk->SetFont(btnFont);
    btnClear->SetFont(btnFont);

    //Размещение на форме
    hBox->Add(-1,-1,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(btnOk, 2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(btnClear, 2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    hBox->Add(-1,-1,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    vBox->Add(lblNumTS, 0, wxALL|wxEXPAND, 5);
    vBox->Add(txtNumTS, 0, wxALL|wxEXPAND, 5);
    vBox->Add(lblNumHours, 0, wxALL|wxEXPAND, 5);
    vBox->Add(txtNumHours, 0, wxALL|wxEXPAND, 5);
    vBox->Add(-1,-1,2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    vBox->Add(hBox, 0, wxALL|wxEXPAND, 5);

    this->SetSizer(vBox);
    vBox->Fit(this);
    txtNumTS->SetFocus();

    //tab order
    //txtNumTS->MoveAfterInTabOrder(btnClear);
    txtNumHours->MoveAfterInTabOrder(txtNumTS);
    btnOk->MoveAfterInTabOrder(txtNumHours);
    btnClear->MoveAfterInTabOrder(btnOk);
    //События
    //Connect(ID_BTNOK, wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&reception::OnBtnOK);
    Connect(ID_BTNCLEAR, wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&reception::OnBtnClear);
    //Connect(ID_TXTNUMHOURS, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&reception::OnEnterTXTNumHours);
    Connect(ID_TXTNUMTS, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&reception::OnEnterTXTNumTS);


}
void reception::OnBtnOK(wxCommandEvent& event)
{
    wxMessageBox(wxT("Техника принята"));
    ClearAll();
}
void reception::OnBtnClear(wxCommandEvent& event)
{
    ClearAll();
}
void reception::OnEnterTXTNumHours(wxCommandEvent& event)
{
    wxCommandEvent eventt(wxEVT_BUTTON, ID_BTNOK);
    OnBtnOK(eventt);
}
void reception::OnEnterTXTNumTS(wxCommandEvent& event)
{
        txtNumHours->Clear();
        txtNumHours->SetFocus();
}
void reception::ClearAll()
{
    txtNumHours->Clear();
    txtNumTS->Clear();
    txtNumTS->SetFocus();
}

//get
int reception::getTxtNumHours()
{
    return wxAtoi(txtNumHours->GetValue());
}
wxString reception::getTxtNumTS()
{
	return txtNumTS->GetValue();
    //return wxAtoi(txtNumTS->GetValue());
}
