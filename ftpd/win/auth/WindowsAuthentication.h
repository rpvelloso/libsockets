/*
 * WindowsAuthentication.h
 *
 *  Created on: 27 de nov de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSAUTHENTICATION_H_
#define WINDOWSAUTHENTICATION_H_

#include "auth/AuthenticationImpl.h"

class WindowsAuthentication: public AuthenticationImpl {
public:
	WindowsAuthentication();
	virtual ~WindowsAuthentication();
	bool authenticate(const std::string &username, const std::string &password) override;
};

#endif /* WINDOWSAUTHENTICATION_H_ */
