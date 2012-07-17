#include "wdm.h"

#include "memory.h"
#include "queue.h"

// ---------------------------------------------------------
// Queue item functions
// ---------------------------------------------------------

WDM_PQueueItem
wdm_queue_item_new() {
    WDM_PQueueItem item;

    item = WDM_ALLOC(WDM_QueueItem);
    item->user_data = NULL;
    item->previous    = NULL;
    item->next        = NULL;

    ZeroMemory(&item->buffer, WDM_BUFFER_SIZE);

    return item;
}

void
wdm_queue_item_free(WDM_PQueueItem item) {
    // We can't really do anything to the prev pointer nor the next pointer,
    // because we might break any linking the user has established.
    free(item);
}

// ---------------------------------------------------------
// Queue functions
// ---------------------------------------------------------

WDM_PQueue
wdm_queue_new() {
    WDM_PQueue queue;

    queue = WDM_ALLOC(WDM_Queue);
    queue->front = NULL;
    queue->rear  = NULL;

    if ( ! InitializeCriticalSectionAndSpinCount(&queue->lock,
            0x00000400) ) // TODO: look into the best value for spinning.
    {
        rb_raise(eWDM_Error, "Can't create a lock for the queue");
    }

    return queue;
}

void
wdm_queue_free(WDM_PQueue queue) {
    wdm_queue_empty(queue);
    free(queue);
}

void
wdm_queue_enqueue(WDM_PQueue queue, WDM_PQueueItem item) {
    EnterCriticalSection(&queue->lock);

    if ( queue->rear == NULL && queue->front == NULL )  {
        queue->front = queue->rear = item;
    }
    else {
        queue->rear->next = item;
        item->previous = queue->rear;
        queue->rear = item;
    }

    LeaveCriticalSection(&queue->lock);
}

WDM_PQueueItem
wdm_queue_dequeue(WDM_PQueue queue) {
    WDM_PQueueItem item;

    EnterCriticalSection(&queue->lock);

    if ( queue->rear == NULL && queue->front == NULL ) {
        item = NULL;
    }
    else {
        item = queue->front;
        queue->front = queue->front->next;

        // Reset the rear when the queue is empty
        if ( queue->front == NULL ) queue->rear = NULL;

        // Don't allow the user to mess with the queue
        item->previous = item->next = NULL;
    }

    LeaveCriticalSection(&queue->lock);

    return item;
}

void wdm_queue_empty(WDM_PQueue queue) {
    while( ! wdm_queue_is_empty(queue) ) {
        wdm_queue_item_free(wdm_queue_dequeue(queue));
    }
}

BOOL
wdm_queue_is_empty(WDM_PQueue queue) {
    return queue->front == NULL && queue->rear == NULL;
}