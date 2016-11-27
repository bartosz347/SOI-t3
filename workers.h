#include <time.h>

#define CONSUMER_A_SLEEP_TIME 1
#define CONSUMER_B_SLEEP_TIME 2

#define PRODUCER_A_SLEEP_TIME 1
#define PRODUCER_B_SLEEP_TIME 1


#define producerA 10
#define producerB 20
#define consumerA 30
#define consumerB 40


void producer(sharedData* data, int type);
void consumer(sharedData* data, int type);
int produceItem();


void consumer(sharedData* data, int type)
{
    int semSetId = data->semaphoreSetId;
    printf("New consumer added\n");
    
    while(!data->shouldExit) {
        down(semSetId, FULL); 
        down(semSetId, SIZE_REQUIREMENT_FOR_READING);
        down(semSetId, MUTEX);

        pop(&data->buffer);

        if(getElementsNo(&data->buffer) > 3)
            up(semSetId, SIZE_REQUIREMENT_FOR_READING); // set semaphore to 1, because we still have enough elements
        up(semSetId, EMPTY);
        if(getElementsSum(&data->buffer) <= 20)
        	downNoWait(semSetId, SUM_REQUIREMENT_FOR_PRODUCER_A_WRITING); // set semaphore to 0, because condition is no longer met

        printf("Consumer%s removed elem\n", type == consumerA ? "A" : "B");
        printBuffer(&data->buffer);
        up(semSetId, MUTEX);
        
        sleep(type == consumerA ? CONSUMER_A_SLEEP_TIME : CONSUMER_B_SLEEP_TIME);
    }
}


void producer(sharedData* data, int type)
{
    int semSetId = data->semaphoreSetId;
    printf("New producer added\n");
     
    while (!data->shouldExit) {
        if(type == producerA)
           down(semSetId, SUM_REQUIREMENT_FOR_PRODUCER_A_WRITING);
        down(semSetId, EMPTY);
        down(semSetId, MUTEX);

        put(&data->buffer, produceItem());

        up(semSetId, FULL);
        if(getElementsNo(&data->buffer) == 4) // ALTERNATIVE: >=4 and not increase but explicitly set to 1
            up(semSetId, SIZE_REQUIREMENT_FOR_READING); // set semaphore to 1, after our put the size condition is met
        
        // ALTERNATIVE: if sum condition is met, explicitly set to 1
        downNoWait(semSetId, SUM_REQUIREMENT_FOR_PRODUCER_A_WRITING); // set semaphore to 0
        if(getElementsSum(&data->buffer) > 20)
            up(semSetId, SUM_REQUIREMENT_FOR_PRODUCER_A_WRITING); // if the sum condition is met, set semaphore to 1

        printf("Producer%s added elem\n", type == producerA ? "A" : "B");
        printBuffer(&data->buffer);
        up(semSetId, MUTEX);
        
        sleep(type == producerA ? PRODUCER_A_SLEEP_TIME : PRODUCER_B_SLEEP_TIME);
    }
}


int produceItem()
{
    return( rand() % 10 + 3);
}