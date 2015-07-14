#!/bin/bash

let i=1
last=""
for x in `find|grep htm`; do
	fn=`echo $x|cut -d '/' -f 5`
	file=`echo $fn|cut -d '.' -f 1`
	ext=`echo $fn|cut -d '.' -f 2`
	if [ "$file" != "$last" ]; then
		echo $last $file
		last=$file
		let i=1
	fi
	cp $x ./$file$i.$ext
	echo ./$file$i.$ext
	let i=i+1
done