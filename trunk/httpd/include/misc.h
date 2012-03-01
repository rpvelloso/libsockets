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

#ifndef MISC_H_
#define MISC_H_

#ifdef WIN32
	#define DRIVE "C:"
	#define PHP_BIN DRIVE "\\Progra~1\\PHP\\php-cgi.exe "
	#define CMD_BIN DRIVE "\\Windows\\System32\\cmd.exe /c "
#else
	#define PHP_BIN "/usr/bin/php-cgi"
#endif

#endif /* MISC_H_ */
