clc;
clear all;
close all;
pkg load signal;

function t = transform(sig)
	size = length(sig);
	lowpass(1:size) = 1/size;
	%lowpass(ceil(size/8):size)=0;
	
	sig = real(ifft(fft(sig).*lowpass));
	
	sig = sig .* welchwin(length(sig),"periodic")';
	t=abs(fft(sig)).^2;
end

function a = linearReg(sig)
	l = length(sig);
	x = [1:l]';
	y = sig';
	X = [ones(l,1) x];
	a = (pinv(X'*X))*X'*y;
end

function p = symfil(sig,f)
	[a,b] = hist(sig,unique(sig));
	
	c = find(a<=f);
	z = zeros(length(sig),1);
	for i=1:length(sig)
		if sum(c==sig(i))!=0
			z(i)=1;
		end
	end

	s=1;e=1;c=0;j=1;
	for i=1:length(sig)
		if c==0
			s=s+1;
			if z(i)==0
				c=1;
				e=s;
			end
		end
		if c==1 
			if z(i)==0
				e=e+1;
			else
				e=i-1;
				if e-s > 3
					reg = sig(s:e); %-mean(sig(s:e)); %regiao 
					alphabet=unique(reg); % alfabeto da regiao
					if j>1
						if length(intersect(alphabet,ab{j-1})) > 0
							p{j-1}(2)=e;
							r{j-1}= sig(p{j-1}(1):e); %-mean(sig(p{j-1}(1):e));
							ab{j-1}=union(ab{j-1},alphabet);
							c=0;
							s=i;
							printf("merge r{%d} + r{%d} [%d %d]\n",j-1,j,p{j-1}(1),p{j-1}(2));
							continue;
						end
					end
					r{j}= reg;
					p{j}=[s e]; %posicao original 
					ab{j}=alphabet;
					j=j+1;
				end
				c=0;
				s=i;
			end
		end
	end
end

function [p,d] = findPeaks(s)
	size = length(s);
	
	q = s;
	%q(find(q<0))=-1;
	%q(find(q>0))=+1;
	
	d1 = diff(q,1);
	d1 = d1<0;
	d1 = d1.* s(2:size);
	p = find(d1<0);
	d = d1(p);
	p = p+1;
	
	p = find(s<0);
	d = s(p);
end

function v=score(p,d,s,f)
	size = length(s);
	candidates = unique(d);
	printf("\n");
	maxScore = -Inf;
	v=-Inf;
	
	while length(candidates) > 0
		value = min(candidates);
		candidates = setdiff(candidates,value);
		pos = find(d==value);
		reccount = length(pos);
		if reccount>1
			recsize = diff(p(pos));
			dev=std(recsize);
			m=mean(recsize);
			%coverage=((p(pos(length(pos)))-p(pos(1)))/size);%-coefVar;
			regionCoverage = min([m*reccount/size 1]);
			rcountRatio = (min([reccount size/m])/max([reccount size/m]));
			if dev>1
				rsizeRatio = (min([m/dev f])/max([m/dev f]));
			else
				rsizeRatio = (min([m f])/max([m f]));
			end
			tpcRatio = abs(value)/max(abs(s));
			scr = regionCoverage*rcountRatio*rsizeRatio*tpcRatio;
			if scr > maxScore
				maxScore = scr;
				v=value;
			end
			printf("value=%.2f, cov=%.2f, #=%.2f, size=%.2f, t=%.2f, s=%.4f - %d\n",value,regionCoverage,rcountRatio,rsizeRatio,tpcRatio,scr,f);
		end
	end
end

function q = reencode(s)
	setCodes = unique(s);
	
	pos = 1; n=1;
	while pos<=length(s)
		if find(setCodes==s(pos))
			n=n+1;
			c{s(pos)}=n;
			setCodes = setdiff(setCodes,s(pos));
			q(pos)=n;
		else
			q(pos)=c{s(pos)};
		end
		pos = pos+1;
	end
end

%signal = load('~/Downloads/tps/Amazon.html.tps')';
%signal = load('~/Downloads/tps/wine.html.tps')';
%signal = load('~/Downloads/tps/rubylane.html.tps')';
%signal = load('~/Downloads/tps/magazineOutlet.html.tps')';
%signal = load('~/Downloads/tps/yahoo.html.tps')';
%signal = load('~/Downloads/tps/chapters.html.tps')';
signal = load('Debug/x');

figure; plot(signal);

p = symfil(signal,8);

sig(1:length(signal))=0;
for i=1:length(p)
	r{i} = reencode(signal(p{i}(1):p{i}(2)));
	r{i} = r{i} - mean(r{i});
	sig(p{i}(1):p{i}(2)) = r{i};

	l{i} = linearReg(r{i});
	ffts{i}=transform(r{i});

	angle = atand(abs(l{i}(2)));

	if angle < 5
		figure; hold on; 

		size = length(r{i});
		Sdiv2 = ceil(size/4);
		pos = find( ffts{i}(2:Sdiv2-1)==max(ffts{i}(2:Sdiv2-1)) );
		if length(pos > 1)
			pos=pos(1);
			plotPeriod=1;
		else
			plotPeriod=0;
		end
		period = round(size / pos);
		
		subplot(1,2,1);
		plot(1:size,r{i},'.-'); hold on;
		text(size,max(r{i}),num2str(std(r{i})));
		
		[peaks,d1] = findPeaks(r{i});
		value=score(peaks,d1,r{i},period);
		if value!=-Inf
			plot(find(r{i}==value),r{i}(find(r{i}==value)),'rx');
		end
		%plot(3:size,diff(r{i},2),'g');
		
		plot(1:size,(l{i}(2).*[1:size]) + l{i}(1),'k');
		text(size,(l{i}(2).*size) + l{i}(1),num2str(angle));
		
		subplot(1,2,2);
		plot(1:Sdiv2,ffts{i}(1:Sdiv2),'.-');
		if plotPeriod==1
			text(pos+1,ffts{i}(pos+1),num2str(period));
		end
	end
end
signal = sig;
%----

signal = signal-mean(signal);
len = length(signal);

figure;
plot(transform(signal)(1:round(len/2)),'.-'); figure;
plot(signal); hold;
return;
