#include "Frame_Queue.h"

struct Frame_Queue* frame_queue = NULL;


struct Node* newNode(uint8_t* frame){
	struct Node* node = (struct Node*)malloc(sizeof(struct Node));
	node->frame = frame;
	node->next = NULL;
	return node;
}


void createFrameQueue(){
	if(frame_queue !=NULL){
		free(frame_queue);
		
	}
	frame_queue = (struct Frame_Queue*)malloc(sizeof(struct Frame_Queue));
	
	frame_queue->front = frame_queue->rear = NULL;
	
	//return frame_queue;
}

struct Node* addFrameToQueue(uint8_t* frame){
	
	struct Node* node = newNode(frame);
	
	if(frame_queue->rear == NULL){
		frame_queue->front = frame_queue->rear = node;
		return node;
	}
	
	frame_queue->rear->next = node;
	frame_queue->rear = node;
	
	return node;
}

struct Node* getFrameFromQueue(){
	if(frame_queue->front == NULL){
		return NULL;
	}
	
	struct Node* node = frame_queue->front;
	frame_queue->front = frame_queue->front->next;
	
	if(frame_queue->front == NULL){
		frame_queue->rear = NULL;
	}
	
	return node;
}

struct Node* peekFrameFromQueue(){
	if(frame_queue->front == NULL){
		return NULL;
	}
	return frame_queue->front;
}


