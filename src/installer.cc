/**
 * @file	installer.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "jcu-daemon/installer.h"
#include "platform-installer.h"

namespace jcu {
    namespace daemon {

        Installer::Installer(PlatformInstaller *platform_installer) : platform_installer_(platform_installer), result_chain_(*this) {
        }

        Installer::Installer(Installer&& o) : result_chain_(*this, o.result_chain_.accept(), o.result_chain_.result()) {
            platform_installer_ = o.platform_installer_;
            o.platform_installer_ = NULL;
        }

        Installer::~Installer() {
            if(platform_installer_) {
                delete platform_installer_;
                platform_installer_ = NULL;
            }
        }

        Installer& Installer::withServiceName(const std::string& service_name) {
            platform_installer_->setServiceName(service_name);
            return *this;
        }
        Installer& Installer::withDisplayName(const std::string& display_name) {
            platform_installer_->setDisplayName(display_name);
            return *this;
        }
        Installer& Installer::withArguments(const std::string& arguments) {
            platform_installer_->setArguments(arguments);
            return *this;
        }
        Installer& Installer::withStartMode(StartMode start_mode) {
            platform_installer_->setStartMode(start_mode);
            return *this;
        }
        Installer& Installer::withPath(const file::Path &path) {
            platform_installer_->setPath(path);
            return *this;
        }

        InstallResultChain &Installer::install() {
            PlatformInstaller::Result r(platform_installer_->install());
            result_chain_.accept_ = r.accept_;
            result_chain_.result_ = r.result_;
            return result_chain_;
        }

        InstallResultChain &Installer::start() {
            PlatformInstaller::Result r(platform_installer_->start());
            result_chain_.accept_ = r.accept_;
            result_chain_.result_ = r.result_;
            return result_chain_;
        }

        InstallResultChain& InstallResultChain::start() {
            if(!accept_)
                return *this;
            return installer_.start();
        }
    }
}

