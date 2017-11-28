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

#ifndef LINUX_AUTH_LINUXAUTHENTICATION_H_
#define LINUX_AUTH_LINUXAUTHENTICATION_H_

#include "auth/AuthenticationImpl.h"

class LinuxAuthentication: public AuthenticationImpl {
public:
	LinuxAuthentication();
	virtual ~LinuxAuthentication();
	bool authenticate(const std::string &username, const std::string &password) override;
private:
	static bool pamAuthentication(
		const std::string &service,
		const std::string &username,
		const std::string &password);
};

#endif /* LINUX_AUTH_LINUXAUTHENTICATION_H_ */
