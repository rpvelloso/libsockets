#!/bin/bash

let i=1
for x in `find|grep htm`; do
	x=${x/./}
	x=${x/\//}
	dest=${x//\//_}
	cp $x ./$dest
	echo $dest
	let i=i+1
done