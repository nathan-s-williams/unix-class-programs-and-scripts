#! /bin/bash
#Ensure function is run using bash.

if (( $# == 0 )); then	#Validate initial argument list. If none, prompt user.
	echo "Please enter a directory to recursively search."
	exit 1
fi

#Recursive function used to traverse a given directory.
function recursivesearch() { 
	
	cd "$1"			#Move into $1 directory and print full path.
	echo "$(pwd "$1"):"
	local current=$(pwd)	#Set local variables for printing current directory and listing directories files. 
	local list=$(echo *)	
	if [[ $list == "*" ]]; then	#If list is empty, print a blank instead of "*" after variable expansion.
		echo
		return 0
	fi
	echo "$list"		#Print directories files.
	echo
	
	local chk=""		#Set local variable for output validation.
	for i in $list; do	#Loop through 
		cd $current	#Move back to directory for which we are listing its subfiles.
		if [[ $chk == $i ]]; then #If directory was already traversed but reentered due 
					  #to loop/recursion, skip it.
			continue
		fi
		if [ ! -L "$i" ]; then	  #If i is not a simlink and it is a directory, 
					  #then recursively call function	
			if [ -d "$i" ]; then
				chk="$i"
				recursivesearch "$i"
			fi
		fi
	done
	
}

recursivesearch $1
