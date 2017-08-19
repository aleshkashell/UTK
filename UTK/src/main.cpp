#include "main.h"
#include "utkGuiFrame.h"

IMPLEMENT_APP(myApp)

bool myApp::OnInit()
{
    utkGuiFrame *frame = new utkGuiFrame(wxString::FromUTF8("Учёт выдачи техники"));
    frame->Show();
    return true;
}
