#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/ipc.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "FIFObuffer.h"
#include "semaphoreManager.h"
#include "memoryManager.h"
#include "workers.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)


#define RUNNING_TIME 100
// more precise sleep -> usleep(microseconds)


int sharedMemorySegmentId;
sharedData* mSharedData;


void starter();
void starterChild(int shmId);
int spawnNewProcess(char* program, char** argList);


int main(int argc, char *argv[])
{    
    srand(time(0));
    // Starter program mode
    if(argc == 1) {
        starter();
        char *programFileName = argv[0];
        char sharedMemorySegmentIdStr[30];
        sprintf(sharedMemorySegmentIdStr, "%d", sharedMemorySegmentId);

        char* argList[] = {
            programFileName, /* argv[0], the name of the program. */
            "99",
            sharedMemorySegmentIdStr,
            NULL /* The argument list must end with a NULL. */
        };

        pid_t childProc[4];

        argList[1] = TOSTRING(consumerA);
        childProc[0] = spawnNewProcess(programFileName, argList);

        argList[1] = TOSTRING(consumerB);
        childProc[1] = spawnNewProcess(programFileName, argList);

        argList[1] = TOSTRING(producerA);
        childProc[2] = spawnNewProcess(programFileName, argList);

        argList[1] = TOSTRING(producerB);
        childProc[3] = spawnNewProcess(programFileName, argList);

        sleep(RUNNING_TIME);

        printf("Killing processes...\n");
        mSharedData->shouldExit = 1;
        int i;
        for(i = 0; i < 4; i++)
            kill(childProc[i], SIGKILL);
        cleanup(sharedMemorySegmentId, mSharedData->semaphoreSetId);
        printf("Bye!\n");        
    }

    // Child process mode - worker or producer
    if(argc == 3)
    {
        starterChild(atoi(argv[2]));
        printf("Child process created\n");

        int workerType = atoi(argv[1]);

        switch(workerType) {
            case consumerA:
                consumer(mSharedData, consumerA);
                break;
            case consumerB:
                consumer(mSharedData, consumerB);
                break;
            case producerA:
                producer(mSharedData, producerA);
                break;
            case producerB:
                producer(mSharedData, producerB);
                break;
        }  
    }

    return 0;
}

int spawnNewProcess(char* program, char** argList)
{
    pid_t childPID;
    childPID = fork();
    if (childPID != 0) {
        return childPID; // parent returns
    } else {
        execvp(program, argList); // exec for the new process
        printf("Failed to create process\n");
        return -1;
    }
}



void starter()
{
    printf("Initializing ...\n");

    sharedMemorySegmentId = allocateSharedMemory();
    attachSharedMemory(sharedMemorySegmentId, &mSharedData);
    mSharedData->semaphoreSetId = setupSemaphores();

    int semSetId = mSharedData->semaphoreSetId;
    down(semSetId, SIZE_REQUIREMENT_FOR_READING);
    down(semSetId, SUM_REQUIREMENT_FOR_PRODUCER_A_WRITING);
}

void starterChild(int shmId)
{
    mSharedData = shmat(shmId, NULL, 0);
}
