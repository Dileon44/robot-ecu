#include "ring_buff.h"

#define RING_BUFF_DEFAULT_DELAY	(3)

/*
 * Cut head or tail fields with known about rBuff capacity.
 * Use only with local copy of variable!
 */
static void ring_buff_cut_ptr(RingBuff_t* ringBuff, u8** ptr)
{
	if(*ptr >= ringBuff->pBuff + ringBuff->Capacity)
		*ptr -= ringBuff->Capacity;
}

/*
 * Can works with real rBuff structure
 * deltaPtr variable is the difference between two pointers.
 */
static u16 ring_buff_compute_diff_ptr(RingBuff_t* ringBuff, u8* stringPtr)
{
	u16 deltaPtr;

	SYS_CRITICAL_ON();
	if(ringBuff->Tail < stringPtr)
		deltaPtr = stringPtr - ringBuff->Tail;
	else if(ringBuff->Tail > stringPtr)
		deltaPtr = ringBuff->Capacity - (ringBuff->Tail - stringPtr);
	else
		deltaPtr = 0;
	SYS_CRITICAL_OFF();

	return deltaPtr;
}

/*
 * Can works with real rBuff structure
 * decrement the ringBuff main counter
 */
static void ring_buff_dec_count(RingBuff_t* ringBuff, u16 number)
{
	ringBuff->Count -= number;
}

/*
 * Can works with real rBuff structure
 * increment the ringBuff main counter
 */
static void ring_buff_inc_count(RingBuff_t* ringBuff, u16 number)
{
	ringBuff->Count += number;
}

RET_STATE_t RingBuff_Init(RingBuff_t* ringBuff, u8* mainBuff, u16 buffSize)
{
	if(mainBuff == NULL || ringBuff == NULL || buffSize == 0)
		EXIT(RET_STATE_ERR_PARAM);

	ringBuff->pBuff = mainBuff;		// main address of buffer
	ringBuff->Head = &mainBuff[0];  // address of first element equ main address of buffer
	ringBuff->Tail = &mainBuff[0];
	ringBuff->Count = 0;
	ringBuff->Capacity = buffSize;

	memset(ringBuff->pBuff, 0, ringBuff->Capacity);

	return RET_STATE_SUCCESS;
}

/*
 * Getters for RingBuff_t structure
 */
u16 RingBuff_GetCnt(RingBuff_t* ringBuff) { return ringBuff->Count; }
u16 RingBuff_GetCap(RingBuff_t* ringBuff) { return ringBuff->Capacity; }
u8* RingBuff_GetHead(RingBuff_t* ringBuff) { return ringBuff->Head; }
u8* RingBuff_GetTail(RingBuff_t* ringBuff) { return ringBuff->Tail; }
u8* RingBuff_GetMainBuffPtr(RingBuff_t* ringBuff) { return ringBuff->pBuff; }

RET_STATE_t RingBuff_WaitBuffFill(RingBuff_t* ringBuff, u32 timeout, u16 charNum)
{
	if(ringBuff->Head == NULL || ringBuff->Tail == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	u32 endSearchTime = SYS_TICK_GET_MS_CNT() + timeout;
	while(SYS_TICK_GET_MS_CNT() < endSearchTime)
	{
		if(RingBuff_GetCnt(ringBuff) >= charNum)
			return RET_STATE_SUCCESS;

		SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY);
	}

	return RET_STATE_ERR_TIMEOUT;
}

