#!/usr/bin/env bash
if [ ! $# -eq 4 ]; then
  echo "format: ./run.sh num_parties num_inputs num_ref debug"
  echo "example: ./run.sh 4 10 2 1"
  exit
fi

num_parties=$1
num_input=$2
num_ref=$3
debug=$4


if [ "$num_ref" -gt "$num_parties" ]; then
  echo "Number of reference parties cannot exceed number parties"
  exit
fi

echo "num_parties=$num_parties"
echo "num_ref=$num_ref"
echo "num_half=$num_half"


#config-auto
circuit="hamming-sort2"
input_dir="config_hamming_sorting2/gen"
mkdir -p $input_dir
rm $input_dir/config*.txt
rm $input_dir/address.txt
rm $input_dir/inputs*.txt
port_init=6676

#calculate partylength
((number=$num_parties-1))
partylength=1
while [ $number -gt 1 ];do
	((number=$number / 2))
	((partylength=$partylength+1))
done


  rand=0
  count=0
  #create input files
  for((partyid=0;partyid<$num_parties;++partyid))
  do

  	#((rand=$RANDOM % 2))
    temp="0 "
  	#if [ "$rand" == 1 ] && [ $count != $num_ref ]; then
    if [ $count != $num_ref ]; then
  	   temp="1 "
       ((count=$count+1))
       #echo $count
    fi

  	range=2
  	#temp=""
  	number=$RANDOM
  	((number= number % range))
  	temp+=$number

  	for ((i=1;i<$num_input;++i))
  	do
  		number=$RANDOM
  		((number= number % range))
  		temp+=" "$number
  	done


  	leng=1
  	number=$partyid
  	while [ $number -gt 1 ];do
  		((a=$number % 2))
  		b[$leng]=$a
  		((number=$number / 2))
  		((leng=$leng+1))
  	done
  	b[$leng]=$number
  	#echo length $leng

  	for((i=$partylength;i>$leng;--i))
  	do
  		temp+=" "0
  	done

  	for((i=$leng;i>0;--i))
  	do
  		p=${b[$i]}
  		temp+=" "$p
  	done


  	echo "$temp" >>$input_dir/inputs$((partyid+1)).txt
  	echo $partyid - $temp
  done



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
    echo "create-circuit $circuit $num_input $num_ref" >> $input_dir/configs${COUNTER}.txt
    echo "" >> $input_dir/configs${COUNTER}.txt
    echo "num_input $((num_input+partylength+1))" >> $input_dir/configs${COUNTER}.txt
    #echo "num_input $((num_input))" >> $input_dir/configs${COUNTER}.txt


    echo -e "input\t$input_dir/inputs$COUNTER.txt\n" >> $input_dir/configs${COUNTER}.txt
    cat temp2 >> $input_dir/configs${COUNTER}.txt



    #add address
    echo -e "$((COUNTER - 1))\t127.0.0.1\t$((port_init+COUNTER-1))" >> $input_dir/address.txt


    #echo $COUNTER
    let COUNTER=COUNTER+1
done


sleep 2

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
	    #gdb ./mpc.exe $input_dir/configs${COUNTER}.txt &
	    #with windows, but close automatically at the end
	    #gnome-terminal -e "bash -c \"!!; ./mpc.exe \""  &
	    #with windows, but need to close manually at the end
	    gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
	    #gnome-terminal -e "bash -c  \"!!; gdb ./mpc.exe; \"" &
      #gnome-terminal -e "bash -c \"(echo run $input_dir/configs${COUNTER}.txt; cat ) | gdb ./mpc.exe; \""
      #echo "run $input_dir/configs${COUNTER}.txt"


	    sleep 0.25
	    let COUNTER=COUNTER+1
	done

elif [ $debug -eq 2 ]
then
	while [ $COUNTER -le $num_parties ]; do
	    echo "===== [DEBUG] run party: $COUNTER -- $input_dir/configs${COUNTER}.txt ===="
	    #no windows
	    ./mpc.exe $input_dir/configs${COUNTER}.txt &
	    #with windows, but close automatically at the end
	    #gnome-terminal -e "bash -c \"!!; ./mpc.exe \""  &
	    #with windows, but need to close manually at the end
	    #gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
	    #gnome-terminal -e "bash -c  \"!!; gdb ./mpc.exe; \"" &
      #gnome-terminal -e "bash -c \"(echo run $input_dir/configs${COUNTER}.txt; cat ) | gdb ./mpc.exe; \""
      #echo "test"
      #echo "run $input_dir/configs${COUNTER}.txt"


	    sleep 0.25
	    let COUNTER=COUNTER+1
	done
elif [ $debug -eq 4 ]
then
	while [ $COUNTER -le $num_parties ]; do
	    echo "===== [DEBUG] run party: $COUNTER -- $input_dir/configs${COUNTER}.txt ===="
	    #no windows
	    #gdb ./mpc.exe $input_dir/configs${COUNTER}.txt &
	    #with windows, but close automatically at the end
	    #gnome-terminal -e "bash -c \"!!; ./mpc.exe \""  &
	    #with windows, but need to close manually at the end
	    #gnome-terminal -e "bash -c \"!!; ./mpc.exe $input_dir/configs${COUNTER}.txt; read -p 'click anything to close the window'  \""  &
	    #gnome-terminal -e "bash -c  \"!!; gdb ./mpc.exe; \"" &
      gnome-terminal -e "bash -c \"( echo catch syscall 231; echo catch signal SIGKILL; echo run $input_dir/configs${COUNTER}.txt; cat ) | gdb ./mpc.exe core; \""
      #echo "run $input_dir/configs${COUNTER}.txt"


	    sleep 0.25
	    let COUNTER=COUNTER+1
	done



fi
