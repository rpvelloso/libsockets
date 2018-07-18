/*
    Copyright 2018 Roberto Panerai Velloso.
    This file is part of indexer.
    Indexer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Indexer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Indexer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include <functional>
#include <unordered_map>

#include "libsockets-no-ssl.h"

#include "HTTPClient.h"
#include "Services.h"

namespace idx {

class HTTPServer {
public:
	HTTPServer(bool verbose = false);
	void start(const std::string &host, const std::string &port);

	void registerService(const std::string &methodStr, const std::string &uri, ServiceFunction service);
	bool isVerbose() const;
	void setVerbose(bool verbose);

private:
	socks::Server server;
	Services services;
	bool verbose;

	void onReceive(socks::Context<HTTPClient> &ctx, std::istream &inp, std::ostream &outp);
	void onConnect(socks::Context<HTTPClient> &ctx, std::istream &inp, std::ostream &outp);
	static bool readline(std::istream &inp, std::string &line);
	socks::Server makeServer();
};

} /* namespace idx */

#endif /* HTTPSERVER_H_ */
