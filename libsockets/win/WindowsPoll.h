/*
 * WindowsPoll.h
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#ifndef WIN_WINDOWSPOLL_H_
#define WIN_WINDOWSPOLL_H_

#include "Poll.h"

namespace socks {

class WindowsPoll: public Poll {
public:
	WindowsPoll();
	virtual ~WindowsPoll();
	std::vector<pollTuple> pollClients(ClientListType &clients) override;
};

}
#endif /* WIN_WINDOWSPOLL_H_ */
