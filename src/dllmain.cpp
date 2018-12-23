// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <windows.h>
#include <algorithm>
#include "../include/MemAccess.h"
#include "../include/SonicRModLoader.h"

// Raw assembly code to call 
// IDirect3DDevice9::SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADDSIGNED)
__declspec(naked) void FixColoring()
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

		ret
	}
}

extern "C" {
	__declspec(dllexport) ModInfo SonicRModInfo = { ModLoaderVer };
	__declspec(dllexport) void __cdecl Init()
	{
		unsigned char jmp = 0xEB;	// JMP opode
		unsigned char nop[255];      // Many NOP opcodes
		std::fill_n(nop, 255, 0x90);

		WriteData((void *)0x43A2DD, &nop, (0x43A2F1 - 0x43A2DD));
		WriteCall((void *)0x43A2DD, &FixColoring);
		WriteData((void *)0x409ABA, &jmp, 1);	// Track_TintGourads RenderMode check
		WriteData((void *)0x409C2F, &jmp, 1);	// Char_TintGourads RenderMode check
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

