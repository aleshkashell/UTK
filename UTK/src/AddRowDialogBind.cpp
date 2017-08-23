#include "AddRowDialogBind.h"

AddRowDialogBind::AddRowDialogBind(wxWindow *parent, wxString lSN, wxString lLogin, wxString lNumUnit)
    : wxDialog(parent, wxID_ANY, wxT("Добавить строку"))
{
    wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hBoxNumber = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hBoxLogin = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hBoxSN = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hBoxButton = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText *lblSN = new wxStaticText(this, wxID_ANY, lSN, wxDefaultPosition, wxSize(100, 15));
    wxStaticText *lblNumUnit = new wxStaticText(this, wxID_ANY, lNumUnit, wxDefaultPosition, wxSize(100, 15));
    wxStaticText *lblLogin = new wxStaticText(this, wxID_ANY, lLogin,wxDefaultPosition, wxSize(100, 15));

    mSN = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250, 25));
    mLogin = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250, 25));
    mNumUnit = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250, 25));
    btnOk = new wxButton(this, wxID_OK, wxT("OK"));
    btnCancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    hBoxSN->Add(lblSN, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    hBoxSN->Add(mSN, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

    hBoxLogin->Add(lblLogin, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    hBoxLogin->Add(mLogin, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

    hBoxNumber->Add(lblNumUnit, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    hBoxNumber->Add(mNumUnit, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

    hBoxButton->Add(btnOk, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    hBoxButton->Add(btnCancel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

    vBox->AddSpacer(20);
    vBox->Add(hBoxSN, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
    vBox->Add(hBoxLogin, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
    vBox->Add(hBoxNumber, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
    vBox->AddStretchSpacer();
    vBox->Add(hBoxButton, 0, wxALIGN_RIGHT, 10);
    vBox->AddSpacer(20);
    this->SetSizer(vBox);
}
wxString AddRowDialogBind::GetSN(){
    return mSN->GetValue();
}
wxString AddRowDialogBind::GetLogin(){
    return mLogin->GetValue();
}
wxString AddRowDialogBind::GetNumUnit(){
    return mNumUnit->GetValue();
}
void AddRowDialogBind::SetLogin(wxString lLogin){
    mLogin->SetValue(lLogin);
}
void AddRowDialogBind::SetSN(wxString lSN){
    mSN->SetValue(lSN);
}
void AddRowDialogBind::SetNumUnit(wxString lNumUnit){
    mNumUnit->SetValue(lNumUnit);
}
