#include "systemcalls.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int ret;
    bool system_status;

    ret = system(cmd);
    if(ret == 0)
    {
        system_status = true;
    }
    else
    {
        system_status = false;
    }

    return system_status;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    // create a child process using fork
    int execv_status;
    int child_process_status;
    int child_process_wait_status;
    bool exit_status = true;
    pid_t pid;
    
    fflush(stdout);
    switch(pid = fork()) 
    {
    case -1:
       // On error fork() returns -1.
       exit_status = false;
       perror("fork failed");
       break;
    
    case 0:
       // On success fork() returns 0 in the child.
       // Child process code. 
       execv_status = execv(command[0], command);
       if(execv_status == -1)
       {
            exit(EXIT_FAILURE);
       }
       break;
    
    default:
       // On success fork() returns the pid of the child to the parent.
       // Parent process code.
       // wait for the child process to finish
        child_process_wait_status = wait(&child_process_status);
        if (child_process_wait_status != -1)
        { 
            if(WIFEXITED(child_process_status) && WEXITSTATUS(child_process_status) != 0)
            {
                exit_status = false;
                perror("Wait statue error");
            }
            else
            {
                exit_status = true;
            }
        }
        else
        {
            exit_status = false;
            perror("Wait failed");
        }
        break;
   }


    va_end(args);
    return exit_status;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    int fd;
    int execv_status;
    int child_process_status;
    int child_process_wait_status;
    bool exit_status = true;
    pid_t pid;
    
    // this piece of code follows the reference mentioned here
    fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0600);
    if(fd < 0) 
    { 
        perror("couldn't open file");
        return false;
    }
    fflush(stdout);
    switch(pid = fork()) 
    {
    case -1:
       // On error fork() returns -1.
       exit_status = false;
       perror("fork failed");
       break;
    
    case 0:
       // On success fork() returns 0 in the child.
       // Child process code. 
       if(dup2(fd, 1) < 0)   // 1 represents the stdout
       { 
           perror("couldn't redirect stdout to the specified file");
           return false;
       }
       close(fd);
       execv_status = execv(command[0], command);
       if(execv_status == -1)
       {
           exit(EXIT_FAILURE);
       }
       break;
    
    default:
       // On success fork() returns the pid of the child to the parent.
       // Parent process code.
       // wait for the child process to finish
        child_process_wait_status = wait(&child_process_status);
        if (child_process_wait_status != -1)
        { 
            if(WIFEXITED(child_process_status) && WEXITSTATUS(child_process_status) != 0)
            {
                exit_status = false;
                perror("Wait statue error");
            }
            else
            {
                exit_status = true;
            }
        }
        else
        {
            exit_status = false;
            perror("Wait failed");
        }
        break;
   }


    va_end(args);
    return exit_status;
}
