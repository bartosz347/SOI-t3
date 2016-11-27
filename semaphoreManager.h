#include <sys/sem.h>

#define NO_SEMAPHORES 5

// semaphores
#define MUTEX 0 //binary
#define FULL 1
#define EMPTY 2
#define SIZE_REQUIREMENT_FOR_READING 3 // binary
#define SUM_REQUIREMENT_FOR_PRODUCER_A_WRITING 4 // binary


int setupSemaphores();
void down(int semaphoreSetId, int whichSemaphore);
void downNoWait(int semaphoreSetId, int whichSemaphore);
void up(int semaphoreSetId, int whichSemaphore);



void down(int semaphoreSetId, int whichSemaphore)
{
    struct sembuf semopArg[1];
    semopArg[0].sem_num = whichSemaphore;
    semopArg[0].sem_op = -1;
    semopArg[0].sem_flg = 0;
    assert(semop(semaphoreSetId, semopArg, 1) == 0);

        //   http://man7.org/linux/man-pages/man2/semtimedop.2.html
}

void downNoWait(int semaphoreSetId, int whichSemaphore)
{
    struct sembuf semopArg[1];
    semopArg[0].sem_num = whichSemaphore;
    semopArg[0].sem_op = -1;
    semopArg[0].sem_flg = IPC_NOWAIT;
    semop(semaphoreSetId, semopArg, 1);
}

void up(int semaphoreSetId, int whichSemaphore)
{
/*
   struct  sembuf {
        ushort sem_num; // identifies which semaphore in the set
        short sem_op; // could be positive,negative or zero
        short sem_flg; //coud be IPC_NOWAIT ,SEM_UNDO
   };
 */
    struct sembuf semopArg[1];
    semopArg[0].sem_num = whichSemaphore;
    semopArg[0].sem_op = 1;
    semopArg[0].sem_flg = 0;
    assert( semop(semaphoreSetId, semopArg, 1) == 0);
}


int setupSemaphores()
{
    union semun {
        int val;
        struct  semid_ds *buf;
        ushort  *array;
    } semctlArg;

    // create semaphore set
    int semaphoreSetId = semget(IPC_PRIVATE, NO_SEMAPHORES,
        IPC_CREAT | S_IRUSR
        | S_IWUSR | S_IROTH | S_IWOTH);

    // initialize semaphores
    semctlArg.val = 1;
    assert( semctl(semaphoreSetId, MUTEX, SETVAL, semctlArg) == 0 );

    semctlArg.val = 0;
    assert( semctl(semaphoreSetId, FULL, SETVAL, semctlArg) == 0 );

    semctlArg.val = BUFFER_SIZE;
    assert( semctl(semaphoreSetId, EMPTY, SETVAL, semctlArg) == 0 );

    semctlArg.val = 1; // TODO
    assert( semctl(semaphoreSetId, SIZE_REQUIREMENT_FOR_READING, SETVAL, semctlArg) == 0 );

    semctlArg.val = 1;
    assert( semctl(semaphoreSetId, SUM_REQUIREMENT_FOR_PRODUCER_A_WRITING, SETVAL, semctlArg) == 0 );

    return semaphoreSetId;
}



