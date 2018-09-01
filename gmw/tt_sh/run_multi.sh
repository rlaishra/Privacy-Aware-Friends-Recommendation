#!/usr/bin/env bash
if [ ! $# -eq 3 ]; then
  echo "format: ./run_multi.sh num_parties num_inputs num_ref "
  echo "example: ./run_multi.sh 4 10 2 "
  exit
fi

num_parties=$1
num_input=$2
num_ref=$3

gnome-terminal -e "bash -c \"!!; ssh bebe@sucloud /home/bebe/GitLab/gmw/tt_sh/n-hamming-sort-multi.sh $1 $2 $3 2; read -p 'click anything to close the window'  \""  &

sleep 5

./tt_sh/n-hamming-sort-multi.sh $1 $2 $3 2