RET_STATE_t RingBuff_Flush(RingBuff_t* ringBuff)
{
	if(ringBuff->Head == NULL || ringBuff->Tail == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	SYS_CRITICAL_ON();
	ringBuff->Tail = ringBuff->Head;
	ringBuff->Count = 0;
	SYS_CRITICAL_OFF();

	return RET_STATE_SUCCESS;
}

RET_STATE_t RingBuff_RemoteToNewTail(RingBuff_t* ringBuff, u8* newTail)
{
	if(newTail == NULL || ringBuff->Tail == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	u16 charNum = ring_buff_compute_diff_ptr(ringBuff, newTail);

	SYS_CRITICAL_ON();
	ringBuff->Tail += charNum;
	ring_buff_cut_ptr(ringBuff, &ringBuff->Tail);
	ringBuff->Count -= charNum;
	SYS_CRITICAL_OFF();

	return RET_STATE_SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------------------

RET_STATE_t RingBuff_InterruptCallback(RingBuff_t* ringBuff, u16 bytesReceived)
{
	if(ringBuff->Head == NULL || ringBuff->Tail == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	if(bytesReceived == 0)
		return RET_STATE_ERR_EMPTY;

	ringBuff->Count += bytesReceived;
	ringBuff->Head += bytesReceived;

	ring_buff_cut_ptr(ringBuff, &ringBuff->Head);  // rebase head to start if it's out of capacity

	if(ringBuff->Count > ringBuff->Capacity)
	{
		ringBuff->Tail = ringBuff->Head;
		ringBuff->Count = ringBuff->Capacity;
	}

	return RET_STATE_SUCCESS;
}

RET_STATE_t RingBuff_Line2Ring_Copy(RingBuff_t* ringBuff, u8* srcBuff, u16 charNum, u16* rwCnt)
{
	if(rwCnt != NULL)
		*rwCnt = 0;

	if(charNum == 0)
		return RET_STATE_SUCCESS;

	if(ringBuff->Head == NULL || ringBuff->Tail == NULL || srcBuff == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	SYS_CRITICAL_ON();
	u8* locHead = ringBuff->Head;
	u16 locCount = ringBuff->Count;
	SYS_CRITICAL_OFF();

	u16 wasReadWriteCnt = 0;

	if(charNum + locCount > ringBuff->Capacity)
		charNum = ringBuff->Capacity - locCount;

	for(; wasReadWriteCnt < charNum; wasReadWriteCnt++)
	{
		*locHead++ = *srcBuff++;
		ring_buff_cut_ptr(ringBuff, &locHead);
	}

	SYS_CRITICAL_ON();
	ring_buff_inc_count(ringBuff, wasReadWriteCnt);
	ringBuff->Head = locHead;
	SYS_CRITICAL_OFF();

	if(rwCnt != NULL)
		*rwCnt = wasReadWriteCnt;

	return RET_STATE_SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------------------
/*
 *[input] ringBuff:	ring buffer address
 *[input] dstBuff:	destination buffer address
 *[input] charNum:	length of destination buffer
 *[i/o]	  rwCnt:	count of written bytes from ring buffer
 *[output] Return State
 */
//--------------------------------------------------------------------------------------------------------------------------
RET_STATE_t RingBuff_Ring2Line_Copy(RingBuff_t* ringBuff, u8* dstBuff, u16 charNum, u16* rwCnt)
{
	if(rwCnt != NULL)
		*rwCnt = 0;

	if(charNum == 0)
		return RET_STATE_SUCCESS;

	if(ringBuff->Head == NULL || ringBuff->Tail == NULL || dstBuff == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	SYS_CRITICAL_ON();
	u8* locTail = ringBuff->Tail;
	u16 locCount = ringBuff->Count;
	SYS_CRITICAL_OFF();
	if(locCount == 0)
		return RET_STATE_ERR_EMPTY;

	u16 wasReadWriteCnt = 0;

	if(charNum > locCount)
		charNum = locCount;

	for(; wasReadWriteCnt < charNum; wasReadWriteCnt++)
	{
		*dstBuff++ = *locTail++;
		ring_buff_cut_ptr(ringBuff, &locTail);
	}

	SYS_CRITICAL_ON();
	ring_buff_dec_count(ringBuff, wasReadWriteCnt);
	ringBuff->Tail = locTail;
	SYS_CRITICAL_OFF();

	if(rwCnt != NULL)
		*rwCnt = wasReadWriteCnt;

	return RET_STATE_SUCCESS;
}

RET_STATE_t RingBuff_Ring2Ring_Copy(RingBuff_t* ringBuffSrc, RingBuff_t* ringBuffDst, u16 charNum, u16* rwCnt)
{
	if(rwCnt != NULL)
		*rwCnt = 0;

	if(charNum == 0)
		return RET_STATE_SUCCESS;

	if(ringBuffSrc->Head == NULL || ringBuffSrc->Tail == NULL || ringBuffDst->Head == NULL || ringBuffDst->Tail == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	SYS_CRITICAL_ON();
	u8* locTailSrc = ringBuffSrc->Tail;
	u8* locHeadDst = ringBuffDst->Head;
	u16 locCountSrc = ringBuffSrc->Count;
	u16 locCountDst = ringBuffDst->Count;
	SYS_CRITICAL_OFF();
	if(locCountSrc == 0)
		return RET_STATE_ERR_EMPTY;

	u16 wasReadWriteCnt = 0;

	if(charNum > locCountSrc)
		charNum = locCountSrc;
	if(charNum + locCountDst > ringBuffDst->Capacity)
		charNum = ringBuffDst->Capacity - locCountDst;

	for(; wasReadWriteCnt < charNum; wasReadWriteCnt++)
	{
		*locHeadDst++ = *locTailSrc++;
		ring_buff_cut_ptr(ringBuffSrc, &locTailSrc);
		ring_buff_cut_ptr(ringBuffDst, &locHeadDst);
	}

	SYS_CRITICAL_ON();
	ring_buff_dec_count(ringBuffSrc, wasReadWriteCnt);
	ring_buff_inc_count(ringBuffDst, wasReadWriteCnt);
	ringBuffSrc->Tail = locTailSrc;
	ringBuffDst->Head = locHeadDst;
	SYS_CRITICAL_OFF();

	if(rwCnt != NULL)
		*rwCnt = wasReadWriteCnt;

	return RET_STATE_SUCCESS;
}

RET_STATE_t RingBuff_Buff_Search(RingBuff_t* ringBuff, u32 timeout, u8* matchBuff, u16 buffLen)
{
	if(ringBuff->Head == NULL || ringBuff->Tail == NULL || matchBuff == NULL || buffLen == 0)
		EXIT(RET_STATE_ERR_PARAM);

	u8* locTail = ringBuff->Tail;
	u16 wasReadCnt = 0, ptr = 0;
	u32 endSearchTime = SYS_TICK_GET_MS_CNT() + timeout;

	while(SYS_TICK_GET_MS_CNT() <= endSearchTime)
	{
		if(ringBuff->Count < buffLen)
		{
			SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY); //Wait more data
			continue;
		}
		else
		{
			while(*locTail != matchBuff[0])
			{
				if(wasReadCnt >= ringBuff->Count - buffLen)
				{
					wasReadCnt = 0;
					locTail = ringBuff->Tail;
					SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY);
					break;
				}

				wasReadCnt++;
				locTail++;

				ring_buff_cut_ptr(ringBuff, &locTail);
			}
		}

		if(*locTail == matchBuff[0])
		{
			for(ptr = 0; ptr < buffLen; ptr++)
			{
				if(*locTail != matchBuff[ptr])
					break;

				locTail++;
				ring_buff_cut_ptr(ringBuff, &locTail);
			}

			wasReadCnt += ptr;

			if(ptr == buffLen)
			{
				ring_buff_dec_count(ringBuff, wasReadCnt);
				ringBuff->Tail = locTail;

				return RET_STATE_SUCCESS;
			}
		}
	}

	return RET_STATE_ERR_TIMEOUT;
}

RET_STATE_t RingBuff_Str_Search(RingBuff_t* ringBuff, u32 timeout, char* matchStr)
{
	return RingBuff_Buff_Search(ringBuff, timeout, (u8*)matchStr, (u16)strlen(matchStr));
}

RET_STATE_t RingBuff_Buff_Peek(RingBuff_t* ringBuff, u8* matchBuff, u16 buffLen, u8** pMatchBuff, u16* rCnt)
{
	if(rCnt != NULL)
		*rCnt = 0;

	if(ringBuff->Head == NULL || ringBuff->Tail == NULL || matchBuff == NULL || buffLen == 0)
		EXIT(RET_STATE_ERR_PARAM);

	SYS_CRITICAL_ON();
	u8* locTail = ringBuff->Tail;
	u16 locCount = ringBuff->Count;
	SYS_CRITICAL_OFF();

	u16 availableLen = locCount - buffLen;
	u16 wasReadCnt = 0, ptr = 0;

	if(locCount < buffLen)
		return RET_STATE_ERR_EMPTY;

	while(wasReadCnt <= availableLen)
	{
		if(*locTail != matchBuff[0])
		{
			wasReadCnt++;
			locTail++;
			ring_buff_cut_ptr(ringBuff, &locTail);
		}
		else
		{
			if(pMatchBuff != NULL)
				*pMatchBuff = locTail;

			for(ptr = 0; ptr < buffLen; ptr++, locTail++)
			{
				ring_buff_cut_ptr(ringBuff, &locTail);
				if(matchBuff[ptr] == '~')
					continue;
				if(matchBuff[ptr] != *locTail)
					break;
			}

			wasReadCnt += ptr;

			if(ptr == buffLen)
			{
				if(rCnt != NULL)
					*rCnt = wasReadCnt - ptr;
				return RET_STATE_SUCCESS;
			}
		}
	}

	if(pMatchBuff != NULL)
		*pMatchBuff = NULL;

	return RET_STATE_ERR_EMPTY;
}

RET_STATE_t RingBuff_Str_Peek(RingBuff_t* ringBuff, char* matchStr, u8** pMatchStr, u16* rCnt)
{
	return RingBuff_Buff_Peek(ringBuff, (u8*)matchStr, (u16)strlen(matchStr), pMatchStr, rCnt);
}

RET_STATE_t RingBuff_Buff_WaitForCompare(RingBuff_t* ringBuff, u32 timeout, u32* timeSpent, u8* compareBuff, u16 buffLen, u8** pMatchBuff, u16* rCnt)
{
	RET_STATE_t retState = RET_STATE_ERR_TIMEOUT;
	u32 endSearchTime = SYS_TICK_GET_MS_CNT() + timeout;

	while(SYS_TICK_GET_MS_CNT() < endSearchTime)
	{
		retState = RingBuff_Buff_Peek(ringBuff, compareBuff, buffLen, pMatchBuff, rCnt);
		if(retState == RET_STATE_SUCCESS)
			break;

		SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY);
	}

	if(timeSpent != NULL)
		*timeSpent = SYS_TICK_GET_MS_CNT() - (endSearchTime - timeout);
    
	return retState;
}

RET_STATE_t RingBuff_Str_WaitForCompare(RingBuff_t* ringBuff, u32 timeout, u32* timeSpent, char* compareStr, u8** pMatchStr, u16* rCnt)
{
	return RingBuff_Buff_WaitForCompare(ringBuff, timeout, timeSpent, (u8*)compareStr, (u16)strlen(compareStr), pMatchStr, rCnt);
}

/**
 * @brief 	Peek data chunk from the ring buffer without moving head or tail.
 * 			Ring buffer be like this: ...<data><one><data to peek><two>...,
 * 			and destination buffer gets the <data to peek> bytes.
 * @param 	pRingBuff Ring buffer pointer.
 * @param 	pDstBuff Destination buffer pointer.
 * @param 	dstBuffMaxSize Maximum number of bytes that destination buffer can
 * 			fit.
 * @param	entryNum If the ring buffer data contains more than one chunk that
 * 			is guarded by the oneTwo strings, the function caller can specify
 * 			which entry it is interested in. The numeration starts with '1'.
 * @param 	pOneTwo Casting this pointer we get the array of strings that
 * 			guards data chunks to be transfer.
 * @param 	pRwCnt Pointer to the value that counts the bytes that has been
 * 			transfered to the destination buffer from the ring list after
 * 			successful function execution.
 * @return	RET_STATE_t Returns RET_STATE_SUCCESS if the 'data to peak' has been
 * 			found and all of these found bytes (or just part of them, - see
 * 			pRwCnt) have been successfully transfered.
 */
RET_STATE_t RingBuff_Str_PeakBetween(RingBuff_t* pRingBuff, u8* pDstBuff,
		u16 dstBuffMaxSize, u8 entryNum, char** pOneTwo, u16* pRwCnt)
{
	if(pRingBuff == NULL || dstBuffMaxSize == 0 || entryNum == 0 ||
			pOneTwo == NULL)
	{
		EXIT(RET_STATE_ERR_PARAM);
	}

	/* Enter critical section right there because we have to keep the ring
	 * buffer copy that we are going to create to be synchronized with the
	 * original ring buffer */
	SYS_CRITICAL_ON();
	RET_STATE_t retState;
	RingBuff_t locRingBuff = *pRingBuff;
	u8* pMatchStr = NULL;
	char* pOne = pOneTwo[0];
	for(u8 i = 0; i < entryNum; i++)
	{
		/* Trying to find the first string that guards the 'data to peek' */
		retState = RingBuff_Str_Peek(&locRingBuff, pOne,
				&pMatchStr, NULL);
		if(retState != RET_STATE_SUCCESS)
		{
			SYS_CRITICAL_OFF();
			return retState;
		}
		/* ...<data><one><data to peek><two>... */
		RingBuff_RemoteToNewTail(&locRingBuff, pMatchStr + strlen(pOne));
		/* <data to peek><two>... */
	}

	u16 rCnt = 0;
	/* Now let's see if the second guarding string exist further in the
	 * ring buffer and how many bytes is there to it */
	retState = RingBuff_Str_Peek(&locRingBuff, pOneTwo[1],
			&pMatchStr, &rCnt);
	if(retState != RET_STATE_SUCCESS)
	{
		SYS_CRITICAL_OFF();
		return retState;
	}
	if(rCnt > dstBuffMaxSize)
		rCnt = dstBuffMaxSize;

	retState = RingBuff_Ring2Line_Copy(&locRingBuff, pDstBuff, rCnt, pRwCnt);
	SYS_CRITICAL_OFF();

	return retState;
}

//-----------------------------------------------------------------------------  ---------------------------------------------

RET_STATE_t RingBuff_Ring2Line_CopyBeforeEntry(RingBuff_t* ringBuff, u32 timeout, u8* dstBuff, u16 dstBuffMaxSize, char* stringEntry, u16* rwCnt)
{
	if(dstBuffMaxSize == 0)
		EXIT(RET_STATE_ERR_PARAM);

	if(rwCnt != NULL)
		*rwCnt = 0;

	RET_STATE_t retState = RET_STATE_UNDEF;
	u8* pStrEntry = NULL;
	u32 endSearchTime = SYS_TICK_GET_MS_CNT() + timeout;

	while(SYS_TICK_GET_MS_CNT() < endSearchTime)
	{
		retState = RingBuff_Str_Peek(ringBuff, stringEntry, &pStrEntry, NULL);
		if(retState == RET_STATE_SUCCESS)
		{
			u16 charNum = ring_buff_compute_diff_ptr(ringBuff, pStrEntry);
			if(charNum > dstBuffMaxSize)
				charNum = dstBuffMaxSize;

			retState = RingBuff_Ring2Line_Copy(ringBuff, dstBuff, charNum, rwCnt);
			return retState;
		}

		SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY);
	}

	return RET_STATE_ERR_TIMEOUT;
}

RET_STATE_t RingBuff_Str_SearchEntryBetween(RingBuff_t* ringBuff, u32 timeout, u8* dstBuff, u16 dstBuffMaxSize, char** oneTwo, u16* rwCnt)
{
	if(dstBuffMaxSize == 0 || oneTwo == NULL)
		EXIT(RET_STATE_ERR_PARAM);

	if(rwCnt != NULL)
		*rwCnt = 0;

	RET_STATE_t retState = RET_STATE_UNDEF;

	retState = RingBuff_Str_Search(ringBuff, timeout, oneTwo[0]);
	if(retState == RET_STATE_SUCCESS)
	{
		retState = RingBuff_Ring2Line_CopyBeforeEntry(ringBuff, timeout, (u8*)dstBuff, dstBuffMaxSize, oneTwo[1], rwCnt);
		if(retState == RET_STATE_SUCCESS && rwCnt != NULL)
			dstBuff[*rwCnt] = '\0';
	}

	return retState;
}

RET_STATE_t RingBuff_Str_SetSearch(RingBuff_t* ringBuff, u32 timeout, char** searchSequence, u16 sequenceNum, s16* matchStr)
{
	if(searchSequence == NULL || sequenceNum == 0)
		EXIT(RET_STATE_ERR_PARAM);

	s16 winStr = -1;

	RET_STATE_t retState = RET_STATE_UNDEF;
	u16 readCnt = 0;
	u32 endSearchTime = SYS_TICK_GET_MS_CNT() + timeout;

	while(SYS_TICK_GET_MS_CNT() < endSearchTime)
	{
		for(u32 strNum = 0; strNum < sequenceNum; strNum++)
		{
			retState = RingBuff_Str_Peek(ringBuff, searchSequence[strNum], NULL, &readCnt);
			if(retState == RET_STATE_SUCCESS)
			{
				winStr = strNum;
				break;
			}
		}

		if(winStr != -1)
		{
			retState = RingBuff_Str_Search(ringBuff, RING_BUFF_DEFAULT_DELAY, searchSequence[winStr]);
			if(retState == RET_STATE_SUCCESS)
			{
				*matchStr = winStr;
				return retState;
			}
		}

		SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY);
	}

	*matchStr = -1;
	return RET_STATE_ERR_TIMEOUT;
}

RET_STATE_t RingBuff_Str_SetPeek(RingBuff_t* ringBuff, u32 timeout, char** searchSequence, u16 sequenceNum, s16* matchStr)
{
	if(searchSequence == NULL || sequenceNum == 0)
		EXIT(RET_STATE_ERR_PARAM);

	s16 winStr = -1;

	RET_STATE_t retState = RET_STATE_UNDEF;
	u16 readCnt = 0;
	u32 endSearchTime = SYS_TICK_GET_MS_CNT() + timeout;

	while(SYS_TICK_GET_MS_CNT() < endSearchTime)
	{
		for(u32 strNum = 0; strNum < sequenceNum; strNum++)
		{
			retState = RingBuff_Str_Peek(ringBuff, searchSequence[strNum], NULL, &readCnt);
			if(retState == RET_STATE_SUCCESS)
			{
				winStr = strNum;
				break;
			}
		}

		if(winStr != -1)
		{
			*matchStr = winStr;
			return RET_STATE_SUCCESS;
		}

		SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY);
	}

	*matchStr = -1;
	return RET_STATE_ERR_TIMEOUT;
}

RET_STATE_t RingBuff_Str_SetPeekFirst(RingBuff_t* ringBuff, u32 timeout, char** searchSequence, u16 sequenceNum, s16* matchStr)
{
	if(searchSequence == NULL || sequenceNum == 0)
		EXIT(RET_STATE_ERR_PARAM);

	s16 winStr = -1;

	RET_STATE_t retState = RET_STATE_UNDEF;
	u16 readCnt = 0;
	u32 endSearchTime = SYS_TICK_GET_MS_CNT() + timeout;

	u16 readCntMin = 0xFFFF;

	while(SYS_TICK_GET_MS_CNT() < endSearchTime)
	{
		for(u32 strNum = 0; strNum < sequenceNum; strNum++)
		{
			retState = RingBuff_Str_Peek(ringBuff, searchSequence[strNum], NULL, &readCnt);
			if(retState == RET_STATE_SUCCESS)
			{
				if(readCnt < readCntMin)
				{
					readCntMin = readCnt;
					winStr = strNum;
				}
			}
		}

		if(winStr != -1)
		{
			*matchStr = winStr;
			return RET_STATE_SUCCESS;
		}

		SYS_DELAY_MS(RING_BUFF_DEFAULT_DELAY);
	}

	*matchStr = -1;
	return RET_STATE_ERR_TIMEOUT;
}
