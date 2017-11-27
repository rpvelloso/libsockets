/*
 * Authentication.h
 *
 *  Created on: 27 de nov de 2017
 *      Author: rvelloso
 */

#ifndef AUTHENTICATION_H_
#define AUTHENTICATION_H_

#include <memory>
#include "AuthenticationImpl.h"

class Authentication {
public:
	Authentication(AuthenticationImpl *impl);
	bool authenticate(const std::string &username, const std::string &password);
private:
	std::unique_ptr<AuthenticationImpl> impl;
};

extern Authentication authService;

#endif /* AUTHENTICATION_H_ */
