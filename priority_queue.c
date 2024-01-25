#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "priority_queue.h"

static int find_element(PriorityQueue queue, PQElement element);
static PriorityQueueResult expand(PriorityQueue queue);
static PriorityQueueResult remove_element_with_priority(PriorityQueue queue, PQElement element,PQElementPriority priority);

#define NULL_QUEUE -1
#define ELEMENT_NOT_FOUND -17
#define EXPAND_FACTOR 2
#define INITIAL_SIZE 10
#define MAP_NULL_ARGUMENT -2
#define MAP_SUCCESS 1
#define NULL_ITERATOR -22



struct PriorityQueue_t {
    PQElement* elements;
    PQElementPriority* priorities;
    int size;
    int max_size;
    int iterator;
    CopyPQElement copy_element;
    FreePQElement free_element;
    EqualPQElements equal_elements;
    CopyPQElementPriority copy_priority;
    FreePQElementPriority free_priority;
    ComparePQElementPriorities compare_priorities;
};


PriorityQueue pqCreate(CopyPQElement copy_element, FreePQElement free_element, EqualPQElements equal_elements, //1
                       CopyPQElementPriority copy_priority, FreePQElementPriority free_priority,
                        ComparePQElementPriorities compare_priorities){

                        if(!copy_element || !free_element || !equal_elements || 
                        !copy_priority || !free_priority || ! compare_priorities){
                                    return NULL;
                                }

                         PriorityQueue queue = malloc(sizeof(*queue));   
                         if(queue == NULL){
                             return NULL;
                         }
                         queue -> elements = malloc(sizeof(PQElement) * INITIAL_SIZE);
                         if(queue -> elements == NULL){
                             free(queue);
                             return NULL;
                         }
                         queue -> priorities = malloc(sizeof(PQElementPriority) * INITIAL_SIZE);
                         if(queue -> priorities == NULL){
                            free(queue->elements); 
                            free(queue); 
                            return NULL;
                         }
                         
                         queue->size = 0;
                         queue->iterator = 0;
                         queue->max_size = INITIAL_SIZE;
                         queue->copy_element = copy_element;
                         queue->free_element = free_element;
                         queue->equal_elements = equal_elements;
                         queue->copy_priority = copy_priority;
                         queue->free_priority = free_priority;
                         queue-> compare_priorities = compare_priorities;

                    return queue;

                }


static int find_element(PriorityQueue queue, PQElement element){
    if(queue == NULL || element == NULL){
        return ELEMENT_NOT_FOUND;
    }

    for(int i=0; i < queue->size; i++){
        if(queue->equal_elements(queue->elements[i],element)){
            return i;
        }
    }
    return ELEMENT_NOT_FOUND;
}



static PriorityQueueResult remove_element_with_priority(PriorityQueue queue, PQElement element,PQElementPriority priority){
    if(queue == NULL || priority == NULL || element == NULL){
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }

    for(int i=0; i < queue->size; i++){
        if(queue->equal_elements(queue->elements[i],element) && queue->compare_priorities(queue->priorities[i],priority) == 0){
            queue->free_element(queue->elements[i]);
            queue->free_priority(queue->priorities[i]);

        for(int k=i+1;k <= queue->size-1; k++){
        queue->elements[k-1] = queue->elements[k];
        queue->priorities[k-1] = queue->priorities[k];
        }
        --queue->size;
        return PQ_SUCCESS;
        }
    }
    return PQ_ELEMENT_DOES_NOT_EXISTS;
}


bool pqContains(PriorityQueue queue, PQElement element){ //5
    if(find_element(queue,element) == ELEMENT_NOT_FOUND){
        return false;
    }
    else{
        return true;
    }
}


int pqGetSize(PriorityQueue queue){ //4
    if(queue == NULL){
        return NULL_QUEUE;
    }
    return queue->size;
}


PQElement pqGetFirst(PriorityQueue queue){ //11
    if(queue == NULL || queue->size == 0){
        return NULL;
    }

    queue->iterator = 0;  
    return queue->elements[0];
}


PQElement pqGetNext(PriorityQueue queue){ //12
    if(queue == NULL || queue->iterator >= queue->size-1 || queue->iterator == NULL_ITERATOR){
        return NULL;
    }
    return queue->elements[++queue->iterator];
}




PriorityQueueResult pqRemove(PriorityQueue queue){ //8
    if(queue == NULL){
        return PQ_NULL_ARGUMENT;
    }

    if(pqGetSize(queue) == 0){
        return PQ_SUCCESS;
    }
    else if(pqRemoveElement(queue,pqGetFirst(queue)) == PQ_NULL_ARGUMENT){
        return PQ_NULL_ARGUMENT;
    }

    return PQ_SUCCESS;
}


