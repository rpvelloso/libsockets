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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <set>
#include <map>
#include "tnode.h"
#include "tdom.h"
#include "misc.h"

using namespace std;

/* --- main --- */
int main(int argc, char *argv[])
{
	tDOM *d = new tDOM();
	tDOM *p = new tDOM();
	fstream patternFile;
	int r;

	//patternFile.open("p.html");
	//p->scan(patternFile);

	r = d->scan(cin);

	d->printDOM();
	//p->printDOM();
	//cout << "TM: " << d->treeMatch(d->root,p->root) << endl;

	/*d->searchTag("a");
	d->searchTag("img");
	d->searchTag("link");*/
	/*d->searchTag("#text");
	d->searchTag("#comment");*/

	delete d;
	//delete p;
	return r;
}
