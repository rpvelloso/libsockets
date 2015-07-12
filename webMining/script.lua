CRLF = "\n"
--gnuplot = "C:\\Progra~2\\gnuplot\\bin\\gnuplot.exe"
gnuplot = "gnuplot"

term = {}
term["png"] = ".png"
term["postscript"]=".ps"
term["default"]="png"

displayResults = function(dom,method,dir,filename) 
	local j=0
	local regions = getRegionCount(dom,method)
	local outp = io.open(dir..filename,"w")
	
	if method=="drde" then
    local tps = DOMTPS(dom)
    if #tps then
      outp:write("<img src='",filename,".tps",term[term["default"]],"' /><br />",CRLF)
      outp:write("<textarea>",CRLF)
      outp:write(tps[1])
      for k=2,#tps do
        outp:write(",",tps[k])
      end
      outp:write("</textarea><br />")
    end
	end
	for i=1,regions do
		local dr = getDataRegion(dom,method,i-1)
    outp:write("<table border=1><tr><th> region ",i,"</th><th> rows ",dr.rows,"</th><th> cols ",dr.cols,"</th></tr></table>",CRLF)
		
		if (dr.rows > 0) and (dr["records"]) then 
  		outp:write("<table border=1>",CRLF)
  		for r=1,dr.rows do
  			outp:write("<tr>")
  			for c=1,dr.cols do
  				if type(dr.records[r][c])=="string" then
  					outp:write("<td>",dr.records[r][c],"</td>")
  				else
  					outp:write("<td></td>")
  				end
  			end
  			outp:write("</tr>",CRLF)
  			j = j + 1
  		end
  		outp:write("</table><br />",CRLF)
		end
		
    if dr["tps"] then
      outp:write("<img src='",filename,".region",i,term[term["default"]],"' /><br />",CRLF) 
      outp:write(dr.pos,",",#dr.tps,": ",dr["a"],"*x + ",dr["b"],". error = ",dr["e"],"<br />",CRLF)
      local t = dr.tps
      if #t then
        outp:write("<textarea>",CRLF)
        outp:write(t[1])
        for k=2,#t do
          outp:write(",",t[k])
        end
        outp:write("</textarea><br />")
      end
    end
    
    outp:write(CRLF)
	end
	outp:write(regions," regions, ",j," records.",CRLF)
  outp:write("<hr/><br/>",CRLF)
  outp:close()
end

plotSequences = function(dom,output,filename)
  local method = "drde"
  local regions = getRegionCount(dom,method)
  local tps = DOMTPS(dom)

  f = io.open(filename..".plot.txt","w")
  if output == "file" then
    f:write("set term ",term["default"],CRLF)
  else
    f:write("set mouse",CRLF)
    f:write("set multiplot layout ",regions+1,",1",CRLF)
  end

  f:write("set output \"",filename,".tps",term[term["default"]],"\"",CRLF)
  f:write("plot '-' with lines title 'Full TPS'",CRLF)
  for i=1,#tps do
        f:write(i-1,"\t",tps[i],CRLF)
  end
  f:write("e",CRLF)

  for i=1,regions do
    local dr = getDataRegion(dom,method,i-1)
    if dr["tps"] then
      f:write("set output \"",filename,".region",i,term[term["default"]],"\"",CRLF)
      f:write("plot ",dr.a,"*x+",dr.b," with lines title 'Linear regression','-' with lines title 'Region ",i,"' lc rgb \"black\" ",CRLF)
      for j=1,#dr.tps do
        f:write(j-1,"\t",dr.tps[j],CRLF)
      end
      f:write("e",CRLF)
    end
  end
  f:write("unset multiplot",CRLF)
  f:write("quit",CRLF)
  f:close()
  os.execute(gnuplot.." "..filename..".plot.txt")
end

processTestBed = function(dir)
  local t, popen = {}, io.popen
  

  for filename in popen('ls -a "'..dir..'"/*.htm*'):lines() do
    local d, fn, ext = filename:match("(.-)([^\\/]-%.?([^%.\\/]*))$")
    local output = d.."drde/"..fn
    
    print("Loading DOM tree: ",filename)
    local dom = loadDOMTree(filename)
    
    print("Extracting records.")
    DRDExtract(dom,0.70)
    
    print("Outputting results.")
    displayResults(dom,"drde",d.."drde/",fn)
    
    print("Plotting graphs.")
    plotSequences(dom,"file",output)
  end
end

processTestBed("testbed")
processTestBed("testbed2")
processTestBed("testbed3")
processTestBed("testbed4")
exit()