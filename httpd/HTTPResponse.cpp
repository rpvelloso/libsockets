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

#include <unordered_map>

#include "HTTPResponse.h"

namespace idx {

static std::unordered_map<int, std::string> replyStr = {
	{200, "OK"},
	{404, "Not Found."},
	{500, "Internal Server Error."}
};

HTTPResponse::HTTPResponse() {

}

void HTTPResponse::appendBody(const std::string& b) {
	body.append(b.begin(), b.end());
	body.push_back('\n');
}

const std::string& HTTPResponse::getBody() const {
	return body;
}

HTTPReply HTTPResponse::getReply() const {
	return reply;
}

void HTTPResponse::setReply(HTTPReply r) {
	reply = r;
}

void HTTPResponse::buildHeaders() {
	headers.emplace_back(
		httpVersion + " " +
		std::to_string((int)reply) + " " +
		replyStr[(int)reply]);

	headers.emplace_back("Content-length: " + std::to_string(body.size()));
	headers.emplace_back("Content-type: " + contentType);
	headers.emplace_back("Connection: close");
}

const std::vector<std::string>& HTTPResponse::getHeaders() const {
	return headers;
}

const std::string& HTTPResponse::getHTTPVersion() const {
	return httpVersion;
}

void HTTPResponse::setHTTPVersion(const std::string& httpVersion) {
	this->httpVersion = httpVersion;
}

const std::string& HTTPResponse::getContentType() const {
	return contentType;
}

void HTTPResponse::setContentType(const std::string& contentType) {
	this->contentType = contentType;
}

} /* namespace idx */
