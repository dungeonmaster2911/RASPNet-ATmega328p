#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct Node {
	uint8_t* frame;
	struct Node* next;
};

struct Frame_Queue {
	struct Node *front, *rear;
};


struct Node* newNode(uint8_t* frame);

void createFrameQueue();

struct Node* addFrameToQueue(uint8_t* frame);

struct Node* getFrameFromQueue();

struct Node* peekFrameFromQueue();
