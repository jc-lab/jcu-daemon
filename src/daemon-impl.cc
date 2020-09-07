/**
 * @file	daemon.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "daemon-impl.h"
#include <jcu-daemon/installer.h>

#include <csignal>

namespace jcu {
namespace daemon {

static std::unique_ptr<Daemon> singletone;

extern std::unique_ptr<DaemonPlatform> createDaemonPlatform(DaemonImpl* daemon, const char* service_name);
extern std::shared_ptr<PlatformInstaller> createPlatformInstaller(const Daemon* daemon);

DaemonImpl::DaemonImpl(const char *service_name)
    : Daemon(service_name), mode_(kNormalMode) {
  platform_ = std::move(createDaemonPlatform(this, service_name));
}

bool DaemonImpl::running() const {
  return running_.load();
}

bool DaemonImpl::isChild() const {
  return platform_->isChild();
}

void DaemonImpl::setMode(DaemonMode mode) {
  mode_ = mode;
}

DaemonMode DaemonImpl::getMode() const {
  return mode_;
}

void DaemonImpl::setStartup(const StartupFunction &startup) {
  startup_ = startup;
}

void DaemonImpl::setParentRunner(const WorkerFunction& runner) {
  parent_runner_ = runner;
}

int DaemonImpl::runStartup() {
  if (startup_) {
    return startup_(this);
  }
  return 0;
}

int DaemonImpl::run(const WorkerFunction &worker) {
  int rc = 0;
  running_.store(true);
  run_type_ = platform_->run(&rc, worker);
  if (run_type_ == DaemonPlatform::RUN_START_WORKER) {
    std::signal(SIGINT, [](int signum) -> void {
      DaemonImpl *self = (DaemonImpl *) singletone.get();
      StateEventFunction *cb = self->state_event_callback_.get();
      if (cb) {
        (*cb)(self, SEVENT_STOP);
      }

      self->running_.store(false);
    });
    std::signal(SIGTERM, [](int signum) -> void {
      DaemonImpl *self = (DaemonImpl *) singletone.get();
      StateEventFunction *cb = self->state_event_callback_.get();
      if (cb) {
        (*cb)(self, SEVENT_STOP);
      }
      self->running_.store(false);
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
  else if (run_type_ == DaemonPlatform::RUN_START_PARENT) {
    if (parent_runner_) {
      rc = parent_runner_(this);
    }
  }
  return rc;
}

Daemon *DaemonImpl::initialize(const char *service_name) {
  singletone.reset(new DaemonImpl(service_name));
  return singletone.get();
}

void DaemonImpl::setOnStateEvent(const StateEventFunction &state_event_callback) {
  state_event_callback_.reset(new StateEventFunction(state_event_callback));
}

void DaemonImpl::setOnWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction &service_ctrl_event_callback) {
  service_ctrl_event_callback_.reset(new WindowsServiceCtrlEventFunction(service_ctrl_event_callback));
}

void DaemonImpl::onStateEvent(StateEvent state_event) {
  if (state_event == SEVENT_STOP) {
    running_.store(false);
  }
  StateEventFunction *cb = state_event_callback_.get();
  if (cb) {
    (*cb)(this, state_event);
  }
}

bool DaemonImpl::onWindowsServiceCtrlEvent(int ctrl, int event_type, void *event_data) {
  WindowsServiceCtrlEventFunction *cb = service_ctrl_event_callback_.get();
  if (cb) {
    return (*cb)(this, ctrl, event_type, event_data);
  }
  return false;
}

int DaemonImpl::getCurrentPid() const {
  return platform_->getCurrentPid();
}

int DaemonImpl::getParentPid() const {
  return platform_->getParentPid();
}

int DaemonImpl::getChildPid() const {
  return platform_->getChildPid();
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
  return std::move(Installer(createPlatformInstaller(from)));
}

} // namespace daemon
} // namespace jcu

