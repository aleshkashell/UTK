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