#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

/* This executable is just a wrapper for the /usr/local/sbin/atlas_broker_credentials.sh script
which needs root permission in order to send a SIGHUP signal to the cloud mosquitto broker (to reload config) */
int main(int argc, char **argv)
{
    /* argv[1] indicates the path to the newly created (temporary) mosquitto credentials file*/
    char *args[] = {"/usr/local/sbin/atlas_broker_credentials.sh", argv[1], NULL};

    /* Set UID to root */
     if (setuid(0)) {
         printf("An error occured when assuming the root identity\n");
         return 1;
     }

    /* Execute the credentials reload script */
    if (execvp(args[0], args) == -1) {
        printf("An error occured when executing the credentials reload script\n");
        return 1;
    }
    
    return 0;
}
