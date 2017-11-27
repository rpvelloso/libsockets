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

#include "LinuxFileSystem.h"

#include <iostream>
#include <iomanip>

#include <memory>
#include <time.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

FileSystem fs(new LinuxFileSystem());

LinuxFileSystem::LinuxFileSystem() {
}

LinuxFileSystem::~LinuxFileSystem() {
}

bool LinuxFileSystem::changeWorkingDir(const std::string& dirname) {
	DIR *d;
	if ((d = opendir(dirname.c_str()))) {
		std::unique_ptr<DIR, decltype(&closedir)> dirGuard(d, closedir);
		return true;
	}
	return false;
}

bool LinuxFileSystem::makeDir(const std::string& dirname) {
	if (mkdir(dirname.c_str(), 0700) == 0)
		return true;
	return false;
}

bool LinuxFileSystem::size(const std::string& filename, size_t &sz) {
	struct stat st;

	if (stat(filename.c_str(),&st) != -1) {
		if (S_ISREG(st.st_mode)) {
			sz = st.st_size;
			return true;
		}
	}
	return false;
}

bool LinuxFileSystem::deleteFile(const std::string& filename) {
	if (remove(filename.c_str()) != -1)
		return true;
	return false;
}

bool LinuxFileSystem::renameFile(
		const std::string& from,
		const std::string& to) {

	if (rename(from.c_str(),to.c_str()) != -1)
		return true;
	return false;

}

std::pair<std::string, std::string> LinuxFileSystem::fileOwner(uid_t uid) {
	std::pair<std::string, std::string> result;
	struct passwd pwd, *res;
	std::unique_ptr<char[]> buf(new char[512]);

	getpwuid_r(uid, &pwd,buf.get(), 512, &res);
	if (res != nullptr) {
		result.first = std::string(pwd.pw_name);
		struct group grp, *grpres;

		getgrgid_r(pwd.pw_gid, &grp, buf.get(), 512, &grpres);
		if (grpres != nullptr)
			result.second = std::string(grp.gr_name);
	}

	return result;
}

std::vector<ListTuple> LinuxFileSystem::list(const std::string &path) {
	DIR *d;
	std::vector<ListTuple> fileList;
	std::string sep = "";

	if (path.back() != '/')
		sep = "/";

	if ((d = opendir(path.c_str()))) {
		std::unique_ptr<DIR, decltype(&closedir)> dirGuard(d, closedir);

		auto de = readdir(d);
		while (de) {
			struct stat64 st;
			auto fullpath = path + sep + std::string(de->d_name);

			if (stat64(fullpath.c_str(),&st) != -1) {
				struct tm tm1;

				auto owner = fileOwner(st.st_uid);
				if (owner.first.empty())
					owner.first = "User";
				if (owner.second.empty())
					owner.second = "Group";

				gmtime_r(&st.st_mtime,&tm1);
				std::string dateStr = dateToString(tm1);

				fileList.push_back(std::forward_as_tuple(
					std::string(de->d_name),
					owner.first,
					owner.second,
					st.st_mode,
					st.st_size,
					st.st_nlink,
					dateStr));
			}
			de = readdir(d);
		}
	}
	return fileList;
}
