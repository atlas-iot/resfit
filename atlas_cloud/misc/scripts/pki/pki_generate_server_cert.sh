#!/bin/bash

echo "******** Create server certificate ********"
echo "Please enter the server FQDN:"

read fqdn

FQDN=$fqdn openssl req -out ca/intermediate-server/csr/$fqdn.csr.pem -newkey rsa:2048 -nodes -keyout ca/intermediate-server/private/$fqdn.key.pem -config openssl_server.cnf
openssl ca -config openssl_intermediate_server_subca.cnf -extensions server_cert -days 1800 -notext -md sha512 -in ca/intermediate-server/csr/$fqdn.csr.pem -out ca/intermediate-server/certs/$fqdn.crt.pem

mkdir -p artifacts/servers/$fqdn
cp ca/intermediate-server/private/$fqdn.key.pem artifacts/servers/$fqdn
cp ca/intermediate-server/certs/$fqdn.crt.pem artifacts/servers/$fqdn
cat ca/intermediate-server/certs/int.atlas-server-subca.crt.pem ca/certs/ca.atlas-root.crt.pem > artifacts/servers/$fqdn/$fqdn.chain.pem
