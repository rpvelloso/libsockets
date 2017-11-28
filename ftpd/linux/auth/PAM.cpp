/*
 * PAM.cpp
 *
 *  Created on: 28 de nov de 2017
 *      Author: roberto
 */

#include <memory>
#include <algorithm>
#include <security/pam_appl.h>

#include "PAM.h"

struct pam_response *reply;

static int null_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
	*resp = reply;
	return PAM_SUCCESS;
}

static struct pam_conv conv = { null_conv, NULL };


PAM::PAM() {

}

bool PAM::auth(
	const std::string &service,
	const std::string &username,
	const std::string &password) {

	pam_handle_t *pamh = NULL;
	auto res = pam_start(service.c_str(), username.c_str(), &conv, &pamh);

	if (res == PAM_SUCCESS) {
		std::unique_ptr<char[]> passwordPtr(new char[password.size()]);
		size_t pos = 0;
		std::for_each(password.begin(), password.end(), [&pos, &passwordPtr](char c){passwordPtr[pos++]=c;});
		/*std::unique_ptr<struct pam_response, decltype(&free)> replyPtr(
				new struct pam_response
				(struct pam_response *)malloc(sizeof(struct pam_response)),
				free);*/
		reply = (struct pam_response *)malloc(sizeof(struct pam_response));//replyPtr.get();

		reply[0].resp = passwordPtr.get();//const_cast<char *>(password.c_str());
		reply[0].resp_retcode = 0;

		res = pam_authenticate(pamh, 0);

		pam_end(pamh, PAM_SUCCESS);
	}

	return res == PAM_SUCCESS;
}
