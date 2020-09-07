/**
 * @file	linux_daemon.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/09/07
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef SAMPLE_APP_JCU_DAEMON_SRC_LINUX_LINUX_DAEMON_H_
#define SAMPLE_APP_JCU_DAEMON_SRC_LINUX_LINUX_DAEMON_H_

#include <thread>

#include "../daemon-impl.h"
#include "../platform-daemon.h"

namespace jcu {
namespace daemon {
namespace plat_linux {

class LinuxDaemon : public DaemonPlatform {
 private:
  DaemonImpl *daemon_;
  std::string service_name_;
  int child_pid_;

 public:
  LinuxDaemon(DaemonImpl *daemon, const char *service_name);
  virtual ~LinuxDaemon();
  RunType run(int *rc, const WorkerFunction &worker) override;
  bool isChild() const override;

  int getCurrentPid() const override;
  int getParentPid() const override;
  int getChildPid() const override;
};

} // namespace plat_linux
} // namespace daemon
} // namespace jcu

#endif //SAMPLE_APP_JCU_DAEMON_SRC_LINUX_LINUX_DAEMON_H_