/*
PriorityQueueResult pqRemove(PriorityQueue queue){ //8
    if(queue == NULL){
        return PQ_NULL_ARGUMENT;
    }
    queue->free_element(queue->elements[0]);
    queue->free_priority(queue->priorities[0]);
    for(int i=1;i < queue->size-1; i++){
        queue->elements[i-1] = queue->elements[i];
        queue->priorities[i-1] = queue->priorities[i];
    }
    --queue->size;
    queue->iterator = 0;
    return PQ_SUCCESS;
}
*/

void pqDestroy(PriorityQueue queue){ //2

    if(pqClear(queue) == MAP_NULL_ARGUMENT){
        return;
    }
    queue->iterator = NULL_ITERATOR;
    free(queue->priorities);
    free(queue->elements);
    free(queue);

} 


PriorityQueue pqCopy(PriorityQueue queue){ //3
    if(queue == NULL){
        return NULL;
    }

    PriorityQueue new_queue = pqCreate(queue->copy_element, queue->free_element, queue->equal_elements,
                                       queue->copy_priority, queue->free_priority, queue->compare_priorities);
    if(new_queue == NULL){
        return NULL;
    }
    for(int i=0; i < queue->size ; i++){
        pqInsert(new_queue, queue->elements[i], queue->priorities[i]);
    }

    queue->iterator = NULL_ITERATOR;
    new_queue->iterator = NULL_ITERATOR;
    return new_queue;
}

// compare the given priority with our queue priorities. insert the given element in the place of the first less priority.
// (and moving the "smaller" elements to the right one step). 
PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority){ //6
    if(queue == NULL || element == NULL || priority == NULL){
        return PQ_NULL_ARGUMENT;
    }
    
    if (queue->size == queue->max_size && expand(queue) == PQ_OUT_OF_MEMORY) {
        return PQ_OUT_OF_MEMORY;
    }
    PQElement newElement = queue->copy_element(element);
    PQElementPriority newPriority = queue->copy_priority(priority);

    if (newElement == NULL || newPriority == NULL) {
        return PQ_OUT_OF_MEMORY;
    }
     
    if(queue->size == 0){
        queue->elements[0] = newElement;
        queue->priorities[0] = newPriority;
    }else{
        int compare = 0;
    for(int i=0;  i < queue->size; i++){
        compare = queue->compare_priorities(queue->priorities[i], priority);
        if(compare < 0){
            for(int j=queue->size-1 ; j>i-1; j--){
                queue->elements[j+1] = queue->elements[j];
                queue->priorities[j+1] = queue->priorities[j];
            }
            queue->elements[i] = newElement;
            queue->priorities[i] = newPriority;
            break;
        }
    }
    if (compare >= 0){
        queue->elements[queue->size] = newElement;
        queue->priorities[queue->size] = newPriority;
    }
  } 
    queue->size++;
    queue->iterator = NULL_ITERATOR;
    return PQ_SUCCESS;
}


static PriorityQueueResult expand(PriorityQueue queue) {
    assert(queue != NULL);
    int newSize = EXPAND_FACTOR * queue->max_size;
    PQElement* newElements = realloc(queue->elements, newSize * sizeof(PQElement));
    if (newElements == NULL) {
        return PQ_OUT_OF_MEMORY;
    }
    PQElementPriority* newPriorities = realloc(queue->priorities, newSize * sizeof(PQElementPriority));
    if (newPriorities == NULL) {
        free(newElements);
        return PQ_OUT_OF_MEMORY;
    }
    queue->elements = newElements;
    queue->priorities = newPriorities;
    queue->max_size = newSize;
    return PQ_SUCCESS;
}


PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element,
                                     PQElementPriority old_priority, PQElementPriority new_priority){ //7

    if(!queue || !element || !old_priority || !new_priority){
        return PQ_NULL_ARGUMENT;
    }

    if(remove_element_with_priority(queue,element,old_priority) == PQ_ELEMENT_DOES_NOT_EXISTS){
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }
    
    if(pqInsert(queue,element,new_priority) == PQ_OUT_OF_MEMORY){
        return PQ_OUT_OF_MEMORY;
    }

    queue->iterator = NULL_ITERATOR;
    return PQ_SUCCESS;

}



PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element){ //9
    if(queue == NULL || element == NULL){
        return PQ_NULL_ARGUMENT;
    }

    int i = find_element(queue,element);
    if(i == ELEMENT_NOT_FOUND){
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }

    queue->free_element(queue->elements[i]);
    queue->free_priority(queue->priorities[i]);

    for(int k=i+1;k <= queue->size-1; k++){
        queue->elements[k-1] = queue->elements[k];
        queue->priorities[k-1] = queue->priorities[k];
    }
    --queue->size;
    queue->iterator = NULL_ITERATOR;
    return PQ_SUCCESS;
}  



PriorityQueueResult pqClear(PriorityQueue queue){
    if(queue == NULL){
        return PQ_NULL_ARGUMENT;
    }
    while(queue->size != 0){
        pqRemove(queue);
    }
    return PQ_SUCCESS;
}