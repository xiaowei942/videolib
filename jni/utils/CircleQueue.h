#ifndef __CIRCLEQUEUE_H__
#define __CIRCLEQUEUE_H__

template <typename ElemType>
class CircleQueue {
public:
	CircleQueue(int capacity);
	~CircleQueue();
	bool isQueueFull();
	bool isQueueEmpty();
	bool enQueue(ElemType element);
	bool deQueue(ElemType &element);

private:
	ElemType *data;
	int m_queueCapacity;
	int m_queueLength;
	int readPos;
	int writePos;
};

template <typename ElemType>
CircleQueue<ElemType>::CircleQueue(int capacity) {
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
}

template <typename ElemType>
bool CircleQueue<ElemType>::isQueueFull() {
	return (m_queueLength == m_queueCapacity) ? true : false;	
}

template <typename ElemType>
bool CircleQueue<ElemType>::isQueueEmpty() {
	return (m_queueLength == 0) ? true : false;
};

template <typename ElemType>
bool CircleQueue<ElemType>::enQueue(ElemType element) {
	if(!isQueueFull()) {
		data[writePos] = element;
		writePos = (writePos++) % m_queueCapacity;
		m_queueLength++;
		return true;
	} else {
		return false;
	}
}

template <typename ElemType>
bool CircleQueue<ElemType>::deQueue(ElemType &element) {
	if(!isQueueEmpty()) {
		readPos = (readPos++) % m_queueCapacity;
		element = data[readPos];
		m_queueLength--;
		return true;
	} else {
		return false;
	}
}
#endif
