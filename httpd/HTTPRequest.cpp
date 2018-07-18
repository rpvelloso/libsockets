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

#include "HTTPRequest.h"

namespace idx {

HTTPRequest::HTTPRequest() {
}

void HTTPRequest::addHeader(const std::string& hdr) {
	headers.emplace_back(hdr);
}

void HTTPRequest::appendBody(const std::string& b) {
	body.append(b.begin(), b.end());
	body.push_back('\n');
}

void HTTPRequest::parseHeaders() {
	if (headers.size() > 0) {
		std::string methodStr;
		std::stringstream ss(headers[0]);

		ss >> methodStr;
		ss >> uri;
		ss >> httpVersion;

		method = methodStr2Enum(methodStr);

		/*for (size_t i = 1; i < headers.size(); ++i) {
			//TODO: parse remaining headers (content type, length, etc)
		}*/
	}
}

void HTTPRequest::printRequest() const {
	std::cout << "*** headers: " << std::endl;
	for (auto &h:headers)
		std::cout << h << std::endl;
	std::cout << "*** body: " << std::endl;
	std::cout << body << std::endl;
}


bool HTTPRequest::validURI() { /*TODO: add more validation */
	return !uri.empty();
}

Method HTTPRequest::getMethod() const {
	return method;
}

const std::string& HTTPRequest::getURI() const {
	return uri;
}

const std::string& HTTPRequest::getHTTPVersion() const {
	return httpVersion;
}

size_t HTTPRequest::getContentLength() const {
	return contentLength;
}

const std::string& HTTPRequest::getBody() const {
	return body;
}

} /* namespace idx */
