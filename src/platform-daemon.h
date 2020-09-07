/**
 * @file	daemon-platform.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __PLATFORM_DAEMON_H__
#define __PLATFORM_DAEMON_H__

#include <string>
#include <functional>

namespace jcu {
namespace daemon {

class DaemonPlatformHandler {
 public:
  virtual void onStateEvent(StateEvent state_event) = 0;
  virtual bool onWindowsServiceCtrlEvent(int ctrl, int event_type, void *event_data) { return false; }
};

class DaemonPlatform {
 public:
  enum RunType {
    RUN_DONE = 0,
    RUN_START_WORKER
  };

  /**
   * run in service mode
   *
   * 1. run startup
   * 2. start worker thread or process
   * @param rc
   * @param worker
   * @return
   */
  virtual RunType run(int *rc, const WorkerFunction &worker) = 0;
  virtual bool running() = 0;
};

} // namespace daemon
} // namespace jcu

#endif //__PLATFORM_DAEMON_H__
