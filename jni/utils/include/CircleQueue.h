#ifndef __CIRCLEQUEUE_H__
#define __CIRCLEQUEUE_H__
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//#define TRANSFER_DEBUG

template <typename ElemType>
class CircleQueue {
public:
	CircleQueue(int capacity, const char *name, bool circulation);
	~CircleQueue();
	bool isQueueFull();
	bool isQueueEmpty();
	bool enQueue(ElemType element);
	bool deQueue(ElemType &element);
	void Lock();
	void unLock();
	int getLength();
	int getCapacity();

private:
	char queue_name[128];
	ElemType *data;
	int m_queueCapacity;
	int m_queueLength;
	int readPos;
	int writePos;
	bool isCirculative;
	pthread_mutex_t mutex;
};

template <typename ElemType>
CircleQueue<ElemType>::CircleQueue(int capacity, const char *name, bool circulation) {
	isCirculative = circulation;
	memset(queue_name, '\0', 128);
	strcpy(queue_name, name);
	pthread_mutex_init(&mutex, NULL);
	m_queueCapacity = capacity;
	data = new ElemType[capacity];
	m_queueLength = 0;
	readPos = 0;
	writePos = 0;
}

template <typename ElemType>
CircleQueue<ElemType>::~CircleQueue() {
	delete data;
	data = NULL;
	pthread_mutex_destroy(&mutex);
}

template <typename ElemType>
bool CircleQueue<ElemType>::isQueueFull() {
	printf("m_queueLength: %d\n", m_queueLength);
	return (m_queueLength == m_queueCapacity) ? true : false;	
}

template <typename ElemType>
bool CircleQueue<ElemType>::isQueueEmpty() {
	printf("m_queueLength: %d\n", m_queueLength);
	return (m_queueLength == 0) ? true : false;
};

template <typename ElemType>
bool CircleQueue<ElemType>::enQueue(ElemType element) {
	pthread_mutex_lock(&mutex);
	if(isCirculative) {
#ifdef TRANSFER_DEBUG
printf("<-- EnQueue %s(cir) %d\n", queue_name, writePos);
#endif
		data[writePos++] = element;
		writePos %= m_queueCapacity;
		m_queueLength++;
		pthread_mutex_unlock(&mutex);
		return true;
	} else {
		if(!isQueueFull()) {
#ifdef TRANSFER_DEBUG
printf("<-- EnQueue %s %d\n", queue_name, writePos);
#endif
			data[writePos++] = element;
			writePos %= m_queueCapacity;
			m_queueLength++;
			pthread_mutex_unlock(&mutex);
			return true;
		} else {
			pthread_mutex_unlock(&mutex);
#ifdef TRANSFER_DEBUG
printf("<-- EnQueue %s %d\n", queue_name, writePos);
#endif
			return false;
		}
	}
}

template <typename ElemType>
bool CircleQueue<ElemType>::deQueue(ElemType &element) {
#ifdef TRANSFER_DEBUG
printf("<-- DeQueue %s(cir) %d\n", queue_name, readPos);
#endif
	pthread_mutex_lock(&mutex);
	if(isCirculative) {
#ifdef TRANSFER_DEBUG
//printf("<-- DeQueue %s(cir) %d\n", queue_name, readPos);
#endif
		element = data[readPos];
		memset(&data[readPos], 0x0, sizeof(ElemType));
		readPos++;
		readPos %= m_queueCapacity;
		m_queueLength--;
		pthread_mutex_unlock(&mutex);
		return true;
	} else {
		if(!isQueueEmpty()) {
#ifdef TRANSFER_DEBUG
printf("\n<-- DeQueue %s %d\n", queue_name, readPos);
#endif
			element = data[readPos];
			memset(&data[readPos], 0x0, sizeof(ElemType));
			readPos++;
			readPos %= m_queueCapacity;
			m_queueLength--;
			pthread_mutex_unlock(&mutex);
			return true;
		} else {
			pthread_mutex_unlock(&mutex);
#ifdef TRANSFER_DEBUG
//printf("<-- DeQueue failed %s %d\n", queue_name, readPos);
#endif
			return false;
		}
	}
}

template <typename ElemType>
void CircleQueue<ElemType>::Lock() {
	pthread_mutex_lock(&mutex);
}

template <typename ElemType>
void CircleQueue<ElemType>::unLock() {
	pthread_mutex_unlock(&mutex);
}

template <typename ElemType>
int CircleQueue<ElemType>::getLength() {
	return m_queueLength;
}

template <typename ElemType>
int CircleQueue<ElemType>::getCapacity() {
	return m_queueCapacity;
}
#endif
