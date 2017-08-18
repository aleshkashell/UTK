#ifndef EXTRADITION_H
#define EXTRADITION_H

#include <wx/wx.h>
#include <wx/valtext.h>
#include <wx/notebook.h>
#include <string>


class extradition : public wxPanel
{
    public:
        extradition(wxNotebook *parent);

        //События
        void OnEnterTXTLogin(wxCommandEvent& event);
        void OnEnterTXTNumTS(wxCommandEvent& event);
        void OnBtnOK(wxCommandEvent& event);
        void OnBtnClear(wxCommandEvent& event);

        void ClearAll();

        //get функции
        wxString getTxtLogin();
        wxString getTxtNumTS();


        //ID комплектующих
        static long const ID_TXTLOGIN;
        static long const ID_TXTNUMTS;
        static long const ID_BTNOK;
        static long const ID_BTNCLEAR;

    protected:

    private:
        //Комплектующие формы
        wxStaticText *lblLogin;
        wxStaticText *lblNumTS;
        wxTextCtrl *txtLogin;
        wxTextCtrl *txtNumTS;
        wxButton *btnOk;
        wxButton *btnClear;



};


#endif // EXTRADITION_H
