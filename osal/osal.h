#ifndef _OSAL_H_
#define _OSAL_H_

#include <stdlib.h>
#include <string.h>
#include "portosal.h"
#include "elog.h"

#define    OSAL_SIGNAL_QUEUE_SIZE                             (32)
#define    OSAL_REALTIME_SIGNAL_QUEUE_SIZE                    (16)

#define    OSAL_SIGNAL_CACHE_QUEUE_SIZE                       (4)
#define    OSAL_REALTIME_SIGNAL_CACHE_QUEUE_SIZE              (4)

#define    OSAL_FIFO_NEXT_INDEX(cur_index, fifo_size)         (((cur_index) + 1) & ((fifo_size)-1))   
#define    OSAL_FIFO_IS_FULL(rear, front, fifo_size)          ((OSAL_FIFO_NEXT_INDEX((rear),(fifo_size)))==(front))
#define    OSAL_FIFO_IS_EMPTY(rear, front)                    ((rear)==(front))

/****************************************************
*
****************************************************/
typedef uint32_t osal_size_t;


/****************************************************
*
****************************************************/
typedef struct osal_signalfifo_ctl_s
{
	  uint32_t rear_lock;
		uint32_t front_lock;
    uint32_t rear;
    uint32_t front;
    uint32_t err_cnt;
}osal_signalfifo_ctl_t;
	
typedef struct osal_signal_s
{
    osal_size_t signal;
    osal_size_t parm;
}osal_signal_t;


#define    OSAL_SIGNAL_EMIT_ISR(isr_signal, isr_parm)                                                                                                    \
    do{                                                                                                                                        \
        extern volatile osal_signalfifo_ctl_t     g_osal_signalfifo_ctl;                                                                       \
        extern volatile osal_signal_t             g_osal_signalfifo[];                                                                         \
	      if( !(OSAL_FIFO_IS_FULL(g_osal_signalfifo_ctl.rear, g_osal_signalfifo_ctl.front, OSAL_SIGNAL_QUEUE_SIZE)) )                            \
				{                                                                                                                                      \
				    g_osal_signalfifo[g_osal_signalfifo_ctl.rear].signal = (isr_signal);                                                               \
				    g_osal_signalfifo[g_osal_signalfifo_ctl.rear].parm = (isr_parm);                                                                   \
		        g_osal_signalfifo_ctl.rear = OSAL_FIFO_NEXT_INDEX(g_osal_signalfifo_ctl.rear, OSAL_SIGNAL_QUEUE_SIZE);                             \
				}                                                                                                                                      \
				else                                                                                                                                   \
				{                                                                                                                                      \
				    g_osal_signalfifo_ctl.err_cnt++;                                                                                                   \
				}                                                                                                                                      \
    }while(0)                        

						 
#define    OSAL_REALTIME_SIGNAL_EMIT_ISR(isr_rt_signal, isr_rt_parm)                                                                           \
    do{                                                                                                                                        \
        extern volatile osal_signalfifo_ctl_t     g_osal_rt_signalfifo_ctl;                                                                    \
        extern volatile osal_signal_t             g_osal_rt_signalfifo[];                                                                      \
	      if( !(OSAL_FIFO_IS_FULL(g_osal_rt_signalfifo_ctl.rear, g_osal_rt_signalfifo_ctl.front, OSAL_REALTIME_SIGNAL_QUEUE_SIZE) ) )            \
				{                                                                                                                                      \
				    g_osal_rt_signalfifo[g_osal_rt_signalfifo_ctl.rear].signal = (isr_rt_signal);                                                      \
						g_osal_rt_signalfifo[g_osal_rt_signalfifo_ctl.rear].parm = (isr_rt_parm);                                                         \
		        g_osal_rt_signalfifo_ctl.rear = OSAL_FIFO_NEXT_INDEX(g_osal_rt_signalfifo_ctl.rear, OSAL_REALTIME_SIGNAL_QUEUE_SIZE);            \
				}                                                                                                                                      \
			  else                                                                                                                                   \
				{                                                                                                                                      \
				    g_osal_rt_signalfifo_ctl.err_cnt++;                                                                                                \
			  }                                                                                                                                      \
    }while(0)                        

						 

