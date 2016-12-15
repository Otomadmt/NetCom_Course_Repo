#!/bin/sh
file="/var/www/html/index.html"
if [ -e ${file} ]; then
	cat /dev/null > ${file}
fi
echo "<table border="1"><tr><th>Server</th><th>Status</th></tr>" >> ${file}
cat server.list | while read ip; 
do
	ping -c 1 -w 1 ${ip} > /dev/null
	if [ $? = "0" ]; then
		echo "<tr><td>${ip}</td><td>Online</td></tr>" >> ${file}
	else
		echo "<tr><td>${ip}</td><td>Offline</td></tr>" >> ${file}
	fi
done
