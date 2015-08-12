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
#include <iostream>
#include <set>
#include <functional>
#include <map>
#include "tTPSFilter.h"
#include "misc.h"
#include "hsfft.h"
//#include "Ckmeans.1d.dp.h"

tTPSFilter::tTPSFilter() : count(0),pathCount(0) {
}

tTPSFilter::~tTPSFilter() {
}

const wstring& tTPSFilter::getTagPathSequence(int dr) {
	if (dr < 0)
		return tagPathSequence;
	else {
		size_t i = dr;

		if (i < regions.size())
			return regions[dr].tps;
		else
			return tagPathSequence;
	}
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

map<int,int> tTPSFilter::symbolFrequency(wstring s, set<int> &alphabet) {
	map<int, int> symbolCount;

	// compute symbol frequency
	for (size_t i=0;i<s.size();i++) {
		if (s[i] != 0) {
			if (alphabet.find(s[i]) == alphabet.end()) {
				symbolCount[s[i]]=0;
				alphabet.insert(s[i]);
			}
			symbolCount[s[i]]++;
		}
	}
	return symbolCount;
}

map<int,int> tTPSFilter::frequencyThresholds(map<int,int> symbolCount) {
	map<int,int> thresholds;

	// create sorted list of frequency thresholds
	for (auto i=symbolCount.begin();i!=symbolCount.end();i++)
		thresholds[(*i).second] = (*i).first;

	return thresholds;
}

long int tTPSFilter::searchRegion(wstring s) {
	set<int> alphabet,filteredAlphabet,regionAlphabet,intersect;
	map<int,int> currentSymbolCount,symbolCount,thresholds;
	bool regionFound=false;
	size_t border=0;


	symbolCount = symbolFrequency(s,alphabet);
	thresholds = frequencyThresholds(symbolCount);
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

#define STYLEATTR_COUNT 8
void tTPSFilter::buildTagPath(string s, tNode *n, bool print, bool css, bool fp) {
	string styleAttr[STYLEATTR_COUNT] = {"style", "class", "bgcolor", "width", "height", "align", "valign", "halign"};
	string tagStyle = "";

	if (s == "") {
		pathCount = 0;
		tagPathMap.clear();
		tagPathSequence.clear();
		nodeSequence.clear();
	}

	for (size_t i=0,j=0;i<STYLEATTR_COUNT;i++) {
		string attr = n->getAttribute(styleAttr[i]);

		if (attr != "") {
			if (j)
				tagStyle = tagStyle + " " + attr;
			else
				tagStyle = " " + attr;
			j++;
		}
	}
	if (css && (tagStyle != "")) s = s + "/" + n->getTagName() + tagStyle;
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

	for (auto i = n->nodes.begin();i!=n->nodes.end();i++)
		buildTagPath(s,*i,print,css,fp);
}

map<long int, tTPSRegion> tTPSFilter::detectStructure(map<long int, tTPSRegion> &r) {
	float angCoeffThreshold=0.17633; // 10 degrees
	long int sizeThreshold = (tagPathSequence.size()*3)/100; // % page size
	map<long int,tTPSRegion> structured;

	for (auto i=r.begin();i!=r.end();i++) {
		(*i).second.lc = linearRegression((*i).second.tps);

		cerr << "size: " << (*i).second.len << " ang.coeff.: " << (*i).second.lc.a << endl;

		if (
			(abs((*i).second.lc.a) < angCoeffThreshold) && // test for structure
			((*i).second.len >= sizeThreshold) // test for size
			)
			structured.insert(*i);
	}
	return structured;
}

map<long int, tTPSRegion> tTPSFilter::tagPathSequenceFilter(tNode *n, bool css) {
	wstring originalTPS;
	vector<tNode *> originalNodeSequence;
	queue<pair<wstring,long int>> seqQueue;
	vector<long int> start;
	int originalTPSsize;
	long int sizeThreshold;

	buildTagPath("",n,false,css,false);
	originalTPS = tagPathSequence;
	originalNodeSequence = nodeSequence;
	originalTPSsize = originalTPS.size();
	sizeThreshold = (originalTPSsize*5)/100; // % page size

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
			if (rlen > sizeThreshold) {
				_regions[off+pos].len=rlen;
				_regions[off+pos].tps = originalTPS.substr(off+pos,rlen);
			}
		}
	}

	if (_regions.size()) {
		auto r=_regions.begin();
		if ((*r).first > sizeThreshold) {
			_regions[0].len = (*r).first;
			_regions[0].tps = originalTPS.substr(0,(*r).first);
		}
	} else {
		_regions[0].len=originalTPSsize;
		_regions[0].tps = originalTPS;
	}

	/*buildTagPath("",n,false,false,false); // rebuild TPS without css to increase periodicity
	for (auto i=_regions.begin();i!=_regions.end();i++) {
		(*i).second.tps = tagPathSequence.substr((*i).second.pos,(*i).second.len);
	}*/

	tagPathSequence = originalTPS;
	nodeSequence = originalNodeSequence;

	return detectStructure(_regions);
}