#define    OSAL_FLUSH_SIGNAL_ISR()                                                                                                             \
            do{                                                                                                                                \
                 extern volatile osal_signalfifo_ctl_t     g_osal_signalfifo_ctl;                                                              \
                 extern volatile osal_signalfifo_ctl_t     g_osal_signalfifo_cache_ctl;                                                        \
                 extern volatile osal_signal_t             g_osal_signalfifo[];                                                                \
     						 extern volatile osal_signal_t             g_osal_signalfifo_cache[];                                                          \
                 if(!(OSAL_FIFO_IS_EMPTY(g_osal_signalfifo_cache_ctl.rear, g_osal_signalfifo_cache_ctl.front)))                                  \
								 {                                                                                                                             \
	                   if(! (OSAL_FIFO_IS_FULL(g_osal_signalfifo_ctl.rear, g_osal_signalfifo_ctl.front, OSAL_SIGNAL_QUEUE_SIZE) ) )              \
								     {                                                                                                                         \
                          g_osal_signalfifo[g_osal_signalfifo_ctl.rear] = g_osal_signalfifo_cache[g_osal_signalfifo_cache_ctl.front];          \
		                      g_osal_signalfifo_ctl.rear = OSAL_FIFO_NEXT_INDEX(g_osal_signalfifo_ctl.rear, OSAL_SIGNAL_QUEUE_SIZE);             \
											 		g_osal_signalfifo_cache_ctl.front = OSAL_FIFO_NEXT_INDEX(g_osal_signalfifo_cache_ctl.front, OSAL_SIGNAL_CACHE_QUEUE_SIZE);\
								     }                                                                                                                         \
								     else                                                                                                                      \
								     {                                                                                                                         \
									        g_osal_signalfifo_ctl.err_cnt++;                                                                                     \
								     }                                                                                                                         \
							   }                                                                                   \
             }while(0)                        

						 
#define    OSAL_FLUSH_REALTIME_SIGNAL_ISR()                                                                                                    \
            do{                                                                                                                                \
                 extern volatile osal_signalfifo_ctl_t     g_osal_rt_signalfifo_ctl;                                                           \
                 extern volatile osal_signalfifo_ctl_t     g_osal_rt_signalfifo_cache_ctl;                                                     \
                 extern volatile osal_signal_t             g_osal_rt_signalfifo[];                                                             \
     						 extern volatile osal_signal_t             g_osal_rt_signalfifo_cache[];                                                       \
	               if( !(OSAL_FIFO_IS_EMPTY(g_osal_rt_signalfifo_cache_ctl.rear, g_osal_rt_signalfifo_cache_ctl.front)) )                        \
								 {                                                                                                                             \
	                   if(OSAL_FIFO_NEXT_INDEX(g_osal_rt_signalfifo_ctl.front, OSAL_REALTIME_SIGNAL_QUEUE_SIZE) != g_osal_rt_signalfifo_ctl.rear)\
								     {                                                                                                                         \
                          g_osal_rt_signalfifo[g_osal_rt_signalfifo_ctl.rear] = g_osal_rt_signalfifo_cache[g_osal_rt_signalfifo_cache_ctl.front];\
		                      g_osal_rt_signalfifo_ctl.rear = OSAL_FIFO_NEXT_INDEX(g_osal_rt_signalfifo_ctl.rear, OSAL_REALTIME_SIGNAL_QUEUE_SIZE); \
											 		g_osal_rt_signalfifo_cache_ctl.front = OSAL_FIFO_NEXT_INDEX(g_osal_rt_signalfifo_cache_ctl.front, OSAL_REALTIME_SIGNAL_CACHE_QUEUE_SIZE);\
								     }                                                                                                                         \
								     else                                                                                                                      \
								     {                                                                                                                         \
									        g_osal_signalfifo_ctl.err_cnt++;                                                                                     \
								     }                                                                                                                         \
							   }                                                                                                                             \
             }while(0)                        

//isr-in  user-out
extern volatile osal_signalfifo_ctl_t     g_osal_rt_signalfifo_ctl;
extern volatile osal_signalfifo_ctl_t     g_osal_signalfifo_ctl;
extern volatile osal_signal_t             g_osal_rt_signalfifo[];
extern volatile osal_signal_t             g_osal_signalfifo[];

//user-in  isr-out  
extern volatile osal_signalfifo_ctl_t     g_osal_rt_signalfifo_cache_ctl;
extern volatile osal_signalfifo_ctl_t     g_osal_signalfifo_cache_ctl;
extern volatile osal_signal_t             g_osal_rt_signalfifo_cache[];
extern volatile osal_signal_t             g_osal_signalfifo_cache[];

/****************************************************
*
****************************************************/
typedef void (* osal_taskslot_fn_t)(osal_size_t parm);
typedef struct osal_taskslot_s
{
    osal_signal_id_t signal_id;
	  osal_taskslot_fn_t fn;
}osal_taskslot_t;

int32_t osal_init(void);
int32_t osal_enable(void);
int32_t osal_disable(void);
void osal_taskslot_connect(osal_signal_id_t signal_id, osal_taskslot_fn_t fn);
void osal_taskslot_disconnect(osal_signal_id_t signal_id);
void osal_signal_emit(osal_signal_id_t signal, osal_size_t parm);
void osal_rt_signal_emit(osal_signal_id_t signal, osal_size_t parm);
void osal_loop(void);


#endif /* _OSAL_H_ */

