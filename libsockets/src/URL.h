/*
 * URL.h
 *
 *  Created on: 13 de jun de 2017
 *      Author: Benutzer
 */

#ifndef SRC_URL_H_
#define SRC_URL_H_

#include <regex>

namespace socks {

class URL {
public:
	URL(const std::string &url) {
		std::regex regex(
			"(([[:alpha:]]+)://)?" // protocol
			"(([^ /:@]+)(:([^ /:@]+))?@)?" // user:pass@
			"([^/ :\\x3f]+)" // host
			"(:([1-9][0-9]+))?" // port
			"(((/[^ #?]*)" // path
			"(\\x3f?([^ #]*)#?([^ ]*)|$))|$)" // query
		);
		std::smatch matchs;

		if (std::regex_match(url, matchs, regex)) {
			/*int i = 0;
			for (auto m:matchs)
				std::cout << i++ << ": " << m << std::endl;*/

			protocol = matchs[2];
			user = matchs[4];
			password = matchs[6];
			host = matchs[7];
			port = matchs[9];
			path = matchs[12];
			query = matchs[14];
		} else
			valid = false;
	};

	virtual ~URL() {};

	bool isValid() {return valid;};
	std::string getProtocol() const {return protocol;}
	std::string getUser() const {return user;}
	std::string getPassword() const {return password;}
	std::string getHost() const {return host;}
	std::string getPort() const {return port;}
	std::string getPath() const {return path;}
	std::string getQuery() const {return query;}
private:
	bool valid = true;
	std::string protocol, user, password, host, port, path, query;
};

}
#endif /* SRC_URL_H_ */
