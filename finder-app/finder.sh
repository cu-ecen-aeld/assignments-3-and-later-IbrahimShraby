#!/bin/sh

filesdir=$1
searchstr=$2

# check if any of the arguments isn't passed to the script
# and check if the filesdir is a directory
echo ${filesdir} ${searchstr} $#
if [ $# -lt 2 ] 
then
    echo "all required arguments are not specified"
    exit 1
fi

# calculate the number of files that contain the required searchstr
X=$( grep -r -l ${searchstr} ${filesdir} | wc -l )

# calculate the number of time the required searchstr got matched
Y=$( grep -r ${searchstr} ${filesdir} | wc -l )

# print a message "The number of files are X and the number of matching lines are Y" 
echo "The number of files are ${X} and the number of matching lines are ${Y}" 
