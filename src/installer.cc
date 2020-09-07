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
  if (platform_installer_) {
    platform_installer_->setServiceName(service_name);
  }
  return *this;
}
Installer &Installer::withDisplayName(const std::string &display_name) {
  if (platform_installer_) {
    platform_installer_->setDisplayName(display_name);
  }
  return *this;
}
Installer &Installer::withArguments(const std::string &arguments) {
  if (platform_installer_) {
    platform_installer_->setArguments(arguments);
  }
  return *this;
}
Installer &Installer::withStartMode(StartMode start_mode) {
  if (platform_installer_) {
    platform_installer_->setStartMode(start_mode);
  }
  return *this;
}
Installer &Installer::withPath(const file::Path &path) {
  if (platform_installer_) {
    platform_installer_->setPath(path);
  }
  return *this;
}

InstallResultChain Installer::install() {
  if (platform_installer_) {
    PlatformInstaller::Result r(platform_installer_->install());
    return std::move(InstallResultChain(platform_installer_, r.accept_, r.result_));
  }
  // Not supported
  return std::move(InstallResultChain(platform_installer_, false, -1));
}

Result Installer::uninstall() {
  if (platform_installer_) {
    PlatformInstaller::Result r(platform_installer_->uninstall());
    return std::move(Result(platform_installer_, r.accept_, r.result_));
  }
  // Not supported
  return std::move(InstallResultChain(platform_installer_, false, -1));
}

InstallResultChain Installer::start() {
  if (platform_installer_) {
    PlatformInstaller::Result r(platform_installer_->start());
    return std::move(InstallResultChain(platform_installer_, r.accept_, r.result_));
  }
  // Not supported
  return std::move(InstallResultChain(platform_installer_, false, -1));
}

Result Installer::stop() {
  if (platform_installer_) {
    PlatformInstaller::Result r(platform_installer_->stop());
    return std::move(Result(platform_installer_, r.accept_, r.result_));
  }
  // Not supported
  return std::move(InstallResultChain(platform_installer_, false, -1));
}

InstallResultChain InstallResultChain::start() {
  if (!accept_)
    return *this;

  if (platform_installer_) {
    PlatformInstaller::Result r(platform_installer_->start());
    return std::move(InstallResultChain(platform_installer_, r.accept_, r.result_));
  }
  // Not supported
  return std::move(InstallResultChain(platform_installer_, false, -1));
}

} // namespace daemon
} // namespace jcu
