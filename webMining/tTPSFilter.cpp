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

map<long int, tTPSRegion> tTPSFilter::detectStructure(map<long int, tTPSRegion> &r) {
	float angCoeffThreshold=0.17633; // 10 degrees
	//long int sizeThreshold = (tagPathSequence.size()*3)/100; // % page size
	map<long int,tTPSRegion> structured;

	for (auto i=r.begin();i!=r.end();i++) {
		(*i).second.lc = linearRegression((*i).second.tps);

		cerr << "size: " << (*i).second.len << " ang.coeff.: " << (*i).second.lc.a << endl;

		if (abs((*i).second.lc.a) < angCoeffThreshold)
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

	buildTagPath("",n,false,false,false); // rebuild TPS without css to increase periodicity
	for (auto i=_regions.begin();i!=_regions.end();i++) {
		(*i).second.tps = tagPathSequence.substr((*i).second.pos,(*i).second.len);
	}

	tagPathSequence = originalTPS;
	nodeSequence = originalNodeSequence;

	return detectStructure(_regions);
}

map<long int, tTPSRegion> tTPSFilter::SRDEFilter(tNode *n, bool css) {
	set<int> alphabet;
	wstring s;
	long int lastRegPos = -1;

	buildTagPath("",n,false,css,false);
	s = tagPathSequence;

	auto symbolCount = symbolFrequency(s,alphabet);
	auto thresholds = frequencyThresholds(symbolCount);
	auto threshold = thresholds.begin();
	threshold++;
	threshold++;
	threshold++;

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
		_regions[0].content = true;
		_regions[0].len = tagPathSequence.size();
		_regions[0].pos = 0;
		_regions[0].nodeSeq = nodeSequence;
		_regions[0].tps = tagPathSequence;
	}
	/*
	buildTagPath("",n,false,false,false); // rebuild TPS without CSS to increase periodicity
	for (auto i=_regions.begin();i!=_regions.end();i++) {
		(*i).second.tps = tagPathSequence.substr((*i).second.pos,(*i).second.len);
	}
	*/

	return detectStructure(_regions);
}

void tTPSFilter::SRDE(tNode *n, bool css) {
	vector<unsigned int> recpos;
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
		recpos = SRDElocateRecords(_regions[(*i).first],period);

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
		unsigned int max_size=0;
		for (size_t j=0;j<recpos.size();j++) {
			if (prev==-1) prev=recpos[j];
			else {
				m.push_back(_regions[(*i).first].tps.substr(prev,recpos[j]-prev));
				max_size = max(recpos[j]-prev,max_size);
				prev = recpos[j];
			}
		}
		if (prev != -1) {
			if (period > max_size) max_size = period;
			m.push_back(_regions[(*i).first].tps.substr(prev,max_size));
		}

		// align the records (one alternative to 'center star' algorithm is ClustalW)
		centerStar(m);

		// and extracts them
		if (m.size()) onDataRecordFound(m,recpos,&_regions[(*i).first]);
	}

	if (structured.size()) {
		vector<double> ckmeansInput;
		ClusterResult result;

		ckmeansInput.push_back(0);
		for (auto i=structured.begin();i!=structured.end();i++) {
			ckmeansInput.push_back(_regions[(*i).first].stddev);
		}
		result = kmeans_1d_dp(ckmeansInput,2,2);

		auto j=structured.begin();
		for (auto i=++(result.cluster.begin());i!=result.cluster.end();i++,j++) {
			_regions[(*j).first].content = (((*i) == 2) || (result.nClusters < 2));

			// restore the original region's tps
			_regions[(*j).first].tps = tagPathSequence.substr((*j).first,(*j).second.len);
		}
	}

	regions.clear();

	for (auto i=_regions.begin();i!=_regions.end();i++) {
		(*i).second.pos = (*i).first;
		regions.push_back((*i).second);
	}

    sort(regions.begin(),regions.end(),[](const tTPSRegion &a, const tTPSRegion &b) {
    		return (a.stddev > b.stddev);
    });
}

