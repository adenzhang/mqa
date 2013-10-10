#ifndef MQA_MQASEERT_H_
#define MQA_MQASEERT_H_

#include <assert.h>
#include <stdio.h>

inline void dprintf(const char *s, ...)
{
    va_list arglist;
    char szDebug[256];

    va_start(arglist, s);
    vsprintf(szDebug, s, arglist);
    va_end(arglist);
    OutputDebugStringA(szDebug);
}
inline void dprintf(const wchar_t *s, ...)
{
    va_list arglist;
    wchar_t szDebug[256];

    va_start(arglist, s);
    wvsprintfW(szDebug, s, arglist);
    va_end(arglist);
    OutputDebugStringW(szDebug);
}
#define MY_ASSERT
#ifdef MY_ASSERT

#define ASSERTMSG0(assertv) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__);  assert(__assert_value__);}\
} while(false)

#define ASSERTMSG1(assertv, msg) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg);  assert(__assert_value__);}\
} while(false)

#define ASSERTMSG2(assertv, msg, m1) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1);  assert(__assert_value__);}\
} while(false)

#define ASSERTMSG3(assertv, msg, m1, m2) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1, m2);  assert(__assert_value__);}\
} while(false)
//--------- assert ret

#define ASSERTRET(assertv) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__);  return;}\
} while(false)

#define ASSERTRET0(assertv, ret) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__);  return ret;}\
} while(false)

#define ASSERTRET1(assertv,ret, msg) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg);  return ret;}\
} while(false)

#define ASSERTRET2(assertv,ret, msg, m1) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1);  return ret;}\
} while(false)

#define ASSERTRET3(assertv, ret, msg, m1, m2) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1, m2); return ret;}\
} while(false)

#else  // define MY_ASSERT empty

#define ASSERTMSG0(assertv) do{}while(true)
#define ASSERTMSG1(assertv) do{}while(true)
#define ASSERTMSG2(assertv) do{}while(true)
#define ASSERTMSG3(assertv) do{}while(true)

#define ASSERTRET(assertv) do{}while(true)
#define ASSERTRET0(assertv) do{}while(true)
#define ASSERTRET1(assertv) do{}while(true)
#define ASSERTRET2(assertv) do{}while(true)
#define ASSERTRET3(assertv) do{}while(true)

#endif


#endif // MQA_MQASEERT_H_