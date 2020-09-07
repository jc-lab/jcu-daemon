/**
 * @file	daemon-platform.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __PLATFORM_INSTALLER_H__
#define __PLATFORM_INSTALLER_H__

#include "jcu-daemon/installer.h"

namespace jcu {
namespace daemon {

class PlatformInstaller {
 public:
  struct Result {
    int result_;
    bool accept_;

    Result(bool accept, int result) : accept_(accept), result_(result) {}
    Result(const Result &o) = default;
  };

  virtual void setServiceName(const std::string &service_name) = 0;
  virtual void setDisplayName(const std::string &display_name) = 0;
  virtual void setStartMode(Installer::StartMode start_mode) = 0;
  virtual void setPath(const file::Path &path) = 0;
  virtual void setArguments(const std::string &arguments) = 0;
  virtual Result install() = 0;
  virtual Result uninstall() = 0;
  virtual Result start() = 0;
  virtual Result stop() = 0;
};

} // namespace daemon
} // namespace jcu

#endif //__PLATFORM_INSTALLER_H__
