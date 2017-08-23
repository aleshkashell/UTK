#include "sourceEditPanel.h"


sourceEditPanel::sourceEditPanel(wxWindow * parent, wxString Path, std::vector<wxString> tblsMachine, std::vector<wxString> tblsList, wxNotebook * gridSrc) :
	wxPanel(parent), m_tblsMachine(tblsMachine), m_tblsList(tblsList)
{
	db = new MyDB(Path, m_tblsMachine, m_tblsList);
	gridSource = gridSrc;
	wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
	btnAdd = new wxButton(this, wxID_ANY, wxT("Добавить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator);
	btnEdit = new wxButton(this, wxID_ANY, wxT("Редактировать"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator);
	btnDel = new wxButton(this, wxID_ANY, wxT("Удалить"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator);

	vBox->Add(btnAdd, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);
	vBox->Add(btnEdit, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);
	vBox->Add(btnDel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 10);

	this->SetSizerAndFit(vBox);

	Connect(btnAdd->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&sourceEditPanel::OnButtonAdd);
	Connect(btnDel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&sourceEditPanel::OnButtonDel);
	Connect(btnEdit->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&sourceEditPanel::OnButtonEdit);

}

void sourceEditPanel::OnButtonAdd(wxCommandEvent & event)
{
    if(gridSource->GetSelection() != gridSource->GetPageCount() - 1){
        addNewRow dlg(this);
        long x = dlg.ShowModal();
        while (true) {
            if (x == wxID_OK) {
                if (db->addUnit(dlg.getNum(), dlg.getName(), gridSource->GetSelection())) {
                    //db->addUnit(dlg.getNum(), dlg.getName(), gridSource->GetSelection());
                    break;
                }
                wxMessageBox(db->getErrMsg());
                x = dlg.ShowModal();
            }
            else {
                break;
            }
        }
        dlg.Destroy();
    }
    else {
        AddRowDialogBind dlg(this, wxT("Порядковый номер"), wxT("Логин"), wxT("Номер техники"));
        long x =dlg.ShowModal();
        while (true) {
            if(x == wxID_OK){
                if(db->addBind(dlg.GetSN(), dlg.GetLogin(), dlg.GetNumUnit())) break;
                else {
                    wxMessageBox(db->getErrMsg());
                    x = dlg.ShowModal();
                }
            }
            else break;
        }
        dlg.Destroy();
    }
}
void sourceEditPanel::OnButtonEdit(wxCommandEvent &event) {
    if(gridSource->GetSelection() != gridSource->GetPageCount() - 1){
        gridFrameSource *tmpGrid = dynamic_cast<gridFrameSource*>(gridSource->GetCurrentPage());
        wxArrayInt rows = tmpGrid->GetSelectedRows();
        if (rows.size() == 0) {
            wxMessageBox(wxT("Выберите строку для редактирования"));
            return;
        }
        addNewRow dlg(this);
        wxString oldNum = tmpGrid->GetCellValue(rows[0], 0);
        wxString oldName = tmpGrid->GetCellValue(rows[0], 1);
        dlg.setNum(oldNum);
        dlg.setName(oldName);
        long x = dlg.ShowModal();
        while (true) {
            if (x == wxID_OK) {
                if (db->editUnit(oldNum, oldName, dlg.getNum(), dlg.getName(), gridSource->GetSelection())) {
                    break;
                }
                wxMessageBox(db->getErrMsg());
                x = dlg.ShowModal();
            }
            else break;
        }
        dlg.Destroy();
    }
    else{
        auto *tmpGrid = dynamic_cast<gridFrameBinds*>(gridSource->GetCurrentPage());
        wxArrayInt rows = tmpGrid->GetSelectedRows();
        if (rows.size() == 0) {
            wxMessageBox(wxT("Выберите строку для редактирования"));
            return;
        }
        AddRowDialogBind dlg(this, wxT("Порядковый номер"), wxT("Логин"), wxT("Номер техники"));
        wxString oldSN = tmpGrid->GetCellValue(rows[0], 0);
        wxString oldLogin = tmpGrid->GetCellValue(rows[0], 1);
        wxString oldNumber = tmpGrid->GetCellValue(rows[0], 2);
        dlg.SetSN(oldSN);
        dlg.SetLogin(oldLogin);
        dlg.SetNumUnit(oldNumber);
        long x = dlg.ShowModal();
        while (true){
            if(x == wxID_OK){
                if(db->editBind(oldSN, oldLogin, dlg.GetSN(), dlg.GetLogin(), dlg.GetNumUnit())) break;
                wxMessageBox(db->getErrMsg());
                x = dlg.ShowModal();
            }
            else break;
        }
        dlg.Destroy();

    }
}
void sourceEditPanel::OnButtonDel(wxCommandEvent & event)
{
    GridFrameBase* tmpGrid;
    if(gridSource->GetSelection() != gridSource->GetPageCount() - 1){
        tmpGrid = dynamic_cast<gridFrameSource*>(gridSource->GetCurrentPage());
    }
    else{
        tmpGrid = dynamic_cast<gridFrameBinds*>(gridSource->GetCurrentPage());
    }
	wxArrayInt rows = tmpGrid->GetSelectedRows();
	if (rows.size() == 0) {
		wxMessageBox(wxT("Выберите хотя бы одну строку для удаления."));
		return;
	}
	for (auto row : rows) {
        if(gridSource->GetSelection() != gridSource->GetPageCount() - 1)
            db->removeUnit(tmpGrid->GetCellValue(row, 0), gridSource->GetSelection());
        else db->removeBind(tmpGrid->GetCellValue(row, 0));
	}
	wxMessageBox(wxT("Удалено ") + wxString::Format(wxT("%i"), rows.size()) + wxT(" строк"));

}

void sourceEditPanel::renewDB(wxString path)
{
	delete db;
	db = new MyDB(path, m_tblsMachine, m_tblsList);
}
sourceEditPanel::~sourceEditPanel()
{
}
