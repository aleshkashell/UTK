#pragma once
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include "gridFrameSource.h"
#include "gridFrameBinds.h"
#include "AddRowDialogBind.h"
#include "addNewRow.h"
#include "sqlite.h"

//ќкно редактировани¤ базы
class sourceEditPanel :
	public wxPanel
{
public:
	sourceEditPanel(wxWindow* parent, wxString Path, std::vector<wxString> tblsMachine, std::vector<wxString> tblsList, wxNotebook *gridSrc);

	void OnButtonAdd(wxCommandEvent &event);
	void OnButtonEdit(wxCommandEvent &event);
	void OnButtonDel(wxCommandEvent &event);
	void renewDB(wxString path);
	~sourceEditPanel();
private:
	MyDB *db;			//ƒл¤ редактировани¤ базы
	wxNotebook *gridSource;	//ƒл¤ получени¤ значений из таблицы
	wxButton *btnAdd;
	wxButton *btnEdit;
	wxButton *btnDel;
	std::vector<wxString> m_tblsMachine;
	std::vector<wxString> m_tblsList;
};

