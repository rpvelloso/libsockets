/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <windows.h>
#include "auth/Authentication.h"
#include "auth/WindowsAuthentication.h"

Authentication authService(new WindowsAuthentication());

WindowsAuthentication::WindowsAuthentication() {
}

WindowsAuthentication::~WindowsAuthentication() {
}

bool WindowsAuthentication::authenticate(
	const std::string& username,
	const std::string& password) {

	HANDLE hToken;
	auto logged = LogonUser(
		username.c_str(),
		nullptr,
		password.c_str(),
		LOGON32_LOGON_NETWORK,
		LOGON32_PROVIDER_DEFAULT,
		&hToken);

	if (logged)
		CloseHandle(hToken);

	return logged;
}
