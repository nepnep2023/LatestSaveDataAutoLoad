#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "ScriptHookV.lib")

#include "script.h"

#include "pch.h"
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <shlobj.h>
#include <stdio.h>
#include <tchar.h>
#include <pathcch.h>


#pragma warning(disable : 4244 4305) // double <-> float conversions

namespace fs = std::filesystem;

void main()
{
    TCHAR rspath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, rspath, CSIDL_PERSONAL, 0);
    std::filesystem::path gtav = fs::current_path();
    fs::current_path(rspath);
    fs::current_path("./Rockstar Games/GTA V/Profiles");
    fs::path profiles = fs::current_path();

    //fs::create_directory("./checktest");

    //Sleep(30 * 1000);

    for (const fs::directory_entry& dir : fs::recursive_directory_iterator(profiles)) {
        std::string pathname = dir.path().string();
        if (pathname.find("SGTA") != std::string::npos) {
            if (dir.path().extension() == ".tmp") {
                fs::path cp = dir.path();
                cp.remove_filename();
                fs::current_path(cp);
                std::string fileName = dir.path().filename().stem().string();;
                CopyFile(pathname.c_str(), fileName.c_str(), false);
                DeleteFile(pathname.c_str());
            }
        }
    }
    fs::current_path(gtav);
}

void ScriptMain()
{
	main();
}
