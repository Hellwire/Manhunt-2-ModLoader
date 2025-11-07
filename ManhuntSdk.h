#pragma once
#include "CustomStucts.h"
#include "calling.h"
#include "MemoryMgr.h"

class ManhuntSdk
{
	public:

		// the regular mls GetEntity function (return a cloned entity)
		static CSDKEntity* GetEntityInstance(const char* name);
		static CSDKEntity* GetEntity(const char* name);
		static CSDKEntity* SpawnHunterNearByPlayer(const char* name);
		static CSDKEntity* GetPlayer();

		static CSDKVector3 GetCurrentPosition();

		static MouseKeys GetCurrentMouseKey();
		static CGameSettings* GetGameSettings();

		// internal function to get the real entity (without to force a new clone)
		static int GetEntityPointer(const char* name);
		static int GetWeaponPointer(int character, int itemId);
		static int CreateEntityClass(char* name);

		static void Teleport(CSDKVector3 vector);
		static void ToggleFunWeapons();
		static void ToggleFlowers();
		static void ToggleFunMode();
		static void ClearWriteDebugOutput();
		static void SetWriteDebugDisplayDuration(int duration);
		static void SetWriteDebugFontSize(float size);
		static void SetFreeCamera(bool status);
		static void DrawCurrentPosition();
		static void SetPlayerVisible(bool status);
		static void AiAddEntity(const char* entityName);
		static void MoveEntity(CSDKEntity* entity, CSDKVector3 vec);
		static void SetCameraPosition(Cameras camera, CSDKVector3 vector);
		static void Debug(char* text, int sleep);
		static void CreateInventoryItem(CSDKEntity* ent, int item, bool b1);
		static void AddAmmoToInventoryWeapon(int item, int amount);
		static void Spawn(int item, bool firearm, const char* record);
		static void SetExecutionFlashEffectStatus(bool status);
		static void EffectExecutionColramp(int texture, int rnd, float x, float y, float z);
		static void InterrupExecution();
		static void SetIsmMovementEnabled(bool status);
		static void FreezeCameraAllowMovements();

		static bool IsPlayerCrawling();

		template<typename ...Args>
		static void WriteDebug(int line, const char* msg, Args ...args)
		{
			Call<0x53D8D0, int, const char*, Args...>(line, msg, args...);
		}



};


