#ifndef _PQUEUE_H_
#define _PQUEUE_H_

typedef struct HQ {
    int32_t num_elements;
    int32_t capacity;
    void **messages;
    int (*cmp)(const void *m1, const void *m2);
} HoldbackQueue;

HoldbackQueue *hq_init(int (*cmp)(const void *m1, const void *m2), size_t init_capacity);

void hq_delete(HoldbackQueue *hq);

void hq_push(HoldbackQueue *hq, const void *msg);

void *pq_pop(HoldbackQueue *hq);

#endif
