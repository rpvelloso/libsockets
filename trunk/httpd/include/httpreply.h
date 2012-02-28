/*
    Copyright 2011 Roberto Panerai Velloso.

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

#ifndef HTTPREPLY_H_
#define HTTPREPLY_H_

#define CRLF "\r\n"

#define REPLY_403 \
	"<HEAD><TITLE>Forbidden</TITLE></HEAD>" CRLF \
	"<H1>Forbidden</H1> You don\'t have permission to access " + uri + " on this server."

#define REPLY_404 \
	"<HEAD><TITLE>Not Found</TITLE></HEAD>" CRLF \
	"<H1>Not Found</H1> The requested object " + uri + " does not exist on this server."

#define REPLY_500 \
	"<HTML>" CRLF \
	"<HEAD><TITLE>Internal Server Error</TITLE></HEAD>" CRLF \
	"<BODY><H1>Internal Server Error</H1>The server encountered an unexpected"\
	" condition which prevented it from fulfilling the request.</BODY>" CRLF \
	"</HTML>"

#define REPLY_501 \
	"<HTML>" CRLF \
	"<HEAD><TITLE>Not Implemented</TITLE></HEAD>" CRLF \
	"<BODY><H1>Not Implemented</H1>This server does not implement the requested method.</BODY>" CRLF \
	"</HTML>"

#endif /* HTTPREPLY_H_ */
