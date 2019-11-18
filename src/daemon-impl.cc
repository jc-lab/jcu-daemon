/**
 * @file	daemon.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "daemon-impl.h"

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

        DaemonImpl::DaemonImpl(const char *service_name)
            : Daemon(service_name) {
            platform_.reset(new DAEMON_PLATFORM_CLASS(this, service_name));
        }

        bool DaemonImpl::running() const {
            if(run_type_ == DaemonPlatform::RUN_START_WORKER) {
                return running_;
            }
            return platform_->running();
        }

        int DaemonImpl::run(const WorkerFunction& worker) {
            int rc = 0;
            running_.store(true);
            run_type_ = platform_->runInService(&rc, worker);
            if(run_type_ == DaemonPlatform::RUN_START_WORKER) {
                std::signal(SIGINT, [](int signum) -> void {
                  DaemonImpl *self = (DaemonImpl*)singletone.get();
                  self->running_.store(false);

                  StateEventFunction *cb = self->state_event_callback_.get();
                  if(cb) {
                      (*cb)(self, SEVENT_STOP);
                  }
                });
#ifdef SIGHUP
                std::signal(SIGHUP, [](int signum) -> void {
                      DaemonImpl *self = (DaemonImpl*)singletone.get();
                      StateEventFunction *cb = self->state_event_callback_.get();
                      if(cb) {
                          (*cb)(self, SEVENT_HUP);
                      }
                    });
#endif
                rc = worker(this);
            }
            return rc;
        }

        Daemon *DaemonImpl::initialize(const char *service_name) {
            singletone.reset(new DaemonImpl(service_name));
            return singletone.get();
        }

        void DaemonImpl::setOnStateEvent(const StateEventFunction& state_event_callback) {
            state_event_callback_.reset(new StateEventFunction(state_event_callback));
        }

        void DaemonImpl::setOnWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction& service_ctrl_event_callback) {
            service_ctrl_event_callback_.reset(new WindowsServiceCtrlEventFunction(service_ctrl_event_callback));
        }

        void DaemonImpl::onStateEvent(StateEvent state_event) {
            StateEventFunction *cb = state_event_callback_.get();
            if(cb) {
                (*cb)(this, state_event);
            }
        }

        bool DaemonImpl::onWindowsServiceCtrlEvent(int ctrl) {
            WindowsServiceCtrlEventFunction *cb = service_ctrl_event_callback_.get();
            if(cb) {
                return (*cb)(this, ctrl);
            }
            return false;
        }

        Daemon *initialize(const char *service_name) {
            return DaemonImpl::initialize(service_name);
        }

        Daemon *getInstance() {
            return singletone.get();
        }

        Daemon::Daemon(const char *service_name)
            : service_name_(service_name) {
        }

        const std::string &Daemon::getServiceName() const {
            return service_name_;
        }

        Installer installer(const Daemon *from) {
            return std::move(Installer(std::shared_ptr<PlatformInstaller>(new SERVICE_INSTALLER_PLATFORM_CLASS(from))));
        }
    }
}

