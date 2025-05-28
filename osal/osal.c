#include "osal.h"

//isr-in  user-out
volatile osal_signalfifo_ctl_t     g_osal_rt_signalfifo_ctl                                          = {0};
volatile osal_signalfifo_ctl_t     g_osal_signalfifo_ctl                                             = {0};
volatile osal_signal_t             g_osal_rt_signalfifo[OSAL_REALTIME_SIGNAL_QUEUE_SIZE]             = {0};
volatile osal_signal_t             g_osal_signalfifo[OSAL_SIGNAL_QUEUE_SIZE]                         = {0};

//user-in  isr-out  
volatile osal_signalfifo_ctl_t     g_osal_rt_signalfifo_cache_ctl                                    = {0};
volatile osal_signalfifo_ctl_t     g_osal_signalfifo_cache_ctl                                       = {0};
volatile osal_signal_t             g_osal_rt_signalfifo_cache[OSAL_REALTIME_SIGNAL_CACHE_QUEUE_SIZE] = {0};
volatile osal_signal_t             g_osal_signalfifo_cache[OSAL_SIGNAL_CACHE_QUEUE_SIZE]             = {0};

static osal_taskslot_t g_taskslot_tab[OSAL_SIGNAL_ID_TABLE_SIZE];

int32_t osal_init(void)
{
	  g_osal_rt_signalfifo_ctl.err_cnt    = 0;
	  g_osal_rt_signalfifo_ctl.front      = 0;
	  g_osal_rt_signalfifo_ctl.front_lock = 0;
	  g_osal_rt_signalfifo_ctl.rear       = 0;
	  g_osal_rt_signalfifo_ctl.rear_lock  = 0;
	 
		g_osal_signalfifo_ctl.err_cnt       = 0;
	  g_osal_signalfifo_ctl.front         = 0;
	  g_osal_signalfifo_ctl.front_lock    = 0;
	  g_osal_signalfifo_ctl.rear          = 0;
	  g_osal_signalfifo_ctl.rear_lock     = 0;
	
		g_osal_rt_signalfifo_cache_ctl.err_cnt    = 0;
	  g_osal_rt_signalfifo_cache_ctl.front      = 0;
	  g_osal_rt_signalfifo_cache_ctl.front_lock = 0;
	  g_osal_rt_signalfifo_cache_ctl.rear       = 0;
	  g_osal_rt_signalfifo_cache_ctl.rear_lock  = 0;
	 
		g_osal_signalfifo_cache_ctl.err_cnt       = 0;
	  g_osal_signalfifo_cache_ctl.front         = 0;
	  g_osal_signalfifo_cache_ctl.front_lock    = 0;
	  g_osal_signalfifo_cache_ctl.rear          = 0;
	  g_osal_signalfifo_cache_ctl.rear_lock     = 0;
	
	  memset(g_taskslot_tab, 0, sizeof(g_taskslot_tab));
		if(OSAL_SIGNAL_ID_TABLE_SIZE < OSAL_SIGNAL_ID_MAX)
		{
			  return -1;
		}
		
		return 0;
}

int32_t osal_enable(void)
{
	  osal_taskslot_connect(OSAL_SIGNAL_KEEPALIVE, _portosal_keepalive_task);

		_portosal_keepalive_init();
		_portosal_softirq_init();
   
   return 0;
}

int32_t osal_disable(void)
{
	  //osal_taskslot_connect(OSAL_SIGNAL_KEEPALIVE, _portosal_keepalive_task);
   return 0;
}


static void	osal_idle_handle(void)
{
  	_portosal_wfe();
}

