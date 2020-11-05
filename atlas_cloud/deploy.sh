#!/bin/bash

DEPLOY_DIR=/usr/local/sbin
MOSQUITTO_CONF_DIR=/etc/mosquitto

if [[ $EUID > 0 ]]; then
  echo "Please run this script as root/sudo"
  exit 1
fi

# Verify if mosquitto broker is installed
mosquitto_installed=`systemctl list-units --full -all | grep -i mosquitto`
if [ -z "$mosquitto_installed" ]; then
    echo "Please make sure MQTT Mosquitto broker is installed"
    exit 1
fi

# Verify if mosquitto broker config dir exists
if [ ! -d $MOSQUITTO_CONF_DIR ]; then
    echo "Please make sure the following Mosquitto broker configuration directory exists: $MOSQUITTO_CONF_DIR"
    exit 1
fi

echo "******** Compile MQTT credentials reload wrapper executable ********"
gcc misc/exec/atlas_broker_credentials_exec.c -o atlas_broker_credentials_exec

echo "******** Deploy the MQTT credentials artifcats ********"
mv atlas_broker_credentials_exec $DEPLOY_DIR
cp misc/scripts/atlas_broker_credentials.sh $DEPLOY_DIR
# Set SUID to the credentials script wrapper and proper ownership
chown root:root $DEPLOY_DIR/atlas_broker_credentials.sh
chmod 700 $DEPLOY_DIR/atlas_broker_credentials.sh
chown root:root $DEPLOY_DIR/atlas_broker_credentials_exec
chmod 4755 $DEPLOY_DIR/atlas_broker_credentials_exec

echo "******** Deploy Mosquitto configuration file ********"
cp misc/config/mosquitto/mosquitto.conf $MOSQUITTO_CONF_DIR
chown root:root $MOSQUITTO_CONF_DIR/mosquitto.conf
chmod 644 $MOSQUITTO_CONF_DIR/mosquitto.conf
# Create empty password file
echo -n > $MOSQUITTO_CONF_DIR/mosquitto.passwd
chown root:root $MOSQUITTO_CONF_DIR/mosquitto.passwd
chmod 644 $MOSQUITTO_CONF_DIR/mosquitto.passwd

# Create certs directory
mkdir -p $MOSQUITTO_CONF_DIR/certs
chown root:root $MOSQUITTO_CONF_DIR/certs
chmod 700 $MOSQUITTO_CONF_DIR/certs


