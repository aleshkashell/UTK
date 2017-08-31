#pragma once
#include <wx/wx.h>
#include <wx/dialog.h>
class AddRowDialogBind :
    public wxDialog
{
public:
    AddRowDialogBind(wxWindow *parent, wxString lSN, wxString lLogin);
    wxString GetLogin();
    wxString GetSN();
    void SetLogin(wxString lLogin);
    void SetSN(wxString lSN);
private:
    wxTextCtrl *mNumUnit;
    wxTextCtrl *mLogin;
    wxTextCtrl *mSN;
    wxButton *btnOk;
    wxButton *btnCancel;
};

