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
            if (length(currentAlphabet) > 1) && (abs((n-2*i+gapsize)/(n-gapsize)) > 0.2)
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

x = load('Debug\x');
%output=[1 2 3 4 5 6 4 5 6 7 8 9 10 7 8 9 10 10 10 10 10 10]';

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

d_order=2;
dd=abs(diff(tps-mean(tps),d_order))+mean(tps);

figure;
hold;
plot([1:length(x)],x','k.');
plot([pos:pos+length(tps)-1],tps,'k*');
title('Youtube TPS');
xlabel('sequence position');
ylabel('tag path value');
legend('TPS','Main content','location','northwest');
legend('boxon');
%plot([pos+d_order:pos+length(tps)-1],dd,'g');


