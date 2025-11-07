#pragma once
#define MENU_ENTRIES_PER_PAGE 20

#include "stdafx.h"
#include "System.h"
#include "CustomStucts.h"
#include "ManhuntSdk.h"
#include "mh2.h"
#include "MemoryMgr.h"

#include <fstream>

class CMenu
{
	private:
		int Selection = 0;
		int CurrentPage = 0;
		int MenuPosition = 0;
		int DisplayMenu = 0;
		int CurrentStart = 0;

		bool mainMenuSetting1Active = false;
		bool mainMenuSetting2Active = false;

		std::string CurrentMenuName = "home";
		MenuEntry* CurrentMenu;

	public:
		void BuildMenu();
		void ProcessMenu();
		void ProcessUserNavigation();
		void PrintMenu();
		MenuEntry& GetSelectedEntry();

};

