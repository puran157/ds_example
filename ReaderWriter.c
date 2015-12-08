#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define COUNT 5

pthread_cond_t sem;
pthread_mutex_t read;
pthread_mutex_t write;
pthread_mutex_t count;

int sem_count = 0;
int sem_b = 0;
int isWriteReady = 0;

int data = 1;

void CreateSemaphore()
{
	if(pthread_cond_init(&sem, NULL) != 0)
	{
		printf("Semaphore creation failed\n");
		exit(0);
	}
	
	if(pthread_mutex_init(&read, NULL) != 0)
	{
		printf("Semaphore Creation failed\n");
		exit(0);
	}
	
	if(pthread_mutex_init(&count, NULL) != 0)
	{
			printf("Semaphore Creation failed\n");
			exit(0);
	}
	
	if(pthread_mutex_init(&write, NULL) != 0)
	{
			printf("Semaphore Creation failed\n");
			exit(0);
	}
	
}

int isReading()
{
	if(sem_count != 0)
		return 1;
	else
		return 0;
}

void FinishRead()
{
	/* block new read operation */
	isWriteReady = 1;
	/*wait for existing read to finish */
	while(isReading());
}

void ResumeRead()
{
	isWriteReady = 0;
}

void WriteLock()
{
	if(pthread_mutex_lock(&write) == 0)
	{
		printf("Write Lock acquired\n");
		FinishRead();
	}
}

void WriteUnlock()
{
	if(pthread_mutex_unlock(&write) == 0)
	{
		printf("Write Lock released\n");
		ResumeRead();
	}
	isWriteReady = 0;
}

int ReadLock()
{
	if(isWriteReady == 1)
	{
		printf("writer wants to write, no new reads allowed\n");
		return -1;
	}
	int ret = pthread_mutex_trylock(&read) ;
	if(ret == 0)
	{
		pthread_mutex_lock(&count);
		if(sem_count < COUNT) {
			sem_count++;
			//pthread_cond_wait(&sem, &read);
		}
		else
		{
			printf("No more read locks available\n");
			return -1;
			//ReadLock();
		}
		pthread_mutex_unlock(&count);
		return 0;
	}
	else if(ret == EBUSY)
	{
		pthread_mutex_lock(&count);
		if(sem_count < COUNT) {
			sem_count++;
			//pthread_cond_wait(&sem, &read);
			pthread_mutex_unlock(&count);
			printf("we already have a read lock\n");
			return 0;
		}
		else
		{
			printf("No more read locks available\n");
			pthread_mutex_unlock(&count);
			return -1;
			//ReadLock();
		}
	}
	else {
		printf("Read mutex lock failed Lets try again\n");
		return -1;
	}
	
}

int ReadUnlock()
{
	if(pthread_mutex_trylock(&read) == EBUSY)
	{
		pthread_mutex_lock(&count);
		sem_count--;
		if(sem_count == 0 )
		{
			pthread_mutex_unlock(&read);
		}
		pthread_mutex_unlock(&count);
		//sem_count--;
		//pthread_mutex_unlock(&read);
		
	}
	else
		return -1;
}

void* Reader(void *param)
{
	printf("Reader %d: executing\n", *((int *)param));
	if(ReadLock() == 0)
	{
	usleep(200);
	printf("Data read %d\n", data);
		if(ReadUnlock() != 0)
			printf("Error Unlocking\n");
	}
	else
		printf("Reader %d: No lock\n", *((int *)param));
	printf("Reader %d: done\n\n", *((int *)param));
	return 0;
}


void *Writer(void * param)
{
	printf("Writer %d: executing\n", *((int *)param));
	WriteLock();
	data += 12;
	WriteUnlock();
	sleep(2);
	printf("Writer %d: done\n\n", *((int *)param));
	return 0;
}



int main()
{
	
	pthread_t writer[3];
	pthread_t reader[20];
	int i = 0;
	int arr[20];
	int arrW[3];
	
	for(i = 0; i < 20; ++i)
		arr[i] = i;
	for(i = 0; i < 3; ++i)
		arrW[i] = i;
	
	CreateSemaphore();
	for(i = 0; i < 20; ++i)
	{
		int j= 0 ;
		j = i;
		pthread_create(&reader[i], NULL, Reader, (void *)&arr[i]);
		//printf("Thread %d created\n", i);
				if(i < 3) {
					pthread_create(&writer[i], NULL, Writer, (void *)&arrW[i]);
				}
	}
	
	for(i = 0; i < 20; ++i)
	{
		pthread_join(reader[i], NULL);
		if(i < 3)
			pthread_join(writer[i], NULL);
	}
	printf("main exiting\n");
}
