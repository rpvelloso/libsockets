/*
 * WindowsAuthentication.cpp
 *
 *  Created on: 27 de nov de 2017
 *      Author: rvelloso
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
