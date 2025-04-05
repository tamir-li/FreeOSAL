#include "portosal.h"

void _portosal_wfe(void)
{
    //pmu_wfe();
}

void _portosal_keepalive_init(void)
{
   rtc_init();
	
   wdtdog_init();
   //wdtdog_enable();
}

void _portosal_keepalive_task(osal_size_t parm)
{
//  wdtdog_feed();
}


void _portosal_task_hooks(void)
{

}

void _portosal_softirq_init(void)
{
    //ARM
    nvic_irq_enable(PendSV_IRQn, 0, 0);

    //RISC-V
    //NVIC_SetPriority(Software_IRQn, 0);       

}

void _portosal_softirq_entry(void)
{
    //ARM
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

    //RISC-V
    //NVIC_SetPendingIRQ(Software_IRQn);
}
