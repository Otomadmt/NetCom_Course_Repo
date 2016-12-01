$TTL 1D

@ 	IN	SOA 	dns 	admin (
	20	;SERIAL number for zone update transfer
	1D	;REFRESH time interval before slave refresh zone
	1H	;RETRY time interval before failed REFRESH retry
	3D	;EXPIRE time interval before slave zone no longer authoritative
	3H	;MINIMUM time before negative cache expired
)
@ 	IN 	NS 	dns

dns	IN	A	172.16.13.132
www2	IN	A	13.107.21.200
ftp	IN 	A	172.16.13.161
www	IN	A	172.16.13.132
