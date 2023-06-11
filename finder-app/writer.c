#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    // open a syslog stream
    openlog(NULL, 0, LOG_USER);
    // validate the number of passed arguments "must be 3"
    if(argc != 3)
    {
        // report error and exit
        syslog(LOG_ERR, "Invalid number of arguments: %d", argc);
        return 1;
    }
    // create a file and write a string to it
    const char *writefile = argv[1];
    char *writestr = argv[2];
    // check if the file is created successfully
    FILE *file = fopen(writefile, "w+");
    //  1- if true : write a debugging message using syslog
    //  2- if false : write and error message using syslog and exit
    if(file == NULL)
    {
        syslog(LOG_ERR, "Value of errno attempting to open a file: %s is: %d", writefile, errno);
        return 1;
    }
    else
    {
        fprintf(file, "%s", writestr);
        fclose(file);
        syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
    }
    return 0;
}
