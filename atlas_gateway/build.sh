#!/bin/bash
echo "THIS SCRIPT WILL NOW BUILD THE ATLAS_GATEWAY SECURITY COMPONENTS"

# Create Build directory
echo "*********** Step 1. Creating build directory ************"
mkdir build
echo "******************* Step 1 finished! ********************"

# Building project
echo "******* Step 2. Building ATLAS_GATEWAY components *******"
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j `nproc`
echo "******************* Step 2 finished! ********************"

# Finishing ATLAS_GATEWAY configuration
echo "*** Step 3. Finishing configuration of ATLAS_GATEWAY ****"
cp ../src/mosquitto_plugin/config/mosquitto-atlas.conf .
CONFIG_FILE=mosquitto-atlas.conf
echo "$(grep -v 'auth_plugin' $CONFIG_FILE)" >$CONFIG_FILE
echo "auth_plugin $(pwd)/libatlas_gateway_mosquitto_plugin.so" >>$CONFIG_FILE
echo "******************* Step 3 finished! ********************"

# Generate ATLAS gateway HTTPS server
echo "********** Step 4. Generate HTTPS certificate ***********"
cd ../scripts/pki
./pki_generate_server_cert.sh
cp *.crt *.key ../../build
rm *.crt *.key
echo "******************** Step 4 finished! *******************"

echo "  ALL ATLAS_GATEWAY COMPONENTS HAVE BEEN BUILT"
