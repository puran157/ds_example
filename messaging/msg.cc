#include <iostream>
#include <malloc.h>
#include <cstring>
#include <queue>
#include <pthread.h>
#include <unistd.h>

//TODO: Insert read/write locks for global queue
using namespace std;

typedef void *(*prototype)(void *arg);
bool exitF = false;

enum MSG
{
	READ,
	WRITE,
	EXECUTE
};

class Message
{
	private:
		MSG msg;
		prototype callback;
		void *data;
		
	public:
		Message(MSG, prototype, void *);
		void PerformOperation();
		~Message();
};


Message::Message(MSG m, prototype p, void * d)
{
	msg = m;
	callback = p;
	if(d != NULL) {
	data = malloc(sizeof(d));
	memcpy(data, d, sizeof(d));
	}
	else
	    data = NULL;
}

Message::~Message()
{
	free(data);
}

void Message::PerformOperation()
{
	if(msg == READ)
		cout<<"Read performed\n";
	else if(msg == WRITE)
		cout<<"Write performed\n";
	else if(msg == EXECUTE)
		cout<<"Execution perform\n";
	else
		cout<<"Invalid message\n";
}

std::queue<Message *> *msgQ;

void* Process(void *arg)
{
	while(!exitF)
	{
		if(msgQ->empty())
		{
			cout<<"Queue is Empty, Lets sleep for some time\n";
			sleep(1); /*lets sleep for 1 second here */
		}
		else
		{
			Message *t = msgQ->front();
			cout<<"Processing message\n";
			t->PerformOperation();
			msgQ->pop();
		}
	}
	return NULL;
}

void* Pump(void *arg)
{
	while(!exitF)
	{
		Message *m = new Message(READ, NULL, NULL);
		msgQ->push(m);
		m = new Message(WRITE, NULL, NULL);
		msgQ->push(m);
		m = new Message(EXECUTE, NULL, NULL);
		msgQ->push(m);
		sleep(3); /* lets sleep for 3 seconds before next iteration */
	}
	return NULL;
}


main()
{
	msgQ = new queue<Message *>();
	
	/*Create a processing thread */
	pthread_t thread[2];
	
	int status;
	
	status = pthread_create(&thread[0], NULL, Process, NULL);
	
	if(status != 0)
	{
		cout<<"Some error occured while thread creation \n";
		delete msgQ;
		return -1;
	}
	
	status = pthread_create(&thread[1], NULL, Pump, NULL);
	
	if(status != 0)
	{
		cout<<"Some error occured while thread creation \n";
		delete msgQ;
		return -1;
	}
	
	
	sleep(15); /* lets sleep for 15 seconds to observe the behaviour */
	
	exitF = true;
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);
	
}
