#include "logFile.h"

wxString myFunc::getNowDate()
{
	return wxDateTime::Now().FormatISOCombined(' ');
}
void myFunc::writeLog(wxString text) {
	wxFile logFile(wxT("error.log"), wxFile::write_append);
	logFile.Write(myFunc::getNowDate() + '\t' + text);
	logFile.Write(wxTextFile::GetEOL());
	logFile.Close();
}
void myFunc::logToStd(wxString text){
#ifdef _DEBUG
    std::cout << __LINE__ << wxT(":") << text << std::endl;
#endif
}
void myFunc::logToCerr(wxString text){
#ifdef _DEBUG
    std::cerr << wxT("CERR: ") << text << std::endl;
#endif
}
