#ifndef RECEPTION_H
#define RECEPTION_H

#include <wx/wx.h>
#include <wx/notebook.h>

class reception : public wxPanel
{
    public:
        reception(wxNotebook *parent);
        void OnEnterTXTNumHours(wxCommandEvent& event);
        void OnEnterTXTNumTS(wxCommandEvent& event);
        void OnBtnOK(wxCommandEvent& event);
        void OnBtnClear(wxCommandEvent& event);

        void ClearAll();

        //get функции
        wxString getTxtNumTS();
        int getTxtNumHours();

        //ID комплектующих
        static long const ID_TXTNUMHOURS;
        static long const ID_TXTNUMTS;
        static long const ID_BTNOK;
        static long const ID_BTNCLEAR;

    protected:

    private:
        //Комплектующие формы
        wxStaticText *lblNumHours;
        wxStaticText *lblNumTS;
        wxTextCtrl *txtNumHours;
        wxTextCtrl *txtNumTS;
        wxButton *btnOk;
        wxButton *btnClear;


};

#endif // RECEPTION_H
