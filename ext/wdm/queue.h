#include <Windows.h>

#include "entry.h"

#ifndef WDM_QUEUE_H
#define WDM_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ---------------------------------------------------------
// Types
// ---------------------------------------------------------

typedef struct WDM_QueueItem {
    WDM_PEntryUserData user_data;
    BYTE buffer[WDM_BUFFER_SIZE];
    struct WDM_QueueItem* previous;
    struct WDM_QueueItem* next;
} WDM_QueueItem, *WDM_PQueueItem;

typedef struct WDM_Queue {
    WDM_PQueueItem front;
    WDM_PQueueItem rear;
} WDM_Queue, *WDM_PQueue;

// ---------------------------------------------------------
// Prototypes
// ---------------------------------------------------------

WDM_PQueueItem wdm_queue_item_new();
void wdm_queue_item_free(WDM_PQueueItem);

WDM_PQueue wdm_queue_new();
void wdm_queue_free(WDM_PQueue);

void wdm_queue_enqueue(WDM_PQueue, WDM_PQueueItem);
WDM_PQueueItem wdm_queue_dequeue(WDM_PQueue);
void wdm_queue_empty(WDM_PQueue);
BOOL wdm_queue_is_empty(WDM_PQueue);

// ---------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WDM_QUEUE_H