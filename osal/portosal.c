#include "portosal.h"

void _portosal_wfe(void)
{
    pmu_wfe();
}

void _portosal_keepalive_init(void)
{
    pmu_enable();
 		rtc_enable();
    wdtdog_enable();
}

void _portosal_keepalive_task(osal_size_t parm)
{
    static uint32_t cnt = 0;
    if(0 == (cnt++%8))
       log_d("keepalive_task");
		wdtdog_feed();
}

void _portosal_softirq_init(void)
{
		nvic_irq_enable(PendSV_IRQn, 0, 0);
}

void _portosal_softirq_entry(void)
{
	  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
