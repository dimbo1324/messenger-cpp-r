#include "PlatformOSInfo.h"

#if defined(_WIN32)
#include <windows.h>
#include <sstream>
#include <vector>

typedef LONG NTSTATUS;
typedef NTSTATUS(WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

std::string getOSInfo()
{
    RTL_OSVERSIONINFOEXW osInfo = {0};
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (hMod)
    {
        RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
        if (fxPtr)
        {
            NTSTATUS status = fxPtr((PRTL_OSVERSIONINFOW)&osInfo);
            if (status == 0)
            {
                std::ostringstream oss;
                oss << "Windows " << osInfo.dwMajorVersion << "."
                    << osInfo.dwMinorVersion
                    << " (build " << osInfo.dwBuildNumber << ")";
                return oss.str();
            }
        }
    }
    return "Cannot get Windows version";
}

std::string getProcessInfo()
{
    DWORD pid = GetCurrentProcessId();
    std::vector<wchar_t> exePath(MAX_PATH);
    DWORD size = GetModuleFileNameW(NULL, exePath.data(), MAX_PATH);
    std::wstring wExePath(exePath.data(), size);
    int len = WideCharToMultiByte(CP_UTF8, 0, wExePath.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string exePathStr(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wExePath.c_str(), -1, &exePathStr[0], len, nullptr, nullptr);

    std::ostringstream oss;
    oss << "PID: " << pid << "\nExecutable: " << exePathStr;
    return oss.str();
}

void initConsoleOutput()
{
    SetConsoleOutputCP(65001);
}

#elif defined(__APPLE__)
#include <sys/utsname.h>
#include <unistd.h>
#include <sstream>
#include <cerrno>
#include <vector>
#include <mach-o/dyld.h>

std::string getOSInfo()
{
    struct utsname uts;
    if (uname(&uts) == 0)
    {
        std::ostringstream oss;
        oss << uts.sysname << " " << uts.release << " " << uts.version << " " << uts.machine;
        return oss.str();
    }
    else
    {
        std::ostringstream oss;
        oss << "Cannot get macOS info (errno: " << errno << ")";
        return oss.str();
    }
}

std::string getProcessInfo()
{
    pid_t pid = getpid();
    uint32_t size = 0;
    _NSGetExecutablePath(NULL, &size);
    std::vector<char> exePath(size);
    if (_NSGetExecutablePath(exePath.data(), &size) != 0)
    {
        return "PID: " + std::to_string(pid) + "\nCannot get executable path";
    }
    std::ostringstream oss;
    oss << "PID: " << pid << "\nExecutable: " << exePath.data();
    return oss.str();
}

#elif defined(__linux__)
#include <sys/utsname.h>
#include <unistd.h>
#include <sstream>
#include <cerrno>
#include <vector>
#include <cstdio>
#include <memory>

std::string execCommand(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        return "Error executing command";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

std::string getOSInfo()
{
    struct utsname uts;
    if (uname(&uts) == 0)
    {
        std::ostringstream oss;
        oss << uts.sysname << " " << uts.release << " " << uts.version << " " << uts.machine;
        std::string distroInfo = execCommand("lsb_release -ds 2>/dev/null");
        if (!distroInfo.empty())
        {
            distroInfo.erase(remove(distroInfo.begin(), distroInfo.end(), '\"'), distroInfo.end());
            distroInfo.erase(remove(distroInfo.begin(), distroInfo.end(), '\n'), distroInfo.end());
            oss << " (" << distroInfo << ")";
        }
        return oss.str();
    }
    else
    {
        std::ostringstream oss;
        oss << "Cannot get Linux info (errno: " << errno << ")";
        return oss.str();
    }
}

std::string getProcessInfo()
{
    pid_t pid = getpid();
    char exePath[PATH_MAX] = {0};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    std::ostringstream oss;
    oss << "PID: " << pid;
    if (len != -1)
    {
        exePath[len] = '\0';
        oss << "\nExecutable: " << exePath;
    }
    else
    {
        oss << "\nCannot get executable path (errno: " << errno << ")";
    }
    return oss.str();
}

#else
std::string getOSInfo()
{
    return "Unknown OS";
}

std::string getProcessInfo()
{
    return "Unknown OS";
}
#endif
