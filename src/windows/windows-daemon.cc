#include "windows-daemon.h"

namespace jcu {
    namespace daemon {
        namespace windows {
            static std::basic_string<wchar_t> toWstr(const std::string &text) {
                return std::basic_string<wchar_t>(text.cbegin(), text.cend());
            }

            WindowsDaemon::WindowsDaemon(Daemon *daemon, const char *service_name)
            : daemon_(daemon)
            , service_name_wstr_(toWstr(service_name))
            , CServiceBase(
                    (PWSTR)service_name_wstr_.data(),
                TRUE, TRUE, FALSE
                ),
              retval_(0) {
                stop_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
            }

            WindowsDaemon::~WindowsDaemon() {
                if(thread_.joinable()) {
                    thread_.join();
                }
                ::CloseHandle(stop_event_);
                stop_event_ = NULL;
            }

            void WindowsDaemon::OnStart(DWORD dwArgc, PWSTR *pszArgv) {
                thread_ = std::thread(threadEntry, this);
            }

            void WindowsDaemon::OnStop() {
                ::SetEvent(stop_event_);
            }

            bool WindowsDaemon::running() {
                DWORD dwWait = ::WaitForSingleObject(stop_event_, 0);
                return (dwWait == WAIT_TIMEOUT);
            }

            DaemonPlatform::RunType WindowsDaemon::runInService(int *rc, const WorkerFunction& worker) {
                worker_ = worker;
                BOOL bServiceRun = CServiceBase::Run(*this);
                if(!bServiceRun) {
                    DWORD dwError = ::GetLastError();
                    if(dwError == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
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
        }
    }
}

