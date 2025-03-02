#include "PlatformOSInfo.h"

#if defined(_WIN32)
#include <windows.h>
#include <sstream>
#else
#include <sys/utsname.h>
#include <unistd.h>
#include <sstream>
#endif

std::string getOSInfo()
{
#if defined(_WIN32)
    OSVERSIONINFOEX osInfo;
    ZeroMemory(&osInfo, sizeof(OSVERSIONINFOEX));
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (!GetVersionEx((OSVERSIONINFO *)&osInfo))
    {
        return "Cannot get Windows version";
    }

    std::ostringstream oss;
    oss << "Windows "
        << osInfo.dwMajorVersion << "."
        << osInfo.dwMinorVersion
        << " (build " << osInfo.dwBuildNumber << ")";
    return oss.str();

#elif defined(__APPLE__)
    struct utsname uts;
    if (uname(&uts) == 0)
    {
        std::ostringstream oss;
        oss << uts.sysname << " "
            << uts.release << " "
            << uts.version << " "
            << uts.machine;
        return oss.str();
    }
    else
    {
        return "Cannot get macOS info";
    }

#elif defined(__linux__)
    struct utsname uts;
    if (uname(&uts) == 0)
    {
        std::ostringstream oss;
        oss << uts.sysname << " "
            << uts.release << " "
            << uts.version << " "
            << uts.machine;
        return oss.str();
    }
    else
    {
        return "Cannot get Linux info";
    }

#else
    return "Unknown OS";
#endif
}

std::string getProcessInfo()
{
#if defined(_WIN32)
    DWORD pid = GetCurrentProcessId();
    std::ostringstream oss;
    oss << "PID: " << pid;
    return oss.str();
#else
    pid_t pid = getpid();
    std::ostringstream oss;
    oss << "PID: " << pid;
    return oss.str();
#endif
}