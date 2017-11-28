/*
 * PAM.h
 *
 *  Created on: 28 de nov de 2017
 *      Author: roberto
 */

#ifndef LINUX_AUTH_PAM_H_
#define LINUX_AUTH_PAM_H_

#include <string>

class PAM {
public:
	PAM();
	static bool auth(
		const std::string &service,
		const std::string &username,
		const std::string &password);
};

#endif /* LINUX_AUTH_PAM_H_ */
