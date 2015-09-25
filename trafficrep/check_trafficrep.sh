#!/bin/sh

PIDFILE=/var/run/trafficrep.pid
EXE="/usr/local/bin/trafficrep -l /usr/local/log/trafficrep.log"
DBGFILE=/usr/local/log/check_trafficrep.dbg

DATE=`date +'%F %T'`

# If there is no pid file then start it
if [ ! -e $PIDFILE ]; then
	nohup $EXE -p $PIDFILE >/dev/null 2>&1 &
	echo $DATE "Started" >> $DBGFILE
else
	PID=`cat ${PIDFILE}`
	if [ -e /proc/$PID ]; then
		# Running, so check if there are too many dead connections
		NDEAD=`netstat -a -t --numeric-hosts | grep http | grep _WAIT -c`;
		if [ "$NDEAD" -gt "32" ]; then
			echo $DATE "Too many dead connections - restarting" >> $DBGFILE
			kill $PID
			sleep 5
			# Sometimes, it just won't die ... a thready thingy ...
			kill $PID
			# Wait a while for the port to be released
			sleep 180;
			nohup $EXE -p $PIDFILE >/dev/null 2>&1 &
		fi
	else 
		# Not running any more so start it
		echo $DATE "Not running" >> $DBGFILE
		nohup $EXE -p $PIDFILE >/dev/null 2>&1 &
	fi
fi
exit 0