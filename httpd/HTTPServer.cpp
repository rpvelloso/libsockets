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

#include <functional>

#include "HTTPServer.h"
#include "HTTPClient.h"

namespace idx {

HTTPServer::HTTPServer(bool verbose) :
	server(makeServer()),
	services(2),
	verbose(verbose) {
}

bool HTTPServer::readline(std::istream &inp, std::string &line) {

	auto savePos = inp.tellg();
	std::getline(inp, line);
	if (inp)
		return true;
	else
		inp.seekg(savePos);

	return false;
}

void HTTPServer::start(const std::string &host, const std::string &port) {
	server.listen(host, port);
}

void HTTPServer::registerService(const std::string &methodStr, const std::string& uri,
		ServiceFunction service) {
	auto method = methodStr2Enum(methodStr);
	if (method != Method::Unknown)
		services[(int)method][uri] = service;
	else
		throw std::runtime_error("Unkown method " + methodStr);
}

socks::Server HTTPServer::makeServer() {
	return socks::factory::makeThreadedServer<HTTPClient>(
		std::bind(&HTTPServer::onReceive, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3),
		std::bind(&HTTPServer::onConnect, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3));
}

void HTTPServer::onReceive(socks::Context<HTTPClient> &ctx, std::istream &inp, std::ostream &outp) {
	HTTPClient &context = ctx.getContext();

	while (inp) {
		std::string line;

		if (readline(inp, line)) {
			if (line.back() == '\r')
				line.pop_back();

			if (context.isReadingBody()) {
				if (!line.empty())
					context.getRequest().appendBody(line);
			} else {
				if (line.empty())
					context.setReadingBody(true);
				else {
					context.getRequest().addHeader(line);
				}
			}
		} else {
			context.processRequest(outp);
			break;
		}
	}
}

void HTTPServer::onConnect(socks::Context<HTTPClient> &ctx, std::istream &inp, std::ostream &outp) {
	HTTPClient &context = ctx.getContext();

	context.setServices(services);
	context.setVerbose(verbose);
}

bool HTTPServer::isVerbose() const {
	return verbose;
}

void HTTPServer::setVerbose(bool verbose) {
	this->verbose = verbose;
}

} /* namespace idx */
