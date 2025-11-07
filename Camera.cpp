#pragma once
#include "Camera.h"


ManhuntSdk* mhSDK = new ManhuntSdk();

void __fastcall Camera::EnableFPSCamera(int ptr) {

	int v1;
	int v2;
	signed int v3;
	double v4;
	double v5;
	int v6;
	double v7;
	float v8;
	float v9;
	float v10;
	float v11;

	v1 = ptr;
	if (!(*(int*)(ptr + 4380) & 0x48000))
	{



		v2 = *(int*)(ptr + 700);
		v3 = *(int*)(v2 + 336);
		if (v3 < 49 || v3 > 55)
		{
			if (v3 == 15)
			{

				v6 = *(int*)(v2 + 60);
				if (v6 == 1115 || v6 == 1117)
				{
					v11 = 90.0;
					v10 = -45.0;
					v4 = 150.0;
				LABEL_21:
					v8 = v4;
					v9 = v4;

					((bool(__thiscall*)(int, float, float, float, float, int))0x5AF5A0)(ptr, v9, v8, v10, v11, 1);
					*(int*)(v1 + 4380) |= 0x40000u;
					((signed int(__thiscall*)(int, int))0x503280)(v1, 0);
					*(float*)0x76E37C = *(float*)0x789544;
					*(char*)0x6D02CD = 1;
					*(float*)0x789544 = *(float*)0x6D0C4C;
					return;
				}
			}
			if (((int(__thiscall*)(int))0x4F7640)(*(int*)ptr + 700))
				v7 = 45.0;
			else
				v7 = 90.0;
			v11 = v7;
			v5 = -90.0;
		}
		else
		{
			if (((int(__thiscall*)(int))0x4F76D0)(*(int*)ptr + 700))
			{
				v4 = 150.0;
				v11 = 150.0;
				v10 = 0.0;
			LABEL_20:
				ptr = v1;
				goto LABEL_21;
			}
			if (!(((int(__thiscall*)(int))0x4F7710)(v2)))
			{
				ptr = v1;
				if (*(int*)(v2 + 4 * *(int*)(v2 + 336)) == 1972)
				{
					v11 = 270.0;
					v10 = 90.0;
					v4 = 150.0;
				}
				else
				{
					v11 = 90.0;
					v10 = -90.0;
					v4 = 150.0;
				}
				goto LABEL_21;
			}
			v11 = 0.0;
			v5 = -150.0;
		}
		v10 = v5;
		v4 = 150.0;
		goto LABEL_20;
	}

}

/*
void Camera::EnableFPSCamera(int ptr) {
	int v1;
	int v2;
	signed int v3;
	double v4;
	double v5;
	int v6;
	double v7;
	float v8;
	float v9;
	float v10;
	float v11;

	v1 = ptr;
	if (!(*(int*)(ptr + 4380) & 0x48000))
	{



		v2 = *(int*)(ptr + 700);
		v3 = *(int*)(v2 + 336);
		if (v3 < 49 || v3 > 55)
		{
			if (v3 == 15)
			{

				v6 = *(int*)(v2 + 60);
				if (v6 == 1115 || v6 == 1117)
				{
					v11 = 90.0;
					v10 = -45.0;
					v4 = 150.0;
				LABEL_21:
					v8 = v4;
					v9 = v4;

					((bool(__thiscall*)(int, float, float, float, float, int))0x5AF5A0)(ptr, v9, v8, v10, v11, 1);
					*(int*)(v1 + 4380) |= 0x40000u;
					((signed int(__thiscall*)(int, int))0x503280)(v1, 0);
					*(float*)0x76E37C = *(float*)0x789544;
					*(char*)0x6D02CD = 1;
					*(float*)0x789544 = *(float*)0x6D0C4C;
					return;
				}
			}
			if (((int(__thiscall*)(int))0x4F7640)(*(int*)ptr + 700))
				v7 = 45.0;
			else
				v7 = 90.0;
			v11 = v7;
			v5 = -90.0;
		}
		else
		{
			if (((int(__thiscall*)(int))0x4F76D0)(*(int*)ptr + 700))
			{
				v4 = 150.0;
				v11 = 150.0;
				v10 = 0.0;
			LABEL_20:
				ptr = v1;
				goto LABEL_21;
			}
			if (!(((int(__thiscall*)(int))0x4F7710)(v2)))
			{
				ptr = v1;
				if (*(int*)(v2 + 4 * *(int*)(v2 + 336)) == 1972)
				{
					v11 = 270.0;
					v10 = 90.0;
					v4 = 150.0;
				}
				else
				{
					v11 = 90.0;
					v10 = -90.0;
					v4 = 150.0;
				}
				goto LABEL_21;
			}
			v11 = 0.0;
			v5 = -150.0;
		}
		v10 = v5;
		v4 = 150.0;
		goto LABEL_20;
	}

}
*/
void Camera::DisableFPSCamera( int playerPointer ) {

	return;
	signed int v2; // eax@2
	int v3; // eax@7
	int v4; // ecx@9
	int v5; // ecx@9

	if (*(int*)(*(int*)0x789490 + 4380) & 0x40000)
	{

		//RestoreFpsCamera(*(int*)0x789490);
		CallMethod<0x598EB0, int>(*(int*)0x789490);
		return;

		*(int*)(*(int*)0x789490 + 0x111C) &= 0xFFFBFFFF;
		v2 = *(int*)(*(int*)(*(int*)0x789490 + 700) + 336);
		if (v2 < 39 || v2 > 47)
	//		sub_4FF6D0(*(int*)0x789490);
			CallMethod<0x4FF6D0, int>(*(int*)0x789490);

		if (!*(char*)(*(int*)0x789490 + 4281))
		{
			*(float*)(0x789490 + 780) = *(float*)(0x789490 + 824) + *(float*)(0x789490 + 780);
			*(float*)(0x789490 + 824) = 0.0;
		}
	//	sub_4F8050(*(int*)0x789490);
		CallMethod<0x4F8050, int>(*(int*)0x789490);

		*(int*)(*(int*)0x789490 + 4380) |= 0x8000u;
		v3 = *(int*)(*(int*)(*(int*)0x789490 + 704) + 136);
		if (v3 == 12 || v3 == 13)
		{
			//sub_503280(*(int*)0x789490, 0);
			CallMethod<0x503280, int, int>(*(int*)0x789490, 0);

			//sub_4FE130(*(int*)0x789490);
			CallMethod<0x4FE130, int>(*(int*)0x789490);

			v4 = *(int*)(*(int*)0x789490 + 704);
//			sub_4930C0(0);
			CallMethod<0x4930C0, int>(0);

			v5 = *(int*)(*(int*)0x789490 + 196);
			//sub_4AFB20(*(int*)0x789490);
			CallMethod<0x4AFB20, int>(*(int*)0x789490);

		}
//		playerVisible = 1;
		mhSDK->SetPlayerVisible(true);
//		FPSCameraFOV_0 = FPSCameraFOV_1;
		*(float*)0x789544 = *(float*)0x76E37C;
	}
}
