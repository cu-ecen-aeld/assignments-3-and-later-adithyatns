#!/bin/bash 

# first argument path to a directory, filesdir
filesdir=$1 
# second argument is text string to be searched within these file. searchstr
searchstr=$2

echo "${filesdir} and ${searchstr}"

if [ $# -lt 2 ]
then
  echo "usage: ./finder.sh <dir_name> <text_to_find>"
  exit 1
fi

if [ -d ${filesdir} ]
then
  echo ""
else
  echo "directory ${filesdir} does not exist"
  exit 1
fi
Y=$(grep -a ${searchstr} -r ${filesdir} | wc -l)
X=$(find ${filesdir} -type f| wc -l)

  echo "The number of files are ${X} and the number of matching lines are ${Y}" 

