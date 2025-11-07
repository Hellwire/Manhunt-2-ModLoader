#pragma once

#include "string"


enum MouseKeys
{
	LEFT,
	RIGHT,
	MOVE,
	NONE
};

enum Types
{
	WEAPON,
	SET_PLAYER_POS,
	MENU_ENTRY,
	TOGGLE,
	TRIGGER
};

enum Cameras
{
	DEFAULT_1,
	DEFAULT_2
};

struct CGameSettings
{
	/*
	char version1;				//0x7604B0 + 0	=> 1
	char version2;				//0x7604B0 + 1	=> 0
	char version3;				//0x7604B0 + 2	=> 5
	+ 54 some invertation 1 or 0 results in 0.75 or -0.75

	int fps;					//0x7604B0 + 60 => should be 0 or 1
	int difficult;				//0x7604B0 + 64

	char year;					//0x7604B0 + 70
	char month;					//0x7604B0 + 71
	char day;					//0x7604B0 + 73
	char hour;					//0x7604B0 + 74
	char minute;				//0x7604B0 + 75
	char second;				//0x7604B0 + 76

	int soundType;			//0x7604B0 + 180
	*/

	char unknown2[200];
	int widescreen;			//0x7604B0 + 200
	int screenH;			//0x7604B0 + 204
	int screenV;			//0x7604B0 + 208
	int controllerSetup;	//0x7604B0 + 212
	int invertLook;			//0x7604B0 + 216
	int aimSensitivity;		//0x7604B0 + 220
	int vibration;			//0x7604B0 + 224
	int lookx;				//0x7604B0 + 228
	int looky;				//0x7604B0 + 232
	int advancedgun;		//0x7604B0 + 236
	int exectype;			//0x7604B0 + 240
	int targetx;			//0x7604B0 + 244
	int targety;			//0x7604B0 + 248
};

struct CCamera
{
	float currentPosition;	//0x789490 + 4256 | updated by 0x5AF600
	float center;			//0x789490 + 4260 | updated by 0x5AF600
	float rangeUp;			//0x789490 + 4264
	float rangeDown;		//0x789490 + 4268
	float rangeLeft;		//0x789490 + 4272
	float rangeRight;		//0x789490 + 4276
	char enableFPS;			//0x789490 + 4280
	char enableUnknown;		//0x789490 + 4281
};

struct MenuEntry
{
	Types type;
	std::string label;
	std::string entityName;
	std::string gotoMenu;
	int id;
	bool needAmmo;
	std::string originalLabel;
	bool toggleActive;
};

struct Menus {
	MenuEntry* entries;
};

struct CTest {
	char field[700];
//	float health; // 124 + 4 == 128
	int armour;
};

struct CEntity {

	/*
	PlayerPointer + 124 health


	//fps cam is on (?)
	*(_DWORD *)(PlayerPointer + 4380) & 0x40000

	 SetCameraPosition(
	  (int)&unk_6ECD10,
	  (int)(playerPointer + 5112),
	  (int)(playerPointer + 5116),
	  (int)(playerPointer + 5120));
	
		float currentPosition;	//0x789490 + 4256 | updated by 0x5AF600
	float center;			//0x789490 + 4260 | updated by 0x5AF600
	float rangeUp;			//0x789490 + 4264
	float rangeDown;		//0x789490 + 4268
	float rangeLeft;		//0x789490 + 4272
	float rangeRight;		//0x789490 + 4276
	char enableFPS;			//0x789490 + 4280
	char enableUnknown;		//0x789490 + 4281

	PlayerPointer + 100 => function arguments 1 INT
	PlayerPointer + 448 => function arguments 2 double, int

		(*(void(__stdcall * *)(int))(*(int*)*(int*)0x789490 + 88))(1);


	*/

	//todo: pretty much everything
	char field[124];
	float health; // 124 + 4 == 128
	//544-128 == 416
//	char field2[416];
	//char canJump; //544
};

struct CVector {
	float x, y, z;
};

/*

struct CEntity {
	//todo: pretty much everything
	char unknown[108];
	char weaponId[4];		// this + 108

	char field[12];
	float health;
};
*/
struct CSDKVector3{
	float x;
	float y;
	float z;
};


struct CSDKEntity {
	int functionPointer;		//0 + 4´== 4

	CSDKVector3 position;		//4 + 12 == 16

	int unknownNumber;			//16 + 4 == 20

	CSDKVector3 position2;		//20 + 12 == 32

	char unknown[84];			// 32 + 84 == 116

	int invulnnerable;			// 116 + 4 = 120

	char unknown2[4];			// 120 + 4 == 124

	float health;				// 124 + 4 == 128

	char unknown3[4272];			// 128 + 4272 == 4400


	float heading;			// 4400 + 4 = 4404
};

/*
struct CSDKEntityFunctions {
	char unknown[60];			// read unknown 60bytes 

	int moveFunction;			// 60 + 4



};
*/