#ifndef _PORTOSAL_H_
#define _PORTOSAL_H_

#include "soc.h"

//#define PORTOSALI_SOFTIRQ_PRI

/****************************************************
*
****************************************************/
typedef uint32_t osal_size_t;

/****************************************************
*
****************************************************/
typedef enum osal_signal_id_e
{
	// OSAL-Reserved
    OSAL_SIGNAL_KEEPALIVE = 0,
    OSAL_SIGNAL_APPTIMER  = 1,
	
	//TODO:APP
	//APP_SIGNAL_XXX,

	// OSAL-Reserved
   OSAL_SIGNAL_ID_MAX,
}osal_signal_id_t;
#define  OSAL_SIGNAL_ID_TABLE_SIZE    (16U)     //must gte OSAL_SIGNAL_ID_MAX

void _portosal_keepalive_init(void);
void _portosal_keepalive_task(osal_size_t parm);
void _portosal_wfe(void);
void _portosal_task_hooks(void);
void _portosal_softirq_init(void);   //flush task-queue
void _portosal_softirq_entry(void);
#endif /* _PORTOSAL_H_ */