map<long int, tTPSRegion> tTPSFilter::SRDEFilter(tNode *n, bool css) {
	set<int> alphabet;
	wstring s;
	long int lastRegPos = -1;
	map<long int, tTPSRegion> ret;

	buildTagPath("",n,false,css,false);
	s = tagPathSequence;

	auto symbolCount = symbolFrequency(s,alphabet);
	auto thresholds = frequencyThresholds(symbolCount);
	auto threshold = thresholds.begin();
	threshold++;
	//threshold++;

	do {
		threshold++;

		cerr << "threshold: " << (*threshold).first << " / " << (*(thresholds.rbegin())).first << endl;

		for (size_t i=0;i<s.size();i++)
			if (symbolCount[s[i]] <= (*threshold).first) s[i]=0;

		bool regionOpened=false;
		int regionStart=0;
		int regionEnd=0;

		for (size_t i=0;i<s.size();i++) {
			if (!regionOpened) {
				 if (s[i] != 0) {
					regionOpened = true;
					regionStart = i;
				}
			} else {
				if ((s[i] == 0) || (i == s.size()-1)) {
					tTPSRegion reg;

					regionOpened = false;
					regionEnd = i-1;
					if (i == s.size()-1) regionEnd++;
					reg.pos = regionStart;
					reg.len = regionEnd - regionStart + 1;
					if (reg.len > 3) {
						reg.tps = tagPathSequence.substr(reg.pos,reg.len);

						if (lastRegPos != -1) {
							set<int> palpha,alpha,intersect;

							symbolFrequency(_regions[lastRegPos].tps,palpha);
							symbolFrequency(reg.tps,alpha);

							set_intersection(palpha.begin(),palpha.end(),alpha.begin(),alpha.end(),inserter(intersect,intersect.begin()));

							if (!intersect.empty()) {
								_regions[lastRegPos].len = regionEnd - _regions[lastRegPos].pos + 1;
								_regions[lastRegPos].tps = tagPathSequence.substr(_regions[lastRegPos].pos,_regions[lastRegPos].len);
								//cout << "merge " << _regions[lastRegPos].pos << " " << _regions[lastRegPos].len << endl;
								continue;
							}
						}
						_regions[regionStart] = reg;
						lastRegPos = regionStart;
						//cout << "new   " << _regions[lastRegPos].pos << " " << _regions[lastRegPos].len << endl;
					}
				}
			}
		}

		if (_regions.size() == 0) {
			//_regions.clear();
			_regions[0].content = true;
			_regions[0].len = tagPathSequence.size();
			_regions[0].pos = 0;
			_regions[0].nodeSeq = nodeSequence;
			_regions[0].tps = tagPathSequence;
		}

		/*buildTagPath("",n,false,false,false); // rebuild TPS without CSS to increase periodicity
		for (auto i=_regions.begin();i!=_regions.end();i++) {
			(*i).second.tps = tagPathSequence.substr((*i).second.pos,(*i).second.len);
		}*/
		ret=detectStructure(_regions);
	} while ((ret.size()==0) && (threshold != thresholds.end()));

	return ret;
}

