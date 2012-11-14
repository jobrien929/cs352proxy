#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "linkedlist.h"

linkedListHolder create() {
	linkedListHolder ans = (linkedListHolder)malloc(sizeof(struct llholder));
	ans->head = 0;
	return ans;
}

void add(linkedListHolder list, void * data, int key) {
	linkedList insert = (linkedList)malloc(sizeof(struct ll));
	insert->key = key;
	insert->data = data;
	if (list->head == 0) {
		insert->next = insert;
		insert->prev = insert;
		list->head = insert;
		return;
	}
	insert->prev = head;
	insert->next = head->next;
	head->next->prev = insert;
	return;
}
void * get(linkedListHolder list, int key) {
	linkedList pointer = list->head;
	do {
		if (pointer->key == key) {
			return pointer->data;
		}
		pointer = pointer->next;
	} while (pointer != list->head);
	return 0;
}

int remove(linkedListHolder list, int key) {
	if (list->head->key == key) {
		if (list->head->next == list->head) {
			free(list->head);
			list->head = 0;
			return 1;
		}
		else {
			list->head->next->prev = list->head->prev;
			list->head->prev->next = list->head->next;
			linkedList temp = list->head;
			list->head = list->head->next;
			free(temp);
			return 1;
		}
	}
	linkedList pointer = list->head->next;
	while (pointer != list->head) {
		if (pointer->key == key) {
			pointer->prev->next = pointer->next;
			pointer->next->prev = pointer->prev;
			free(pointer);
			return 1;
		}
		pointer = pointer->next;
	}
	return 0;
}

	
