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

#include <getopt.h>
#include <algorithm>

#include "FTPServer.h"
#include "auth/Authentication.h"

struct Options {
	std::string port = "";
	bool secure = false;
	bool verbose = false;
};

void usage(const std::string &prog) {
	std::cerr << prog << " [-p port] [-s] [-v]" << std::endl;
	std::cerr << "-p port number. Default: 21 or 990 with SSL enabled." << std::endl;
	std::cerr << "-s enables SSL. Default disabled. " << std::endl <<
				 "   Files 'cert.pem' and 'key.pem' are required in current directory. " << std::endl;
	std::cerr << "-v enables verbose mode. Default disabled. " << std::endl <<
				 "   Console display of all messages exchanged with clients" << std::endl;
	std::cerr << "-h this help screen." << std::endl;
	std::cerr << "ex.: " << prog << " -p 21 -v" << std::endl;
	throw std::runtime_error("bad options");
}

Options parseOptions(int argc, char **argv) {
	Options options;
	int c;
	while ((c = getopt(argc, argv, "hvsp:")) != -1) {
		switch (c) {
		case 'p':
			options.port = optarg;
			break;
		case 's':
			options.secure = true;
			break;
		case 'v':
			options.verbose = true;
			break;
		case 'h':
		default:
			usage(argv[0]);
		}
	}

	return options;
}

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
	try{
		Options options = parseOptions(argc, argv);

		FTPServer ftpServer(
			options.secure,
			options.verbose,
			options.port);

		// SITE CLIENT COUNT
		ftpServer.registerSiteCommand(
			"CLIENT",
			[&ftpServer](const std::string &params, FTPClientInfo &clientInfo) {
				std::stringstream ss(params);
				std::string p1;
				ss >> p1;
				std::transform(p1.begin(), p1.end(), p1.begin(), ::toupper);
				if (p1 == "COUNT")
					return "200 Dear " +
							clientInfo.getUsername() +
							", there is/are currently " +
							std::to_string(ftpServer.getClientCount()) +
							" client(s) online.";
				else
					return std::string("501 Invalid SITE CLIENT parameter.");
		});

		ftpServer.start();
	} catch (std::exception &e) {
		std::string what = e.what();
		if (what != "bad options")
			std::cerr << e.what() << std::endl;
	}
}
