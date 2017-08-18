#include "SettingsDialog.h"



SettingsDialog::SettingsDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, wxT("Настройки программы"))
{
	wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *forButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *forPathSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *forTimeSizer = new wxBoxSizer(wxHORIZONTAL);

	wxPanel *panel = new wxPanel(this);

	wxStaticText *lblDatabase = new wxStaticText(panel, wxID_ANY, wxT("База данных"));
	wxStaticText *lblTimeToColor = new wxStaticText(panel, wxID_ANY, wxT("Время до предупреждения"));
	txtPathToDB = new wxTextCtrl(panel, wxID_ANY, dynamic_cast<utkGuiFrame*>(parent)->getPathToBD(), wxDefaultPosition, wxSize(260, -1));
	txtTimeToColor = new wxTextCtrl(panel, wxID_ANY, wxString::Format(wxT("%i"), dynamic_cast<utkGuiFrame*>(parent)->getBadTime()), wxDefaultPosition, wxSize(50, -1));
	wxButton *btnOk = new wxButton(panel, wxID_OK, wxT("OK"));
	wxButton *btnCancel = new wxButton(panel, wxID_CANCEL, wxT("Cancel"));
	wxButton *btnView = new wxButton(panel, wxID_PREVIEW, wxT("Обзор..."));

	forPathSizer->Add(txtPathToDB, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	forPathSizer->Add(btnView, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	forButtonSizer->Add(btnOk, 0, wxALL, 10);
	forButtonSizer->Add(btnCancel, 0, wxALL, 10);
	forTimeSizer->Add(lblTimeToColor, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	forTimeSizer->Add(txtTimeToColor, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

	vBox->AddSpacer(25);
	vBox->Add(lblDatabase, 0, wxLEFT| wxRIGHT| wxTOP, 10);
	vBox->Add(forPathSizer);
	vBox->AddSpacer(5);
	vBox->Add(forTimeSizer);

	vBox->AddStretchSpacer();
	vBox->Add(forButtonSizer, 0, wxALIGN_RIGHT, 10);
	vBox->AddSpacer(25);

	panel->SetSizer(vBox);

	Connect(wxID_PREVIEW, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnBtnView);
}


void SettingsDialog::OnBtnView(wxCommandEvent &event)
{
	wxFileDialog dlg(this, wxT("Выбор базы данных"), "", txtPathToDB->GetValue(), wxT("SQLite files (*.db)|*.db"), wxFD_OPEN);
	if (dlg.ShowModal() == wxID_CANCEL) return;
	txtPathToDB->SetValue(dlg.GetPath());
}

wxString SettingsDialog::getPathToDB()
{
	return txtPathToDB->GetValue();
}
int SettingsDialog::getBadTime() {
	return wxAtoi(txtTimeToColor->GetValue());
}
