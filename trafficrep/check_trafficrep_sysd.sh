#!/bin/sh

DBGFILE=/usr/local/log/check_trafficrep_sysd.dbg
DATE=`date +'%F %T'`

if systemctl is-active --quiet trafficrep; then
    NDEAD=`netstat -a -t --numeric-hosts | grep http | grep _WAIT -c`;
	if [ "$NDEAD" -gt "32" ]; then
		echo $DATE "Too many dead connections - restarting" >> $DBGFILE
		systemctl stop trafficrep
		# 30 s should be enough for the kernel to release the port
		# but allow some time for threads to exit
		sleep 180
		systemctl start trafficrep
	fi
else
	echo $DATE "Inactive - restarting " >> $DBGFILE
	systemctl restart trafficrep
fi

exit 0
