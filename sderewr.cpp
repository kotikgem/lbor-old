#include <iostream>
#include <string>
#include <list>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <WinUser.h>
#include <algorithm>

using namespace std;

DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
 
    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32
 
 
    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}
 
DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, vector<DWORD> offsets)
{
    DWORD pID; // Game process ID
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);
 
    DWORD offset_null;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    DWORD pointeraddress = offset_null; // the address we need
    for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}

// BOOL write_bytes(uintptr_t const address, std::vector<uint8_t> const& bytes) {
//     return WriteProcessMemory(pHandle, reinterpret_cast<LPVOID>(address), bytes.data(), bytes.size(), nullptr);
// }

void record(){
    list<float> xposs;
    list<float> yposs;
    list<double> yaccell;
    list<float> rotationn;
    HWND hwnd_AC = FindWindowA(NULL, "Geometry Dash");
    if(hwnd_AC != FALSE);
    DWORD pID;
    GetWindowThreadProcessId(hwnd_AC, &pID);
    HANDLE phandle;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if(phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    char gamemodule[] = "GeometryDash.exe";
    DWORD gamebaseAddr = GetModuleBaseAddress(_T(gamemodule), pID);
    DWORD xposaddr = 0x003222D0;
    vector<DWORD> xpos_offsets{ 0x164, 0x224, 0x4E8, 0xB4, 0x67C };

    DWORD yposaddr = 0x003222D0;
    vector<DWORD> ypos_offsets{ 0x164, 0x224, 0x680 };

    DWORD yacceladdr = 0x003222D0;
    vector<DWORD> yaccel_offsets{ 0x164, 0x224, 0x628 };

    DWORD rotationaddr = 0x003222D0;
    vector<DWORD> rotation_offsets{ 0x164, 0x224, 0x24 };
    
    DWORD lvladdr = 0x003222D0;
    vector<DWORD> lvl_offsets{ 0x164, 0x22C, 0x114 };
    float xpos;
    float ypos;
    double yaccel;
    float rotation;
    bool is_in_lvl;
    bool exitt;
    while(TRUE){
        DWORD ptrlvl = GetPointerAddress(hwnd_AC, gamebaseAddr, lvladdr, lvl_offsets);
        ReadProcessMemory(phandle, (LPVOID*)(ptrlvl), &is_in_lvl, sizeof(is_in_lvl), NULL);
        if(is_in_lvl){
            DWORD ptrxpos = GetPointerAddress(hwnd_AC, gamebaseAddr, xposaddr, xpos_offsets);
            ReadProcessMemory(phandle, (LPVOID*)(ptrxpos), &xpos, sizeof(xpos), NULL);

            DWORD ptrypos = GetPointerAddress(hwnd_AC, gamebaseAddr, yposaddr, ypos_offsets);
            ReadProcessMemory(phandle, (LPVOID*)(ptrypos), &ypos, sizeof(ypos), NULL);

            DWORD ptryaccel = GetPointerAddress(hwnd_AC, gamebaseAddr, yacceladdr, yaccel_offsets);
            ReadProcessMemory(phandle, (LPVOID*)(ptryaccel), &yaccel, sizeof(yaccel), NULL);

            DWORD ptrrotation = GetPointerAddress(hwnd_AC, gamebaseAddr, rotationaddr, rotation_offsets);
            ReadProcessMemory(phandle, (LPVOID*)(ptrrotation), &rotation, sizeof(rotation), NULL);

            cout << xpos;
            cout << "\n";

            // vluess.push_back((xpos, ypos, yaccel, rotation));
            xposs.push_back(xpos);
            yposs.push_back(ypos);
            yaccell.push_back(yaccel);
            rotationn.push_back(rotation);
            
            if(GetAsyncKeyState(VK_CONTROL)){
                exitt = TRUE;
                break;
            }
        }
        if(is_in_lvl == FALSE){
            cout << "nothing";
            continue;
        }
        if(exitt == TRUE){
            break;
            
        }
    }
    int i = 0;
    float xpossa[xposs.size()];
    float ypossa[yposs.size()];
    float yaccella[yaccell.size()];
    float rotationna[rotationn.size()];
    copy(xposs.begin(), xposs.end(), xpossa);
    copy(yposs.begin(), yposs.end(), ypossa);
    copy(yaccell.begin(), yaccell.end(), yaccella);
    copy(rotationn.begin(), rotationn.end(), rotationna);
    ofstream out;          // поток для записи
    out.open("macros_physics.lbot", ios::app); // окрываем файл для записи
    while(out.is_open() && xpossa[i] != xposs.size()) // press cords write
    {
        // out << to_string( xpossa[i]) +  " " + to_string(ypossa[i]) +  " " + to_string(yaccella[i]) +  " " + to_string(rotationna[i]) + "\n";
        out << to_string( xpossa[i]) + "\n";
        out << to_string( ypossa[i]) + "\n";
        out << to_string( yaccella[i]) + "\n";
        out << to_string( rotationna[i]) + "\n";
        i = i + 1;
        if(i == xposs.size()){
            break;
        }

    }
    out.close();
    // ofstream f("macros.lbot", ios::out);
    // copy(actions.begin(), actions.end(), ostream_iterator<Macros>(f, " "));
    string contain;
    cin >> contain;

}

void play(){
    cout << "playing";
    ifstream outt;          // поток для записи
    string datta;
    vector<float> xposs;
    vector<float> yposs;
    vector<double> yaccell;
    vector<float> rotationn;
    // bool releasebool;
    // bool end;
    outt.open("macros_physics.lbot"); // окрываем файл для записи
    // pressp.push_back(stof(datta));
    // getline(outt, datta);
    float xpossn;
    float ypossn;
    double yaccelln;
    float rotationnn;
    // const char *SEPARATORS = " ";
    while(outt.is_open()){
        getline(outt, datta);
        if(datta == "end"){
            break;
        }
        
        // xpossn = stof(datta);
        // ypossn = stof(datta);
        // yaccelln = stof(datta);
        // rotationnn = stof(datta);
        // cout << datta + "\n";
        cout << datta;
        xpossn = stof(datta);
        xposs.push_back(xpossn);
        
        getline(outt, datta);
        cout << datta;
        ypossn = stof(datta);
        yposs.push_back(ypossn);

        getline(outt, datta);
        cout << datta;
        yaccelln = stod(datta);
        yaccell.push_back(yaccelln);
        
        getline(outt, datta);
        cout << datta;
        rotationnn = stof(datta);
        rotationn.push_back(rotationnn);
        // while(i != 3){

        //     presspp.push_back(presspos);
        // // getline(outt, datta);
        // // cout << datta;
        // // releasepos = stof(datta);
        // // releasepp.push_back(releasepos);
        // }
    }
    outt.close();
    cout << "\n";
    // cout << datta;
    cout << xposs.size();
    cout << "\n";
    cout << yposs.size();
    cout << "\n";
    cout << yaccell.size();
    cout << "\n";
    cout << rotationn.size();
    // json macross = json::parse(datta);
    // cout << macross[2][1];
    // string contain;
    // cin >> contain;
    // cin.get();
    //-------------------------------------
    HWND hwnd_AC = FindWindowA(NULL, "Geometry Dash");
    if(hwnd_AC != FALSE);
    DWORD pID;
    GetWindowThreadProcessId(hwnd_AC, &pID);
    HANDLE phandle;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if(phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    char gamemodule[] = "GeometryDash.exe";
    DWORD gamebaseAddr = GetModuleBaseAddress(_T(gamemodule), pID);
    DWORD xposaddr = 0x003222D0;
    vector<DWORD> xpos_offsets{ 0x164, 0x224, 0x4E8, 0xB4, 0x67C };

    DWORD yposaddr = 0x003222D0;
    vector<DWORD> ypos_offsets{ 0x164, 0x224, 0x680 };

    DWORD yacceladdr = 0x003222D0;
    vector<DWORD> yaccel_offsets{ 0x164, 0x224, 0x628 };

    DWORD rotationaddr = 0x003222D0;
    vector<DWORD> rotation_offsets{ 0x164, 0x224, 0x24 };

    DWORD lvladdr = 0x003222D0;
    vector<DWORD> lvl_offsets{ 0x164, 0x22C, 0x114 };
    float xpos;
    bool is_in_lvl;
    bool exitt;
    // const int KEYEVENT_KEYUP = 0x02;
    // INPUT inputt = {0};
    // inputt.type = INPUT_MOUSE;
    while(TRUE){
        DWORD ptrlvl = GetPointerAddress(hwnd_AC, gamebaseAddr, lvladdr, lvl_offsets);
        ReadProcessMemory(phandle, (LPVOID*)(ptrlvl), &is_in_lvl, sizeof(is_in_lvl), NULL);

        while(is_in_lvl){
            DWORD ptrxpos = GetPointerAddress(hwnd_AC, gamebaseAddr, xposaddr, xpos_offsets);
            ReadProcessMemory(phandle, (LPVOID*)(ptrxpos), &xpos, sizeof(xpos), NULL);
            // cout << xpos;
            // cout << "\n";
            vector<float>::iterator it = find(xposs.begin(), xposs.end(), xpos);
            if(it != xposs.end()){
                // keybd_event(VK_SPACE,0,0,0);
                // ZeroMemory(&inputt, sizeof(INPUT));
                // inputt.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                // SendInput(1, &inputt, sizeof(INPUT));
                // cout << "key press\n";
                int itt = it - xposs.begin();
                // DWORD ptrxpos = GetPointerAddress(hwnd_AC, gamebaseAddr, xposaddr, xpos_offsets);
                // ReadProcessMemory(phandle, (LPVOID*)(ptrxpos), &xpos, sizeof(xpos), NULL);
                ypossn = yposs[itt];
                yaccelln = yaccell[itt];
                rotationnn = rotationn[itt];
                
                DWORD ptrypos = GetPointerAddress(hwnd_AC, gamebaseAddr, yposaddr, ypos_offsets);
                WriteProcessMemory(phandle, reinterpret_cast<LPVOID>(ptrypos), &ypossn, sizeof(yposs), nullptr);

                DWORD ptryaccel = GetPointerAddress(hwnd_AC, gamebaseAddr, yacceladdr, yaccel_offsets);
                WriteProcessMemory(phandle, reinterpret_cast<LPVOID>(ptryaccel), & yaccelln, sizeof( yaccell), nullptr);

                DWORD ptrrotation = GetPointerAddress(hwnd_AC, gamebaseAddr, rotationaddr, rotation_offsets);
                WriteProcessMemory(phandle, reinterpret_cast<LPVOID>(ptrrotation), &rotationn, sizeof(rotationn), nullptr);


            }
            // if(find(presspp.begin(), presspp.end(), xpos) == 0){
            //     continue;
            // }
                // if(reels == "release"){
            // if(find(releasepp.begin(), releasepp.end(), xpos) != releasepp.end()){
            //     // actions.insert(xpos, FALSE, actions.begin(), actions.end());
            //     // actions.push_back((xpos, TRUE));
            //     // list<Macros> temppp{Macros(FALSE, xpos)};
            //     ZeroMemory(&inputt, sizeof(INPUT));
            //     inputt.type = INPUT_MOUSE;
            //     inputt.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            //     SendInput(1, &inputt, sizeof(INPUT));
            //     // keybd_event(VK_SPACE,0,KEYEVENT_KEYUP,0);
            //     cout << "key release\n";


            // }
            // if(find(releasepp.begin(), releasepp.end(), xpos) == 0){
            //     continue;
            // }
            if(GetAsyncKeyState(VK_CONTROL)){
                exitt = TRUE;
                break;
            }
        }
        if(is_in_lvl == FALSE){
            cout << "nothing";
            continue;
        }
        if(exitt == TRUE){
            break;
            
        }
    }
    string containss;
    cin >> containss;
}

int main(){
    // HWND hwnd_AC = FindWindowA(NULL, "Geometry Dash");
    // if(hwnd_AC != FALSE);
    // DWORD pID;
    // GetWindowThreadProcessId(hwnd_AC, &pID);
    // HANDLE phandle;
    // phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    // if(phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    // char gamemodule[] = "GeometryDash.exe";
    // DWORD gamebaseAddr = GetModuleBaseAddress(_T(gamemodule), pID);
    // // DWORD xposaddr = 0x003222D0;
    // // vector<DWORD> xpos_offsets{ 0x164, 0x224, 0x4E8, 0xB4, 0x67C };
    // DWORD lvladdr = 0x20A23C;
    // vector<DWORD> lvl_offsets{ 0x0 };
    // BYTE nolcipon[] = { 0xE9, 0x79, 0x06, 0x00, 0x00 };
    // DWORD ptrlvl = GetPointerAddress(hwnd_AC, gamebaseAddr, lvladdr, lvl_offsets);
    // WriteProcessMemory(phandle, (LPVOID*)(ptrlvl), &nolcipon, sizeof(nolcipon), NULL);
    // int hj;
    // cin >> hj;
    cout << "LBOT v0.1.2(with physics breaker)\n";
    cout << "What do like?(record/play/options)\n";
    string optio;
    cin >> optio;
    if(optio == "record"){

        record();

    }
    if(optio == "play"){

        play();

    }
}