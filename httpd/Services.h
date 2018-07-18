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

#ifndef SERVICES_H_
#define SERVICES_H_

#include <vector>
#include <functional>
#include <unordered_map>


#include "json.hpp"

using json = nlohmann::json;

namespace idx {

enum class Method {
	Unknown = -1,
	GET = 0,
	POST = 1
};

inline Method methodStr2Enum(const std::string& methodStr) {
	if (methodStr == "GET")
		return Method::GET;
	if (methodStr == "POST")
		return Method::POST;

	return Method::Unknown;
}

class HTTPClient;

using ServiceFunction = std::function<void(idx::HTTPClient &context, json &input)>;
using Services = std::vector<std::unordered_map<std::string, ServiceFunction> >;

}
#endif /* SERVICES_H_ */
