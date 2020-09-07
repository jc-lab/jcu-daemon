#include <windows.h>
#include <tlhelp32.h>

#include "windows-daemon.h"

namespace jcu {
namespace daemon {

std::unique_ptr<DaemonPlatform> createDaemonPlatform(DaemonImpl* daemon, const char* service_name) {
  return std::unique_ptr<DaemonPlatform>(new windows::WindowsDaemon(daemon, service_name));
}

namespace windows {

static std::basic_string<wchar_t> toWstr(const std::string &text) {
  return std::basic_string<wchar_t>(text.cbegin(), text.cend());
}

WindowsDaemon::WindowsDaemon(DaemonImpl *daemon, const char *service_name)
    : daemon_(daemon), CServiceBase(
    (PWSTR) service_name_wstr_,
    TRUE, TRUE, FALSE
),
      retval_(0) {
  std::basic_string<wchar_t> wstr_service_name(toWstr(service_name));
  wcscpy_s(service_name_wstr_, wstr_service_name.c_str());
}

WindowsDaemon::~WindowsDaemon() {
  if (thread_.joinable()) {
    thread_.join();
  }
}

int WindowsDaemon::OnStart(DWORD dwArgc, PWSTR *pszArgv) {
  int rc = daemon_->runStartup();
  if (rc) {
    return rc;
  }
  thread_ = std::thread(threadEntry, this);
  return 0;
}

void WindowsDaemon::OnStop() {
  daemon_->onStateEvent(SEVENT_STOP);
  if (thread_.joinable()) {
    thread_.join();
  }
}

bool WindowsDaemon::OnServiceCtrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData) {
  return daemon_->onWindowsServiceCtrlEvent((int) dwControl, dwEventType, lpEventData);
}

bool WindowsDaemon::isChild() const {
  return false;
}

int WindowsDaemon::getCurrentPid() const {
  return ::GetCurrentProcessId();
}

int WindowsDaemon::getParentPid() const {
  HANDLE hSnapshot;
  PROCESSENTRY32 pe32;
  DWORD ppid = 0, pid = GetCurrentProcessId();

  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  __try {
      if ( hSnapshot == INVALID_HANDLE_VALUE) __leave;

      ZeroMemory(&pe32, sizeof(pe32));
      pe32.dwSize = sizeof( pe32 );
      if ( !Process32First( hSnapshot, &pe32 )) __leave;

      do {
        if (pe32.th32ProcessID == pid) {
          ppid = pe32.th32ParentProcessID;
          break;
        }
      } while ( Process32Next( hSnapshot, &pe32 ));
  }
  __finally {
      if ( hSnapshot != INVALID_HANDLE_VALUE) CloseHandle( hSnapshot );
  }
  return -1;
}

int WindowsDaemon::getChildPid() const {
  return 0;
}

DaemonPlatform::RunType WindowsDaemon::run(int *rc, const WorkerFunction &worker) {
  worker_ = worker;
  BOOL bServiceRun = CServiceBase::Run(*this);
  if (!bServiceRun) {
    DWORD dwError = ::GetLastError();
    if (dwError == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {

      *rc = daemon_->runStartup();
      if (*rc) {
        return RUN_DONE;
      }

      return RUN_START_WORKER;
    }
    *rc = dwError;
    return RUN_DONE;
  }
  *rc = retval_;
  return RUN_DONE;
}

void WindowsDaemon::threadEntry(WindowsDaemon *self) {
  self->retval_ = self->worker_(self->daemon_);
}

} // namespace windows
} // namespace daemon
} // namespace jcu