static void	osal_task_process(void)
{
	//TODO:
    for(;;)
  	{
	      //task_process
        do
		    {
		        if( !(OSAL_FIFO_IS_EMPTY(g_osal_rt_signalfifo_ctl.rear, g_osal_rt_signalfifo_ctl.front)))
	        	{
		  		  	  osal_signal_t temp_rt_signal = g_osal_rt_signalfifo[g_osal_rt_signalfifo_ctl.front];
								osal_taskslot_fn_t temp_rt_fn = g_taskslot_tab[temp_rt_signal.signal].fn;
                
                //log_i("1-deq. %d\n", temp_rt_signal.signal);                                            
			  				g_osal_rt_signalfifo_ctl.front = OSAL_FIFO_NEXT_INDEX(g_osal_rt_signalfifo_ctl.front, OSAL_REALTIME_SIGNAL_QUEUE_SIZE);
				    	  if(temp_rt_fn)
				  		  	 temp_rt_fn((osal_size_t)temp_rt_signal.parm);
							
				        continue;
	    	    }
            break;
	  	  }while(1);
			
			
		 	  if( !(OSAL_FIFO_IS_EMPTY(g_osal_signalfifo_ctl.rear, g_osal_signalfifo_ctl.front)))
		   	{
		  		  osal_signal_t temp_signal = g_osal_signalfifo[g_osal_signalfifo_ctl.front];
						osal_taskslot_fn_t temp_fn = g_taskslot_tab[temp_signal.signal].fn;
            //log_i("0-deq. %d, %d, %d\n", temp_signal.signal, g_osal_signalfifo_ctl.front, g_osal_signalfifo_ctl.rear);                                            

			  		g_osal_signalfifo_ctl.front = OSAL_FIFO_NEXT_INDEX(g_osal_signalfifo_ctl.front, OSAL_SIGNAL_QUEUE_SIZE);
				    if(temp_fn)
				  		  	 temp_fn((osal_size_t)temp_signal.parm);
						
		  			continue;
				}
				break;
  	}
 
}

void osal_taskslot_connect(osal_signal_id_t signal_id, osal_taskslot_fn_t fn)
{
    uint32_t offset = signal_id;
    g_taskslot_tab[offset].signal_id = signal_id;
	  g_taskslot_tab[offset].fn = fn;
}

void osal_taskslot_disconnect(osal_signal_id_t signal_id)
{
    uint32_t offset = signal_id;
    g_taskslot_tab[offset].signal_id = signal_id;
	  g_taskslot_tab[offset].fn = NULL;
}


void osal_signal_emit(osal_signal_id_t signal, osal_size_t parm)
{
	  uint32_t offset = (uint32_t)signal;
		if(NULL == g_taskslot_tab[offset].fn)
			return;
	
    if(OSAL_FIFO_NEXT_INDEX(g_osal_signalfifo_cache_ctl.front, OSAL_SIGNAL_CACHE_QUEUE_SIZE) == g_osal_signalfifo_cache_ctl.rear)
		{
		    g_osal_signalfifo_cache_ctl.err_cnt++;
			  goto osal_signal_emit_endl;
		}
		g_osal_signalfifo_cache[g_osal_signalfifo_cache_ctl.rear].signal = (osal_size_t)signal;
		g_osal_signalfifo_cache[g_osal_signalfifo_cache_ctl.rear].parm   = (osal_size_t)parm;

		g_osal_signalfifo_cache_ctl.rear = OSAL_FIFO_NEXT_INDEX(g_osal_signalfifo_cache_ctl.rear, OSAL_SIGNAL_CACHE_QUEUE_SIZE);
osal_signal_emit_endl:
		do{ _portosal_softirq_entry();}while(g_osal_signalfifo_cache_ctl.front != g_osal_signalfifo_cache_ctl.rear);
		return;
}

void osal_rt_signal_emit(osal_signal_id_t signal, osal_size_t parm)
{
	  uint32_t offset = (uint32_t)signal;
		if(NULL == g_taskslot_tab[offset].fn)
			return;
		
    if(OSAL_FIFO_NEXT_INDEX(g_osal_rt_signalfifo_cache_ctl.front, OSAL_REALTIME_SIGNAL_CACHE_QUEUE_SIZE) == g_osal_rt_signalfifo_cache_ctl.rear)
		{
		    g_osal_rt_signalfifo_cache_ctl.err_cnt++;
			  goto osal_rt_signal_emit_endl;
		}
		g_osal_rt_signalfifo_cache[g_osal_rt_signalfifo_cache_ctl.rear].signal = (osal_size_t)signal;
		g_osal_rt_signalfifo_cache[g_osal_rt_signalfifo_cache_ctl.rear].parm   = (osal_size_t)parm;
		g_osal_rt_signalfifo_cache_ctl.rear = OSAL_FIFO_NEXT_INDEX(g_osal_rt_signalfifo_cache_ctl.rear, OSAL_REALTIME_SIGNAL_CACHE_QUEUE_SIZE);
		
osal_rt_signal_emit_endl:
		do{ _portosal_softirq_entry();}while(g_osal_rt_signalfifo_cache_ctl.front != g_osal_rt_signalfifo_cache_ctl.rear);
		return;
}

void osal_loop(void)
{
//	for(;;)
//	{
		osal_task_process();
		osal_idle_handle();
//	}
}
