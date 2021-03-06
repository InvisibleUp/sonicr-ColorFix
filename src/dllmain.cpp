// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <windows.h>
#include <algorithm>
#include "../include/MemAccess.h"
#include "../include/SonicRModLoader.h"

// Raw assembly code to call 
// IDirect3DDevice9::SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADDSIGNED)
__declspec(naked) void SetSaturnMode() {
	__asm {
		mov eax, dword ptr ds : [005EDD14h]
		push 8
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]
		ret
	}
}

__declspec(naked) void InLevel_Start1() {
	__asm {
		mov eax, 0x417F00  // D3D_RenderOval
		call eax

		mov eax, dword ptr ds : [005EDD14h]
		push 4
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]
		ret
	}
}
__declspec(naked) void InLevel_Start2()
{
	__asm {
		push eax
		push ecx

		mov eax, dword ptr ds : [005EDD14h]
		push 8
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]

		pop ecx
		pop eax

		// Direct from source
		mov edx, dword ptr ds : [0085DF28h]
		push edx
		mov eax, 0x412E30 // D3D_RenderCharacters
		call eax
		add esp, 4
		mov eax, 0x40E2A0 // D3D_RenderTrack
		call eax

		ret
	}
}

__declspec(naked) void InLevel_End2()
{
	__asm {
		push eax
		push ecx

		mov eax, dword ptr ds : [005EDD14h]
		push 4
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]

		pop ecx
		pop eax

		mov eax, 0x4425F0
		call eax	// D3D_EndScene

		ret
	}
}

__declspec(naked) void MakeTrackTranslucent_0()
{
	__asm {
		shl eax, 8
		or eax, edx
		add esi, 0x20
		shl eax, 8
		or eax, ecx
		shl eax, 8
		or eax, ebp

		cmp dword ptr ds : [0x7AF13C], 5		// Radiant emerald?
		jne return

		and eax, 0x00FFFFFF
		add eax, 0x9F000000

		return:
		ret
	}
}

__declspec(naked) void MakeTrackTranslucent_1()
{
	__asm {		
		shl eax, 8
		or eax, edx
		add esi, 0x20
		shl eax, 8
		or eax, ecx
		shl eax, 8
		or eax, ebx

		cmp dword ptr ds : [0x7AF13C], 5		// Radiant emerald?
		jne return

		and eax, 0x00FFFFFF
		add eax, 0x9F000000

		return:
		ret
	}
}

__declspec(naked) void MakeTrackTranslucent_3()
{
	__asm {
		shl eax, 8
		or eax, edx
		shl eax, 8
		or eax, ecx
		shl eax, 8
		or eax, ebx

		cmp dword ptr ds : [0x7AF13C], 5		// Radiant emerald?
		jne return

		and eax, 0x00FFFFFF
		add eax, 0x9F000000

		return:
		ret
	}
}

__declspec(naked) void D3D_Render2DObject_Fix() {
	__asm {
		// Load the argument containing the vertex color
		mov eax, [esp+0x40]
		push ebx
		push ecx
		push edx
		// Load each component into a new register
		mov ebx, eax // A
		mov ecx, eax
		mov edx, eax
		// Isolate each component and move to the lowest byte
		shr ecx, 16 // R
		shr edx, 8  // G
		and eax, 0xFF
		and ebx, 0xFF000000
		and ecx, 0xFF
		and edx, 0xFF
		// Divide everything by 2
		shr al, 1
		shr cl, 1
		shr dl, 1
		// Merge back into a single int
		shl edx, 8
		shl ecx, 16
		add eax, edx
		add eax, ecx
		add eax, ebx
		pop edx
		pop ecx
		pop ebx
		mov [esp+0x40], eax

		// What we patched over
		fild ds:[0x7BCB88]

		ret
	}
}

__declspec(naked) void D3D_RenderOverlay_Start() {
	__asm {
		mov eax, dword ptr ds : [005EDD14h]
		push 8
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]

		mov eax, dword ptr ds : [0x73604C]
		ret
	}
}

__declspec(naked) void D3D_RenderOverlay_Demo() {
	__asm {
		mov eax, 0x40C270	// D3D_Render2DObject
		call eax
		add esp, 0x2C

		mov eax, 0x4425F0	// D3D_EndScene
		call eax

		mov eax, dword ptr ds : [005EDD14h]
		push 4
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]

		mov eax, 0x4424D0	// D3D_BeginScene
		call eax

		ret
	}
}

__declspec(naked) void D3D_RenderOverlay_Pause() {
	__asm {
		mov eax, 0x4425F0	// D3D_EndScene
		call eax

		mov eax, dword ptr ds : [005EDD14h]
		push 4
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]

		mov eax, 0x4424D0	// D3D_BeginScene
		call eax

		ret
	}
}

