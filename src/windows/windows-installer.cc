/**
 * @file	windows-installer.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "windows-installer.h"
#include "jcu-daemon/daemon.h"

namespace jcu {
    namespace daemon {
        namespace windows {
            WindowsServiceInstaller::WindowsServiceInstaller(const jcu::daemon::Daemon *daemon)
            : scm_handler_(NULL), start_mode_(Installer::START_DEMAND), path_(file::Path::self()) {
                if(daemon) {
                    setServiceName(daemon->getServiceName());
                }
            }

            WindowsServiceInstaller::~WindowsServiceInstaller() {
                close();
            }

            void WindowsServiceInstaller::close() {
                if(scm_handler_ && (scm_handler_ != INVALID_HANDLE_VALUE)) {
                    ::CloseServiceHandle(scm_handler_);
                    scm_handler_ = NULL;
                }
            }

            int WindowsServiceInstaller::openServiceManager() {
                if(scm_handler_ && (scm_handler_ != INVALID_HANDLE_VALUE)) {
                    return 0;
                }
                scm_handler_ = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP);
                if(!scm_handler_ || scm_handler_ == INVALID_HANDLE_VALUE) {
                    return ::GetLastError();
                }
                return 0;
            }

            void WindowsServiceInstaller::setServiceName(const std::string& service_name) {
                service_name_ = std::basic_string<TCHAR>(service_name.cbegin(), service_name.cend());
            }

            void WindowsServiceInstaller::setDisplayName(const std::string& display_name) {
                display_name_ = std::basic_string<TCHAR>(display_name.cbegin(), display_name.cend());
            }

            void WindowsServiceInstaller::setArguments(const std::string& arguments) {
                arguments_ = std::basic_string<TCHAR>(arguments.cbegin(), arguments.cend());;
            }

            void WindowsServiceInstaller::setStartMode(Installer::StartMode start_mode) {
                start_mode_ = start_mode;
            }
            void WindowsServiceInstaller::setPath(const file::Path &path) {
                path_ = path;
            }

            PlatformInstaller::Result WindowsServiceInstaller::install() {
                int rc = openServiceManager();

                if(rc != 0) {
                    return Result(false, rc);
                }

                std::basic_string<TCHAR> display_name = display_name_.empty() ? service_name_ : display_name_;
                DWORD dwStartType = SERVICE_DEMAND_START;

                std::basic_string<TCHAR> commandLine;

                switch(start_mode_) {
                    case Installer::START_AUTO:
                        dwStartType = SERVICE_AUTO_START;
                        break;
                    case Installer::START_DEMAND:
                        dwStartType = SERVICE_DEMAND_START;
                        break;
                }

                commandLine.append(_T("\""));
                commandLine.append(path_.getSystemString());
                commandLine.append(_T("\""));
                if(!arguments_.empty()) {
                    commandLine.append(_T(" "));
                    commandLine.append(arguments_);
                }

                service_handle_ = CreateService(
                    scm_handler_,                   // SCManager database
                    service_name_.c_str(),                 // Name of service
                    display_name_.c_str(),                 // Name to display
					SERVICE_ALL_ACCESS,           // Desired access
                    SERVICE_WIN32_OWN_PROCESS,      // Service type
                    dwStartType,                    // Service start type
                    SERVICE_ERROR_NORMAL,           // Error control type
                    commandLine.c_str(),                         // Service's binary
                    NULL,                           // No load ordering group
                    NULL,                           // No tag identifier
                    NULL,                // Dependencies
                    NULL,                     // Service running account
                    NULL                     // Password of the account
                );
                if (!service_handle_)
                {
                    DWORD dwError = ::GetLastError();
                    if(dwError == ERROR_SERVICE_EXISTS) {
                        service_handle_ = ::OpenService(scm_handler_, service_name_.c_str(), SERVICE_ALL_ACCESS);
                        return Result(true, dwError);
                    }
                    return Result(false, dwError);
                }

                return Result(true, 0);
            }

            PlatformInstaller::Result WindowsServiceInstaller::start() {
                if(!::StartService(service_handle_, 0, NULL)) {
                    DWORD dwError = ::GetLastError();
                    if(dwError == ERROR_SERVICE_ALREADY_RUNNING) {
                        return Result(true, dwError);
                    }
                    return Result(false, dwError);
                }
                return Result(true, 0);
            }
        }
    }
}