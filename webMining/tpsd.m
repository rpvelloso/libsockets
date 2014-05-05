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
  
  while (thresholds(t) < 5) t = t + 1; end
  
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
    for i=1:length(tagPathSequence)
      symbol=tagPathSequence(i);
      if ismember(symbol,currentAlphabet)
        regionAlphabet = union(regionAlphabet,symbol);
        currentSymbolCount(symbol) = currentSymbolCount(symbol) - 1;
        if currentSymbolCount(symbol) == 0
          currentAlphabet = setdiff(currentAlphabet,symbol);
          if length(intersect(currentAlphabet,regionAlphabet)) == 0
            if (length(currentAlphabet) > 1) %&& (abs((n-2*i+gapsize)/(n-gapsize)) > 0.02)
              regionFound = 1;
            end
            break;
          end
        end
      end
    end
  end
  if regionFound
      tagPathSequence = tagPathSequence(i+1:n);
      pos = i;
  else
     pos = -1;
  end
  mainregion = tagPathSequence;
end

x = load('Debug/x');
%x=[1 4 4 5 5 6 4 5 6 7 8 9 10 7 8 9 10 10 10 10 10 10 3 2 3 2 3 2 3 2 3 2]';

tps=x';


i = 0;
j = 1;
pos = 1;

while i >= 0
  [i, datareg] = searchRegion(tps);
  l(j) = pos;
  j = j + 1;
  if i>=0
     pos = pos + i;
     tps = datareg;
  end
end
l(j) = length(x);
x=x';

figure(1);
hold;
figure(2);
hold;
figure(3);
hold;
for i = 1:j-1
  w=x(l(i):l(i+1));
  z=abs(xcorr(w-mean(w),'unbiased'));
  f=fft(w).^2;
  [var(w)/1e+3 max(z) max(z)/var(w)]
  figure(1);
  plot([l(i):l(i+1)],w,[int2str(1+mod(i,5)) '-']);
  figure(2);
  plot([1:length(z)],z,[int2str(1+mod(i,5)) '.']);
  figure(3);
  plot([1:length(f)],f,int2str(1+mod(i,5)));
end

figure(1);
title('TPS');
xlabel('posicao da sequencia');
ylabel('codigo tag path');
