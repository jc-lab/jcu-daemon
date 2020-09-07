/**
 * @file	linux_daemon.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/09/07
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <errno.h>
#include <unistd.h>

#include <jcu-daemon/installer.h>

#include "linux_daemon.h"

namespace jcu {
namespace daemon {

std::unique_ptr<DaemonPlatform> createDaemonPlatform(DaemonImpl* daemon, const char* service_name) {
  return std::unique_ptr<DaemonPlatform>(new plat_linux::LinuxDaemon(daemon, service_name));
}

std::shared_ptr<PlatformInstaller> createPlatformInstaller(const Daemon* daemon) {
  return nullptr;
}

namespace plat_linux {

LinuxDaemon::LinuxDaemon(DaemonImpl *daemon, const char *service_name) {
  daemon_ = daemon;
  service_name_ = service_name;
  child_pid_ = 0;
}

LinuxDaemon::~LinuxDaemon() {
}

DaemonPlatform::RunType LinuxDaemon::run(int *rc, const WorkerFunction &worker) {
  // run startup before forking
  *rc = daemon_->runStartup();
  if (*rc) {
    // startup failed
    return RUN_DONE;
  }

  if (daemon_->getMode() == kForkMode) {
    int cpid = fork();
    if (cpid < 0) {
      // error
      *rc = errno;
      return RUN_DONE;
    } else if (cpid > 0) {
      // Parent
      child_pid_ = cpid;
      return RUN_START_PARENT;
    }
  }

  return RUN_START_WORKER;
}

bool LinuxDaemon::isChild() const {
  return child_pid_ == 0;
}

int LinuxDaemon::getCurrentPid() const {
  return ::getpid();
}

int LinuxDaemon::getParentPid() const {
  return ::getppid();
}

int LinuxDaemon::getChildPid() const {
  return child_pid_;
}

} // namespace plat_linux
} // namespace daemon
} // namespace jcu
