#!/usr/bin/env bash
if [ ! $# -eq 3 ]; then
  echo "format: ./config_hmm.sh file1 file2 no_of_lines"
#  echo "example: ./run.sh 4 10"
  exit
fi

file1=$1
file2=$2
num_parties=3

circuit="test-add"
input_dir="config_hamming_jain"
mkdir -p $input_dir

#config-auto
mkdir -p $input_dir
rm $input_dir/config*.txt
rm $input_dir/address.txt
port_init=6676

 #worker config
tt_sh/cutoff.py -i1 $file1 -i2 $file2 -i3 $3 
touch $input_dir/input3.txt
tt_sh/converter2Binary.py -i1 intermediate1.txt -i2 intermediate2.txt
a=($(wc input1.txt))
lines=${a[0]}
words=${a[1]}
mv input1.txt input2.txt intermediate1.txt intermediate2.txt $input_dir/

COUNTER=1
while [ $COUNTER -lt $num_parties ]; do
    rm -f temp2 && cat > temp2 <<MYEOF
seed 4373984170943781378434349  

p 8936097950764538541647693880373941060412422053330581106416547022143872696986309618392492140173286901378451169263228782167065246660381840799235731486130087

g 7660915846360464914746169975675514711978996378800371841846530440188167304050642048045944447898172503094027848348928324282770185052123039167705188057761352
MYEOF

    echo " " >> $input_dir/configp${COUNTER}.txt
    echo "num_parties $num_parties" >> $input_dir/configp${COUNTER}.txt
    echo "pid $((COUNTER - 1))" >> $input_dir/configp${COUNTER}.txt
    echo -e "address-book\t$input_dir/address.txt" >> $input_dir/configp${COUNTER}.txt
    echo "create-circuit $circuit $lines 1 75 $words" >> $input_dir/configp${COUNTER}.txt
    echo "" >> $input_dir/configp${COUNTER}.txt
    echo "num_input $words" >> $input_dir/configp${COUNTER}.txt

    echo -e "input\t$input_dir/input${COUNTER}.txt\n" >> $input_dir/configp${COUNTER}.txt
    cat temp2 >> $input_dir/configp${COUNTER}.txt
    #add address
    echo -e "$((COUNTER - 1))\t127.0.0.1\t$((port_init+COUNTER-1))" >> $input_dir/address.txt
    echo $COUNTER
    let COUNTER=COUNTER+1
done

 #consumer config
num_workers=$((num_parties - 1))
index=0

    rm -f temp2 && cat > temp2 <<MYEOF
seed 4373984170943781378434349  

p 8936097950764538541647693880373941060412422053330581106416547022143872696986309618392492140173286901378451169263228782167065246660381840799235731486130087

g 7660915846360464914746169975675514711978996378800371841846530440188167304050642048045944447898172503094027848348928324282770185052123039167705188057761352
MYEOF

    echo " " >> $input_dir/configp3.txt
    echo "num_parties $num_parties" >> $input_dir/configp3.txt
    echo "pid $((3 - 1))" >> $input_dir/configp3.txt
    echo -e "address-book\t$input_dir/address.txt" >> $input_dir/configp3.txt
    echo "create-circuit $circuit $lines 1 75 $words" >> $input_dir/configp3.txt
    echo "" >> $input_dir/configp3.txt
    echo "num_input 0" >> $input_dir/configp${COUNTER}.txt

    echo -e "input\t$input_dir/input3.txt\n" >> $input_dir/configp3.txt
    cat temp2 >> $input_dir/configp3.txt
    #add address
    echo -e "$((3 - 1))\t127.0.0.1\t$((port_init+3-1))" >> $input_dir/address.txt


sleep 0.5

#execute


COUNTER=1
while [ $COUNTER -lt $((num_parties + 1)) ]; do
    xterm -e "bash -c \" ./mpc.exe $input_dir/configp${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
    #gnome-terminal -e "bash -c \" ./mpc.exe $input_dir/configp${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
    #gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
    sleep 0.25
    let COUNTER=COUNTER+1 
done




