#!/bin/bash

if [ $# -lt 1 ];then #Number of numbers either given or 4 :)
    numbers=4;
else
    numbers=$1;
fi;

# Number of processors  
calc=$(echo "(l($numbers)/l(2))+1" | bc -l)
proc=$(python3 -c "from math import ceil; print (ceil($calc))") 

#Number of procesors generated based of input numbers 
#proc=$(echo "(l($numbers)/l(2))+1" | bc -l | xargs printf "%1.0f") 

# Compilation 
mpic++ --prefix /usr/local/share/OpenMPI -o pms pms.cpp

# Generate file with random nums  
dd if=/dev/random bs=1 count=$numbers of=numbers 2> /dev/null	 

mpirun --prefix /usr/local/share/OpenMPI -np $proc --oversubscribe pms 				

rm -f pms numbers					

