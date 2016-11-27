#include <stdio.h>
#include <assert.h>


#define BUFFER_SIZE 9

typedef struct FIFOBuffer {
	int bufferArray[BUFFER_SIZE];
	int count;
	int first;
	int last;	
} FIFOBuffer;


void put(FIFOBuffer *buffer, int number);
int pop(FIFOBuffer *buffer);
void printBuffer(FIFOBuffer *buffer);
long int getElementsSum(FIFOBuffer *buffer);



void put(FIFOBuffer *buffer, int number)
{
	assert(!(buffer->count >= BUFFER_SIZE));
	buffer->count++;
	buffer->bufferArray[buffer->last] = number;
	buffer->last = (buffer->last+1) % BUFFER_SIZE;
}

int pop(FIFOBuffer *buffer)
{
	int el = buffer->bufferArray[buffer->first];
	buffer->first= (buffer->first+1)% BUFFER_SIZE;
	buffer->count--;
	return(el);
}

long int getElementsSum(FIFOBuffer *buffer)
{
	long int sum = 0;
	int i;
	for(i = 0; i < buffer->count; i++) {
		sum += buffer->bufferArray[i];
	}
	return sum;
}

void printBuffer(FIFOBuffer *buffer)
{
	int i,j;
	printf("Elements in queue: %d\n", buffer->count);
	i = buffer->first;
	j = buffer->count;
	while(j>0) {
		printf("%d  ", buffer->bufferArray[i]);
		i=(i+1)% BUFFER_SIZE;
		j--;
	}
	printf("\n\n");
}

int getElementsNo(FIFOBuffer *buffer)
{
	return buffer->count;
}
