#ifndef __RINGBUFF_H
#define __RINGBUFF_H

#include "main.h"

typedef struct
{
	u8* pBuff;		//Pointer to static buffer in RAM
	u8* Head;		//Address where the new entry will be placed
	u8* Tail;		//Address from which reading will be carried out
	u16 Count;		//Number of unread entries
	u16 Capacity;	//Full space for useful data
} RingBuff_t;

//Low-level
RET_STATE_t	RingBuff_Init(RingBuff_t* ringBuff, u8* mainBuff, u16 buffSize);
u16			RingBuff_GetCnt(RingBuff_t* ringBuff);
u16			RingBuff_GetCap(RingBuff_t* ringBuff);
u8*			RingBuff_GetHead(RingBuff_t* ringBuff);
u8*			RingBuff_GetTail(RingBuff_t* ringBuff);
u8*			RingBuff_GetMainBuffPtr(RingBuff_t* ringBuff);
RET_STATE_t RingBuff_WaitBuffFill(RingBuff_t* ringBuff, u32 timeout, u16 charNum);
RET_STATE_t RingBuff_Flush(RingBuff_t* ringBuff);
RET_STATE_t RingBuff_RemoteToNewTail(RingBuff_t* ringBuff, u8* newTail);

//Mid-level
RET_STATE_t RingBuff_InterruptCallback(RingBuff_t* ringBuff, u16 bytesReceived);
RET_STATE_t RingBuff_Line2Ring_Copy(RingBuff_t* ringBuff, u8* srcBuff, u16 charNum, u16* rwCnt);
RET_STATE_t RingBuff_Ring2Line_Copy(RingBuff_t* ringBuff, u8* dstBuff, u16 charNum, u16* rwCnt);
RET_STATE_t RingBuff_Ring2Ring_Copy(RingBuff_t* ringBuffSrc, RingBuff_t* ringBuffDst, u16 charNum, u16* rwCnt);
RET_STATE_t RingBuff_Buff_Search(RingBuff_t* ringBuff, u32 timeout, u8* matchBuff, u16 buffSize);
RET_STATE_t RingBuff_Str_Search(RingBuff_t* ringBuff, u32 timeout, char* matchStr);
RET_STATE_t RingBuff_Buff_Peek(RingBuff_t* ringBuff, u8* matchBuff, u16 buffLen, u8** pMatchBuff, u16* rCnt);
RET_STATE_t RingBuff_Str_Peek(RingBuff_t* ringBuff, char* matchStr, u8** pMatchStr, u16* rCnt);
RET_STATE_t RingBuff_Buff_WaitForCompare(RingBuff_t* ringBuff, u32 timeout, u32* timeSpent, u8* compareBuff, u16 buffLen, u8** pMatchBuff, u16* rCnt);
RET_STATE_t RingBuff_Str_WaitForCompare(RingBuff_t* ringBuff, u32 timeout, u32* timeSpent, char* compareStr, u8** pMatchStr, u16* rCnt);
RET_STATE_t RingBuff_Str_PeakBetween(RingBuff_t* pRingBuff, u8* pDstBuff,
		u16 dstBuffMaxSize, u8 entryNum, char** pOneTwo, u16* pRwCnt);

//High-level
RET_STATE_t RingBuff_Ring2Line_CopyBeforeEntry(RingBuff_t* ringBuff, u32 timeout, u8* dstBuff, u16 dstBuffMaxSize, char* stringEntry, u16* rwCnt);
RET_STATE_t RingBuff_Str_SearchEntryBetween(RingBuff_t* ringBuff, u32 timeout, u8* dstBuff, u16 dstBuffMaxSize, char** oneTwo, u16* rwCnt);
RET_STATE_t RingBuff_Str_SetSearch(RingBuff_t* ringBuff, u32 timeout, char** searchSequence, u16 sequenceNum, s16* matchStr);
RET_STATE_t RingBuff_Str_SetPeek(RingBuff_t* ringBuff, u32 timeout, char** searchSequence, u16 sequenceNum, s16* matchStr);
RET_STATE_t RingBuff_Str_SetPeekFirst(RingBuff_t* ringBuff, u32 timeout, char** searchSequence, u16 sequenceNum, s16* matchStr);

#endif /* __RINGBUFF_H */
