#!/bin/bash

echo "hello world!"

echo "hello world!" >> handin.txt

#variables for the command line
#Numbers
w=${1}
x=${2}
#filename
y=${3}
#foldername
z=${4}


echo "this many parameters passed: ${#}"
echo "this is the value of the second parameter: ${x}"

if [[ $w -gt $x ]]; then
	echo "first parameter ${w} is bigger" >> handin.txt
else
	echo "second parameter ${x} is bigger" >> handin.txt
fi

if [[ -e $y ]]; then
	echo "first file/dir parameter exists" >> handin.txt
	if [[ -f $y ]]; then
		echo "first parameter is a file" >> handin.txt
	else
		echo "firt parameter isn't a file" >> handin.txt
	fi
else
	echo "first file/dir parameter doesn't exist" >> handin.txt
fi

if [[ -e $z ]]; then
	echo "second file/dir parameter exists" >> handin.txt
	if [[ -d $z ]]; then
		echo "second parameter is a directory" >> handin.txt
	else
		echo "second parameter isn't a directory" >> handin.txt
	fi
else
	echo "second file/dir parameter doesn't exist" >> handin.txt
fi
#prints the name of every file in the current directory that ends in .sh
for file in ${z}/*.sh; do
	echo "${file}" >> handin.txt
done


exit