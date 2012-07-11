#include <Windows.h>

#ifndef WDM_BUFFER_H
#define WDM_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



// ---------------------------------------------------------
// Types
// ---------------------------------------------------------

typedef LPBYTE WDM_PBuffer;

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

#endif // WDM_BUFFER_H