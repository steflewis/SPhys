#!/bin/bash
FILE=/home/Stefanie/NestedSampling/BeamRecoil/testme.out
echo "Entering shell script" >>$FILE
[3]<>$FILE

#exec 3&>1

flag=0
hour=$((`date +%H`))
min=$((`date +%M` + 2))
#DIR=/home/Stefanie/NestedSampling/BeamRecoil/text_files/
for NTH in 1 2 4 8 16 32 64
do
     scons dev=GPU impl=GPU iters=50000 data=${DIR}datatest_100000_1.txt nth=$NTH
#     #FILE=/home/Stefanie/NestedSampling/BeamRecoil/times_ocl_gpu_test_100000_${NTH}thr.out
#     #if [ -e $FILE ];
#     #then
#     #  echo "File $FILE exists"
#     #  rm $FILE
#     #fi
#     
#     for (( c=1; c<=10; c++ ))
#     do
#while [ $flag=0 ]; do
  
  
  if [[ $(date +%H) -lt $hour || $(date +%M) -lt $min ]]; then
    flag=0
  else
    flag=1
    echo "Time is up." >>$FILE
    exit
  fi
      # # 	  
      # # 	  
  echo "Hurray $NTH" >>$FILE 
  sleep 30s
      # # #      { time ./usercode; } 2>>$FILE
done
      
#      
#     echo Done $NTH Threads
#       
# done
#       
      