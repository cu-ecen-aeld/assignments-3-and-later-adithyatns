#!/bin/bash 

writefile=$1
writestr=$2

#echo "${writefile}"
#echo "${writestr}"

if [[ $# -lt 2 ]]
then
  echo "usage: ./finder.sh <dir_path> <text_to_find>"
  exit 1
fi

if [[ -e ${writefile} ]]
then
  echo "the file already exists"
else
 # echo "writing ${writestr} to ${writefile}"
  mkdir -p $(dirname ${writefile})
  touch  ${writefile}
  echo "${writestr}" > ${writefile}
fi

