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

#include "WindowsFileSystem.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include <memory>
#include <time.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>
#include <windows.h>
#include <fcntl.h>
#include <wincred.h>

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

std::string WindowsFileSystem::path2Windows(const std::string &path) {
	auto result = "c:\\" + path;
	std::transform(result.begin(), result.end(), result.begin(), [](char c){
		if (c == '/')
			return '\\';
		return c;
	});
	return result;
}

std::pair<std::string, std::string> WindowsFileSystem::fileOwner(const std::string &filename) {
	auto winPath = path2Windows(filename);
	SID_NAME_USE SIDNameUse;
	DWORD len, domainLen;
	std::unique_ptr<char[]> nameBuf(new char[512]);
	std::unique_ptr<char[]> domainBuf(new char[512]);
	std::pair<std::string, std::string> result;

	GetFileSecurity(
		winPath.c_str(),
		OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,
		nullptr, 0, &len);

	std::unique_ptr<char[]> securityDescriptor(new char[len]);

	auto ret = GetFileSecurity(
		winPath.c_str(),
		OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,
		securityDescriptor.get(),
		len,
		&len);

	if (ret) {
		PSID ownerID, groupID;
		WINBOOL flag;

		GetSecurityDescriptorOwner(securityDescriptor.get(), &ownerID, &flag);
		GetSecurityDescriptorGroup(securityDescriptor.get(), &groupID, &flag);

		len = 512;
		domainLen = 512;
		ret = LookupAccountSid(
			nullptr,
			ownerID,
			nameBuf.get(),
			&len,
			domainBuf.get(),
			&domainLen,
			&SIDNameUse);
		result.first = std::string(nameBuf.get());

		len = 512;
		domainLen = 512;
		ret = LookupAccountSid(
			nullptr,
			groupID,
			nameBuf.get(),
			&len,
			domainBuf.get(),
			&domainLen,
			&SIDNameUse);
		result.second = std::string(nameBuf.get());
	}

	return result;
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

				auto owner = fileOwner(path + "/" + std::string(de->d_name));
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
