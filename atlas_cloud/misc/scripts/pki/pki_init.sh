#!/bin/bash

# Cleanup
rm -rf ca
rm -rf artifacts
mkdir -p artifacts/servers artifacts/clients

echo "******** Create ROOT CA ********"
mkdir -p ca/newcerts ca/certs ca/crl ca/private ca/requests
touch ca/index.txt
touch ca/index.txt.attr
echo '1000' > ca/serial

openssl genrsa -out ca/private/ca.atlas-root.key.pem 4096
openssl req -config openssl_root.cnf -new -x509 -sha512 -extensions v3_ca -key ca/private/ca.atlas-root.key.pem -out ca/certs/ca.atlas-root.crt.pem -days 3650 -set_serial 0

echo "******** Create intermediate server subCA ********"
mkdir -p ca/intermediate-server/certs ca/intermediate-server/newcerts ca/intermediate-server/crl ca/intermediate-server/csr ca/intermediate-server/private
touch ca/intermediate-server/index.txt
touch ca/intermediate-server/index.txt.attr
echo 1000 > ca/intermediate-server/crlnumber
echo '1234' > ca/intermediate-server/serial

openssl req -config openssl_intermediate_server_subca.cnf -new -newkey rsa:4096 -keyout ca/intermediate-server/private/int.atlas-server-subca.key.pem -out ca/intermediate-server/csr/int.atlas-server-subca.csr -nodes
openssl ca -config openssl_root.cnf -extensions v3_intermediate_ca -days 3650 -notext -md sha512 -in ca/intermediate-server/csr/int.atlas-server-subca.csr -out ca/intermediate-server/certs/int.atlas-server-subca.crt.pem

# Create client trust store
cat ca/intermediate-server/certs/int.atlas-server-subca.crt.pem ca/certs/ca.atlas-root.crt.pem > artifacts/client.truststore.pem

echo "******** Create intermediate client subCA ********"
mkdir -p ca/intermediate-client/certs ca/intermediate-client/newcerts ca/intermediate-client/crl ca/intermediate-client/csr ca/intermediate-client/private
touch ca/intermediate-client/index.txt
touch ca/intermediate-client/index.txt.attr
echo 1000 > ca/intermediate-client/crlnumber
echo '1234' > ca/intermediate-client/serial

openssl req -config openssl_intermediate_client_subca.cnf -new -newkey rsa:4096 -keyout ca/intermediate-client/private/int.atlas-client-subca.key.pem -out ca/intermediate-client/csr/int.atlas-client-subca.csr -nodes
openssl ca -config openssl_root.cnf -extensions v3_intermediate_ca -days 3650 -notext -md sha512 -in ca/intermediate-client/csr/int.atlas-client-subca.csr -out ca/intermediate-client/certs/int.atlas-client-subca.crt.pem

# Create server truststore
cat ca/intermediate-client/certs/int.atlas-client-subca.crt.pem ca/certs/ca.atlas-root.crt.pem > artifacts/server.truststore.pem

echo "********Generate server (web and MQTT) certificate********"
./pki_generate_server_cert.sh

echo "********Generate client certificate********"
./pki_generate_client_cert.sh

