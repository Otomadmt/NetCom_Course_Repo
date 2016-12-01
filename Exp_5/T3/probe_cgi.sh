#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "<html><body><table border="1"><tr><th>Server</th><th>Status</th></tr>"
cat server.list | while read ip; 
do
	ping -c 1 -w 1 ${ip} > /dev/null
	if [ $? = "0" ]; then
		echo "<tr><td>${ip}</td><td>Online</td></tr>"
	else
		echo "<tr><td>${ip}</td><td>Offline</td></tr>"
	fi
done

echo "</body></html>"
