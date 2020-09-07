/**
 * @file	daemon.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JCU_DAEMON_DAEMON_H__
#define __JCU_DAEMON_DAEMON_H__

#include <string>
#include <functional>

namespace jcu {
namespace daemon {

class Daemon;

enum StateEvent {
  SEVENT_STOP = 1,
  SEVENT_HUP = 2,
};

typedef std::function<void(Daemon *daemon, StateEvent event)> StateEventFunction;
typedef std::function<int(Daemon *daemon)> WorkerFunction;
typedef std::function<bool(Daemon *daemon, int ctrl, int event_type, void *event_data)> WindowsServiceCtrlEventFunction;

class Daemon {
 protected:
  std::string service_name_;

  Daemon(const char *service_name);

  virtual void setOnStateEvent(const StateEventFunction &state_event_callback) = 0;
  virtual void setOnWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction &service_ctrl_event_callback) {}

 public:
  const std::string &getServiceName() const;

  Daemon *onStateEvent(const StateEventFunction &state_event_callback) {
    this->setOnStateEvent(state_event_callback);
    return this;
  }

  Daemon *onWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction &service_ctrl_event_callback) {
    this->setOnWindowsServiceCtrlEvent(service_ctrl_event_callback);
    return this;
  }

  virtual int run(const WorkerFunction &worker) = 0;
  virtual bool running() const = 0;
};

extern Daemon *initialize(const char *service_name);
extern Daemon *getInstance();

} // namespace daemon
} // namespace jcu

#endif //__JCU_DAEMON_DAEMON_H__