void tTPSFilter::SRDE(tNode *n, bool css) {
	vector<size_t> recpos;
	vector<wstring> m;
	map<long int, tTPSRegion> structured;
	double period;

	_regions.clear();

	structured=SRDEFilter(n,css); // segment page and detects structured regions
	//structured = tagPathSequenceFilter(n,css);

	for (auto i=structured.begin();i!=structured.end();i++) {
		auto firstNode = nodeSequence.begin()+(*i).first;
		auto lastNode = firstNode + (*i).second.len;

		_regions[(*i).first].nodeSeq.assign(firstNode,lastNode);
		m.clear();
		recpos.clear();

		cerr << "TPS: " << endl;
		for (size_t j=0;j<_regions[(*i).first].tps.size();j++)
			cerr << _regions[(*i).first].tps[j] << " ";
		cerr << endl;

		// identify the start position of each record
		recpos = SRDELocateRecords(_regions[(*i).first],period);
		//recpos = LZLocateRecords(_regions[(*i).first],period);

		// consider only leaf nodes when performing field alignment
		auto j = _regions[(*i).first].nodeSeq.begin();
		auto t=_regions[(*i).first].tps.begin();
		size_t k=0;
		while (k < _regions[(*i).first].tps.size()) {
			bool erase = (!IMG((*j)) && !TEXT((*j)) && !LINK((*j)));
			for (size_t w=0;w<recpos.size();w++) {
				if (recpos[w] == k) {
					erase=false;
					break;
				}
			}

			if (erase) {
				j = _regions[(*i).first].nodeSeq.erase(j);
				t = _regions[(*i).first].tps.erase(t);
				for (size_t w=0;w<recpos.size();w++) {
					if (recpos[w] > k) recpos[w]--;
				}
			} else {
				j++;
				t++;
				k++;
			}
		}

		// create a sequence for each record found
		int prev=-1;
		size_t max_size=0;
		for (size_t j=0;j<recpos.size();j++) {
			if (prev==-1) prev=recpos[j];
			else {
				if ((recpos[j]-prev) > 0) {
					m.push_back(_regions[(*i).first].tps.substr(prev,recpos[j]-prev));
					max_size = max(recpos[j]-prev,max_size);
					prev = recpos[j];
				}
			}
		}
		if (prev != -1) {
			if (period > max_size) max_size = period;
			m.push_back(_regions[(*i).first].tps.substr(prev,max_size));
		}

		if (m.size()) {
			// align the records (one alternative to 'center star' algorithm is ClustalW)
			//align(m);
			_regions[(*i).first].score = centerStar(m);

			// and extracts them
			onDataRecordFound(m,recpos,&_regions[(*i).first]);
		}
	}

	// remove regions with only a single record
	for (auto i=structured.begin();i!=structured.end();) {
		if (_regions[(*i).first].records.size() < 2) structured.erase(i++);
		else {
			auto stddev = _regions[(*i).first].stddev;
			auto recCount = _regions[(*i).first].records.size();
			auto recSize = _regions[(*i).first].records[0].size();

			_regions[(*i).first].score = //stddev;
					((min((double)recCount,(double)recSize) /
					max((double)recCount,(double)recSize))) * stddev * ((double)_regions[(*i).first].len / (double)tagPathSequence.size());
					//(double)recCount * (double)recSize * stddev;
			++i;
		}
	}

	if (structured.size()) {
		vector<double> ckmeansScoreInput;
		ClusterResult scoreResult;

		ckmeansScoreInput.push_back(0);
		for (auto i=structured.begin();i!=structured.end();i++) {
			ckmeansScoreInput.push_back(_regions[(*i).first].score);
		}
		scoreResult = kmeans_1d_dp(ckmeansScoreInput,2,2);

		auto j=++(scoreResult.cluster.begin());
		for (auto i=structured.begin();i!=structured.end();i++,j++) {
			_regions[(*i).first].content = (((*j) == 2) || (scoreResult.nClusters < 2));

			// restore the original region's tps
			_regions[(*i).first].tps = tagPathSequence.substr((*i).first,(*i).second.len);
		}

		// -----

		/*ckmeansScoreInput.clear();
		ckmeansScoreInput.push_back(0);
		for (auto i=structured.begin();i!=structured.end();i++) {
			ckmeansScoreInput.push_back(_regions[(*i).first].stddev);
		}
		scoreResult = kmeans_1d_dp(ckmeansScoreInput,2,2);

		j=++(scoreResult.cluster.begin());
		for (auto i=structured.begin();i!=structured.end();i++,j++)
			_regions[(*i).first].content |= (((*j) == 2) || (scoreResult.nClusters < 2));*/
	}

	for (auto i = _regions.begin();i!=_regions.end();) {
		if ((*i).second.records.size() < 2) _regions.erase(i++);
		else ++i;
	}

	regions.clear();

	for (auto i=_regions.begin();i!=_regions.end();i++) {
		(*i).second.pos = (*i).first;
		regions.push_back((*i).second);
	}

    sort(regions.begin(),regions.end(),[](const tTPSRegion &a, const tTPSRegion &b) {
    		return (a.score > b.score);
    });
}

