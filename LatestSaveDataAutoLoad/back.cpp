#pragma comment(lib, "Shlwapi.lib")

#include "pch.h"
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <shlobj.h>
#include <stdio.h>
#include <tchar.h>
#include <pathcch.h>

namespace fs = std::filesystem;

int main() {
    TCHAR rspath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, rspath, CSIDL_PERSONAL, 0);
    std::filesystem::path gtav = fs::current_path();
    fs::current_path(rspath);
    fs::current_path("./Rockstar Games/GTA V/Profiles");
    fs::path profiles = fs::current_path();
    
    fs::create_directory("./checktest");

    Sleep(30 * 1000);

    int count = 0; 
    for (const fs::directory_entry& dir : fs::recursive_directory_iterator(profiles)) {
        std::string pathname = dir.path().string();
        if (pathname.find("SGTA") != std::string::npos) {
            if (dir.path().extension() == ".temp") {
                fs::path cp = dir.path();
                cp.remove_filename();
                fs::current_path(cp);
                std::string fileName = dir.path().filename().stem().string();;
                CopyFile(pathname.c_str(), fileName.c_str(), true);
                DeleteFile(pathname.c_str());
                count++;
            }
        }
    }
    /*
    bool done = 0;
    while (true) {
        if (checkList[count].path == "") {
            //std::cout << "–ß‚·ˆ—‚ÍI—¹" << std::endl;
            break;
        }
        if (checkList[count].path.has_extension()) {
            std::string temp = checkList[count].path.string();
            std::string back = checkList[count].path.stem().string();
            done = CopyFile(temp.c_str(), back.c_str(), true);
            //std::cout << checkList[count].path << "Šg’£Žq‚ð–ß‚·ˆ—‚ð‚µ‚Ä‚¢‚é" << std::endl;
            DeleteFile(checkList[count].path.string().c_str());
        }
        if (done) {
            //std::cout << "–ß‚¹‚½" << std::endl;
        }
        count++;
    }*/
    return(0);
}