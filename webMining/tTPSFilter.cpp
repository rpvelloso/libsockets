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

#include <queue>
#include "tTPSFilter.h"
#include "misc.h"

tTPSFilter::tTPSFilter(tDOM *d) {
	count = pathCount = 0;
	nodeSequence.clear();
	dom = d;
}

tTPSFilter::~tTPSFilter() {
}

bool tTPSFilter::prune(tNode *n) {
	list<tNode *> remove;

	if (n == NULL) return false;

	for (auto i=n->nodes.begin();i!=n->nodes.end();i++) {
		if (prune(*i)) remove.push_back(*i);
	}

	for (auto i=remove.begin();i!=remove.end();i++) {
		n->size -= (*i)->size;
		n->nodes.remove(*i);
		count--;
	}

	if (find(nodeSequence.begin(),nodeSequence.end(),n) == nodeSequence.end() &&
		n->nodes.empty()) {
		return true;
	}
	return false;
}

long int tTPSFilter::searchRegion(wstring s) {
	set<int> alphabet,filteredAlphabet,regionAlphabet,intersect;
	map<int,int> currentSymbolCount,symbolCount,thresholds;
	bool regionFound=false;
	size_t border=0;


	// compute symbol frequency
	for (size_t i=0;i<s.size();i++) {
		if (alphabet.find(s[i]) == alphabet.end()) {
			symbolCount[s[i]]=0;
			alphabet.insert(s[i]);
		}
		symbolCount[s[i]]++;
	}

	// create sorted list of frequency thresholds
	for (auto i=symbolCount.begin();i!=symbolCount.end();i++)
		thresholds[(*i).second] = (*i).first;
	auto threshold = thresholds.begin();

	while (!regionFound && (threshold != thresholds.end())) {
		// filter alphabet
		filteredAlphabet.clear();
		for (auto j=symbolCount.begin();j!=symbolCount.end();j++) {
			if ((*j).second > (*threshold).first) filteredAlphabet.insert((*j).first);
		}
		if (filteredAlphabet.size() < 2) break;
		threshold++;

		regionAlphabet.clear();
		currentSymbolCount = symbolCount;
		for (size_t i=0;i<s.size();i++) {
			if (filteredAlphabet.find(s[i]) != filteredAlphabet.end()) {
				regionAlphabet.insert(s[i]);
				currentSymbolCount[s[i]]--;
				if (currentSymbolCount[s[i]]==0) {
					filteredAlphabet.erase(s[i]);
					set_intersection(filteredAlphabet.begin(),filteredAlphabet.end(),regionAlphabet.begin(),regionAlphabet.end(),inserter(intersect,intersect.begin()));

					if (intersect.empty()) {
						if (!filteredAlphabet.empty()) {
							regionFound = true;
							border=i;
						}
						break;
					}
					intersect.clear();
				}
			}
		}
	}

	if (regionFound) {
		if (border <= s.size()/2) {
			border++;
			s = s.substr(border,s.size());
		} else {
			s = s.substr(0,border);
			border = 0;
		}
		tagPathSequence = s;
		border += searchRegion(s);
	}
	return border;
}

void tTPSFilter::buildTagPath(string s, tNode *n, bool print, bool css, bool fp) {
	auto i = n->nodes.begin();
	string tagStyle,tagClass;

	if (s == "") {
		pathCount = 0;
		tagPathMap.clear();
		tagPathSequence.clear();
		nodeSequence.clear();
	}

	tagStyle = n->getAttribute("style");
	tagClass = n->getAttribute("class");
	if (tagStyle != "") tagStyle = " " + tagStyle;
	if (tagClass != "") tagClass = " " + tagClass;
	if (css) s = s + "/" + n->getTagName() + tagClass + tagStyle;
	else s = s + "/" + n->getTagName();

	if (tagPathMap.find(s) == tagPathMap.end()) {
		pathCount++;
		tagPathMap[s] = pathCount;
	}
	tagPathSequence = tagPathSequence + wchar_t(tagPathMap[s]);
	nodeSequence.push_back(n);

	if (print) {
		cout << tagPathMap[s];
		if (fp) cout << ":\t" << s;
		cout << endl;
	}

	if (!(n->nodes.size())) return;

	for (;i!=n->nodes.end();i++)
		buildTagPath(s,*i,print,css,fp);
}

map<long int, long int> tTPSFilter::tagPathSequenceFilter(bool css) {
	wstring originalTPS;
	vector<tNode *> originalNodeSequence;
	queue<pair<wstring,long int>> seqQueue;
	vector<long int> start;
	map<long int,long int> region;
	size_t originalTPSsize;
	long int sizeThreshold;

	buildTagPath("",dom->getBody(),false,css,false);
	originalTPS = tagPathSequence;
	originalNodeSequence = nodeSequence;
	originalTPSsize = originalTPS.size();
	sizeThreshold = (originalTPSsize*10)/100; // 10% page size

	seqQueue.push(make_pair(originalTPS,0)); // insert first sequence in queue for processing

	while (seqQueue.size()) {
		long int len,off,rlen,pos;
		wstring tps;

		auto s = seqQueue.front();

		seqQueue.pop();

		tps = s.first;
		len = tps.size();
		off = s.second;
		pos = searchRegion(tps);
		rlen = tagPathSequence.size();

		if (len > rlen) {
			if (pos > 0)
				seqQueue.push(make_pair(tps.substr(0,pos),off));
			if ((len-pos-rlen) > 0)
				seqQueue.push(make_pair(tps.substr(pos+rlen),off+pos+rlen));
			if (rlen > sizeThreshold)
				region[off+pos]=rlen;
		}
	}

	if (region.size()) {
		auto r=region.begin();
		if ((*r).first > 0) region[0] = (*r).first;
	} else {
		region[0]=originalTPSsize;
	}
	// select structured regions
	float angCoeffThreshold=0.3;

	map<long int, long int> structured;
	for (auto i=region.begin();i!=region.end();i++) {
		float a = linearRegression(originalTPS.substr((*i).first,(*i).second));

		cerr << "size: " << (*i).second << " ang.coeff.: " << a << endl;

		if (a < angCoeffThreshold)
			structured.insert(*i);
	}
	region.clear();
	region = structured;

	tagPathSequence = originalTPS;
	nodeSequence = originalNodeSequence;

	return region;
}

