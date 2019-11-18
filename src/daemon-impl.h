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
            DaemonPlatform::RunType run_type_;
            std::unique_ptr<DaemonPlatform> platform_;
            std::atomic<bool> running_;

            std::unique_ptr<StateEventFunction> state_event_callback_;
            std::unique_ptr<WindowsServiceCtrlEventFunction> service_ctrl_event_callback_;

        public:
            DaemonImpl(const char *service_name);

            bool running() const override;

            int run(const WorkerFunction& worker) override;

            static Daemon *initialize(const char *service_name);

            void setOnStateEvent(const StateEventFunction& state_event_callback) override;

            void setOnWindowsServiceCtrlEvent(const WindowsServiceCtrlEventFunction& service_ctrl_event_callback) override;

            void onStateEvent(StateEvent state_event) override;

            bool onWindowsServiceCtrlEvent(int ctrl, int event_type, void *event_data) override;
        };
    }
}

#endif //__DAEMON_IMPL_H__
