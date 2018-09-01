#!/usr/bin/env bash
if [ ! $# -eq 2 ]; then
  echo "format: ./run.sh num_parties num_inputs"
  echo "example: ./run.sh 4 10"
  exit
fi

num_parties=$1
num_input=$2
echo "num_parties=$num_parties"

circuit="p2p"
input_dir="config_$circuit"
mkdir -p $input_dir

#config-auto
mkdir -p $input_dir
rm $input_dir/config*.txt
rm $input_dir/address.txt
port_init=6676

 #worker config
COUNTER=0
while [ $COUNTER -lt $num_input ]; do
    worker_input="0 "$worker_input
    let COUNTER=COUNTER+1 
done

echo $worker_input > $input_dir/inputs1000.txt

COUNTER=1
while [ $COUNTER -lt $num_parties ]; do
    rm -f temp2 && cat > temp2 <<MYEOF
seed 4373984170943781378434349  

p 8936097950764538541647693880373941060412422053330581106416547022143872696986309618392492140173286901378451169263228782167065246660381840799235731486130087

g 7660915846360464914746169975675514711978996378800371841846530440188167304050642048045944447898172503094027848348928324282770185052123039167705188057761352
MYEOF

    echo " " >> $input_dir/configs${COUNTER}.txt
    echo "num_parties $num_parties" >> $input_dir/configs${COUNTER}.txt
    echo "pid $((COUNTER - 1))" >> $input_dir/configs${COUNTER}.txt
    echo -e "address-book\t$input_dir/address.txt" >> $input_dir/configs${COUNTER}.txt
    echo "create-circuit $circuit $num_input 65535" >> $input_dir/configs${COUNTER}.txt
    echo "" >> $input_dir/configs${COUNTER}.txt
    echo "num_input $num_input" >> $input_dir/configs${COUNTER}.txt

    echo -e "input\t$input_dir/inputs1000.txt\n" >> $input_dir/configs${COUNTER}.txt
    cat temp2 >> $input_dir/configs${COUNTER}.txt
    #add address
    echo -e "$((COUNTER - 1))\t127.0.0.1\t$((port_init+COUNTER-1))" >> $input_dir/address.txt
    echo $COUNTER
    let COUNTER=COUNTER+1
done

 #consumer config
num_workers=$((num_parties - 1))
if [ $circuit == "p2p" ]; then
    total_input=$((num_workers * num_input))
else
    total_input=$((num_input))
fi

index=0
while [ $index -lt $total_input ]; do
    consumer_input="0 "$consumer_input
    let index=index+1
done

rm -f $input_dir/inputc.txt
echo $consumer_input > $input_dir/inputc.txt

    rm -f temp2 && cat > temp2 <<MYEOF
seed 4373984170943781378434349  

p 8936097950764538541647693880373941060412422053330581106416547022143872696986309618392492140173286901378451169263228782167065246660381840799235731486130087

g 7660915846360464914746169975675514711978996378800371841846530440188167304050642048045944447898172503094027848348928324282770185052123039167705188057761352
MYEOF

    echo " " >> $input_dir/configc.txt
    echo "num_parties $num_parties" >> $input_dir/configc.txt
    echo "pid $((num_parties-1))" >> $input_dir/configc.txt
    echo -e "address-book\t$input_dir/address.txt" >> $input_dir/configc.txt
    echo "create-circuit $circuit $num_input 65535" >> $input_dir/configc.txt
    echo "" >> $input_dir/configc.txt
    echo "num_input $total_input" >> $input_dir/configc.txt
    echo -e "input\t$input_dir/inputc.txt\n" >> $input_dir/configc.txt
    cat temp2 >> $input_dir/configc.txt
    echo -e "$((num_parties-1))\t127.0.0.1\t$((port_init+num_parties-1))" >> $input_dir/address.txt

sleep 0.5

#execute
./tt_sh/run.sh $num_parties $input_dir