__declspec(naked) void  FlagBG_Start() {
	__asm {
		mov eax, 0x4425F0	// D3D_EndScene
		call eax

		mov eax, dword ptr ds : [005EDD14h]
		push 4
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]

		mov eax, 0x4424D0	// D3D_BeginScene
		call eax

		xor ecx, ecx
		mov eax, 2
		ret
	}
}

__declspec(naked) void Oval_Begin() {
	__asm {
		mov eax, 0x4424D0  // D3D_BeginScene
		call eax

		mov eax, dword ptr ds : [005EDD14h]
		push 4
		push 1
		push 0
		mov ecx, [eax]
		push eax
		call[ecx + 0x10C]
		ret
	}
}

extern "C" {
	__declspec(dllexport) ModInfo SonicRModInfo = { ModLoaderVer };
	__declspec(dllexport) void __cdecl Init()
	{
		unsigned char jmp = 0xEB;	// JMP opode
		unsigned char ringfix[] = { 0x0D, 0x7F, 0x7F, 0x7F, 0x00 }; // or eax, (half-bright)
		unsigned char jumpfix[] = { 0xB8, 0x7F, 0x00, 0x00, 0x00 };
		unsigned char bgfix[] = { 0xBB, 0x7F, 0x7F, 0x7F, 0xFF };
		unsigned char nop[255];      // Many NOP opcodes
		std::fill_n(nop, 255, 0x90);

		WriteData((void *)0x43A2DD, &nop, (0x43A2F1 - 0x43A2DD));
		WriteCall((void *)0x43A2DD, &InLevel_Start2);
		WriteCall((void *)0x43A377, &InLevel_End2);
		WriteData((void *)0x40B15A, &ringfix, 5);  // rings
		WriteData((void *)0x41C6C0, &ringfix, 5);  // RI flickies
		WriteData((void *)0x40BD85, &ringfix, 5);  // Sonic jump GFX brightness
		WriteData((void *)0x40BD71, &jumpfix, 5);  // Sonic jump GFX opacity
		WriteData((void *)0x40B5F8, &ringfix, 5);  // Itemp pickup
		WriteData((void *)0x40B9DD, &ringfix, 5);  // Shields
		WriteData((void *)0x417BA0, &bgfix, 5);    // Title bg et. all
		WriteData((void *)0x40C1C6, &ringfix, 5);  // Robotnik targeting icon

		WriteData((void *)0x40C273, &nop, (0x40C279 - 0x40C273));
		WriteCall((void *)0x40C273, &D3D_Render2DObject_Fix);

		WriteData((void *)0x409ABA, &jmp, 1);	// Track_TintGourads RenderMode check
		WriteData((void *)0x409C2F, &jmp, 1);	// Char_TintGourads RenderMode check

		// Radiant Emerald track translucency
		WriteData((void *)0x40EEDA, &nop, (0x40EEEC - 0x40EEDA));
		WriteCall((void *)0x40EEDA, &MakeTrackTranslucent_0);
		WriteData((void *)0x40EFE4, &nop, (0x40EFF6 - 0x40EFE4));
		WriteCall((void *)0x40EFE4, &MakeTrackTranslucent_1);
		WriteData((void *)0x40F0F2, &nop, (0x40F4CF - 0x40F4BD));
		WriteCall((void *)0x40F0F2, &MakeTrackTranslucent_1);
		WriteData((void *)0x40F6D5, &nop, (0x40F6E4 - 0x40F6D5));
		WriteCall((void *)0x40F6D5, &MakeTrackTranslucent_3);

		// Next two calls fix all menus
		WriteCall((void *)0x417F06, &Oval_Begin);
		WriteJump((void *)0x418223, &SetSaturnMode);  // Oval end

		WriteData((void *)0x437696, &nop, (0x43769D - 0x437696));
		WriteCall((void *)0x437696, &FlagBG_Start);
		WriteJump((void *)0x437AFB, &SetSaturnMode); // FlagBG_End

		WriteCall((void *)0x43A1D0, &InLevel_Start1);
		WriteData((void *)0x43D320, &nop, (0x43D325 - 0x43D320));
		WriteCall((void *)0x43D320, &D3D_RenderOverlay_Start);
		WriteData((void *)0x43D376, &nop, (0x43D37F - 0x43D376));
		WriteJump((void *)0x43D376, &D3D_RenderOverlay_Demo); // demo
		WriteData((void *)0x43D4B3, &nop, (0x43D4BF - 0x43D4B3));
		WriteJump((void *)0x43D4B3, &D3D_RenderOverlay_Pause); // pause
	}
}

BOOL APIENTRY DllMain(
	HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call){
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