void tTPSFilter::DRDE(tNode *n, bool css, float st) {
	vector<size_t> recpos;
	vector<wstring> m;
	map<long int, tTPSRegion> structured;

	_regions.clear();

	structured=tagPathSequenceFilter(n,css); // locate main content regions

	for (auto i=structured.begin();i!=structured.end();i++) {
		auto firstNode = nodeSequence.begin()+(*i).first;
		auto lastNode = firstNode + (*i).second.len;

		_regions[(*i).first].tps = tagPathSequence.substr((*i).first,(*i).second.len);
		_regions[(*i).first].nodeSeq.assign(firstNode,lastNode);
		m.clear();
		recpos.clear();

		cerr << "TPS: " << endl;
		for (size_t j=0;j<_regions[(*i).first].tps.size();j++)
			cerr << _regions[(*i).first].tps[j] << " ";
		cerr << endl;

		// identify the start position of each record
		recpos = locateRecords(_regions[(*i).first].tps,st);

		// consider only leaf nodes when searching record boundary & performing field alignment
		auto j = _regions[(*i).first].nodeSeq.begin();
		auto t=_regions[(*i).first].tps.begin();
		size_t k=0;
		while (k < _regions[(*i).first].tps.size()) {
			bool erase = ((*j)->tagName!="img" && (*j)->tagName != "#text" && (*j)->tagName!="a");
			for (size_t w=0;w<recpos.size();w++) {
				if (recpos[w] == k) {
					erase=false;
					break;
				}
			}

			if (erase) {
				j = _regions[(*i).first].nodeSeq.erase(j);
				t = _regions[(*i).first].tps.erase(t);
				for (size_t w=0;w<recpos.size();w++) {
					if (recpos[w] > k) recpos[w]--;
				}
			} else {
				j++;
				t++;
				k++;
			}
		}

		// create a sequence for each record found
		int prev=-1;
		size_t max_size=0;
		for (size_t j=0;j<recpos.size();j++) {
			if (prev==-1) prev=recpos[j];
			else {
				m.push_back(_regions[(*i).first].tps.substr(prev,recpos[j]-prev));
				max_size = max(recpos[j]-prev,max_size);
				prev = recpos[j];
			}
		}
		if (prev != -1) {
			if (max_size == 0) max_size = recpos.size();
			m.push_back(_regions[(*i).first].tps.substr(prev,max_size));
		}

		// align the records (one alternative to 'center star' algorithm is ClustalW)
		centerStar(m);

		// and extracts them
		if (m.size()) onDataRecordFound(m,recpos,&_regions[(*i).first]);
	}

	regions.clear();
	for (auto i=_regions.begin();i!=_regions.end();i++) {
		(*i).second.pos = (*i).first;
		regions.push_back((*i).second);
	}
}

