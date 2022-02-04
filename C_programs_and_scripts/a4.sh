#! /bin/bash
#Ensure shell script is run with bash.

#Check if arg was passed.
if [ $# -eq 0 ]; then
	echo "Please choose a directory or file."
	exit 1
fi

#===================================================
#Loop through all args
#Check if arg[i] is directory. If so,
#	execute all files in directory.
#Else 
#	execute single file.
#===================================================
for i in $*; do
	if [ -d $i ]; then
		for i in $i/*; do
			. "$i" 2> /dev/null
		done
	else
		. "$i" 2> /dev/null
	fi
done
