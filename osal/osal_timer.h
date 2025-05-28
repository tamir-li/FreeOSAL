#ifndef _OSAL_TIMER_H_
#define _OSAL_TIMER_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "portosal.h"
#include "osal.h"

//#define  OSAL_TIMER_DEBUG
#define  OSAL_TIMERSTAMP_INF        (0x7FFFFFFF)

typedef void (* osal_timer_cb_t)(osal_size_t parm);

typedef enum osal_timer_mode_e
{
    OSAL_TIMER_MODE_INVALID = 0,
    OSAL_TIMER_MODE_SINGLE = 1,
    OSAL_TIMER_MODE_REPEAT = 2,
}osal_timer_mode_t;

typedef struct osal_timestamp_s
{
    uint32_t timestamp_s;
    uint32_t timestamp_ms;
}osal_timestamp_t;

typedef struct osal_timer_s osal_timer_t;
struct osal_timer_s
{
    osal_timer_t          *next;
    osal_timer_t          *prev;

    osal_timestamp_t       timestamp;
    osal_timer_mode_t      mode;
    uint32_t               period_ms;
    osal_timer_cb_t        cb;
    osal_size_t            parm;
};

#define  OSAL_TIMER_HANDLE()            \
      do{                               \
          extern volatile  osal_timestamp_t g_osal_timestamp;            \
          extern volatile  osal_timestamp_t g_osal_next_timestamp;            \
          g_osal_timestamp.timestamp_ms = g_osal_timestamp.timestamp_ms + 1;                    \
          if(1000 <= g_osal_timestamp.timestamp_ms)                                             \
          {                                                                                     \
               g_osal_timestamp.timestamp_ms = 0;                                               \
               g_osal_timestamp.timestamp_s  =  g_osal_timestamp.timestamp_s + 1;               \
          }                                                                                     \
          if( (g_osal_timestamp.timestamp_s >= g_osal_next_timestamp.timestamp_s)              \
                          && (g_osal_timestamp.timestamp_ms >= g_osal_next_timestamp.timestamp_ms)  )   \
           {                                                               \
                g_osal_next_timestamp.timestamp_s  = OSAL_TIMERSTAMP_INF; \
                g_osal_next_timestamp.timestamp_ms = OSAL_TIMERSTAMP_INF; \
                OSAL_REALTIME_SIGNAL_EMIT_ISR(OSAL_SIGNAL_APPTIMER, 1);\
           }              \
        }while(0);



extern volatile  osal_timestamp_t g_osal_timestamp; 
extern volatile  osal_timestamp_t g_osal_next_timestamp; 

void osal_timer_init(void);
void osal_timer_create(osal_timer_t *ins, osal_timer_mode_t mode, osal_timer_cb_t cb);
void osal_timer_start(osal_timer_t  *ins, uint32_t timeout_ms, osal_size_t parm);
int32_t osal_timer_is_running(osal_timer_t *ins);
void osal_timer_stop(osal_timer_t  *ins);
void osal_timer_uninit(osal_timer_t *ins);
void osal_timer_task(uint32_t parm);


#endif //_OSAL_TIMER_H_
