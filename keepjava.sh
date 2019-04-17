#!/bin/bash

dir=${1}

for file in !${1}/*.java; do
	rm ${file}
done

