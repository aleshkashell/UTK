#ifndef MYFRAME_H_INCLUDED
#define MYFRAME_H_INCLUDED

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/datetime.h>
#include <wx/filename.h>	
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <json.hpp>
#include <algorithm>
#include <memory>
#include "reception.h"
#include "extradition.h"
#include "BatteryPanel.h"
#include "gridFrame.h"
#include "gridFrameSource.h"
#include  "gridFrameBattery.h"
#include "sourceEditPanel.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"
#include "logFile.h"
#include "Definitions.h"

class utkGuiFrame : public wxFrame
{
    public:
        utkGuiFrame(const wxString &title);

        //События
        void OnEnterExtradition(wxCommandEvent& event);
        void OnExtBtnOK(wxCommandEvent& event);
        void OnEnterReception(wxCommandEvent &event);
        void OnRecBtnOK(wxCommandEvent &event);
		void OnEnterBattery(wxCommandEvent &event);
		void OnBatBtnOK(wxCommandEvent &event);
        void OnSaveFile(wxCommandEvent &event);
        void OnViewMenu(wxCommandEvent &event);
		void OnSettingsMenu(wxCommandEvent &event);
		void OnQuitMenu(wxCommandEvent &event);
		void OnTimer(wxCommandEvent& event);
		void OnEditUnit(wxCommandEvent& event);
		void OnAboutMenu(wxCommandEvent& event);

		//Helpers
		wxString getPathToBD();
		int getBadTime();
		
		bool updateUI();
        void tableUpdate();
		void preTableUpdate();	//Проверка, что с изначальной БД можно работать
		void loadConfig();
		void writeConfig();		//Сохранить конфиг с текущими параметрами
		
		//Наша база данных
		MyDB *db;
    private:
		//Инициализация постоянных данных
		void StableIniT();
		//Инициализация обновляемых данных
		void DynInit();
        //Панели
        extradition *extPanel;
        reception *recPanel;
		BatteryPanel* mBatteryPanel;
        //Путь до БД
        wxString pathOfBD;

        //Основная таблица
        gridFrame *grid;
		//Таблица ремтонта
		gridFrame *remGrid;
		//Таблица с батареями
		gridFrameBattery *mBatteryGrid;
		//Таблица со списком батарей
		gridFrameSource *sourceBattery;
		//Таблицы со списком техники
		std::vector<gridFrameSource*> mSourceGrid;
		//Панелька с вводом данных
		wxNotebook *nb;
		//Панель с таблицами выданной техники
		wxNotebook *nbGrid;
		//Панель с исходными таблицами
		wxNotebook *srcNb;
		//Панель редактирования исходной таблицы
		sourceEditPanel *srcEdit;
		//таймер обновления
        wxTimer *pTimer;
        int field, sourceField;	//Поле сортировки
        bool asc, sourceAsc;	//По возрастанию?
        bool workS;	//Только в работе?
		bool editTable;	//Показать таблицу редактирования
		int badTime; //Просроченная техника
		std::vector<wxString> tblsMachine;	//Таблицы выдачи
		std::vector<wxString> tblsList;		//Таблицы со списком техники
		std::string programVersion;

		//Основной слой
		wxBoxSizer *mainSizer;
		//Дополнительный слой
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout,
            idMenuSave,
			idMenuEditSource,
            idMenuView,
			idMenuSettings,
            idTimer
        };
};

#endif
