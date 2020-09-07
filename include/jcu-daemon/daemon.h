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
typedef std::function<int(Daemon *daemon)> StartupFunction;
typedef std::function<int(Daemon *daemon)> WorkerFunction;
typedef std::function<bool(Daemon *daemon, int ctrl, int event_type, void *event_data)> WindowsServiceCtrlEventFunction;

enum DaemonMode {
  kNormalMode = 0,
  kForkMode = 1,
};

class Daemon {
 protected:
  std::string service_name_;

  Daemon(const char *service_name);

  virtual void setMode(DaemonMode mode) = 0;
  virtual void setStartup(const StartupFunction &startup) = 0;
  virtual void setParentRunner(const WorkerFunction &runner) = 0;
  virtual void setOnStateEvent(const StateEventFunction &state_event_callback) = 0;
  virtual void setOnWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction &service_ctrl_event_callback) {}

 public:
  const std::string &getServiceName() const;

  Daemon* mode(DaemonMode mode) {
    this->setMode(mode);
    return this;
  }

  Daemon *onStateEvent(const StateEventFunction &state_event_callback) {
    this->setOnStateEvent(state_event_callback);
    return this;
  }

  Daemon *onWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction &service_ctrl_event_callback) {
    this->setOnWindowsServiceCtrlEvent(service_ctrl_event_callback);
    return this;
  }

  Daemon* startup(const StartupFunction& startup) {
    this->setStartup(startup);
    return this;
  }

  Daemon* parent(const WorkerFunction &worker) {
    this->setParentRunner(worker);
    return this;
  }

  virtual DaemonMode getMode() const = 0;

  virtual int run(const WorkerFunction &worker) = 0;
  virtual bool running() const = 0;
  virtual bool isChild() const = 0;
  virtual void stop() = 0;

  virtual int getCurrentPid() const = 0;
  virtual int getParentPid() const = 0;
  virtual int getChildPid() const = 0;
};

extern Daemon *initialize(const char *service_name);
extern Daemon *getInstance();

} // namespace daemon
} // namespace jcu

#endif //__JCU_DAEMON_DAEMON_H__
