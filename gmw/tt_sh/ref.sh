#!/usr/bin/env bash
if [ ! $# -eq 3 ]; then
  echo "format: ./run.sh num_parties len_vectors num_reference_parties"
  echo "example: ./run.sh 4 10 2"
  exit
fi

num_parties=$1
len_vectors=$2
num_ref_parties=$3
echo "num_parties=$num_parties"
echo "len_vectors=$len_vectors"
echo "num_ref_parties=$num_ref_parties"

if [ $num_parties -lt 2 ]; then
	echo "parties number should larger than 2"
	exit
fi

if [ $num_ref_parties -lt 1 ]; then
	echo "reference parties number should larger than 1"
	exit
fi

circuit="hammsort-ref"
input_dir="config_$circuit"
mkdir -p $input_dir

#config-auto
mkdir -p $input_dir
rm $input_dir/config*.txt
rm $input_dir/address.txt
rm $input_dir/inputs*.txt
port_init=6676

#cacluate parties number length($ptlen)
((number=$num_parties-1))
ptlen=0
while [ $number -gt 0 ]; do
	((number=$number/2))
	((ptlen=$ptlen+1))
done

#create input files
for ((pid = 0; pid < $num_parties; pid++))
do
	#write the reference bit into temp as the first bit
	if [ $pid -lt $num_ref_parties ]; then
		temp="0" #reference parties
	else
		temp="1"
	fi

	#random generate len_vectors bits and write into temp
	for ((len=0;len<$len_vectors;len++))
	do
		((temp_num=$RANDOM%2))
		temp+=" "$temp_num
	done
	
	#write pid as binary string into temp
	number=$pid
	for ((i = $ptlen-1; i >= 0; i--))
	do
		((buff[$i]=$number%2))
		((number=$number/2))
	done
	for ((i=0; i<$ptlen; i++))
	do
		temp+=" "${buff[$i]}
	done
	echo "$temp" >> $input_dir/inputs$((pid+1)).txt
done

#create config files
COUNTER=1
while [ $COUNTER -lt $((num_parties+1)) ]; do
    rm -f temp2 && cat > temp2 <<MYEOF
seed 4373984170943781378434349  

p 8936097950764538541647693880373941060412422053330581106416547022143872696986309618392492140173286901378451169263228782167065246660381840799235731486130087

g 7660915846360464914746169975675514711978996378800371841846530440188167304050642048045944447898172503094027848348928324282770185052123039167705188057761352
MYEOF

echo " " >> $input_dir/configs${COUNTER}.txt
    echo "num_parties $num_parties" >> $input_dir/configs${COUNTER}.txt
    echo "pid $((COUNTER - 1))" >> $input_dir/configs${COUNTER}.txt
    echo -e "address-book\t$input_dir/address.txt" >> $input_dir/configs${COUNTER}.txt
    echo "create-circuit $circuit $num_parties $len_vectors $num_ref_parties" >> $input_dir/configs${COUNTER}.txt
    echo "" >> $input_dir/configs${COUNTER}.txt
    echo "num_input $((1+len_vectors+ptlen))" >> $input_dir/configs${COUNTER}.txt

    echo -e "input\t$input_dir/inputs$COUNTER.txt\n" >> $input_dir/configs${COUNTER}.txt
    cat temp2 >> $input_dir/configs${COUNTER}.txt
    #add address
    echo -e "$((COUNTER - 1))\t127.0.0.1\t$((port_init+COUNTER-1))" >> $input_dir/address.txt
    echo $COUNTER
    let COUNTER=COUNTER+1
done

sleep 0.5

#execute
COUNTER=1
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

