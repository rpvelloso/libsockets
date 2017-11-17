/*
 * WindowsFileSystem.cpp
 *
 *  Created on: 13 de nov de 2017
 *      Author: rvelloso
 */

#include "WindowsFileSystem.h"

#include <iostream>
#include <iomanip>

#include <memory>
#include <time.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

FileSystem fs(new WindowsFileSystem());

WindowsFileSystem::WindowsFileSystem() {
}

WindowsFileSystem::~WindowsFileSystem() {
}

bool WindowsFileSystem::changeWorkingDir(const std::string& dirname) {
	DIR *d;
	if ((d = opendir(dirname.c_str()))) {
		std::unique_ptr<DIR, decltype(&closedir)> dirGuard(d, closedir);
		return true;
	}
	return false;
}

bool WindowsFileSystem::makeDir(const std::string& dirname) {
	if (mkdir(dirname.c_str()) == 0)
		return true;
	return false;
}

bool WindowsFileSystem::size(const std::string& filename, size_t &sz) {
	struct stat st;

	if (stat(filename.c_str(),&st) != -1) {
		if (S_ISREG(st.st_mode)) {
			sz = st.st_size;
			return true;
		}
	}
	return false;
}

bool WindowsFileSystem::deleteFile(const std::string& filename) {
	if (remove(filename.c_str()) != -1)
		return true;
	return false;
}

bool WindowsFileSystem::renameFile(
		const std::string& from,
		const std::string& to) {

	if (rename(from.c_str(),to.c_str()) != -1)
		return true;
	return false;

}

std::vector<ListTuple> WindowsFileSystem::list(const std::string &path) {
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

			if (stat64(std::string(path + sep + std::string(de->d_name)).c_str(),&st) != -1) {
				struct tm tm1;

				gmtime_r(&st.st_mtime,&tm1);
				std::string dateStr = dateToString(tm1);

				fileList.push_back(std::forward_as_tuple(
					std::string(de->d_name),
					"user",
					"group",
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
