/*
 * Authentication.cpp
 *
 *  Created on: 27 de nov de 2017
 *      Author: rvelloso
 */

#include <auth/Authentication.h>

Authentication::Authentication(AuthenticationImpl *impl) : impl(impl) {
}

bool Authentication::authenticate(
	const std::string& username,
	const std::string& password) {

	return impl->authenticate(username, password);
}
