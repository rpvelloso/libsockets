clc;
clear all;
close all;
pkg load signal;

function filteredAlphabet = filterAlphabet(alphabet, symbolCount, threshold)
  filteredAlphabet = [];
  for i=1:length(alphabet)
    if symbolCount(alphabet(i)) >= threshold
      filteredAlphabet = union(filteredAlphabet,alphabet(i));
    end
  end
  filteredAlphabet = setdiff(filteredAlphabet,[0]);
end

function [pos, mainregion] = searchRegion(tagPathSequence,tolerance)
  alphabet = [];
  t = 1;
  n = length(tagPathSequence);
  
  for i=1:length(tagPathSequence)
    symbol = tagPathSequence(i);
    if ~ismember(symbol,alphabet)
      alphabet = union(alphabet,symbol);
      symbolCount(symbol)=0;
    end
    symbolCount(symbol) = symbolCount(symbol)+1;
  end
  thresholds = setdiff(unique(symbolCount),[0]);
  
  regionFound = 0;
  while ~regionFound
    t = t + 1;
    if t > length(thresholds)
      break;
    end
    currentAlphabet = filterAlphabet(alphabet,symbolCount,thresholds(t));
    if length(currentAlphabet) < 2
      break;
    end
    currentSymbolCount = symbolCount;
    regionAlphabet = [];
    gapsize = 0;
    for i=1:length(tagPathSequence)
      symbol=tagPathSequence(i);
      if ismember(symbol,currentAlphabet)
        regionAlphabet = union(regionAlphabet,symbol);
        currentSymbolCount(symbol) = currentSymbolCount(symbol) - 1;
        if currentSymbolCount(symbol) == 0
          currentAlphabet = setdiff(currentAlphabet,symbol);
          if length(intersect(currentAlphabet,regionAlphabet)) == 0
            if (length(currentAlphabet) > 1) && (abs((n-2*i+gapsize)/(n-gapsize)) > tolerance)
              regionFound = 1;
              break;
            else
              gapsize = 1;
            end
          end
        end
      else
        if (gapsize) 
          gapsize = gapsize + 1;
        end
      end
    end
  end
  if regionFound
    if i < floor(n/2)
      tagPathSequence = tagPathSequence(i+1:n);
      pos = i;
    else
      tagPathSequence = tagPathSequence(1:i);
      pos = 0;
    end
  else
     pos = -1;
  end
  mainregion = tagPathSequence;
end

function pos = findsubseq(seq,subseq)
	pos = 0;
	for i=1:(length(seq)-length(subseq)+1)
		if ( seq(i)==subseq(1) )
			if seq(i:i+length(subseq)-1) == subseq
				pos = i;
				break;
			end
		end
	end
end

function [blks,blkfreq] = LZDecomp(seq)
	blkcount=0; i=1;
	blkfreq=zeros(1,length(seq));
	while i < length(seq)
	len=pos=0;
	prefix = seq(1:i-1);
	suffix = seq(i:length(seq));
	for l=min(length(suffix),length(prefix)):-1:1
		prior = suffix(1:l);
		pos=findsubseq(prefix,prior);
		if pos > 0
			len=l;
			break;
		end
	end
	if len > 0
		blkcount = blkcount + 1;
		blks{blkcount}=[i pos pos+len len];
		%blkfreq=blkfreq + [zeros(1,pos-1) ones(1,len) zeros(1,i-pos-len) ones(1,len) zeros(1,length(seq)-i-len+1)];
		blkfreq=blkfreq + [zeros(1,pos-1) ones(1,len) zeros(1,i-pos-len) zeros(1,length(seq)-i+1)];
		i = i + l - 1;
	end
	i = i + 1;
	end
end

function [records] = recordDetect(tps)
d = diff(tps - mean(tps));
d = d.*(d<0);

figure; plot(d,'o'); title('negative difference');

[v,p]=sort(d);
l=p(1);
r=p(1);
level=v(1);
gap=0;
records = [];

i=1;
while ((gap/length(tps)) < 0.80) and (i<=length(tps))
j=1;
interval=+Inf;
while level==v(i)
  if p(i) < l l=p(i); end
  if p(i) > r r=p(i); end
  records = [records p(i)+1];
  if j>1
    int=abs(p(i)-p(i-1));
    if int < interval interval=int; end
  end
  i=i+1;
  j=j+1;
end
if interval!=+Inf gap=gap+((j-1)*interval); end
level=v(i);
end
end


x = load('Debug/x');

tps=x';


i = 0;
pos = 1;

while i >= 0
  [i, datareg] = searchRegion(tps,0.1);
  if i>=0
     pos = pos + i;
     tps = datareg;
  end
end

records = recordDetect(tps);

%[blks,blkfreq] = LZDecomp(tps);
%figure; hold;
%th = 0; j=1;
%for i=1:length(blks)	
%	b = blks{i};	
%	plot(j*[zeros(1,b(2)-1) ones(1,b(4)) zeros(1,length(tps)-b(4)-b(2)+1)],'k.');
%	j=j+1;
%	th = b(2);
%end
figure;
hold;
plot([1:pos],x'(1:pos),'k.');
plot([pos+length(tps):length(x)],x'(pos+length(tps):length(x)),'k.');
plot([pos:pos+length(tps)-1],tps,'ko');
plot(pos+records,tps(records),'rx');
title('TPS de pagina do site Youtube');
xlabel('posicao da sequencia');
ylabel('codigo tag path');
legend('TPS','','Regiao principal','location','northwest');
legend('boxon');

figure; plot(tps,'o');
