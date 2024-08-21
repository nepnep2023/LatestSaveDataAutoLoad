// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "ScriptHookV.lib")

#include "pch.h"
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <shlobj.h>
#include <stdio.h>
#include <tchar.h>
#include <pathcch.h>
#include <Shlwapi.h>

#include "inc/main.h"
#include "script.h"

namespace fs = std::filesystem;

class subDirectory {
public:
    fs::path dirPath;
    std::vector<fs::path> filePath;
};

subDirectory addFiles(fs::directory_entry dir) {
    fs::current_path(dir);
    std::vector<fs::path> pathList;
    int i = 0;
    for (const fs::directory_entry& dir : fs::recursive_directory_iterator(dir)) {
        std::string pathname = dir.path().string();
        if (pathname.find("SGTA") != std::string::npos && dir.path().extension() != ".tmp") {
            std::string fileName = dir.path().filename().string();
            std::string copyName = fileName + ".tmp";
            CopyFile(pathname.c_str(), copyName.c_str(), false);
            fs::path direx = dir.path();
            direx.replace_extension(".tmp");
            pathList.push_back(direx);           
            DeleteFile(pathname.c_str());
        }
    }
    return { dir, pathList };
}

void latestCheck(fs::path dir, std::vector<fs::path> files  ) {
    fs::current_path(dir);
    int latest = 0;
    for (int count = 0; count < files.size();) {
        if (count == 0) {
            FILETIME ft1, ft2;
            HANDLE hFile = CreateFile(
                files[count].string().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            GetFileTime(hFile, NULL, NULL, &ft1);
            CloseHandle(hFile);
            count++;
            hFile = CreateFile(
                files[count].string().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            GetFileTime(hFile, NULL, NULL, &ft2);
            CloseHandle(hFile);
            LONG lRet = CompareFileTime(&ft1, &ft2);

            if (lRet >= 0) {
                std::string temp = files[0].stem().string();
                CopyFile(files[0].string().c_str(), temp.c_str(), true);
                DeleteFile(files[0].string().c_str());
                files[0].replace_extension("");
                latest = 0;
                //std::cout << temp << "が新しいデータになる" << std::endl;
            }
            else if (lRet < 0) {
                std::string temp = files[1].stem().string();
                CopyFile(files[1].string().c_str(), temp.c_str(), true);
                DeleteFile(files[1].string().c_str());
                files[1].replace_extension("");
                latest = 1;
                //std::cout << temp << "が新しいデータになる" << std::endl;
            }
        }
        else {
            FILETIME ft1, ft2;
            HANDLE hFile = CreateFile(
                files[latest].string().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            GetFileTime(hFile, NULL, NULL, &ft1);
            CloseHandle(hFile);
            hFile = CreateFile(
                files[count].string().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            GetFileTime(hFile, NULL, NULL, &ft2);
            CloseHandle(hFile);
            LONG lRet = CompareFileTime(&ft1, &ft2);

            if (lRet < 0) {
                std::string temp = files[count].stem().string();
                CopyFile(files[count].string().c_str(), temp.c_str(), true);
                DeleteFile(files[count].string().c_str());
                files[count].replace_extension("");
                std::string fileName = files[latest].stem().string();
                std::string backName = fileName + ".tmp";
                CopyFile(files[latest].string().c_str(), backName.c_str(), true);
                DeleteFile(files[latest].string().c_str());
                files[latest].replace_extension(".tmp");
                latest = count;
                //std::cout << temp << "が新しいデータになる" << std::endl;
            }
        }
        count++;
    }
}

void Main(HMODULE hModule) {
    if (!fs::exists("ScriptHookV.dll")) {
        return;
    }

    TCHAR rspath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, rspath, CSIDL_PERSONAL, 0);
    std::filesystem::path gtav = fs::current_path();
    fs::current_path(rspath);
    fs::current_path("./Rockstar Games/GTA V/Profiles");
    fs::path profiles = fs::current_path();

    //fs::create_directory("./temp");

    //std::vector<std::string>files;

    std::vector<fs::directory_entry> directories;
    std::vector<subDirectory> dirList;
    
    for (const fs::directory_entry& directory : fs::recursive_directory_iterator(profiles)) {
        if (directory.is_directory()) {
            directories.push_back(directory);
        }
    }
    for (int i = 0; i < directories.size(); i++) {
        dirList.push_back(addFiles(directories[i]));
    }
    for (int i = 0; i < dirList.size(); i++) {
        latestCheck(dirList[i].dirPath, dirList[i].filePath);
    }

    
    /*
    Sleep(10 * 1000);

    count = 0;
    bool done = 0;
    while (true) {
        if (checkList[count].path == "") {
            //std::cout << "戻す処理は終了" << std::endl;
            break;
        }
        if (checkList[count].path.has_extension()) {
            std::string temp = checkList[count].path.string();
            std::string back = checkList[count].path.stem().string();
            done = CopyFile(temp.c_str(), back.c_str(), true);
            //std::cout << checkList[count].path << "拡張子を戻す処理をしている" << std::endl;
            DeleteFile(checkList[count].path.string().c_str());
        }
        if (done) {
            //std::cout << "戻せた" << std::endl;
        }
        count++;
    }*/
    fs::current_path(gtav);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Main(hModule);
        scriptRegister(hModule, ScriptMain);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        scriptUnregister(hModule);
        break;
    }
    return TRUE;

}