void tTPSFilter::DRDE(bool css, float st) {
	wstring originalTPS;
	vector<tNode *> originalNodeSequence;
	vector<unsigned int> recpos;
	vector<wstring> m;
	map<long int, long int> region;

	dataRegions.clear();

	region=tagPathSequenceFilter(css); // locate main content regions
	originalTPS = tagPathSequence;
	originalNodeSequence = nodeSequence;

	for (auto i=region.begin();i!=region.end();i++) {
		auto firstNode = originalNodeSequence.begin()+(*i).first;
		auto lastNode = firstNode + (*i).second;

		nodeSequence.assign(firstNode,lastNode);
		tagPathSequence = originalTPS.substr((*i).first,(*i).second);
		m.clear();
		recpos.clear();

		cerr << "TPS: " << endl;
		for (size_t i=0;i<tagPathSequence.size();i++)
			cerr << tagPathSequence[i] << " ";
		cerr << endl;

		// identify the start position of each record
		recpos = locateRecords(tagPathSequence,st);

		// create a sequence for each record found
		int prev=-1;
		for (size_t i=0;i<recpos.size();i++) {
			if (prev==-1) prev=recpos[i];
			else {
				m.push_back(tagPathSequence.substr(prev,recpos[i]-prev+1));
				prev = recpos[i];
			}
		}
		if (prev != -1)
			m.push_back(tagPathSequence.substr(prev,tagPathSequence.size()-prev+1));


		// align the records (one alternative to 'center star' algorithm is ClustalW)
		centerStar(m);

		// and extracts them
		if (m.size()) onDataRecordFound(m,recpos);
	}
}

vector<unsigned int> tTPSFilter::locateRecords(wstring s, float st) {
	vector<int> d(s.size()-1);
	map<int, vector<int> > diffMap;
	map<int, int> TPMap;
	vector<unsigned int> recpos;
	int rootTag;
	int tagCount=0;
	size_t gap=0;
	size_t interval=0xffffffff;

	/* compute sequence's first difference, keeping only the negative values (i.e. keeping only
	 * fast transitions from very high to very low values, L - H = negative difference). The
	 * difference points are stored and processed in ascending order (higher absolute values first).
	 *
	 * the difference is weighted with the inverse TPC value, the lower, the better
	*/
	cerr << "diff: " << endl;
	for (size_t i=1;i<s.size();i++) {
		d[i-1]=(s[i]-s[i-1])*s[i-1];
		if (d[i-1] < 0) {
			cerr << d[i-1]*s[i] << " ";
			diffMap[d[i-1]].push_back(i);
		} else cerr << 0 << " ";
	}
	cerr << endl;

	// process lowest values until the gap between points achieve enough sequence coverage
	int l=(*(diffMap.begin())).second[0];
	int r=l;
	for (auto i=diffMap.begin();i!=diffMap.end();i++) {

		for (size_t j=0; j<(*i).second.size();j++) {
			if ((*i).second[j]<l) l = (*i).second[j];
			if ((*i).second[j]>r) r = (*i).second[j];
			TPMap[s[(*i).second[j]]]++;
			cerr << "TPS[" << (*i).second[j] << "] = " << s[(*i).second[j]] << endl;
			if (j>1) {
				size_t itv = abs((*i).second[j]-(*i).second[j-1]);
				if (itv < interval) interval = itv;
			}
		}
		if (interval!=0xffffffff) gap += interval*(*i).second.size();
		if (((float)gap / (float)s.size()) > st) break;
	}

	// find the most frequent tag path code within the lowest difference values
	for (auto i=TPMap.begin();i!=TPMap.end();i++) {
		cerr << (*i).first << " " << (*i).second << endl;
		if ((*i).second > tagCount) {
			tagCount = (*i).second;
			rootTag = (*i).first;
		}
	}

	// find the beginning of each record, using the tag path code found before
	for (size_t i=0;i<s.size();i++) {
		if (s[i] == rootTag) {
			cerr << "root: " << i << " " << nodeSequence[i]->tagName << " : " << nodeSequence[i]->text << endl;
			recpos.push_back(i);
		}
	}

	return recpos;
}

void tTPSFilter::onDataRecordFound(vector<wstring> &m, vector<unsigned int> &recpos) {
	if ((m.size() == 0) || (recpos.size() == 0)) return;

	size_t rows=m.size(),cols=m[0].size();
	vector<vector<tNode *> > table(rows, vector<tNode *>(cols));

	for (size_t i=0;i<rows;i++) {
		for (size_t j=0,k=0;j<cols;j++) {
			if (m[i][j] != 0) {
				table[i][j] = nodeSequence[recpos[i]+k];
				k++;
			} else table[i][j]=NULL;
		}
	}

	dataRegions.push_back(table);
}
