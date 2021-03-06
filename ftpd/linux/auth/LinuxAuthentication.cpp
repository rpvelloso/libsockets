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

#include <memory>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>

#include "auth/Authentication.h"
#include "auth/LinuxAuthentication.h"

Authentication authService(new LinuxAuthentication());

/*template<class Type>
using UniqueMallocPtr = std::unique_ptr<Type, decltype(&free)>;

template<typename Type>
static UniqueMallocPtr<Type> makeUniqueMallocPtr(Type *data) {
	return UniqueMallocPtr<Type>(data, free);
}*/

int conversationFunction(
	int num_msg,
	const struct pam_message **msg,
	struct pam_response **resp,
	void *appdata_ptr) {

	/*auto replyPtr = makeUniqueMallocPtr((struct pam_response *)calloc(num_msg, sizeof(struct pam_response)));
	auto passwordPtr = makeUniqueMallocPtr(strdup((const char *)appdata_ptr));
	auto reply = replyPtr.get();*/

	auto reply = (struct pam_response *)calloc(num_msg, sizeof(struct pam_response)); // this memory is free'd by PAM

	for (int i = 0; i < num_msg; ++i) {
		reply[i].resp = nullptr;
		reply[i].resp_retcode = 0;
	}

	/*reply[0].resp = passwordPtr.release();
	*resp = replyPtr.release();*/ // this memory is free'd by PAM

	reply[0].resp = strdup((const char *)appdata_ptr); // this memory is free'd by PAM
	*resp = reply;
	return PAM_SUCCESS;
}

class PAMGuard {
public:
	PAMGuard(pam_handle_t *pamHandle, int &res) :
		pamHandle(pamHandle), res(res) {};
	~PAMGuard() {
		pam_end(pamHandle, res);
	}
private:
	pam_handle_t *pamHandle;
	int &res;
};

LinuxAuthentication::LinuxAuthentication() {
}

LinuxAuthentication::~LinuxAuthentication() {
}

bool LinuxAuthentication::authenticate(const std::string& username,
		const std::string& password) {
	return pamAuthentication("remote", username, password);
}

bool LinuxAuthentication::pamAuthentication(
	const std::string &service,
	const std::string &username,
	const std::string &password) {

	pam_handle_t *pamHandle = NULL;

	struct pam_conv conv = {
		conversationFunction,
		const_cast<char *>(password.c_str()) };

	auto res = pam_start(service.c_str(), username.c_str(), &conv, &pamHandle);
	if (res == PAM_SUCCESS) {
		PAMGuard pamGuard(pamHandle, res);
		res = pam_authenticate(pamHandle, PAM_SILENT);
		if (res == PAM_SUCCESS)
			res = pam_acct_mgmt(pamHandle, PAM_SILENT);
	}
	return res == PAM_SUCCESS;
}
