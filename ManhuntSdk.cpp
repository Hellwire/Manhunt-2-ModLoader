#include "ManhuntSdk.h"
#include "MemoryMgr.h"

void ManhuntSdk::ClearWriteDebugOutput() {

	//todo , es gibt hier nen clear befehl ....
	SetWriteDebugDisplayDuration(1);

	for (int i = 0; i <= 22; i++)
	{
		WriteDebug(i, "");
	}


	SetWriteDebugDisplayDuration(6000);
}

void ManhuntSdk::SetWriteDebugDisplayDuration( int duration ) {
	Memory::VP::Patch<char>(0x53D8DC, duration);
}

void ManhuntSdk::SetWriteDebugFontSize(float size)
{
	Memory::VP::Patch<float>(0x63D294, size);
}

void ManhuntSdk::ToggleFunMode() {
	ToggleFunWeapons();
	ToggleFlowers();
}

void ManhuntSdk::ToggleFunWeapons() {
	*(int*)0x76BE40 = *(int*)0x76BE40 == 32 ? 0 : 32;
}

void ManhuntSdk::ToggleFlowers() {
	*(char*)0x6b26e5 ^= 1;
}

void ManhuntSdk::SetFreeCamera(bool status) {
	*(int*)0x7894A4 = (int)status;
}

void ManhuntSdk::SetIsmMovementEnabled(bool status) {
	*(int*)0x7894A8 = (int)status;
}


void ManhuntSdk::FreezeCameraAllowMovements() {
	SetFreeCamera(true);
	SetIsmMovementEnabled(true);
}

void ManhuntSdk::SetCameraPosition(Cameras camera, CSDKVector3 vector) {
	/*
	
	CSDKVector3 vec;
	vec.x = 0.0;
	vec.y = 0.0;
	vec.z = 0.0;

	SetCameraPosition(Cameras::DEFAULT_1, vec);
	SetCameraPosition(Cameras::DEFAULT_2, vec);
	*/

	switch (camera) {
		case Cameras::DEFAULT_1:
			*(CSDKVector3*)0x76F2F0 = vector;
			break;
		case Cameras::DEFAULT_2:
			*(CSDKVector3*)0x76F300 = vector;
			break;
	}

}

MouseKeys ManhuntSdk::GetCurrentMouseKey() {
	switch (*(int*)0x76BE6C) {
		case 0: return MouseKeys::NONE;
		case 2: return MouseKeys::LEFT;
		case 32: return MouseKeys::RIGHT;
		case 1024: return MouseKeys::MOVE;
	}

	return MouseKeys::NONE;
}

void ManhuntSdk::InterrupExecution() {
	CGameSettings* sett = GetGameSettings();
	sett->exectype ^= 1;
	Sleep(100);
	sett->exectype ^= 1;
}


CGameSettings* ManhuntSdk::GetGameSettings() {
	return (CGameSettings*) * (int*)0x7604B0;
}


void ManhuntSdk::DrawCurrentPosition() {
	char buffer[64];
	CSDKVector3 pos = GetCurrentPosition();
	sprintf_s(buffer, "X = %f Y = %f Z = %f", pos.x, pos.y, pos.z);
	WriteDebug(22, buffer);
}

void ManhuntSdk::EffectExecutionColramp(int rolramp, int rnd, float x, float y, float z) {
	/*
	
	int rnd = rand();
	EffectExecutionColramp(
			*(int*)0x797718,
		rnd % 5 + 4,
		0.05,
		0.05,
		0.1
	);

	*/

	((void(__cdecl*)(int, int, float, float, float))0x605E20)(rolramp, rnd, x, y, z);
}

void ManhuntSdk::AiAddEntity(const char* entityName) {
	CallMethod<0x42E0C0, int, const char*>(0x6E5F78, entityName);
}


