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

#include "FTPServer.h"

AuthenticationFunction FTPClientInfo::authenticate =
[](const std::string &username, const std::string &password, FTPClientInfo& clientInfo) {
	/*
	 * in here a user profile can be loaded into 'clientInfo'
	 * upon authentication in order to define, for example,
	 * a home dir, chroot, etc.
	 */
	return authService.authenticate(username, password);
};

int main(int argc, char **argv) {
	FTPServer ftpServer;

	// SITE CLIENT COUNT
	ftpServer.registerSiteCommand(
		"CLIENT",
		[&ftpServer](const std::string &params, FTPClientInfo &clientInfo) {
			std::stringstream ss(params);
			std::string p1;
			ss >> p1;
			std::transform(p1.begin(), p1.end(), p1.begin(), ::toupper);
			if (p1 == "COUNT")
				return "200 There is/are " + std::to_string(ftpServer.getClientCount()) + " client(s) online.";
			else
				return std::string("501 Invalid SITE CLIENT parameter.");
	});

	ftpServer.start();
}
