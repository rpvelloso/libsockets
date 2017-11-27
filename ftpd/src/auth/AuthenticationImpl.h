/*
 * AuthenticationImpl.h
 *
 *  Created on: 27 de nov de 2017
 *      Author: rvelloso
 */

#ifndef AUTHENTICATIONIMPL_H_
#define AUTHENTICATIONIMPL_H_

#include <string>

class AuthenticationImpl {
public:
	AuthenticationImpl();
	virtual ~AuthenticationImpl();
	virtual bool authenticate(const std::string &username, const std::string &password) = 0;
};

#endif /* AUTHENTICATIONIMPL_H_ */
