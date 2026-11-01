// ClueKeyboardFix.cpp
//
// Fixes keyboard text entry in "Clue: Murder at Boddy Mansion" on modern Windows.
//
// Cause: the game calls ToAsciiEx with a WORD buffer it never initializes and then
// reads the whole WORD. ToAsciiEx only writes the low byte when it returns 1, so the
// high byte keeps stack garbage and the character is rejected by the game.
// Fix: clear the WORD before the call and make sure the high byte is zero afterwards.

#include <windows.h>
#include <detours.h>

// Detours requires every injected DLL to export DetourFinishHelperProcess at ordinal 1.
// The implementation already lives in detours.lib, so we only export it here.
// On x86 the function is __stdcall and therefore decorated (_Name@16), so the
// decorated name has to be given explicitly.
#if defined(_M_IX86)
#pragma comment(linker, "/export:DetourFinishHelperProcess=_DetourFinishHelperProcess@16,@1,NONAME")
#else
#pragma comment(linker, "/export:DetourFinishHelperProcess,@1,NONAME")
#endif

static int (WINAPI *TrueToAsciiEx)(UINT, UINT, const BYTE*, LPWORD, UINT, HKL) = ToAsciiEx;

static int WINAPI MyToAsciiEx(UINT vk, UINT sc, const BYTE* state, LPWORD out, UINT flags, HKL hkl)
{
    if (out) *out = 0;                          // clear the WORD before the call
    int r = TrueToAsciiEx(vk, sc, state, out, flags, hkl);
    if (out && r >= 1) *out &= 0x00FF;          // only the low byte is valid
    return r;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID)
{
    if (DetourIsHelperProcess()) return TRUE;

    if (reason == DLL_PROCESS_ATTACH) {
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)TrueToAsciiEx, MyToAsciiEx);
        DetourTransactionCommit();
    } else if (reason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)TrueToAsciiEx, MyToAsciiEx);
        DetourTransactionCommit();
    }
    return TRUE;
}