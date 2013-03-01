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

#include <map>
#include <sstream>

#define lowerCase(s) std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::tolower)

string &trim(string &);
string stringTok(string &, string);

template <class K, class V>
int multimap_pair_search(multimap<K,V> &mm, K k, V v) {

		pair<typename multimap<K,V>::iterator, typename multimap<K,V>::iterator> range;
		typename multimap<K,V>::iterator i;

		range = mm.equal_range(k);
		for (i=range.first; i!=range.second; i++) {
			if ((*i).second == v) return 0;
		}
		return -1;
}

template<typename T>
string to_string(T val) {
	stringstream ss;

	ss << val;
	return ss.str();
}

#endif /* MISC_H_ */
