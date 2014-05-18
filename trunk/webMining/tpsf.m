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

function [tagPathSequence,pos] = searchRegion(tagPathSequence)
  alphabet = [];
  t = 1;
  n = length(tagPathSequence);
  pos = 0;
  
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
    for i=1:length(tagPathSequence)
      symbol=tagPathSequence(i);
      if ismember(symbol,currentAlphabet)
        regionAlphabet = union(regionAlphabet,symbol);
        currentSymbolCount(symbol) = currentSymbolCount(symbol) - 1;
        if currentSymbolCount(symbol) == 0
          currentAlphabet = setdiff(currentAlphabet,symbol);
          if length(intersect(currentAlphabet,regionAlphabet)) == 0
            if (length(currentAlphabet) > 1)
              regionFound = 1;
            end
            break;
          end
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
    [tagPathSequence,p] = searchRegion(tagPathSequence);
    pos = pos + p;
  end
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

function [records, diff] = recordDetect(tps)
   d = diff(tps - mean(tps));
   d = d.*(d<0);

   figure; 
   hold;
   plot(d,'o'); 
   plot(d,'r-'); 
   title('negative difference');

   [v,p]=sort(d);
   l=p(1);
   r=p(1);
   level=v(1);
   gap=0;
   diff = [];

   i=1;
   while ((gap/length(tps)) < 0.80) and (i<=length(tps))
      j=1;
      interval=+Inf;
      while level==v(i)
        if p(i) < l l=p(i); end
        if p(i) > r r=p(i); end
        diff = [diff p(i)+1];
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
   
   count=0;
   j=0;
   for i=1:length(diff)
      smt = sum(diff(:) == diff(i));
      if smt > count
         count = smt;
         j=i;
      end
   end
   
   records = tps(:) == tps(diff(j)); 
   delta=max(tps)-tps(diff(j));
   records = (records.*delta).+tps(diff(j));
end

function [a,b,e] = lslr(y)
   n = length(y);
   x=(1:n);
   xy = x.*y;
   x2=x.^2;
   sx = sum(x);
   sy=sum(y);
   sxy=sum(xy);
   sx2=sum(x2);
   delta=n*sx2-sx^2;
   
   a=(n*sxy-sy*sx)/delta;
   b=(sx2*sy-sx*sxy)/delta;
   e = sum((y - a*x+b).^2);
   
   figure; hold;
   plot(x,y,'.');
   plot(x,a*x + b,'r-');
   
end

x = load('Debug/x');

tps=x';


[tps,pos] = searchRegion(tps);

[records,diff] = recordDetect(tps);

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
plot(x(1:pos),'k.');
plot([pos+length(tps)+1:length(x)],x'(pos+length(tps)+1:length(x)),'k.');
plot([pos:pos+length(tps)-1],tps,'ko');
plot(pos+diff,tps(diff),'r*');
title('TPS de pagina do site Youtube');
xlabel('posicao da sequencia');
ylabel('codigo tag path');
legend('TPS','','Regiao principal','location','northwest');
legend('boxon');

figure; hold;
plot(tps,'o');
plot(records,'r-');