void ManhuntSdk::MoveEntity(CSDKEntity* entity, CSDKVector3 vec) {

	//TODO: why is my struct pointer not working ?! ask ermaccer...
//	(*(void(__thiscall * *)(CSDKEntity*, CSDKVector3*, signed int))(entityFunctions.moveFunction))(entity, &vec, 1);
	(*(void(__thiscall * *)(CSDKEntity*, CSDKVector3*, signed int))(*(int*)entity + 60))(entity, &vec, 1);
}


CSDKVector3 ManhuntSdk::GetCurrentPosition() {

	CSDKVector3 pos;
	pos.x = *(float*)0x704F68;
	pos.z = *(float*)0x704F70;
	pos.y = *(float*)0x704F6C;
	return pos;
}

int currentCloneHunterIndex = 1;

CSDKEntity* ManhuntSdk::SpawnHunterNearByPlayer(const char* name) {

	AiAddEntity(name);

	CSDKEntity* hunter = GetEntity(name);

	CSDKVector3 pos = GetCurrentPosition();

	pos.z += 0.7;

	MoveEntity(hunter, pos);

	return hunter;
}

int ManhuntSdk::CreateEntityClass(char * name) {

	//int* scriptEntity = CallAndReturn<int*, 0x4E9130, char*>(name);
	CSDKEntity* scriptEntity = GetEntityInstance(name);

	if (scriptEntity) {
		return CallAndReturn<int, 0x4E12A0, CSDKEntity*>(scriptEntity);
	}
	else {
		printf("Entity %s not found!", name);
	}
}

void ManhuntSdk::Teleport(CSDKVector3 vec) {
	(*(void(__thiscall * *)(CSDKEntity *, CSDKVector3*, signed int))(*(int*) * (int*)0x789490 + 60))(GetPlayer(), &vec, 1);
}

void ManhuntSdk::Spawn(int itemId, bool firearm, const char* record)
{

	if (GetEntityInstance(record))
	{
		CreateInventoryItem(GetPlayer(), itemId, true);
		if (firearm == true) AddAmmoToInventoryWeapon(itemId, 150);
	}
	else printf("record %s does not exist here!\n", record);
}

CSDKEntity* ManhuntSdk::GetPlayer()
{
	return *(CSDKEntity * *)0x789490;
}

CSDKEntity* ManhuntSdk::GetEntityInstance(const char* name)
{
	return CallAndReturn<CSDKEntity*, 0x4E9130, const char*>(name);
}

int ManhuntSdk::GetEntityPointer(const char* name)
{
	return CallAndReturn<int, 0x4EC530, const char*>(name);
}

CSDKEntity* ManhuntSdk::GetEntity(const char* name)
{ 
	CSDKEntity* entity = CallAndReturn<CSDKEntity*, 0x4EC530, const char*>(name);
	if (!entity) {
		printf("Entity %s is not loaded.\n", name);
	}
	return entity;
}


void ManhuntSdk::CreateInventoryItem(CSDKEntity* ent, int item, bool b1)
{
	CallMethod<0x4F6500, CSDKEntity*, int, bool>(ent, item, b1);
}

void ManhuntSdk::AddAmmoToInventoryWeapon(int itemId, int amount)
{
	int player = *(int*)(*(int*)0x789490 + 444);
	int weaponPointer = GetWeaponPointer(player, itemId );

	((void(__thiscall*)(int, int))0x5D2B30)(*(int*)(weaponPointer + 516), amount);
}

int ManhuntSdk::GetWeaponPointer( int character, int itemId) {

	//generate a weapon pointer for the given character
	return ((int(__thiscall*)(int, int))0x5726F0)(character, itemId);
}

void ManhuntSdk::SetPlayerVisible(bool status) {

	*(char*)0x6D02CD = (char)status;
}

bool ManhuntSdk::IsPlayerCrawling()
{
	return (bool)*(int*)(*(int*)(*(int*)(0x789490 + 700)) + 336) == 69;
}


void ManhuntSdk::Debug(char* text, int sleep) {
	WriteDebug(22, text);
	Sleep(sleep);
}

void ManhuntSdk::SetExecutionFlashEffectStatus(bool status) {
	Memory::VP::Patch<INT8>(0x4AB1B0, status ? 0x74 : 0x75);
}


