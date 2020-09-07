/**
 * @file	windows-installer.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __WINDOWS_WINDOWS_INSTALLER_H__
#define __WINDOWS_WINDOWS_INSTALLER_H__

#include <windows.h>

#include "../platform-installer.h"

#include <jcu-file/path.h>

namespace jcu {
namespace daemon {
namespace windows {

class WindowsServiceInstaller : public PlatformInstaller {
 private:
  SC_HANDLE scm_handler_;
  SC_HANDLE service_handle_;
  std::basic_string<TCHAR> service_name_;
  std::basic_string<TCHAR> display_name_;
  std::basic_string<TCHAR> arguments_;
  Installer::StartMode start_mode_;
  file::Path path_;

  DWORD openServiceHandle();

 public:
  WindowsServiceInstaller(const Daemon *daemon = nullptr);
  ~WindowsServiceInstaller();

  int openServiceManager();
  void close();

  void setServiceName(const std::string &service_name) override;
  void setDisplayName(const std::string &service_name) override;
  void setArguments(const std::string &arguments) override;
  void setStartMode(Installer::StartMode start_mode) override;
  void setPath(const file::Path &path) override;
  Result install() override;
  Result uninstall() override;
  Result start() override;
  Result stop() override;
};

} // namespace windows
} // namespace daemon
} // namespace jcu

#endif //__WINDOWS_WINDOWS_INSTALLER_H__
