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

#include <iostream>
#include <ctime>

#include "HTTPClient.h"
#include "json.hpp"

struct InfoDisplay {
	template<class T>
	std::ostream &operator<<(const T &p) {
		auto time_now = std::time(nullptr);

		return (
		std::cout << std::put_time(std::localtime(&time_now), "%Y-%m-%d %H:%M:%S")
		<< " [INFO] " << p);
	}
};

static InfoDisplay infoDisplay;

#define INFO std::cout <<

using json = nlohmann::json;

namespace idx {

HTTPClient::HTTPClient() {
}

bool HTTPClient::isReadingBody() {
	return readingBody;
}

void HTTPClient::setReadingBody(bool r) {
	readingBody = r;
}

void HTTPClient::processRequest(std::ostream& outp) {
	if (verbose)
		infoDisplay << ">>> INPUT: " << std::endl << request.getBody() << std::endl;

	response.setReply(HTTPReply::NotFound);

	request.parseHeaders();
	response.setHTTPVersion(request.getHTTPVersion());

	auto method = request.getMethod();
	if (method != Method::Unknown && request.validURI()) {
		if (services != nullptr) {
			auto serviceIt = (*services)[(int)method].find(request.getURI());
			if (serviceIt != (*services)[(int)method].end()) {
				auto service = serviceIt->second;

				try {
					json input = json::parse(request.getBody());

					//std::cerr << ">>> INPUT: " << std::endl << input.dump(2) << std::endl;

					service(*this, input);
				} catch (...) {
					response.setReply(HTTPReply::InternalServerError);
				}
			}
		}
	}

	outputResponse(outp);
}

void HTTPClient::setServices(Services& services) {
	this->services = &services;
}

#define CRLF "\r\n"

HTTPRequest& HTTPClient::getRequest() {
	return request;
}

HTTPResponse& HTTPClient::getResponse() {
	return response;
}

void HTTPClient::outputResponse(std::ostream& outp) {
	response.buildHeaders();
	for (auto &hdr:response.getHeaders())
		outp << hdr << CRLF;
	outp << CRLF;
	outp << response.getBody() << std::endl;

	if (verbose)
		infoDisplay << "<<< OUTPUT:" << std::endl << response.getBody() << std::endl;
}

bool HTTPClient::isVerbose() const {
	return verbose;
}

void HTTPClient::setVerbose(bool verbose) {
	this->verbose = verbose;
}

} /* namespace idx */
