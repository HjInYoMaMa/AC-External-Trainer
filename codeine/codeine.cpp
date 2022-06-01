#include <iostream>
#include "stdafx.h"
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"
#include "Console.h"
namespace con = JadedHoboConsole;

int main()
{
	SetConsoleTitleA("Double cup, styrofoam, fill it with codeine");
    HANDLE hProcess = 0;

    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0, rapidfire = 0, grenadeAddr = 0, armorAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false, bSpeedhack = false, bRapidfire = false, bGrenade = false, bArmor = false;

    const int newValue = 1337; //Health amount if we press f1
    const int newGrenadeValue = 69; //Grenade Amount if we press f6
    const int newArmorValue = 69; //Armor amount if we press f7

    int rapidValue = NULL;
    int newrapidValue = 5; //Rapidfire value
    int offrapidValue = 120; //original rapidfire value
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
        grenadeAddr = FindDMAddy(hProcess, localPlayerPtr, { 0x158 });
        armorAddr = FindDMAddy(hProcess, localPlayerPtr, { 0x0FC });
        //What prints to our console if process is found :)

        std::cout << con::fg_magenta << R"(
                 ____ 
                /        \ _ 
                \x    .-~_ )_ 
                  \~x".-~   ~-. 
              _   (  /              \     _ 
              ||    T   o   o       Y   || 
          ==:l    l     <           !    I;== 
               \\    \   .__/       /    // 
                \\  ,r"-,___.---,,.,   // 
                 }^ \ , ( )     _-'     //. 
                /     }~X---~       //  \ 
              Y    Y  I  \   \     "     Y 
              |      |   | o \   \           | 
              |      l__l    Y  T          | 
              l      """" o  l__j          ! 
               \              """""        / 
      ___,.---^.        o              .^---.._____   

)" << '\n';
        std::cout << con::fg_white << "[" << con::fg_green << "F1" << con::fg_white << "]" << con::fg_magenta << " Health Hack\n" << con::fg_white;
        std::cout << "[" << con::fg_green << "F2" << con::fg_white << "]" << con::fg_magenta << " Gun Ammo Hack\n" << con::fg_white;
        std::cout << "[" << con::fg_green << "F3" << con::fg_white << "]" << con::fg_magenta << " No Recoil Hack\n" << con::fg_white;
        std::cout << "[" << con::fg_green << "F4" << con::fg_white << "]" << con::fg_magenta << " SpeedHack\n" << con::fg_white;
        std::cout << "[" << con::fg_green << "F5" << con::fg_white << "]" << con::fg_magenta << " RapidFire\n" << con::fg_white;
        std::cout << "[" << con::fg_green << "F6" << con::fg_white << "]" << con::fg_magenta << " Grenade Hack\n" << con::fg_white;
        std::cout << "[" << con::fg_green << "F7" << con::fg_white << "]" << con::fg_magenta << " Armor Hack\n\n" << con::fg_white;
            
        std::cout << "[" << con::fg_green << "+" << con::fg_white << "]" << con::fg_magenta << " Build: May 31\n" << con::fg_white;
        std::cout << "[" << con::fg_green << "+" << con::fg_white << "]" << con::fg_magenta << " Made by hj.#0010\n" << con::fg_white;
    }
    else
    {
        std::cout << "Process not found, press enter to exit\n";
        getchar();
        return 0;
    }

    //Rapidfire offsets
    std::vector<unsigned int> rapidfireOffsets = { 0x374,  0xc, 0x10A };
    uintptr_t rapidfireAddr = FindDMAddy(hProcess, localPlayerPtr, rapidfireOffsets);
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

        //SpeedHack
        if (GetAsyncKeyState(VK_F4) & 1)
        {
            bSpeedhack = !bSpeedhack;

            if (bSpeedhack)
            {
                mem::PatchEx((BYTE*)moduleBase + 0x5BEA0, (BYTE*)"\xB8\x03\x00\x00\x00", 5, hProcess);
                mem::PatchEx((BYTE*)moduleBase + 0x5BE40, (BYTE*)"\xB8\xFD\xFF\xFF\xFF", 5, hProcess);
                mem::PatchEx((BYTE*)moduleBase + 0x5BF00, (BYTE*)"\xB8\x03\x00\x00\x00", 5, hProcess);
                mem::PatchEx((BYTE*)moduleBase + 0x5BF60, (BYTE*)"\xB8\xFD\xFF\xFF\xFF", 5, hProcess);
            }

            else
            {
                mem::PatchEx((BYTE*)moduleBase + 0x5BEA0, (BYTE*)"\xB8\x01\x00\x00\x00", 5, hProcess);
                mem::PatchEx((BYTE*)moduleBase + 0x5BE40, (BYTE*)"\xB8\xFF\xFF\xFF\xFF", 5, hProcess);
                mem::PatchEx((BYTE*)moduleBase + 0x5BF00, (BYTE*)"\xB8\x01\x00\x00\x00", 5, hProcess);
                mem::PatchEx((BYTE*)moduleBase + 0x5BF60, (BYTE*)"\xB8\xFF\xFF\xFF\xFF", 5, hProcess);
            }
        }

        //rapidfire
        if (GetAsyncKeyState(VK_F5) & 1)
        {
            bRapidfire = !bRapidfire;

            if (bRapidfire)
            {
                ReadProcessMemory(hProcess, (BYTE*)rapidfireAddr, &rapidValue, sizeof(rapidValue), nullptr);
                WriteProcessMemory(hProcess, (BYTE*)rapidfireAddr, &newrapidValue, sizeof(newrapidValue), nullptr);
                ReadProcessMemory(hProcess, (BYTE*)rapidfireAddr, &rapidValue, sizeof(rapidValue), nullptr);
            }
            else
            {
                ReadProcessMemory(hProcess, (BYTE*)rapidfireAddr, &rapidValue, sizeof(rapidValue), nullptr);
                WriteProcessMemory(hProcess, (BYTE*)rapidfireAddr, &offrapidValue, sizeof(offrapidValue), nullptr);
                ReadProcessMemory(hProcess, (BYTE*)rapidfireAddr, &rapidValue, sizeof(rapidValue), nullptr);
            }
        }

        if (GetAsyncKeyState(VK_F6) & 1)
        {
            bGrenade = !bGrenade;
        }

        if (GetAsyncKeyState(VK_F7) & 1)
        {
            bArmor = !bArmor;
        }

        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            return 0;
        }

        //Continuous write
        //Health Hack Code
        if (bHealth)
        {
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }
        //Grenade Hack Code
        if (bGrenade)
        {
            mem::PatchEx((BYTE*)grenadeAddr, (BYTE*)&newGrenadeValue, sizeof(newGrenadeValue), hProcess);
        }
        //Armor Hack Code
        if (bArmor)
        {
            mem::PatchEx((BYTE*)armorAddr, (BYTE*)&newArmorValue, sizeof(newArmorValue), hProcess);
        }
        Sleep(10);
    }

    std::cout << "Process not found, press enter to exit\n";
    getchar();
    return 0;
}
