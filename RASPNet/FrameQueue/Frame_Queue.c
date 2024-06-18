/**
* - This file implements basic functionalities for a FIFO queue, to handle multiple sending frames.
*/


#include "Frame_Queue.h"

struct Frame_Queue* frame_queue = NULL;


struct Node* newNode(uint8_t* frame){
	struct Node* node = (struct Node*)malloc(sizeof(struct Node));
	node->frame = frame;
	node->next = NULL;
	return node;
}

/**
* - This function initializes a FIFO queue (frame_queue) to store the sending frames.
* - There can only be one queue at a time, if new queue is created the old queue is cleared and freed.
*/
void createFrameQueue(){
	if(frame_queue !=NULL){
		free(frame_queue);
		
	}
	frame_queue = (struct Frame_Queue*)malloc(sizeof(struct Frame_Queue));
	
	frame_queue->front = frame_queue->rear = NULL;
	
}

/**
* - Adds new frame (in form of a node) to the frame_queue.
* 
*/
struct Node* addFrameToQueue(uint8_t* frame){
	
	struct Node* node = newNode(frame); /// Create a new node with provided frame.
	
	if(frame_queue->rear == NULL){
		frame_queue->front = frame_queue->rear = node;
		return node;
	}
	
	frame_queue->rear->next = node; /// Add the node to the frame_queue.
	frame_queue->rear = node;
	
	return node; /// Return the pointer to newly added frame node.
}

/**
* - Pop the front most node from the frame_queue.
*/
struct Node* getFrameFromQueue(){
	if(frame_queue->front == NULL){
		return NULL;
	}
	
	/// Remove the front most node from the frame_queue
	struct Node* node = frame_queue->front;
	frame_queue->front = frame_queue->front->next;
	
	if(frame_queue->front == NULL){
		frame_queue->rear = NULL;
	}
	
	/// Return the removed node
	return node;
}


/**
* - Returns the front most node from the frame_queue (without removing it from the queue).
*/
struct Node* peekFrameFromQueue(){
	if(frame_queue->front == NULL){
		return NULL;
	}
	return frame_queue->front;
}


