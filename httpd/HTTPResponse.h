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

#ifndef SERVER_HTTPRESPONSE_H_
#define SERVER_HTTPRESPONSE_H_

#include <string>
#include <vector>

namespace idx {

enum class HTTPReply {
	OK = 200,
	NotFound = 404,
	InternalServerError = 500
};

//TODO: add more options: content-type, etc
class HTTPResponse {
public:
	HTTPResponse();
	void appendBody(const std::string &b);
	const std::string &getBody() const;
	HTTPReply getReply() const;
	void setReply(HTTPReply r);
	void buildHeaders();
	const std::vector<std::string> &getHeaders() const;
	const std::string& getHTTPVersion() const;
	void setHTTPVersion(const std::string& httpVersion);
	const std::string& getContentType() const;
	void setContentType(const std::string& contentType);

private:
	std::string httpVersion;
	std::string contentType = "application/json";
	HTTPReply reply = HTTPReply::NotFound;
	std::vector<std::string> headers;
	std::string body;
};

} /* namespace idx */

#endif /* SERVER_HTTPRESPONSE_H_ */
