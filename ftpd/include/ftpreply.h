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
 **/
#ifndef FTPREPLY_H_
#define FTPREPLY_H_

#define R150		"150 Opening data connection.\n"
#define R200		"200 Command successful.\n"
#define R200_MODE	"200 Mode set to S.\n"
#define R200_TYPE	"200 Type set to "
#define R200_STRU	"200 Structure set to F.\n"
#define R211		"211 Help not available.\n"
#define R213		"213 "
#define R215		"215 UNIX Type: L8.\n"
#define R220		"220 Server ready.\n"
#define R221		"221 Connection ended.\n"
#define R226		"226 Transfer complete.\n"
#define R227		"227 Entering Passive Mode ("
#define R230		"230 User logged in.\n"
#define R250		"250 Command successful.\n"
#define R257		"257 Directory successfully created.\n"
#define R257_PWD	"257 \""
#define R331		"331 Password required.\n"
#define R350		"350 Restarted position accepted.\n"
#define R350_RNFR	"350 Rename from accepted.\n"
#define R421		"421 Server full.\n"
#define R425		"425 Can't open data connection.\n"
#define R426		"426 Connection closed; transfer aborted.\n"
#define R500		"500 Invalid command.\n"
#define R501		"501 Invalid arguments.\n"
#define R503		"503 User name required.\n"
#define R503_RNTO	"503 Bad sequence of commands.\n"
#define R504		"504 Unsupported parameter.\n"
#define R530		"530 Not logged in.\n"
#define R550_MKD	"550 Could not create directory.\n"
#define R550_SIZE1	"550 Not a regular file.\n"
#define R550_SIZE2	"550 No such file or directory.\n"
#define R550_CWD	"550 Can't change directory.\n"
#define R550_RETR	"550 Can't send file.\n"
#define R550_STOR	"550 Can't receive file.\n"
#define R550_DELE	"550 Can't delete file.\n"
#define R550_RNTO	"550 Can't rename file.\n"

#endif
