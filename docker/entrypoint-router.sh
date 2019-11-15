#!/bin/sh
set -x

echo "config router start ..."

echo "config router succeed"

cat router.json
# start router
# zmq
./router
