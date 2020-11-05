#/bin/bash

MOSQUITTO_PASSWD_FILE_PATH=/etc/mosquitto/mosquitto.passwd

mv $1 $MOSQUITTO_PASSWD_FILE_PATH
if [ $? -ne 0 ]; then
    echo "An error occured when moving the newly created credentials file into the mosquitto.passwd file"
    exit 1
fi

# Set back root ownership to credentials file
chown root:root $MOSQUITTO_PASSWD_FILE_PATH
if [ $? -ne 0 ]; then
    echo "An error occured when setting root ownership to the mosquitto.passwd file"
    exit 1
fi

# Set R/W permission to credentials file
chmod 644 $MOSQUITTO_PASSWD_FILE_PATH
if [ $? -ne 0 ]; then
    echo "An error occured when setting R/W permissions to the mosquitto.passwd file"
    exit 1
fi

# If there are multiple mosquitto brokers installed, choose the one which is a service
pid=`systemctl show --property MainPID --value mosquitto`
if [ $? -ne 0 ]; then
    echo "An error occured when obtaining the mosquitto service PID"
    exit 1
fi

# Reload the password file on the mosquitto broker
kill -HUP $pid
if [ $? -ne 0 ]; then
    echo "An error occurent when reloading the mosquitto broker credentials file"
    exit 1
fi
