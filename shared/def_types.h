#ifndef __DEF_TYPES_H
#define __DEF_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef char* string;

typedef enum
{
	BIT_RESET = 0,
	BIT_SET = 1
} BIT_t;

typedef enum
{
	STATE_DISABLE = 0,
	STATE_ENABLE = !STATE_DISABLE
} FUNCTIONAL_STATE_t;

#define RET_STATE_TABLE()\
X(RET_STATE_UNDEF,			"UNDEF")\
X(RET_STATE_SUCCESS,		"SUCCESS")\
X(RET_STATE_ERR_MEMORY,		"ERR_MEMORY")\
X(RET_STATE_ERR_CRC,		"ERR_CRC")\
X(RET_STATE_ERR_EMPTY,		"ERR_EMPTY")\
X(RET_STATE_ERR_PARAM,		"ERR_PARAM")\
X(RET_STATE_ERR_BUSY,		"ERR_BUSY")\
X(RET_STATE_ERR_OVERFLOW,	"ERR_OVERFLOW")\
X(RET_STATE_ERR_TIMEOUT,	"ERR_TIMEOUT")\
X(RET_STATE_ERROR,			"ERROR")\
X(RET_STATE_WARNING,		"WARNING")

#define X(a, b) a,
typedef enum
{
	RET_STATE_TABLE()
} RET_STATE_t;
#undef X

typedef struct
{
	u32 Err;
	u32 Luck;
} ErrorCounter_t;

typedef struct
{
	char* Key;
	char* Value;
} DictEntry_t;

typedef struct
{
	u32 EntriesNum;
	DictEntry_t* DictBuff;
} Dict_t;

typedef enum
{
	VALUE_UNKNOWN = 0,
	VALUE_UINT,
	VALUE_INT,
	VALUE_FLOAT,
	VALUE_DOUBLE,
	VALUE_STRING,
	VALUE_ENUM,
	VALUE_FLAG
} VALUE_t;

typedef struct
{
	float X;
	float Y;
	float Z;
} ThreeAxesSensor_t;

typedef struct
{
	u64 TS;
	ThreeAxesSensor_t Sensor;
	float Module;
} SingleSensorIMU_t;

string RetStateStr_Get(RET_STATE_t retState);

#endif /* __DEF_TYPES_H */
