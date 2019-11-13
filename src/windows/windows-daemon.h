/**
 * @file	windows-daemon.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __WINDOWS_WINDOWS_DAEMON_H__
#define __WINDOWS_WINDOWS_DAEMON_H__

#include <thread>

#include "jcu-daemon/daemon.h"
#include "../platform-daemon.h"

#include "service_base.h"

namespace jcu {
    namespace daemon {
        namespace windows {
            class WindowsDaemon : public DaemonPlatform, public CServiceBase {
            private:
                Daemon *daemon_;

                std::basic_string<wchar_t> service_name_wstr_;
                HANDLE stop_event_;

                WorkerFunction worker_;
                std::thread thread_;
                int retval_;

            public:
                WindowsDaemon(Daemon *daemon, const char *service_name);
                virtual ~WindowsDaemon();

            protected:
                void OnStart(DWORD dwArgc, PWSTR *pszArgv) override;
                void OnStop() override;

            public:
                RunType runInService(int *rc, const WorkerFunction& worker) override;
                bool running() override;

            private:
                static void threadEntry(WindowsDaemon *self);
            };
        }
    }
}

#endif //__WINDOWS_WINDOWS_DAEMON_H__
