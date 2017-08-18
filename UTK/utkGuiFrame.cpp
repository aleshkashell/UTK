#include "utkGuiFrame.h"

utkGuiFrame::utkGuiFrame(const wxString &title): wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 500), wxDEFAULT_FRAME_STYLE)
{
	StableIniT();
	DynInit();
	preTableUpdate();
}
void utkGuiFrame::StableIniT() {
	programVersion = reinterpret_cast<char const*>(completeVersion);
	//Загрузка иконки из файла
	//wxIcon icon3(wxT("sample3.ico"), wxBITMAP_TYPE_ICO, 32, 32);
	//Загрузка иконки из ресурса
	wxIcon icon3(wxT("aaaIconFirst"));
	if (!icon3.Ok())
		wxMessageBox(wxT("Ресурс не загружен"));
	this->SetIcon(icon3);
	editTable = true;
	loadConfig();
	//Загружаем БД
	db = new MyDB(pathOfBD, tblsMachine, tblsList);
	
	//Меню
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(_T(""));
	fileMenu->Append(idMenuSave, wxT("Сохранить журнал\tCtrl-S"), wxT("Выгрузить журнал в Excel"));
	fileMenu->Append(idMenuEditSource, wxT("Редактировать список техники\tCtrl-E"), wxT("Открыть окно для редактирования списка техники"));
	fileMenu->Append(idMenuSettings, wxT("Настройки программы\tCtrl-P"), wxT("Изменить настройки программы"));
	fileMenu->Append(idMenuQuit, wxT("&Выход\tAlt-F4"), wxT("Выход из приложения"));
	mbar->Append(fileMenu, wxT("&Файл"));
	wxMenu* viewMenu = new wxMenu(_T(""));
	viewMenu->Append(idMenuView, wxT("Отобразить весь журнал\\в работе"), wxT("Отобразить весь список"));
	wxMenu* helpMenu = new wxMenu(_T(""));
	helpMenu->Append(idMenuAbout, wxT("&О программе\tF1"), wxT("Показывает информацию о программе"));
	mbar->Append(helpMenu, wxT("&Помощь"));
	SetMenuBar(mbar);

	//Статус бар
	CreateStatusBar(2);
	SetStatusText(wxT("Программа управления техникой"), 0);

	//Панели
	this->SetMinSize(wxSize(600, 400));
	mainSizer = new wxBoxSizer(wxHORIZONTAL);
	nb = new wxNotebook(this, -1, wxPoint(-1, -1), wxSize(300, 500), wxNB_TOP);
	extPanel = new extradition(nb);
	recPanel = new reception(nb);
	mBatteryPanel = new BatteryPanel(nb);
	nb->AddPage(recPanel, wxT("Приём техники"));
	nb->AddPage(extPanel, wxT("Выдача техники"));
	nb->AddPage(mBatteryPanel, wxT("Аккумуляторы"));
	
	nbGrid = new wxNotebook(this, wxID_ANY);
	grid = new gridFrame(nbGrid, wxID_ANY);
	remGrid = new gridFrame(nbGrid, wxID_ANY);
	mBatteryGrid = new gridFrameBattery(nbGrid, wxID_ANY);
	nbGrid->AddPage(grid, wxT("Техника в работе"));
	nbGrid->AddPage(remGrid, wxT("Техника в ремонте"));
	nbGrid->AddPage(mBatteryGrid, wxT("Список аккумуляторов"));
	//Панели для списков с техникой
	srcNb = new wxNotebook(this, wxID_ANY);
	srcEdit = new sourceEditPanel(this, pathOfBD, tblsMachine, tblsList, srcNb);
	mainSizer->Add(nb, 0, wxALL | wxEXPAND, 1);
	mainSizer->Add(nbGrid, 1, wxALL | wxEXPAND, 1);
	mainSizer->Add(srcEdit, 0, wxALL | wxEXPAND, 1);
	mainSizer->Add(srcNb, 1, wxALL | wxEXPAND, 1);
	mainSizer->Hide(srcEdit);
	mainSizer->Hide(srcNb);
	SetSizer(mainSizer);
	//Сортировки по умолчанию
	asc = true;
	sourceAsc = true;
	field = 2;
	sourceField = 0;
	workS = true;
	//Запускаем таймер
	pTimer = new wxTimer(this, idTimer);
	pTimer->Start(5000);
	Maximize(true);
	//События
	Connect(extradition::ID_BTNOK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&utkGuiFrame::OnExtBtnOK);
	Connect(extradition::ID_TXTNUMTS, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&utkGuiFrame::OnEnterExtradition);
	Connect(reception::ID_BTNOK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&utkGuiFrame::OnRecBtnOK);
	Connect(reception::ID_TXTNUMHOURS, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&utkGuiFrame::OnEnterReception);
	Connect(BatteryPanel::ID_TXTNUM, wxEVT_TEXT_ENTER, (wxObjectEventFunction)&utkGuiFrame::OnEnterBattery);
	Connect(BatteryPanel::ID_BTNOK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&utkGuiFrame::OnBatBtnOK);
	Connect(idMenuSave, wxEVT_MENU, (wxObjectEventFunction)&utkGuiFrame::OnSaveFile);
	Connect(idMenuView, wxEVT_MENU, (wxObjectEventFunction)&utkGuiFrame::OnViewMenu);
	Connect(idMenuQuit, wxEVT_MENU, (wxObjectEventFunction)&utkGuiFrame::OnQuitMenu);
	Connect(idMenuAbout, wxEVT_MENU, (wxObjectEventFunction)&utkGuiFrame::OnAboutMenu);
	Connect(idMenuSettings, wxEVT_MENU, (wxObjectEventFunction)&utkGuiFrame::OnSettingsMenu);
	Connect(pTimer->GetId(), wxEVT_TIMER, (wxObjectEventFunction)&utkGuiFrame::OnTimer);
	//Connect(grid->GetId(), wxEVT_GRID_COL_SORT, (wxObjectEventFunction)&utkGuiFrame::OnClickColumn);
	//Connect(sourceGrid->GetId(), wxEVT_GRID_COL_SORT, (wxObjectEventFunction)&utkGuiFrame::OnSourceClickColumn);
	Connect(idMenuEditSource, wxEVT_MENU, (wxObjectEventFunction)&utkGuiFrame::OnEditUnit);
	//Connect(grid->GetId(), wxEVT_GRID_LABEL_LEFT_CLICK, (wxObjectEventFunction)&utkGuiFrame::OnClickColumn);
}
void utkGuiFrame::DynInit() {
	//Нужно очистить текущие связи, если есть
	srcNb->DeleteAllPages();
	mSourceGrid.clear();
	//Созадём новые связи
	for (auto tableName : db->getSourceTableName()) {
		gridFrameSource* tmp =new gridFrameSource(srcNb, wxID_ANY);
		srcNb->AddPage(tmp, tableName);
		mSourceGrid.push_back(tmp);
	}
	//Добавляем вкладку аккумуляторы
	gridFrameSource* tmp = new gridFrameSource(srcNb, wxID_ANY);
	srcNb->AddPage(tmp, wxT("Аккумуляторы"));
	mSourceGrid.push_back(tmp);
}
void utkGuiFrame::OnEnterReception(wxCommandEvent& event) {
	wxCommandEvent eventt(wxEVT_BUTTON, reception::ID_BTNOK);
	OnRecBtnOK(eventt);
}
void utkGuiFrame::OnRecBtnOK(wxCommandEvent &event) {
	if (recPanel->getTxtNumTS() == "") {
		wxMessageBox(wxT("Не указан номер техники."));
		return;
	}
	if (recPanel->getTxtNumHours() == 0) {
		wxMessageBox(wxT("Не указаны моточасы."));
		return;
	}
	if (!db->inputUnit(recPanel->getTxtNumTS(), recPanel->getTxtNumHours())) {
		wxMessageBox(db->getErrMsg());
		return;
	}
	tableUpdate();
	recPanel->ClearAll();
}
void utkGuiFrame::OnEnterExtradition(wxCommandEvent& event) {
	wxCommandEvent eventt(wxEVT_BUTTON, extradition::ID_BTNOK);
	OnExtBtnOK(eventt);
}
void utkGuiFrame::OnExtBtnOK(wxCommandEvent& event) {
	if (extPanel->getTxtLogin() == "") {
		wxMessageBox(wxT("Логин не может быть пустым."));
		return;
	}
	if (extPanel->getTxtNumTS() == "") {
		wxMessageBox(wxT("Номер техники не может быть пустым"));
		return;
	}
	if (!db->outputUnit(extPanel->getTxtLogin(), extPanel->getTxtNumTS())) {
		wxMessageBox(db->getErrMsg());
		tableUpdate();
		return;
	}
	tableUpdate();
	extPanel->ClearAll();
}
void utkGuiFrame::OnEnterBattery(wxCommandEvent& event) {
	wxCommandEvent eventt(wxEVT_BUTTON, BatteryPanel::ID_BTNOK);
	OnBatBtnOK(eventt);
}
void utkGuiFrame::OnBatBtnOK(wxCommandEvent& event) {
	if (mBatteryPanel->GetTxtNum() == "") {
		wxMessageBox(wxT("Номер батареи не может быть пустым"));
		return;
	}
	auto rows = mBatteryGrid->GetRows();
	int find = -1;
	for (auto i = 0; i < rows.size(); i++) {
		if (mBatteryPanel->GetTxtNum() == rows[i][0]) {
			find = i;
			break;
		}
	}
	if (find == -1) {
		wxMessageBox(wxT("Батарея с таким номером отсутствует в базе"));
		return;
	}
	//Смотрим статус
	if (rows[find][1] != wxT("В работе")) {
		if (!db->outputBattery(mBatteryPanel->GetTxtNum())) {
			wxMessageBox(db->getErrMsg());
		}
	}
	else {
		if (!db->inputBattery(mBatteryPanel->GetTxtNum(), rows[find][3])) {
			wxMessageBox(db->getErrMsg());
		}
	}
	mBatteryGrid->Update();
	mBatteryPanel->Clear();
	return;
}
wxString utkGuiFrame::getPathToBD() {
	return pathOfBD;
}
int utkGuiFrame::getBadTime() {
	return badTime;
}
void utkGuiFrame::tableUpdate()
{

	if (!db->getDbIsCorrect()) {
		return;
	}
	std::vector<std::vector<wxString>> result;
	std::vector<std::vector<wxString>> remResult;
	std::vector<std::vector<wxString>> batResult;
	if (editTable) {
		result = db->getAllNeed(workS);
		grid->ClearGrid();
		if (result.size() > 0) grid->writeTable(result, badTime);
		remResult = db->getAllNeedRem();
		remGrid->ClearGrid();
		if (remResult.size() > 0) remGrid->writeTable(remResult, badTime);
		batResult = db->getBatteryList();
		mBatteryGrid->ClearGrid();
		if (batResult.size() > 0) mBatteryGrid->writeTable(batResult, badTime);
	}
	else {
		auto countPage = srcNb->GetPageCount();
		std::vector<gridFrameSource*> pages;
		for (auto i = 0; i < countPage; i++) {
			pages.push_back(dynamic_cast<gridFrameSource*>(srcNb->GetPage(i)));
		}
		for(auto i = 0; i < countPage; i++){
			result = db->getSourceUnit(i);
			pages[i]->ClearGrid();
			if (result.size() != 0) {
				if (!pages[i]->writeTable(result))
					wxMessageBox(wxT("Данные не записались"));
			}
		}
	}
}
void utkGuiFrame::preTableUpdate() {
	SettingsDialog *dlg = new SettingsDialog(this);
	if (!db->getDbIsCorrect()) {
		wxMessageBox(wxT("Текущая база данных не корректна, выберите другую базу данных."));
		do {
			if (dlg->ShowModal() == wxID_OK) {
				pathOfBD = dlg->getPathToDB();
			}
			//Если отмена, то выйти из приложения
			else break;
		} while (!updateUI());
	}
	if (!db->getDbIsCorrect()) Destroy();
	tableUpdate();
}
void utkGuiFrame::OnSaveFile(wxCommandEvent &event){
	wxFileDialog saveFile(this, wxT("Сохранение файла"), "", wxT("Учёт_техники"), wxT("excel файлы (*.xlsx)|*.xlsx|CSV files (*.csv)|*.csv"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFile.ShowModal() == wxID_CANCEL)
		return;
	wxString savefilename = saveFile.GetFilename();
	wxFileName fn(saveFile.GetFilename());	
	if (fn.GetExt() == wxT("xlsx")) {
		if (db->exportXLSX(saveFile.GetPath()))
			wxMessageBox(wxT("Файл записан"));
		else
			wxMessageBox(wxT("Файл не сохранен.\n") + db->getErrMsg(), wxT("ОШИБКА"), wxICON_ERROR);
	}
	else {
		if (db->exportCSV(saveFile.GetPath()))
			wxMessageBox(wxT("Файл записан"));
		else
			wxMessageBox(wxT("Файл не сохранен.\n") + db->getErrMsg(), wxT("ОШИБКА"), wxICON_ERROR);
	}
	tableUpdate();
}
void utkGuiFrame::OnViewMenu(wxCommandEvent &event){
    if(workS) workS = false;
    else workS = true;
    tableUpdate();
}
void utkGuiFrame::OnSettingsMenu(wxCommandEvent &event){
	SettingsDialog *dlg = new SettingsDialog(this);
	wxString tmpPathOfDB = pathOfBD;
	do {
		if (dlg->ShowModal() == wxID_OK) {
			pathOfBD = dlg->getPathToDB();
			badTime = dlg->getBadTime();
		}
		else pathOfBD = tmpPathOfDB;
	} while (!updateUI());
	DynInit();
	tableUpdate();
}
void utkGuiFrame::OnQuitMenu(wxCommandEvent &event){
	Destroy();
}
void utkGuiFrame::OnAboutMenu(wxCommandEvent &event) {
	AboutDialog *dlg = new AboutDialog(this, programVersion);
	dlg->ShowModal();
}
void utkGuiFrame::OnTimer(wxCommandEvent &event){
	tableUpdate();
}
void utkGuiFrame::OnEditUnit(wxCommandEvent & event){
	//Переключения вида таблиц (редактирование/использование)
	if (editTable) {
		//сделать основным и единственным окно редактирования
		mainSizer->Hide(nbGrid);
		mainSizer->Hide(nb);
		mainSizer->Show(srcEdit);
		mainSizer->Show(srcNb);
		editTable = false;
		srcNb->ChangeSelection(1);	//Почему-то не работает, если сразу активна первая вкладка
		srcNb->ChangeSelection(0);
		//mainSizer->Layout();
		mainSizer->RecalcSizes();
		
	}
	else {
		//Вернуть исходное состояние
		mainSizer->Hide(srcEdit);
		mainSizer->Hide(srcNb);
		mainSizer->Show(nb);
		mainSizer->Show(nbGrid);
		editTable = true;
		mainSizer->Layout();
	}
	tableUpdate();
}
bool utkGuiFrame::updateUI(){
	db->renewDB(pathOfBD, tblsMachine, tblsList);
	srcEdit->renewDB(pathOfBD);
	if (db->getDbIsCorrect()) {
		//Сохраняем в файле настроек
		//wxString path = wxT("config.ini");
		writeConfig();
		/*
		wxFile file;
		wxString path = wxT("config.ini");
		if (!wxFile::Exists(path)) {
			file.Create(path, true);
		}
		file.Open(path, wxFile::write);
		if (!file.IsOpened()) {
			myFunc::writeLog("Не возможно открыть файл: \t" + path);
		}
		else {
			file.Write(pathOfBD);
			file.Close();
		}*/
		return true;
	}
	else {
		wxMessageBox(db->getErrMsg());
		return false;
	}
	//tableUpdate();
}
void utkGuiFrame::loadConfig() {
	wxFile file;
	wxString path = wxT("config.ini");
	pathOfBD = "utk_database.db";
	tblsMachine = { wxT("Штабеллёры"), wxT("Электротележки"), wxT("Комплектовщики"), wxT("ЭлТележки_с_площ") };
	tblsList = { wxT("списокШтабеллёры"), wxT("списокЭлектротележки"), wxT("списокКомплектовщики"), wxT("списокЭл_тел_отк_пл") };
	
	badTime = 13;
	
	
	if (!wxFile::Exists(path)) {
		writeConfig();
	}
	else {
		nlohmann::json config;
		std::ifstream inFile(path.ToStdString());
		try {
			inFile >> config;
			try {
				if (config.at("pathOfBD") != "") pathOfBD = wxString(config.at("pathOfBD").get<std::string>());
			}
			catch (nlohmann::detail::exception &e) {
				wxMessageBox(wxT("Отсутствует аргумент pathOfBD.\nБудет использовано значение по умолчанию."));
				myFunc::writeLog(e.what());
			}
			try {
				if (config.at("badTime") != "") badTime = config.at("badTime").get<int>();
			}
			catch (nlohmann::detail::exception &e) {
				wxMessageBox(wxT("Отсутствует аргумент badTime.\nБудет использовано значение по умолчанию."));
				myFunc::writeLog(e.what());
			}
			try {
				if (config.at("fields") != "") {
					std::vector<std::string> strTblsMachine = config.at("fields");
					tblsMachine.clear();
					tblsList.clear();
					for (auto f : strTblsMachine) {
						tblsMachine.push_back(wxString::FromUTF8(f));
					}
					for (auto f : tblsMachine) {
						tblsList.push_back(wxT("список") + f);
					}
				}
			}
			catch (nlohmann::detail::exception &e) {
				wxMessageBox(wxT("Отсутствует аргумент field.\nБудет использовано значение по умолчанию."));
				myFunc::writeLog(e.what());
				writeConfig();
			}
		}
		catch (nlohmann::json::parse_error &e) {
			wxMessageBox(wxT("Некорректный файл конфигурации.\nБудут использованы значения по умолчанию."));
			myFunc::writeLog(e.what());
			writeConfig();
		}
		
	}	
}

void utkGuiFrame::writeConfig()
{
	nlohmann::json j;
	j["pathOfBD"] = std::string(pathOfBD.utf8_str());
	j["badTime"] = badTime;
	std::vector<std::string> strTblsMachine;
	for (auto f : tblsMachine) {
		strTblsMachine.push_back(std::string(f.utf8_str()));
	}
	j["fields"] = strTblsMachine;

	std::ofstream out("config.ini");
	out << "\xef\xbb\xbf";		//BOM for UTF
	out << std::setw(4) << j << std::endl;
}
