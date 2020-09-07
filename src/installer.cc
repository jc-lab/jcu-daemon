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

Installer::Installer(std::shared_ptr<PlatformInstaller> platform_installer) : platform_installer_(platform_installer) {
}

Installer &Installer::withServiceName(const std::string &service_name) {
  platform_installer_->setServiceName(service_name);
  return *this;
}
Installer &Installer::withDisplayName(const std::string &display_name) {
  platform_installer_->setDisplayName(display_name);
  return *this;
}
Installer &Installer::withArguments(const std::string &arguments) {
  platform_installer_->setArguments(arguments);
  return *this;
}
Installer &Installer::withStartMode(StartMode start_mode) {
  platform_installer_->setStartMode(start_mode);
  return *this;
}
Installer &Installer::withPath(const file::Path &path) {
  platform_installer_->setPath(path);
  return *this;
}

InstallResultChain Installer::install() {
  PlatformInstaller::Result r(platform_installer_->install());
  return std::move(InstallResultChain(platform_installer_, r.accept_, r.result_));
}

Result Installer::uninstall() {
  PlatformInstaller::Result r(platform_installer_->uninstall());
  return std::move(Result(platform_installer_, r.accept_, r.result_));
}

InstallResultChain Installer::start() {
  PlatformInstaller::Result r(platform_installer_->start());
  return std::move(InstallResultChain(platform_installer_, r.accept_, r.result_));
}

Result Installer::stop() {
  PlatformInstaller::Result r(platform_installer_->stop());
  return std::move(Result(platform_installer_, r.accept_, r.result_));
}

InstallResultChain InstallResultChain::start() {
  if (!accept_)
    return *this;

  PlatformInstaller::Result r(platform_installer_->start());
  return std::move(InstallResultChain(platform_installer_, r.accept_, r.result_));
}

} // namespace daemon
} // namespace jcu
