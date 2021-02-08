#!/bin/sh -x



cd ../../..

cnt=200

trace=`mktemp /tmp/$STREAMS_INSTANCE_ID.vthreadpool.XXXXXX`

time -p ./vthreadpool --hithread 128 --lothread 2 --item $cnt -DDDDD --Xdebug-file $trace
rc=$?

if test $rc -ne 0 ; then

    tar cvzf $STREAMS_UPLOAD_RESULTS $trace

fi

rm -vf $trace

exit $rc
