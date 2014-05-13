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

// copiado da wikipedia!
template <class T>
unsigned int edit_distance( T &s1,  T &s2) {
	const size_t len1 = s1.size(), len2 = s2.size();
	vector<vector<unsigned int> > d(len1 + 1, vector<unsigned int>(len2 + 1));

	d[0][0] = 0;
	for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
	for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

	for(unsigned int i = 1; i <= len1; ++i) {
		for(unsigned int j = 1; j <= len2; ++j) {
			d[i][j] = min(min(d[i - 1][j] + 1,d[i][j - 1] + 1),
					d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) );
		}
	}

	for(unsigned int i = 1; i <= len1; ++i) {
		for(unsigned int j = 1; j <= len2; ++j) {
			cout << d[i][j] << "\t";
		}
		cout << endl;
	}

	unsigned int i=len1;
	unsigned int j=len2;
	T s11,s22;
	while ((i>0) && (j>0)) {
		if ((d[i-1][j-1] <= d[i-1][j]) && (d[i-1][j-1] <= d[i][j-1])) {
			s11 = s1[i-1] + s11;
			s22 = s2[j-1] + s22;
			i--;
			j--;
		} else if (d[i-1][j] <= d[i][j-1]) {
			s11 = s1[i-1] + s11;
			s22 = (typename T::value_type)(0) + s22;
			i--;
		} else {
			s11 = (typename T::value_type)(0) + s11;
			s22 = s2[j-1] + s22;
			j--;
		}
	}

	s1=s11;
	s2=s22;
	return d[len1][len2];
}

#endif /* MISC_H_ */
