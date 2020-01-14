/**
 * @file	installer.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JCU_DAEMON_INSTALLER_H__
#define __JCU_DAEMON_INSTALLER_H__

#include <string>
#include <memory>

#include <jcu-file/path.h>

namespace jcu {
    namespace daemon {
        class Daemon;
        class PlatformInstaller;

        class Result {
        protected:
            friend class Installer;

            std::shared_ptr<PlatformInstaller> platform_installer_;

            bool accept_;
            int result_;

        public:
            Result(std::shared_ptr<PlatformInstaller> platform_installer, bool accept, int result) : platform_installer_(std::move(platform_installer)), accept_(accept), result_(result) {}

            bool accept() const {
                return accept_;
            }

            int result() const {
                return result_;
            }
        };

        class InstallResultChain : public Result {
        private:
            friend class Installer;

        public:
            InstallResultChain(std::shared_ptr<PlatformInstaller> platform_installer, bool accept, int result) : Result(std::move(platform_installer), accept, result) {}
            InstallResultChain start();
        };

        class Installer {
        private:
			std::shared_ptr<PlatformInstaller> platform_installer_;

			Installer(const Installer& o) = delete;
			Installer& operator=(const Installer& o) = delete;

        public:
            Installer(std::shared_ptr<PlatformInstaller> platform_installer);
			Installer(Installer&& o) = default;
			Installer& operator=(Installer&& o) = default;

            enum StartMode {
                START_DEMAND = 0,
                START_AUTO = 1,
            };

            Installer& withServiceName(const std::string& service_name);
            Installer& withDisplayName(const std::string& display_name);
            Installer& withArguments(const std::string& arguments);
            Installer& withStartMode(StartMode start_mode);
            Installer& withPath(const file::Path &path);
            InstallResultChain install();
            InstallResultChain start();

            Result stop();
            Result uninstall();
        };

        extern Installer installer(const Daemon *from = nullptr);
    }
}

#endif //__JCU_DAEMON_INSTALLER_H__
