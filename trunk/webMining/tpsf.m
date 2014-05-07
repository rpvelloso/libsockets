clc;
clear all;
close all;

function filteredAlphabet = filterAlphabet(alphabet, symbolCount, threshold)
  filteredAlphabet = [];
  for i=1:length(alphabet)
    if symbolCount(alphabet(i)) >= threshold
      filteredAlphabet = union(filteredAlphabet,alphabet(i));
    end
  end
  filteredAlphabet = setdiff(filteredAlphabet,[0]);
end

function [pos, mainregion] = searchRegion(tagPathSequence)
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
            if (length(currentAlphabet) > 1) && (abs((n-2*i+gapsize)/(n-gapsize)) > 0.02)
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
    if len > 15
      blkcount = blkcount + 1;
      blks{blkcount}=[i pos pos+len len];
      blkfreq=blkfreq + [zeros(1,pos-1) ones(1,len) zeros(1,i-pos-len) ones(1,len) zeros(1,length(seq)-i-len+1)];
      i = i + l - 1;
    end
    i = i + 1;
  end
end

x = load('Debug/x');

tps=x';


i = 0;
pos = 1;

while i >= 0
  [i, datareg] = searchRegion(tps);
  if i>=0
     pos = pos + i;
     tps = datareg;
  end
end

[blks,blkfreq] = LZDecomp(tps');
figure; hold;
for i=1:length(blks)	
	b = blks{i};	
	plot(i*[zeros(1,b(2)-1) ones(1,b(4)) zeros(1,length(tps)-b(4)-b(2)+1)],'k.');
end
figure; plot(tps,'k.');

figure;
hold;
plot([1:pos],x'(1:pos),'k.');
plot([pos+length(tps):length(x)],x'(pos+length(tps):length(x)),'k.');
plot([pos:pos+length(tps)-1],tps,'ko');
title('TPS de pagina do site Youtube');
xlabel('posicao da sequencia');
ylabel('codigo tag path');
legend('TPS','','Regiao principal','location','northwest');
legend('boxon');
