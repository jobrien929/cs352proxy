#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct ll {
	struct ll * prev;
	struct ll * next;
	int key;
	void * data;
};

typedef struct ll * linkedList;

struct llholder { 
	linkedList head;
};
typedef struct llholder * linkedListHolder;

linkedListHolder create();

void add(linkedListHolder list, void * data, int key);

void * get(linkedListHolder list, int key);

int remove(linkedListHolder list, int key);
