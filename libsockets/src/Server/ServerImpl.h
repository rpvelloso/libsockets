/*
 * ServerImpl.h
 *
 *  Created on: 11 de out de 2017
 *      Author: rvelloso
 */

#ifndef SRC_SERVER_SERVERIMPL_H_
#define SRC_SERVER_SERVERIMPL_H_

namespace socks {

class ServerImpl {
public:
	ServerImpl() {};
	virtual ~ServerImpl() {};
	virtual void listen() = 0;
};

}
#endif /* SRC_SERVER_SERVERIMPL_H_ */
