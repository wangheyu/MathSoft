#!/bin/sh

x=0
while [ $x -ne 100 ]; do
    echo lec$x.tex
    x=$(($x+1))
done
exit 0