vector<size_t> tTPSFilter::SRDELocateRecords(tTPSRegion &region, double &period) {
	wstring s = region.tps;
	vector<double> signal(s.size());
	float avg;
	set<float> candidates;
	vector<size_t> recpos,ret;
	set<int> alphabet;
	map<int,int> reencode;
	double estPeriod,estFreq;
	double maxCode=0,maxScore=0;

	// reencode signal
	symbolFrequency(s,alphabet);
	for (size_t i=0,j=0;i<s.size();i++) {
		if (alphabet.find(s[i])!=alphabet.end()) {
			alphabet.erase(s[i]);
			j++;
			reencode[s[i]]=j;
		}
		//signal[i]=reencode[s[i]];
		signal[i]=s[i];
	}
	avg = mean(signal);

	// remove DC & compute signal's std.dev.
	region.stddev = 0;
	for (size_t i=0;i<s.size();i++) {
		signal[i] = signal[i] - avg;
		region.stddev += signal[i]*signal[i];
		if (signal[i] < 0) candidates.insert(signal[i]);
		if (abs(signal[i]) > maxCode) maxCode = abs(signal[i]);
	}
	region.stddev = sqrt(region.stddev/max((double)1,(double)(s.size()-2)));

	estPeriod = estimatePeriod(signal);
	estFreq = ((double)signal.size() / estPeriod);

	cout << endl;

	for (auto value = candidates.begin(); value != candidates.end(); value ++ ) {
		double stddev,avgsize;

		recpos.clear();
		stddev=0;
		avgsize=0;

		for (size_t i=0;i<s.size();i++) {
			if ((signal[i] == *value))
				recpos.push_back(i);
		}

		if (recpos.size() > 1) {
			for (size_t i=1;i<recpos.size();i++) {
				avgsize += (recpos[i] - recpos[i-1]);
			}
			avgsize /= (float)(recpos.size()-1);

			for (size_t i=1;i<recpos.size();i++) {
				float diff = (float)(recpos[i] - recpos[i-1])-avgsize;
				stddev += (diff*diff);
			}
			stddev = sqrt(stddev/max((float)(recpos.size()-2),(float)1));

			double regionCoverage = min(avgsize*(double)recpos.size()/(double)signal.size(), (double)1);
			double estRegionCoverage = min(estPeriod*estFreq/(double)signal.size(), (double)1);
			double regCoverageRatio = min(regionCoverage,estRegionCoverage)/max(regionCoverage,estRegionCoverage);
			double freqRatio =
					min( (double)recpos.size() ,estFreq) /
					max( (double)recpos.size() ,estFreq);
			if (stddev>1) avgsize /= stddev; // SNR
			double recSizeRatio = min( avgsize, estPeriod )/max( avgsize, estPeriod );
			//recSizeRatio = sqrt(recSizeRatio);
			double tpcRatio = (double)abs(*value)/maxCode; // DNR

			double score = (regCoverageRatio + freqRatio + recSizeRatio + tpcRatio)/(double)4;
			//double score = regionCoverage * recCountRatio * recSizeRatio * tpcRatio;
			if (score > maxScore) {
				maxScore = score;
				ret = recpos;
			}
			printf("value=%.2f, cov=%.2f, #=%.2f, size=%.2f, t=%.2f, s=%.4f - %.2f\n",*value,regCoverageRatio,freqRatio,recSizeRatio,tpcRatio,score,estPeriod);
		}
	}

	return ret.size()?ret:recpos;
}

