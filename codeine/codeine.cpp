#include <iostream>
#include "stdafx.h"
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

int main()
{
	SetConsoleTitleA("Double cup, styrofoam, fill it with codeine");
    HANDLE hProcess = 0;

    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false;

    const int newValue = 1337; //Ammo amount if we press f1

    //Getting procid of the process
    DWORD procId = GetProcId(L"ac_client.exe");

    if (procId)
    {
        //Get Handle to Process
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

        //Getmodulebaseaddress
        moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

        //Resolve address
        localPlayerPtr = moduleBase + 0x10f4f4;

        //Resolve base address of the pointer chain
        healthAddr = FindDMAddy(hProcess, localPlayerPtr, { 0xF8 });

        //What prints to our console if process is found :)
        std::cout << "F1 - Health hack\n";
        std::cout << "F2 - Gun Ammo hack [Increases]\n";
        std::cout << "F3 - NoRecoil hack\n\n";
            
        std::cout << "Build: 30 May\n";
        std::cout << "Made by hj.#0010\n";
    }
    else
    {
        std::cout << "Process not found, press enter to exit\n";
        getchar();
        return 0;
    }

    DWORD dwExit = 0;
    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        //Health continuous write
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            bHealth = !bHealth;
        }

        //unlimited ammo patch
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            bAmmo = !bAmmo;

            if (bAmmo)
            {
                //FF 06 = inc [esi]
                mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);
            }

            else
            {
                //FF 0E = dec [esi]
                mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E", 2, hProcess);
            }
        }

        //no recoil NOP
        if (GetAsyncKeyState(VK_F3) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
            }

            else
            {
                //50 8D 4C 24 1C 51 8B CE FF D2; Original
                mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10, hProcess);
            }
        }

        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            return 0;
        }

        //Continuous write
        if (bHealth)
        {
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }

        Sleep(10);
    }

    std::cout << "Process not found, press enter to exit\n";
    getchar();
    return 0;
}