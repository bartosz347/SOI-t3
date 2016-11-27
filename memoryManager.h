

typedef struct sharedData {
    FIFOBuffer buffer;
    int semaphoreSetId;
    int shouldExit;
} sharedData;

int allocateSharedMemory();
void attachSharedMemory(int sharedMemorySegmentId, sharedData** shData);
void cleanup(int sharedMemorySegmentId, int semaphoreSetId);

int allocateSharedMemory()
{
    int size = sizeof(sharedData);
    int sharedMemorySegmentId = shmget(IPC_PRIVATE, size, IPC_CREAT | S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH );
    assert(sharedMemorySegmentId != -1);

    return sharedMemorySegmentId;
}

void attachSharedMemory(int sharedMemorySegmentId, sharedData** shData)
{
	*shData = shmat(sharedMemorySegmentId, NULL, 0);
}

void cleanup(int sharedMemorySegmentId, int semaphoreSetId)
{
    semctl(semaphoreSetId, 0, IPC_RMID);
    shmctl(sharedMemorySegmentId, IPC_RMID, NULL);
}
