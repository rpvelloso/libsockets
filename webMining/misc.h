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
#include <cmath>

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

#define SPACE T(1,'\0')

template <class T>
unsigned int edit_distance( T &s1,  T &s2, bool align, vector<unsigned int> *spaces) {
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

	if (align) {
		unsigned int i=len1;
		unsigned int j=len2;
		T s11,s22;
		while ((i>0) && (j>0)) {
			if ((d[i-1][j-1] <= d[i-1][j]) && (d[i-1][j-1] <= d[i][j-1])) {
				s11 = s1[i-1] + s11;
				s22 = s2[j-1] + s22;
				i--;
				j--;
			} else if (d[i-1][j] < d[i][j-1]) {
				s11 = s1[i-1] + s11;
				s22 = SPACE + s22;
				i--;
			} else {
				spaces->push_back(i-1);
				s11 = SPACE + s11;
				s22 = s2[j-1] + s22;
				j--;
			}
		}
		s1=s11;
		s2=s22;
	}

	return d[len1][len2];
}

template <class T>
void centerStar(vector<T> &M) {
	size_t len = M.size();
	vector<vector<unsigned int> > d(len,vector<unsigned int>(len));
	//unsigned int d[len][len];
	unsigned int minscore=INFINITY,center;

	// find the center string
	unsigned int score;
	for (size_t i=0;i<len;i++) {
		d[i][i]=0;
		score = 0;
		for (size_t j=i+1;j<len;j++) {
			d[i][j] = edit_distance(M[i],M[j],false,NULL);
			d[j][i] = d[i][j];
			score += d[i][j];
		}
		for (size_t j=0;j<=i;j++) score += d[i][j];
		if (score < minscore) {
			minscore = score;
			center = i;
		}
	}

	// align
	for (size_t i=0; i<M.size();i++) {
		if (i!=center) {
			vector<unsigned int> spaces;

			edit_distance(M[center],M[i],true,&spaces);
			if (spaces.size()) {
				for (size_t j=i;j>0;j--) {
					if ((j-1)!=center) {
						for (size_t k=0;k<spaces.size();k++) {
							M[j-1].insert(spaces[k],SPACE);
						}
					}
				}
			}
		}
	}

	for (size_t i=0;i<M.size();i++) {
		for (size_t j=0;j<M[i].size();j++)
			cerr << M[i][j] << ";";
		cerr << endl;
	}
}

template <class T>
float linearRegression(T s) {
	float delta,x,y,xy,x2,sx=0,sy=0,sxy=0,sx2=0,n=s.size();
	float a,b,e=0;

	for (long int i=0;i<n;i++) {
		y = s[i];
		x = i;
		xy = x*y;
		x2 = x*x;

		sx += x;
		sy += y;
		sxy += xy;
		sx2 += x2;
	}

	delta = (n*sx2)-(sx*sx);
	a = (float)((float)((n*sxy)-(sy*sx))/(float)delta);
	b = (float)((float)((sx2*sy)-(sx*sxy))/(float)delta);


	for (long int i=0;i<n;i++) {
		float ee;
		ee = abs(s[i] - (a*i + b));
		e += ee*ee;
	}
	e /= n;

	return abs(a);
}

#endif /* MISC_H_ */
