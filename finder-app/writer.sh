#!/bin/sh

writefile=$1
writestr=$2

# check if any of the arguments isn't passed to the script
# and check if the filesdir is a directory
if [ $# -lt 2 ] 
then
    echo "all required arguments are not specified"
    exit 1
fi

# get directory of the passed file
file_dire=$( dirname $writefile )

# check if the file location exists or not
if [ -d ${file_dire} ]
then
    echo ${writestr} > ${writefile}
else
    mkdir -p ${file_dire} && echo ${writestr} > ${writefile}
fi

# check state of the last operation has failed
if [ $? -eq 1 ]
then
    echo "${writefile} file could not be created."
    exit 1
fi
