/*
 * LinuxAuthentication.h
 *
 *  Created on: 27 de nov de 2017
 *      Author: roberto
 */

#ifndef LINUX_AUTH_LINUXAUTHENTICATION_H_
#define LINUX_AUTH_LINUXAUTHENTICATION_H_

#include "auth/AuthenticationImpl.h"

class LinuxAuthentication: public AuthenticationImpl {
public:
	LinuxAuthentication();
	virtual ~LinuxAuthentication();
	bool authenticate(const std::string &username, const std::string &password) override;
};

#endif /* LINUX_AUTH_LINUXAUTHENTICATION_H_ */
