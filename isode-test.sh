#!/bin/sh
# A program to test out the isode services etc.
#
# Pretty simple minded - but gives some indications.
#
# Julian Onions <jpo@cs.nott.ac.uk> 15/1/86

# usage : isode-test [ hostname ]
if [ ! -f support/xisoc ]; then
    PATH=$PATH:/usr/local/bin export PATH
    P1= P2=
else
    P1=support/x P2=imisc/x
fi

error=0 fast=0
if [ "x$1" = "x-lpp" ]; then
    fast=1 S=-lpp
    shift
else
    S=
fi

if [ "x$1" = "x-iaed" ]; then
    ECHO= SSAP= RECHO=
    shift
else
    ECHO=echo SSAP=ssap RECHO=ros_echo
fi

IMISC=${P2}imisc${S} ISOC=${P1}isoc

if [ $# -gt 0 ]; then
    host="$1"
    echo "Hostname set to $host"
elif host="`hostname`"; then
    echo "Hostname set to $host"
elif host="`uname`"; then
    echo "Hostname set to $host"
else
    echo -n "I give up, what is your host name? "
    read host
fi

for i in utctime gentime time users chargen qotd finger pwdgen
do
echo "$i:"
$IMISC "$host" $i || error=`expr $error + 1`
done

for i in ping sink echo
do
echo "$i:"
$IMISC -c 100 -l 1020 "$host" $i || error=`expr $error + 1`
done

if [ $fast = 1 ]; then
    echo "Test done, Errors: $error"
    exit $error
fi

for i in $ECHO isode/echo
do
echo "rosap $i:"
$ISOC "$host" rosap $i < /etc/passwd || error=`expr $error + 1`
done

for i in $ECHO $RECHO "isode/rtse echo" isode/ros_echo
do
echo "rtsap $i:"
$ISOC "$host" rtsap "$i" < /etc/passwd || error=`expr $error + 1`
done

for i in $ECHO isode/echo
do
echo "psap $i:"
$ISOC "$host" psap $i < /etc/passwd || error=`expr $error + 1`
done

for i in $SSAP tsap
do
echo "$i echo:"
$ISOC "$host" $i echo < /etc/passwd || error=`expr $error + 1`
done

echo "Test done, Errors: $error"
exit $error
