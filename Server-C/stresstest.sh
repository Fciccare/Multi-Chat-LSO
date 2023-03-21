#/bin/bash

#value=$(nc 127.0.0.1 9192 -v < stress.txt)#-v verbose, -k keep connection

#echo $value

i=0

while [[ i==0 ]]
do
	echo '[LGN]a<>a'
	sleep 1
    echo '[CRT]Stanza'
    sleep 1
	i=1
done | nc localhost 9192


