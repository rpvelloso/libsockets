CRLF = "\n"
gnuplot = "/Progra~2/gnuplot/bin/gnuplot.exe"
--gnuplot = "gnuplot"

term = {}
term["png"] = ".png"
term["postscript"]=".ps"
term["default"]="png"

displayResults = function(dom,method,dir,filename) 
  local j=0
  local regions = getRegionCount(dom,method)
  local outp = io.open(dir..filename,"w")
  
  if method=="srde" then
    local tps = DOMTPS(dom)
    if #tps then
      outp:write("<style>table {border-collapse: collapse;} table, td, th {border: 1px solid black;}</style>")
      outp:write("<font face=courier><img src='",filename,".tps",term[term["default"]],"' /><br />",CRLF)
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
    if dr["content"] then
      outp:write("<font color=red><b>*** Content detected ***</b></font><br>",CRLF)
    end
    outp:write("<table border=0><tr><th> region ",i,"</th><th> rows ",dr.rows,"</th><th> cols ",dr.cols,"</th></tr></table>",CRLF)
    
    if (dr.rows > 0) and (dr["records"]) then 
      outp:write("<table border=0>",CRLF)
      print(dr.rows)
      for r=1,dr.rows do
        outp:write("<tr>")
        for c=1,dr.cols do
          if type(dr.records[r][c])=="string" then
            outp:write("<td>",dr.records[r][c],"</td>")
          else
            outp:write("<td>[",type(dr.records[r][c]),"]</td>")
          end
        end
        outp:write("</tr>",CRLF)
        j = j + 1
      end
      outp:write("</table><br />",CRLF)
    end
    
    if dr["tps"] then
      outp:write("<img src='",filename,".region",i,term[term["default"]],"' /><br />",CRLF)
      outp:write(string.format("offset: %d, size: %d, angle: %.2f, dev: %.2f<br/>",dr.pos,#dr.tps,math.atan(math.abs(dr["a"]))*180/math.pi,dr["d"]),CRLF)
      local t = dr.tps
      if #t then
        outp:write("<textarea>",CRLF)
        outp:write(t[1])
        for k=2,#t do
          outp:write(",",t[k])
        end
        outp:write("</textarea><br />",CRLF)
      end
    end
    
    outp:write(CRLF)
  end
  outp:write(regions," regions, ",j," records.",CRLF)
  outp:write("</font><hr/><br/>",CRLF)
  outp:close()
end

plotSequences = function(dom,output,filename)
  local method = "srde"
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
  

  for filename in popen('ls -a "'..dir..'"'):lines() do
    --if filename=="dailymotion" then
    local d, fn, ext = filename:match("(.-)([^\\/]-%.?([^%.\\/]*))$")
    local output = dir.."/srde/"

    if (fn~="extract_input.pl") and (fn~="srde") and (fn~="drde") and (fn:sub(1,1)~='.') then
      print(string.format("Loading DOM tree: %s",filename),CRLF)
      local dom = loadDOMTree(dir.."/"..filename.."/index.html")
      
      --print("Extracting records.")
      local start = os.clock()
      SRDExtract(dom)
      print(string.format("elapsed time: %.2f",os.clock() - start),CRLF)
      
      --print("Outputting results.")
      displayResults(dom,"srde",output,fn..".html")
      
      --print("Plotting graphs.")
      plotSequences(dom,"file",output..fn..".html")
    end
    --end
  end
end

processTestBed2 = function(dir)
  local t, popen = {}, io.popen
  

  for filename in popen('ls -a "'..dir..'"/*.htm*'):lines() do
    local d, fn, ext = filename:match("(.-)([^\\/]-%.?([^%.\\/]*))$")
    local output = d.."srde/"..fn
    
    print(string.format("Loading DOM tree: %s",filename),CRLF)
    local dom = loadDOMTree(filename)
    
    --print("Extracting records.")
    local start = os.clock()
    SRDExtract(dom)
    print(string.format("elapsed time: %.2f",os.clock() - start),CRLF)
    
    --print("Outputting results.")
    displayResults(dom,"srde",d.."srde/",fn)
    
    --print("Plotting graphs.")
    plotSequences(dom,"file",output)
  end
end

--processTestBed2("../datasets/tpsf")
--processTestBed2("../datasets/wien")
processTestBed2("../datasets/zhao1")
--processTestBed2("../datasets/zhao2")
--processTestBed2("../datasets/zhao3")
--processTestBed2("../datasets/yamada")
--processTestBed("../datasets/trieschnigg1")
--processTestBed("../datasets/trieschnigg2")
exit()