void tTPSFilter::DRDE(tNode *n, bool css, float st) {
	vector<unsigned int> recpos;
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
		unsigned int max_size=0;
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

vector<unsigned int> tTPSFilter::SRDElocateRecords(tTPSRegion &region, double &period) {
	wstring s = region.tps;
	vector<float> signal(s.size());
	float avg;
	set<float> candidates;
	vector<unsigned int> recpos,ret;
	set<int> alphabet;
	map<int,int> reencode;
	double estPeriod;
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
	cout << endl;

	auto value = candidates.begin();
	while (value != candidates.end()) {
		double stddev,avgsize;

		recpos.clear();
		stddev=0;
		avgsize=0;

		for (size_t i=0;i<s.size();i++) {
			if (signal[i] == *value)
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

			double regionCoverage = min(estPeriod*(double)recpos.size()/(double)signal.size(), (double)1);
			double recCountRatio =
					min( (double)recpos.size() ,(double)signal.size()/estPeriod) /
					max( (double)recpos.size() ,(double)signal.size()/estPeriod);
			if (stddev>1) avgsize /= stddev; // SNR
			double recSizeRatio = min( avgsize, estPeriod )/max( avgsize, estPeriod );
			double tpcRatio = (double)abs(*value)/maxCode; // DNR

			double score = (regionCoverage + recCountRatio + recSizeRatio + tpcRatio)/(double)4;
			//double score = regionCoverage * recCountRatio * recSizeRatio * tpcRatio;
			if (score > maxScore) {
				maxScore = score;
				ret = recpos;
			}
			printf("value=%.2f, cov=%.2f, #=%.2f, size=%.2f, t=%.2f, s=%.4f - %.2f\n",*value,regionCoverage,recCountRatio,recSizeRatio,tpcRatio,score,estPeriod);
		}
		value++;
	}

	return ret.size()?ret:recpos;
}

vector<unsigned int> tTPSFilter::locateRecords(wstring s, double st) {
	vector<int> d(s.size()-1);
	map<int, vector<int> > diffMap;
	map<int, int> TPMap;
	vector<unsigned int> recpos;
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

double tTPSFilter::estimatePeriod(vector<float> signal) {
	size_t N = (signal.size() + (signal.size()%2));
	double peak,maxPeak=0;
	size_t peakFreq=1;

	fft_object obj = fft_init(N,1); // Initialize FFT object obj . N - FFT Length. 1 - Forward FFT and -1 for Inverse FFT

	fft_data* inp = (fft_data*) malloc (sizeof(fft_data) * N);
	fft_data* oup = (fft_data*) malloc (sizeof(fft_data) * N);

	for (size_t i = 0; i < signal.size(); i++) {
		inp[i].re = signal[i];
		inp[i].im = 0;

		inp[i].re *= (1.0-(((double)i-0.5*(double)(N-1))
					/(0.5*(double)(N+1)))
					*(((double)i-0.5*(double)(N-1))
					/(0.5*(double)(N+1))));
	}

	if (signal.size() != N) { // repeat last sample when signal size is odd
		inp[N-1].re = signal[N-2];
		inp[N-1].im = 0;
	}

	fft_exec(obj,inp,oup);

	for (size_t i = 2; i < (N/4)-1; i++) {
		peak = sqrt((oup[i].re*oup[i].re) + (oup[i].im*oup[i].im));
		if (peak > maxPeak) {
			maxPeak = peak;
			peakFreq = i;
		}
	}

	free(inp);
	free(oup);
	free_fft(obj);

	return ((double)N/(double)peakFreq);
}

void tTPSFilter::onDataRecordFound(vector<wstring> &m, vector<unsigned int> &recpos, tTPSRegion *reg) {
	if ((m.size() == 0) || (recpos.size() == 0)) return;// -1;

	int rows=m.size(),cols=m[0].size();
	vector<tNode *> rec;

	for (int i=0;i<rows;i++) {
		rec.clear();
		cerr << endl;
		for (int j=0,k=0;j<cols;j++) {
			if (m[i][j] != 0) {
				rec.push_back(reg->nodeSeq[recpos[i]+k]);
				cerr << reg->nodeSeq[recpos[i]+k]->tagName << "[" <<
						reg->nodeSeq[recpos[i]+k]->text << "];";
				k++;
			} else rec.push_back(NULL);
		}
		reg->records.push_back(rec);
	}
	cleanRegion(reg->records);
	cerr << endl;
}
