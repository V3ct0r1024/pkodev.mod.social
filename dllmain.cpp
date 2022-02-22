#include <Windows.h>
#include <detours.h>

#include <string>
#include <map>
#include <fstream>
#include <algorithm>

#include "loader.h"

namespace pkodev
{
    namespace address
    {
        // Game.exe 0 (1222073761)
        namespace GAME_13X_0
        {
            // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
            const unsigned int CMiniMapMgr___MiniFormMouseEvent = 0x00473BF0;
        }

        // Game.exe 1 (1243412597)
        namespace GAME_13X_1
        {
            // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
            const unsigned int CMiniMapMgr___MiniFormMouseEvent = 0x00473CB0;
        }

        // Game.exe 2 (1252912474)
        namespace GAME_13X_2
        {
            // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
            const unsigned int CMiniMapMgr___MiniFormMouseEvent = 0x00473D50;
        }

        // Game.exe 3 (1244511158)
        namespace GAME_13X_3
        {
            // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
            const unsigned int CMiniMapMgr___MiniFormMouseEvent = 0x00473D40;
        }

        // Game.exe 4 (1585009030)
        namespace GAME_13X_4
        {
            // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
            const unsigned int CMiniMapMgr___MiniFormMouseEvent = 0x00473EC0;
        }

        // Game.exe 5 (1207214236)
        namespace GAME_13X_5
        {
            // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
            const unsigned int CMiniMapMgr___MiniFormMouseEvent = 0x004738C0;
        }
    }

    namespace pointer
    {
        // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
        typedef void(__cdecl* CMiniMapMgr___MiniFormMouseEvent__Ptr)(void*, int, int, int, DWORD);
        CMiniMapMgr___MiniFormMouseEvent__Ptr CMiniMapMgr___MiniFormMouseEvent = (CMiniMapMgr___MiniFormMouseEvent__Ptr)(void*)(pkodev::address::MOD_EXE_VERSION::CMiniMapMgr___MiniFormMouseEvent);
    }

    namespace hook
    {
        // void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
        void __cdecl CMiniMapMgr___MiniFormMouseEvent(void* pSender, int nMsgType, int x, int y, DWORD dwKey);
    }

    // List of social buttons
    std::map<std::string, std::string> buttons;
}

// Entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // Nothing to do . . .
    return TRUE;
}

// Get mod information
void GetModInformation(mod_info& info)
{
    strcpy_s(info.name, TOSTRING(MOD_NAME));
    strcpy_s(info.version, TOSTRING(MOD_VERSION));
    strcpy_s(info.author, TOSTRING(MOD_AUTHOR));
    info.exe_version = MOD_EXE_VERSION;
}

// Start the mod
void Start(const char* path)
{
    // Build path to the config file
    char buf[MAX_PATH]{ 0x00 };
    sprintf_s(buf, sizeof(buf), "%s\\%s.cfg", path, TOSTRING(MOD_NAME));

    // Open the config file
    std::ifstream file(buf, std::ifstream::in);

    // Check that file is open
    if ( file.is_open() == true )
    {
        // Current line that read from the file
        std::string line{ "" };

        // Read lines from file
        while (getline(file, line))
        {
            // Remove spaces from line
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

            // Check that line is empty
            if ( line.empty() == true )
            {
                // Skip line
                continue;
            }

            // Search for symbol '='
            const std::size_t pos = line.find('=');

            // Check that the symbol is not found
            if ( pos == std::string::npos )
            {
                // Skip the line
                continue;
            }
            // Get social button name
            std::string button = line.substr(0, pos);

            // Get social button url
            const std::string url = line.substr(pos + 1, line.length() - pos);

            // Convert the social button name to the lower case
            std::transform(button.begin(), button.end(), button.begin(), std::tolower);

            // Search button name in the list of the social buttons
            auto it = std::find_if(pkodev::buttons.begin(), pkodev::buttons.end(),
                [&button](const std::pair<std::string, std::string>& p) { return (p.first == button); }
            );

            // Check that the button name is found in the list
            if ( it != pkodev::buttons.end() )
            {
                // Update button's url
                it->second = url;
            }
            else
            {
                // Add button to the list
                pkodev::buttons.insert({ button, url });
            }
        }

        // Close the file
        file.close();
    }

    // Enable hooks
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pkodev::pointer::CMiniMapMgr___MiniFormMouseEvent, pkodev::hook::CMiniMapMgr___MiniFormMouseEvent);
    DetourTransactionCommit();
}

// Stop the mod
void Stop()
{
    // Disable hooks
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)pkodev::pointer::CMiniMapMgr___MiniFormMouseEvent, pkodev::hook::CMiniMapMgr___MiniFormMouseEvent);
    DetourTransactionCommit();
}

// void CMiniMapMgr::_MiniFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
void __cdecl pkodev::hook::CMiniMapMgr___MiniFormMouseEvent(void* pSender, int nMsgType, int x, int y, DWORD dwKey)
{
    // Get pointer to the social button name
    const char* szName = reinterpret_cast<const char*>(reinterpret_cast<int>(pSender) + 0x14);

    // Check the pointer
    if ( szName != nullptr )
    {
        // Convert const char * to std::string type
        std::string button{ szName };

        // Convert the button name to the lower case
        std::transform(button.begin(), button.end(), button.begin(), std::tolower);

        // Search the button in the list
        const auto it = pkodev::buttons.find(button);

        // Check that buttin is found
        if ( it != pkodev::buttons.end() )
        {
            // Go to the social resource address
            ShellExecuteA(NULL, "open", it->second.c_str(), NULL, NULL, SW_SHOWNORMAL);

            // Exit form the hook
            return;
        }
    }

    // Call the original function CMiniMapMgr___MiniFormMouseEvent()
    pkodev::pointer::CMiniMapMgr___MiniFormMouseEvent(pSender, nMsgType, x, y, dwKey);
}
