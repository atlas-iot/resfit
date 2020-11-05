#!/bin/bash

echo "******** Create ATLAS Gateway server certificate ********"
echo "Please enter the server FQDN:"

read fqdn

FQDN=$fqdn openssl req -x509 -new -nodes -newkey rsa:2048 -nodes -keyout $fqdn.key -sha256 -days 1024 -out $fqdn.crt -config openssl_server.cnf

