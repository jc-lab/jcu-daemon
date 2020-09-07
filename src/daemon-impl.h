//
// Created by jichan on 2019-11-18.
//

#ifndef __DAEMON_IMPL_H__
#define __DAEMON_IMPL_H__

#include "jcu-daemon/daemon.h"
#include "platform-daemon.h"

#include <memory>
#include <atomic>

namespace jcu {
namespace daemon {

class DaemonImpl : public Daemon, public DaemonPlatformHandler {
 private:
  DaemonMode mode_;

  DaemonPlatform::RunType run_type_;
  std::unique_ptr<DaemonPlatform> platform_;
  std::atomic<bool> running_;

  StartupFunction startup_;
  WorkerFunction parent_runner_;

  std::unique_ptr<StateEventFunction> state_event_callback_;
  std::unique_ptr<WindowsServiceCtrlEventFunction> service_ctrl_event_callback_;

 public:
  DaemonImpl(const char *service_name);

  bool running() const override;

  bool isChild() const override;

  void setMode(DaemonMode mode) override;

  void setStartup(const StartupFunction &startup) override;

  void setParentRunner(const WorkerFunction& runner) override;

  int runStartup() override;

  DaemonMode getMode() const override;

  int run(const WorkerFunction &worker) override;

  static Daemon *initialize(const char *service_name);

  void setOnStateEvent(const StateEventFunction &state_event_callback) override;

  void setOnWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction &service_ctrl_event_callback) override;

  void onStateEvent(StateEvent state_event) override;

  bool onWindowsServiceCtrlEvent(int ctrl, int event_type, void *event_data) override;

  int getCurrentPid() const override;
  int getParentPid() const override;
  int getChildPid() const override;
};

} // namespace daemon
} // namespace jcu

#endif //__DAEMON_IMPL_H__
