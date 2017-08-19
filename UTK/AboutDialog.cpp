#include "AboutDialog.h"



AboutDialog::AboutDialog(wxWindow *parent, std::string version): wxDialog(parent, wxID_ANY, wxT("О программе")), progVersion(version){
	wxBoxSizer* vBox = new wxBoxSizer(wxVERTICAL);
	wxPanel *panel = new wxPanel(this);

	wxStaticText *lblDescription = new wxStaticText(panel, wxID_ANY, wxT("Программа учёта техники."));
	wxStaticText *lblVersion = new wxStaticText(panel, wxID_ANY, wxT("Версия: ") + progVersion);
	wxStaticText *lblAuthor = new wxStaticText(panel, wxID_ANY, wxT("Автор: Шелудченков Алексей."));
	wxButton *btnOk = new wxButton(panel, wxID_OK, wxT("OK"));

	vBox->Add(lblDescription);
	vBox->Add(lblVersion);
	vBox->Add(lblAuthor);
	vBox->Add(-1, -1, 2, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	vBox->Add(btnOk, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 10);
	vBox->Add(-1, -1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);

	panel->SetSizer(vBox);
}
