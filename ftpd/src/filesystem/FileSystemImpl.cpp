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

#include "FileSystemImpl.h"

#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <algorithm>

FileSystemImpl::FileSystemImpl() {
}

FileSystemImpl::~FileSystemImpl() {
}

#define DIR_DELIM_STR "/"
#define DIR_DELIM_CHAR '/'

std::string FileSystemImpl::resolvePath(const std::string &cwd, const std::string &path) {
	std::list<std::string> pathStack;
	std::string pathToken;

	std::stringstream ss(
			path.front() != DIR_DELIM_CHAR?
					cwd + DIR_DELIM_STR + path: // relative path
					path); // absolute path

	while (std::getline(ss, pathToken, DIR_DELIM_CHAR)) {
		if (!pathToken.empty() && pathToken != "." && pathToken != "..") {
			pathStack.push_back(pathToken);
		} else if ((pathToken == "..") && (!pathStack.empty())) {
			pathStack.pop_back();
		}
	}

	std::string resolvedPath = "";
	for (auto i=pathStack.begin(); i != pathStack.end(); i++) {
		resolvedPath = resolvedPath + DIR_DELIM_STR + *i;
	}
	if (resolvedPath == "") resolvedPath = DIR_DELIM_STR;

	return resolvedPath;
}

std::string FileSystemImpl::dateToString(struct tm& t) {
	static std::array<std::string, 12> month =
	{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	std::stringstream dateStr;

	dateStr << month[t.tm_mon] << " " <<
		std::setw(2) << std::to_string(t.tm_mday) << " " <<
		std::to_string(t.tm_year+1900);

	return dateStr.str();
}

