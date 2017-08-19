#include "addNewRow.h"



addNewRow::addNewRow(wxWindow *parent):
	wxDialog(parent, wxID_ANY, wxT("Добавление строки"))
{
	wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hBoxNumber = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hBoxName = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hBoxButton = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText *lblNumber = new wxStaticText(this, wxID_ANY, wxT("Номер техники:"), wxDefaultPosition, wxSize(100, 15));
	wxStaticText *lblName = new wxStaticText(this, wxID_ANY, wxT("Имя техники:"), wxDefaultPosition, wxSize(100, 15));

	//hBoxNumber->Add(lblNumber);
	
	/*
	textNumUnit = new wxTextCtrl(this, wxID_ANY);
	textNameUnit = new wxTextCtrl(this, wxID_ANY);
	btnOk = new wxButton(this, wxID_OK, wxT("OK"));
	btnCancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
	vBox->Add(textNumUnit);
	vBox->Add(textNameUnit);
	vBox->Add(btnOk);
	vBox->Add(btnCancel);
	*/
	textNumUnit = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250, 25));
	textNameUnit = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250, 25));
	btnOk = new wxButton(this, wxID_OK, wxT("OK"));
	btnCancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

	hBoxNumber->Add(lblNumber, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	hBoxNumber->Add(textNumUnit, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

	hBoxName->Add(lblName, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	hBoxName->Add(textNameUnit, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

	hBoxButton->Add(btnOk, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	hBoxButton->Add(btnCancel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

	vBox->AddSpacer(20);
	vBox->Add(hBoxNumber, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	vBox->Add(hBoxName, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	vBox->AddStretchSpacer();
	vBox->Add(hBoxButton, 0, wxALIGN_RIGHT, 10);
	vBox->AddSpacer(20);
	this->SetSizer(vBox);
}

wxString addNewRow::getNum() {
	return textNumUnit->GetValue();
}
wxString addNewRow::getName() {
	return textNameUnit->GetValue();
}
void addNewRow::setNum(wxString numUnit) {
	textNumUnit->SetValue(numUnit);
}
void addNewRow::setName(wxString nameUnit) {
	textNameUnit->SetValue(nameUnit);
}
addNewRow::~addNewRow()
{
}
