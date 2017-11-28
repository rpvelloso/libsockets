/*
 * LinuxAuthentication.cpp
 *
 *  Created on: 27 de nov de 2017
 *      Author: roberto
 */

#include "auth/Authentication.h"
#include "auth/LinuxAuthentication.h"
#include "auth/PAM.h"

Authentication authService(new LinuxAuthentication());

LinuxAuthentication::LinuxAuthentication() {
}

LinuxAuthentication::~LinuxAuthentication() {
}

bool LinuxAuthentication::authenticate(const std::string& username,
		const std::string& password) {

	return PAM::auth("remote", username, password);
}
