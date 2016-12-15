#!/bin/sh
if [ -e probe.txt ]; then
	cat /dev/null > probe.txt
fi
cat server.list | while read ip; 
do
	ping -c 1 -w 1 ${ip} > /dev/null
	if [ $? = "0" ]; then
		echo "${ip} Online" >> probe.txt
	else
		echo "${ip} Offline" >> probe.txt
	fi
done
