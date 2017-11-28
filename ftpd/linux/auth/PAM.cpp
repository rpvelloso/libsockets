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

#include "PAM.h"

int convFunction(
	int num_msg,
	const struct pam_message **msg,
	struct pam_response **resp,
	void *appdata_ptr) {

	auto reply = (struct pam_response *)calloc(
		num_msg,
		sizeof(struct pam_response)); // this memory is free'd by PAM

	for (int i = 0; i < num_msg; ++i) {
		reply[i].resp = nullptr;
		reply[i].resp_retcode = 0;
	}
	reply[0].resp = (char *)appdata_ptr; // passwordPtr's addr

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

PAM::PAM() {
}

bool PAM::auth(
	const std::string &service,
	const std::string &username,
	const std::string &password) {

	pam_handle_t *pamHandle = NULL;
	auto passwordPtr = strdup(password.c_str()); // this memory is free'd by PAM

	struct pam_conv conv = { convFunction, passwordPtr };
	auto res = pam_start(service.c_str(), username.c_str(), &conv, &pamHandle);
	if (res == PAM_SUCCESS) {
		PAMGuard pamGuard(pamHandle, res);
		res = pam_authenticate(pamHandle, PAM_SILENT);
		if (res == PAM_SUCCESS)
			res = pam_acct_mgmt(pamHandle, PAM_SILENT);
	}
	return res == PAM_SUCCESS;
}
