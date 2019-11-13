/**
 * @file	daemon.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "jcu-daemon/daemon.h"
#include "platform-installer.h"

#include <memory>
#include <atomic>
#include <csignal>

#ifdef _WIN32
#include "windows/windows-daemon.h"
#include "windows/windows-installer.h"
#define DAEMON_PLATFORM_CLASS windows::WindowsDaemon
#define SERVICE_INSTALLER_PLATFORM_CLASS windows::WindowsServiceInstaller
#endif

namespace jcu {
    namespace daemon {
        static std::unique_ptr<Daemon> singletone;

        class DaemonImpl : public Daemon {
        public:
            DaemonPlatform::RunType run_type_;
            std::unique_ptr<DaemonPlatform> platform_;
            std::atomic<bool> running_;

            DaemonImpl(const char *service_name)
            : Daemon(service_name) {
                platform_.reset(new DAEMON_PLATFORM_CLASS(this, service_name));
            }

            bool running() const override {
                if(run_type_ == DaemonPlatform::RUN_START_WORKER) {
                    return running_;
                }
                return platform_->running();
            }

            int run(const WorkerFunction& worker) override {
                int rc = 0;
                running_.store(true);
                run_type_ = platform_->runInService(&rc, worker);
                if(run_type_ == DaemonPlatform::RUN_START_WORKER) {
                    std::signal(SIGINT, [](int signum) -> void {
                      DaemonImpl *self = (DaemonImpl*)singletone.get();
                      self->running_.store(false);
                    });
                    rc = worker(this);
                }
                return rc;
            }

            static Daemon *create(const char *service_name) {
                singletone.reset(new DaemonImpl(service_name));
                return singletone.get();
            }
        };

        Daemon *initialize(const char *service_name) {
            return DaemonImpl::create(service_name);
        }

        Daemon::Daemon(const char *service_name)
        : service_name_(service_name) {
        }

        const std::string &Daemon::getServiceName() const {
            return service_name_;
        }

        Installer installer(const Daemon *from) {
            PlatformInstaller *platform_installer = new SERVICE_INSTALLER_PLATFORM_CLASS(from);
            return Installer(platform_installer);
        }
    }
}