vector<size_t> tTPSFilter::locateRecords(wstring s, double st) {
	vector<int> d(s.size()-1);
	map<int, vector<int> > diffMap;
	map<int, int> TPMap;
	vector<size_t> recpos;
	int rootTag;
	int tagCount=0;
	int gap=0;
	size_t interval=0xffffffff;

	/* compute sequence's first difference, keeping only the negative values (i.e. keeping only
	 * fast transitions from very high to very low values, L - H = negative difference). The
	 * difference points are stored and processed in ascending order (higher absolute values first).
	 *
	 * the difference is weighted with the inverse TPC value, the lower, the better
	*/


	auto z=s;
	auto off=trimSequence(z);

	cerr << off << " diff: " << endl;

	for (size_t i=1;i<z.size();i++) {
		d[i-1]=(z[i]-z[i-1])*z[i-1];
		/*if (d[i-1] < 0) {
			cerr << d[i-1]*s[i] << " ";
			diffMap[d[i-1]].push_back(i);
		} else cerr << 0 << " ";*/
	}

	for (size_t i=1;i<d.size();i++) {
		if (sign(d[i-1])==sign(sign(d[i]))) {
			if (sign(d[i])>0)
				d[i] += d[i-1];
			else
				d[i] -= d[i-1];
		} else if (d[i] >= 0) {
			diffMap[d[i-1]].push_back(i+off);
		}
		cerr << (d[i-1]<0?d[i-1]:0) << " ";
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

	/* TESTE */
	TPMap.clear();
	float _max_score=0;
	for (size_t i=0;i<=s.size();i++) {
		TPMap[s[i]]++;
	}
	rootTag=0;
	for (auto i=TPMap.begin();i!=TPMap.end();i++) {
		if ((*i).first <= 0) continue;
		if ((*i).second < (s.size()*0.01)) continue;
		float _score = (((float)((*i).second)) / ((float)((*i).first)));
		cerr << "*** SCORE " << (*i).first << " / " << (*i).second << " = " << _score << endl;
		if ( _score >= _max_score) {
			if ((rootTag > (*i).first) || (!rootTag)) {
				tagCount = (*i).second;
				rootTag = (*i).first;
				_max_score = _score;
			}
		}
	}
	cerr << "*** FINAL " << rootTag << " / " << tagCount << " = " << _max_score << endl;
	/* fim TESTE */

	// find the beginning of each record, using the tag path code found before
	for (size_t i=0;i<s.size();i++) {
		if (s[i] == rootTag) {
			//cerr << "root: " << i << " " << nodeSequence[i]->tagName << " : " << nodeSequence[i]->text << endl;
			recpos.push_back(i);
		}
	}

	return recpos;
}

tTPSRegion *tTPSFilter::getRegion(size_t r) {
	if (r < regions.size())
		return &regions[r];
	return NULL;
}

size_t tTPSFilter::getRegionCount() {
	return regions.size();
}

vector<tNode*> tTPSFilter::getRecord(size_t dr, size_t rec) {
	if (dr < regions.size()) {
		if (rec < regions[dr].records.size())
			return regions[dr].records[rec];
	}
	return vector<tNode *>(0);
}

/*double tTPSFilter::estimatePeriod(vector<double> signal) {
	size_t N = (signal.size() + (signal.size()%2));
	double maxPeak=-INFINITY;
	size_t peakFreq=1;

	if (signal.size() != N) { // repeat last sample when signal size is odd
		signal.resize(N);
		signal[N-1] = signal[N-2];
	}

	for (size_t i = 0; i < N; i++) { // apply 'welch' window to signal
		signal[i] *= (1.0-(((double)i-0.5*(double)(N-1))
					/(0.5*(double)(N+1)))
					*(((double)i-0.5*(double)(N-1))
					/(0.5*(double)(N+1))));
	}


	auto spectrum = fft(signal);

	for (size_t i = 4; i < (N/4)-1; i++) {
		if (spectrum[i] > maxPeak) {
			maxPeak = spectrum[i];
			peakFreq = i;
		}
	}

	return ((double)N/(double)peakFreq);
}*/

#define NUM_PEAKS 15

double tTPSFilter::estimatePeriod(vector<double> signal) {
	size_t N = (signal.size() + (signal.size()%2));
	double maxPeak=-INFINITY;

	if (signal.size() != N) { // repeat last sample when signal size is odd
		signal.resize(N);
		signal[N-1]=signal[N-2];
	}

	auto spectrum = fft(signal);
	auto xcorr = autoCorrelation(signal);

	multimap<double, size_t> candidatePeriods;
	for (size_t i=0;i<N;i++) {
		candidatePeriods.insert(make_pair(xcorr[i],i));
	}

	double period = ceil((double)N/(double)(*(candidatePeriods.begin())).second);
	size_t j=0;
	for (auto i = candidatePeriods.rbegin(); i != candidatePeriods.rend(); i++) {
		if ( ((*i).second > 1) && ((*i).second < N) ) {
			size_t f = ceil((double)N/(double)(*i).second);
			auto peak = spectrum[f];
			if (peak > maxPeak) {
				maxPeak = peak;
				period = (*i).second;
			}
			j++;
			if (j == NUM_PEAKS) break;
		}
	}

	return period;
}

pair<size_t,size_t> searchLongestPrefix(const wstring &prefix, const wstring &suffix) {
	pair<size_t,size_t> ret;

	for (size_t i=min(suffix.size(),prefix.size());i>0;i--) {
		ret.first = prefix.find(suffix.substr(0,i));
		if (ret.first != wstring::npos) {
			ret.second = i;
			break;
		}
	}

	if (ret.first == wstring::npos) {
		ret.first = prefix.size();
		ret.second = 1;
	}

	return ret;
}

vector<size_t> searchPrefix(wstring &s, wstring &prefix) {
	size_t pos=0;
	vector<size_t> ret;

	while ((pos=s.find(prefix,pos)) != wstring::npos) {
		ret.push_back(pos);
		pos += prefix.size();
	}

	return ret;
}


vector<size_t> tTPSFilter::LZLocateRecords(tTPSRegion& region, double& period) {
	wstring seq = region.tps;
	map<size_t,size_t> patternCount, patternSize;
	map<size_t,wstring> pattern;
	vector<size_t> ret;

	size_t n = seq.size();

	for (size_t i=1;i<seq.size()-1;i++) {
		wstring prefix = seq.substr(0,i);
		wstring suffix = seq.substr(i,n-i);
		pair<size_t,size_t> prior = searchLongestPrefix(prefix,suffix);

		cout << "[" << prior.first << ", " << prior.second << "]" << " = ";
		for (size_t j=0;j<prior.second;j++)
			cout << seq[prior.first+j] << " ";
		cout << endl;

		wstring patt = seq.substr(prior.first,prior.second);

		patternCount[prior.first]++;
		patternSize[prior.first]+=prior.second;
		pattern[prior.first]=seq.substr(prior.first,1);

		/*patternCount[seq[prior.first]]++;
		patternSize[seq[prior.first]]+=prior.second;
		pattern[seq[prior.first]]=seq.substr(prior.first,1);*/

		/*if (pattern[seq[prior.first]].size() == 0) pattern[seq[prior.first]] = patt;
		else {
			if (
					( (patt.size() < pattern[seq[prior.first]].size()) && (pattern[seq[prior.first]].size() > 1) )
					|| ((pattern[seq[prior.first]].size() < 2) && (patt.size() > pattern[seq[prior.first]].size()))
				)
				pattern[seq[prior.first]]=patt;
		}*/

		i+=prior.second-1;
	}

	double stddev,avgsize,mindev=INFINITY;

	cout << "code;count;len;prefix;rcount;dev" << endl;

	for (auto i = patternCount.begin();i!=patternCount.end(); i++) {
		vector<size_t> recpos = searchPrefix(seq,pattern[(*i).first]);

		avgsize=0;
		for (size_t i=1;i<recpos.size();i++) {
			avgsize += (recpos[i] - recpos[i-1]);
		}
		avgsize /= (float)(recpos.size()-1);

		stddev = 0;
		for (size_t i=1;i<recpos.size();i++) {
			float diff = (float)(recpos[i] - recpos[i-1])-avgsize;
			stddev += (diff*diff);
		}
		stddev = sqrt(stddev/max((float)(recpos.size()-2),(float)1));

		if ((stddev < mindev) && (patternSize[(*i).first] > seq.size()*.10)) {
			mindev = stddev;
			ret = recpos;
		}

		cout << (*i).first << ";" << (*i).second << ";" << patternSize[(*i).first] << ";";

		for (size_t j=0;j<pattern[(*i).first].size();j++)
			cout << pattern[(*i).first][j] << " ";

		cout << ";" << recpos.size() << ";" << stddev << ";";

		for (size_t j=0;j<recpos.size();j++) {
			cout << recpos[j] << " ";
		}
		cout << endl;
	}

	return ret;
}

void tTPSFilter::onDataRecordFound(vector<wstring> &m, vector<size_t> &recpos, tTPSRegion *reg) {
	if ((m.size() == 0) || (recpos.size() == 0)) return;// -1;

	int rows=m.size(),cols=m[0].size();
	vector<tNode *> rec;
	bool keepRec;

	for (int i=0;i<rows;i++) {
		keepRec = false;

		rec.clear();
		cerr << endl;
		for (int j=0,k=0;j<cols;j++) {
			if (m[i][j] != 0) {
				rec.push_back(reg->nodeSeq[recpos[i]+k]);
				cerr << reg->nodeSeq[recpos[i]+k]->tagName << "[" <<
						reg->nodeSeq[recpos[i]+k]->text << "];";
				k++;
				if (j>0) keepRec=true;
			} else rec.push_back(NULL);
		}
		if (keepRec) reg->records.push_back(rec);
	}
	cleanRegion(reg->records);
	cerr << endl;
}
