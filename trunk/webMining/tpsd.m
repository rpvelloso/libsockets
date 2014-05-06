clc;
clear all;
close all;

pkg load 'signal';

function filteredAlphabet = filterAlphabet(alphabet, symbolCount, threshold)
  filteredAlphabet = [];
  for i=1:length(alphabet)
    if symbolCount(alphabet(i)) >= threshold
      filteredAlphabet = union(filteredAlphabet,alphabet(i));
    end
  end
  filteredAlphabet = setdiff(filteredAlphabet,[0]);
end

function fseq = filterSequence(sequence ,symbolCount, threshold)
  fseq = sequence;
  for i=1:length(sequence)
    if symbolCount(sequence(i)) < threshold
      fseq(i) = 0;
    end
  end
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
  
  while (thresholds(t) < 2) t = t + 1; end
  
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

x = load('Debug\x');
%x=[1 4 4 5 5 6 4 5 6 7 8 9 10 7 8 9 10 10 10 10 10 10 3 2 3 2 3 2 3 2 3 2]';

tps=x';


i = 0;
j = 1;
pos = 1;
diff_order=1;

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
  w=w-mean(w);
  %[var(w)/1e+3 max(z) max(z)/var(w)]
  figure(1);
  plot([l(i):l(i+1)],w,[int2str(mod(i,7)) '-']);
  difference=diff(sign(w),diff_order)!=0;
  difference=diff(w,diff_order).*difference;
  while sum(min(difference)==difference)<5
    difference=(difference!=min(difference)).*difference;
  end
  plot([l(i)+diff_order:l(i+1)],difference,'k.');
  plot([l(i):l(i+1)],repmat(min(difference),1,length([l(i):l(i+1)])),[int2str(mod(i,7)) '-']);
  z=abs(xcorr(difference,'unbiased'));
  f=abs(fft(difference)).^2;
  figure(2);
  plot([1:length(z)],z,[int2str(mod(i,7)) '-']);
  figure(3);
  plot([1:length(f)],f,int2str(mod(i,7)));
end

figure(1);
title('TPS');
xlabel('posicao da sequencia');
ylabel('codigo tag path');
