#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    struct thread_data* thread_func_args = (struct thread_data *)thread_param;

    // wait for wait_to_obtain_ms_var specified timeout
    usleep(thread_func_args->wait_to_obtain_ms_var * 1000);

    //  obtain mutex
    int rc = pthread_mutex_lock(thread_func_args->mutex_var);
    if(rc != 0)
    {
        printf("pthread_mutex_lock failed with %d\n", rc);
        thread_func_args->thread_complete_success = false;
    }
    else
    {
        // wait for wait_to_release_ms_var specified timeout
        usleep(thread_func_args->wait_to_release_ms_var * 1000);

        // update the thread_complete_success flag
        thread_func_args->thread_complete_success = true;

        // release mutex
        rc = pthread_mutex_unlock(thread_func_args->mutex_var);
        if(rc != 0)
        {
            printf("pthread_mutex_unlock failed with %d\n", rc);
            thread_func_args->thread_complete_success = false;
        }
    }
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    bool success = true;
    struct thread_data* data = (struct thread_data*)malloc(sizeof(struct thread_data));
    if(!data)
    { 
        return false;
    }

    data->mutex_var = mutex;
    data->wait_to_obtain_ms_var = wait_to_obtain_ms;
    data->wait_to_release_ms_var = wait_to_release_ms;
    data->thread_complete_success = false;

    // create a thread where specified logic will be executed
    int rc = pthread_create(thread, NULL, threadfunc, data);
    if(rc != 0)
    {
        printf("pthread_create failed with %d\n", rc);
        success = false;
    }

    return success;
}

