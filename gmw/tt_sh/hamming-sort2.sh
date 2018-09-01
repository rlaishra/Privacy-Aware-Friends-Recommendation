#!/usr/bin/env bash
if [ ! $# -eq 0]; then
  echo "format: ./run.sh"
  echo "example: ./run.sh"
  exit
fi

num_parties=$1

debug=0;

if [ "$num_ref" -gt "$num_parties" ]; then
  echo "Number of reference parties cannot exceed number parties"
  exit
fi

echo "num_parties=$num_parties"
echo "num_ref=$num_ref"


#config-auto
circuit="hamming-sort2"
input_dir="config_hamming_sorting2"


sleep 0.5

#execute
COUNTER=1
if [ $debug -eq 0 ]
then
	while [ $COUNTER -lt $num_parties ]; do
	    echo "===== [DEBUG] run party: $COUNTER -- $input_dir/configs${COUNTER}.txt ===="
	    #no windows
	    #./mpc.exe $input_dir/configs${COUNTER}.txt &
	    #with windows, but close automatically at the end
	    gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt  \""  &
	    #with windows, but need to close manually at the end
	    #gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
	    sleep 0.25
	    let COUNTER=COUNTER+1
	done
	echo "===== [DEBUG] run party: $COUNTER -- $input_dir/configs${COUNTER}.txt ===="
	gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt; read -p 'click anything to close the window'  \""  &

elif [ $debug -eq 1 ]
then
	while [ $COUNTER -le $num_parties ]; do
	    echo "===== [DEBUG] run party: $COUNTER -- $input_dir/configs${COUNTER}.txt ===="
	    #no windows
	    #./mpc.exe $input_dir/configs${COUNTER}.txt &
	    #with windows, but close automatically at the end
	    #gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt  \""  &
	    #with windows, but need to close manually at the end
	    gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
	    sleep 0.25
	    let COUNTER=COUNTER+1
	done
fi
