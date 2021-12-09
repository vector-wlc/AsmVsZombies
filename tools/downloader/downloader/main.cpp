/*
 * @Coding: gb2312
 * @Author: vector-wlc
 * @Date: 2020-10-27 10:12:11
 * @Description: AvZ downloader
 */
#include <direct.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

#pragma comment(lib, "URlmon")

void RunExe(const std::string& exe, const std::string& cmd)
{

    SHELLEXECUTEINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SEE_MASK_NOCLOSEPROCESS;
    si.lpVerb = "open";
    si.lpFile = exe.c_str();
    si.lpParameters = cmd.c_str();
    si.nShow = SW_HIDE;

    ShellExecuteExA(&si);

    DWORD dwExitCode;
    GetExitCodeProcess(si.hProcess, &dwExitCode);
    while (dwExitCode == STILL_ACTIVE) {
        Sleep((DWORD)5);
        GetExitCodeProcess(si.hProcess, &dwExitCode);
    }

    CloseHandle(si.hProcess);
}

bool DownloadFile(const std::string& url, const std::string& output_path);

int main()
{
    // 获取版本信息
    std::string gitee_depository = "https://gitee.com/vector-wlc/AsmVsZombies/raw/master/release/";
    std::cout << "正在从 " << gitee_depository << " 获取版本信息...\n";
    if (!DownloadFile(gitee_depository + "version.txt", "version.txt")) {
        system("pause");
        return 0;
    }
    std::fstream version;
    version.open("version.txt", std::ios::in);
    std::vector<std::string> files;
    std::string file;
    if (version.is_open()) {

        while (std::getline(version, file)) {
            files.push_back(file);
        }
    }
    version.close();

    // 选择版本文件
    std::cout << "获取到以下版本的 AvZ\n";
    int index = 1;
    for (const auto& ele : files) {
        std::cout << index << ". " << ele << std::endl;
        ++index;
    }
    std::cout << "输入版本对应的数字以进行更新 :";
    while (true) {
        std::cin >> index;
        getchar();
        if (!std::cin.good()) {
            std::cin.clear();
            std::cout << "您的输入不合理，因为只能输入数字，请重新处输入 : ";
            continue;
        }

        if (index < 1 || index > files.size()) {
            std::cout << "您的输入不合理，因为范围已溢出，请重新处输入 : ";
            continue;
        }

        break;
    }

    // 获取版本文件
    std::cout << "正在从 " << gitee_depository << " 获取相应的版本文件...\n";
    if (!DownloadFile(gitee_depository + files[index - 1], "avz.zip")) {
        system("pause");
        return 0;
    }

    // 解压替换版本文件
    std::cout << "正在解压替换版本文件...\n";
    RunExe("7z\\7z.exe", " x avz.zip -aoa");
    std::cout << "解压替换版本文件成功\n";

    // 删除临时文件
    DeleteFileA("avz.zip");
    DeleteFileA("version.txt");

    std::cout << "更新成功\n";
    system("pause");
    return 0;
}

bool DownloadFile(const std::string& url, const std::string& output_path)
{
    HRESULT Result = URLDownloadToFileA(NULL, url.c_str(), output_path.c_str(), 0, NULL);
    switch (Result) {
    case S_OK:
        printf("获取成功\n");
        return true;
    default:
        printf("获取失败，请检查网络连接\n");
        return false;
    }
}
