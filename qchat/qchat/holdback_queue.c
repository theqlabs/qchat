
//
// Implementation of a templatized holdback queue
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "holdback_queue.h"

void heapify(HoldbackQueue *hq, int32_t index);

int32_t getParent(int32_t index) {
  return index/2;
}

int32_t getLeftChild(int32_t index) {
  return 2*index + 1;
}

int32_t getRightChild(int32_t index) {
  return 2*index + 2;
}

HoldbackQueue *hq_init(int (*cmp)(const void *m1, const void *m2), size_t init_capacity) {
    HoldbackQueue *hq = NULL;
    hq = malloc(sizeof(*hq));
    if(cmp == NULL || hq == NULL) {
      return NULL;
    }
    hq->cmp = cmp;
    hq->messages = malloc(init_capacity * sizeof(*(hq->messages)));
    if(hq->messages == NULL) {
      return NULL;
    }
    hq->num_elements = 0;
    hq->capacity = init_capacity;
    return hq;
}

void hq_push(HoldbackQueue *hq, const void *msg) {
    if(hq == NULL) {
      return;
    }
    if (hq->num_elements >= hq->capacity) {
        //REALLOC

    }
    hq->messages[hq->num_elements] = (void*) msg;
    int32_t idx = hq->num_elements;
    void *temp = NULL;
    while(idx > 0 && hq->cmp(hq->messages[idx], hq->messages[getParent(idx)]) > 0) {
        temp = hq->messages[idx];
        hq->messages[idx] = hq->messages[getParent(idx)];
        hq->messages[getParent(idx)] = temp;
        idx = getParent(idx);
    }
    hq->num_elements++;
}

void *hq_pop(HoldbackQueue *hq) {
    if(hq == NULL) {
      return NULL;
    }
    if (hq->num_elements < 1) {
      return NULL;
    }
    void *item = hq->messages[0];
    hq->messages[0] = hq->messages[hq->num_elements-1];
    hq->num_elements--;
    heapify(hq, 0);
    return (item);
}

void hq_delete(HoldbackQueue *hq) {
    if (hq != NULL) {
      free(hq->messages);
      free(hq);
    }
}

void heapify(HoldbackQueue *hq, int32_t index) {
    if(hq == NULL) {
      return;
    }

    int32_t max_index;
    int32_t l_child = getLeftChild(index);
    int32_t r_child = getRightChild(index);

    if (l_child < hq->num_elements && hq->cmp(hq->messages[l_child], hq->messages[index]) > 0) {
      max_index = l_child;
    } else {
      max_index = index;
    }

    if (r_child < hq->num_elements && hq->cmp(hq->messages[r_child], hq->messages[max_index]) > 0) {
      max_index = r_child;
    }

    if (max_index != index) {
      void *temp= hq->messages[max_index];
      hq->messages[max_index] = hq->messages[index];
      hq->messages[index] = temp;
      heapify(hq, max_index);
    }
}
