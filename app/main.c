#include "main.h"
#include "elog.h"
#include "pelcod.h"
#include "osal.h"


void test_elog(void) 
{
    /* test log output for all level */
    log_a("Hello EasyLogger!111");
    log_e("Hello EasyLogger!222");
    log_w("Hello EasyLogger!333");
    log_i("Hello EasyLogger!444");
    log_d("Hello EasyLogger!555");
    log_v("Hello EasyLogger!666");
  //  elog_raw("Hello EasyLogger!777");
}

int main()
{
    soc_init();
		osal_init();
    osal_timer_init();
    
		log_init();
		log_start();
#if 0
	  test_elog();
#endif
    systick_config();

	  bsp_init();

    osal_enable();
    log_d("osal running...");
   	for(;;)
   	{
        //osal-task-loop
   	   	osal_loop();
   	}
//    return 0;
}
