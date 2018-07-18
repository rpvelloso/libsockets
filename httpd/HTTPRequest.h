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

#ifndef SERVER_HTTPREQUEST_H_
#define SERVER_HTTPREQUEST_H_

#include <vector>
#include <string>

#include "Services.h"

namespace idx {

class HTTPRequest {
public:
	HTTPRequest();
	void addHeader(const std::string &hdr);
	void appendBody(const std::string &b);
	void parseHeaders();
	void printRequest() const;
	bool validURI();

	Method getMethod() const;
	const std::string &getURI() const;
	const std::string &getHTTPVersion() const;
	size_t getContentLength() const;
	const std::string &getBody() const;
private:
	std::vector<std::string> headers;
	std::string httpVersion;
	std::string uri;
	Method method = Method::Unknown;
	std::string body;
	size_t contentLength = 0;
};

} /* namespace idx */

#endif /* SERVER_HTTPREQUEST_H_ */
