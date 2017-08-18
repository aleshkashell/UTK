#include "gridFrameBattery.h"


gridFrameBattery::gridFrameBattery(wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString & name)
:GridFrameBase(parent, id, pos, size, style, name) {
	initVar();
	CreateObject();
}
bool gridFrameBattery::writeTableHelp(std::vector<std::vector<wxString>>& rows, int sortField, bool asc, int badTime)
{
	int timeInWork = 3;		//����� � ������
	int numUnit = 0;		//������� � ������� �������
	int dateOut = 2;		//���� ������
							//int badTime = 13;		//�����, ����� ������� ������ ������������� � �������
							//����� ���������� ����� ��� ���������
	if (rows.size() > this->GetNumberRows())
	{
		this->AppendRows(rows.size() - this->GetNumberRows());
	}
	else if (this->GetNumberRows() > rows.size())
	{
		this->DeleteRows(rows.size(), (this->GetNumberRows() - rows.size()));
	}
	//wxMessageBox(wxString::Format(wxT("%i"),this->GetNumberCols()));
	int maxim = std::min(int(rows[0].size()), this->GetNumberCols());
	//���������� ������� � ������, ���� ������� ��� �� �����
	for (auto &row : rows)
	{
		if (row[timeInWork] == "") row[timeInWork] = diffTime(row[dateOut], wxDateTime::Now().FormatISOCombined(' '));
	}
	//���������� ������ �� ����
	if (sortField != timeInWork && sortField != numUnit) {
		if (asc)
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return a[sortField] < b[sortField]; });
		else
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return a[sortField] > b[sortField]; });
	}
	else {
		if (asc)
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return wxAtoi(a[sortField]) < wxAtoi(b[sortField]); });
		else
			sort(rows.begin(), rows.end(), [sortField](std::vector<wxString> a, std::vector<wxString> b) {return wxAtoi(a[sortField]) > wxAtoi(b[sortField]); });
	}
	//���������� �������
	for (int i = 0; i < rows.size(); i++) {
		for (int g = 0; g < maxim; g++) {
			this->SetCellValue(i, g, rows[i][g]);
			//������������ �����
			if (rows[i][1] != wxT("� ������"))
				//this->SetCellBackgroundColour(i, g, wxColour(wxT("green")));
				this->SetCellBackgroundColour(i, g, colorGood);
			else
				this->SetCellBackgroundColour(i, g, colorBad);
		}
	}
	return true;
}
void gridFrameBattery::initVar(){
	wxString str("���� ������", wxCSConv(wxT("utf8")));
	nameColmn.push_back(wxT("� �������"));
	nameColmn.push_back(wxT("������"));
	nameColmn.push_back(wxT("���� �����/������"));
	nameColmn.push_back(wxT("����� ������/������"));
	nameColmn.push_back(wxT("������/�����"));
}
bool gridFrameBattery::writeTable(std::vector<std::vector<wxString>>& rows, int badTime){
	mRows.clear();
	for (auto line : rows) {
		std::vector<wxString> tmpLine;
		if(line[1] == "� ������"){
			tmpLine.push_back(line[0]);
			tmpLine.push_back(line[1]);
			tmpLine.push_back(line[3]);
			tmpLine.push_back(line[4]);
			tmpLine.push_back(line[7]);
		}
		else {
			tmpLine.push_back(line[0]);
			tmpLine.push_back(line[1]);
			tmpLine.push_back(line[2]);
			tmpLine.push_back(line[4]);
			tmpLine.push_back(line[6]);
		}
		mRows.push_back(tmpLine);
	}
	mBadTime = badTime;
	if (writeTableHelp(mRows, mSortField, mAsc, mBadTime)) return true;
	return false;
}