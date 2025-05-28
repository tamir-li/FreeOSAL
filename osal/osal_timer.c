#include "osal_timer.h"

void osal_timer_task(uint32_t parm);

osal_timer_t  g_osal_single_timer_head    = {0};
osal_timer_t  g_osal_repeat_timer_head    = {0};   
   
volatile  osal_timestamp_t g_osal_timestamp      = {0};           
volatile  osal_timestamp_t g_osal_next_timestamp = {0}; 


#ifdef OSAL_TIMER_DEBUG
void timer_list_debug(osal_timer_t *list_head)
{
    osal_timer_t  *list_node = NULL;   

    list_node = list_head->next;
    while(list_node != list_head)
    {
        log_d("\t->[timestamp_s: %d, timestamp_ms: %d]\n", list_node->timestamp.timestamp_s, list_node->timestamp.timestamp_ms);
        list_node = list_node->next;
    }
    if(list_node == list_head)
        log_d("\t->[]\n");
}
#endif

int32_t timestamp_compare(osal_timestamp_t *timestamp1, osal_timestamp_t *timestamp2)
{
    int32_t result = 0;

    if(timestamp1->timestamp_s == timestamp2->timestamp_s)
    {
        result = (timestamp1->timestamp_ms > timestamp2->timestamp_ms) - (timestamp1->timestamp_ms < timestamp2->timestamp_ms);
    }
    else
    {
        result = (timestamp1->timestamp_s > timestamp2->timestamp_s) - (timestamp1->timestamp_s < timestamp2->timestamp_s);
    }
#ifdef OSAL_TIMER_DEBUG
    log_d("timestamp_compare, result: %d, t1: [.s: %d, .ms: %d], t2: [.s: %d, .ms: %d]\n", result, timestamp1->timestamp_s, timestamp1->timestamp_ms, timestamp2->timestamp_s, timestamp2->timestamp_ms);
#endif
    return result;
}

int32_t timestamp_calculate(osal_timestamp_t *result, osal_timestamp_t *base, uint32_t timeout_ms)
{
    result->timestamp_s = base->timestamp_s + timeout_ms/1000;
    result->timestamp_ms = base->timestamp_ms + timeout_ms%1000;
#ifdef OSAL_TIMER_DEBUG
    log_d("timestamp_calculate, result->timestamp_s: %d, base->timestamp_s: %d, timeout_s: %d\n", result->timestamp_s, base->timestamp_s, timeout_ms/1000);
    log_d("timestamp_calculate, result->timestamp_ms: %d, result->timestamp_ms: %d, timeout_ms: %d\n", result->timestamp_ms, base->timestamp_ms, timeout_ms%1000);
#endif
    if(1000 <= result->timestamp_ms)
    {
        result->timestamp_ms -= 1000;
        result->timestamp_s ++;
    }
    return 0;
}

void update_next_timestamp(void)
{
    osal_timestamp_t next_timestamp  = {.timestamp_s = OSAL_TIMERSTAMP_INF, .timestamp_ms = OSAL_TIMERSTAMP_INF};           
    osal_timer_t *list_node;           

    if(&g_osal_single_timer_head != g_osal_single_timer_head.next)
    {   
        list_node = g_osal_single_timer_head.next;
        if(0 >= timestamp_compare(&(list_node->timestamp), &next_timestamp))
        {
            next_timestamp.timestamp_ms = list_node->timestamp.timestamp_ms;
            next_timestamp.timestamp_s  = list_node->timestamp.timestamp_s;
        }
    }

    if(&g_osal_repeat_timer_head != g_osal_repeat_timer_head.next)
    {
        list_node = g_osal_repeat_timer_head.next;
        if(0 >= timestamp_compare(&(list_node->timestamp), &next_timestamp))
        {
            next_timestamp.timestamp_ms = list_node->timestamp.timestamp_ms;
            next_timestamp.timestamp_s  = list_node->timestamp.timestamp_s;
        }
    }

    g_osal_next_timestamp.timestamp_s = next_timestamp.timestamp_s;
    g_osal_next_timestamp.timestamp_ms = next_timestamp.timestamp_ms;

#ifdef OSAL_TIMER_DEBUG
    log_d("next_timestamp = {.s: %d, .ms: %d}, curr_timestamp = {.s: %d, .ms: %d}\n", g_osal_next_timestamp.timestamp_s, g_osal_next_timestamp.timestamp_ms, 
                                                      g_osal_timestamp.timestamp_s, g_osal_timestamp.timestamp_ms);
#endif
}

void osal_timer_init(void)
{
    g_osal_single_timer_head.prev = g_osal_single_timer_head.next = &g_osal_single_timer_head;
    g_osal_single_timer_head.timestamp.timestamp_s  = 0;
    g_osal_single_timer_head.timestamp.timestamp_ms = 0;

    g_osal_repeat_timer_head.prev = g_osal_repeat_timer_head.next       = &g_osal_repeat_timer_head;
    g_osal_repeat_timer_head.timestamp.timestamp_s  = 0;
    g_osal_repeat_timer_head.timestamp.timestamp_ms = 0;

    g_osal_timestamp.timestamp_ms            = 0;
    g_osal_timestamp.timestamp_s             = 0;

    g_osal_next_timestamp.timestamp_ms       = OSAL_TIMERSTAMP_INF;
    g_osal_next_timestamp.timestamp_s        = OSAL_TIMERSTAMP_INF;

	  osal_taskslot_connect(OSAL_SIGNAL_APPTIMER, osal_timer_task);

}

void osal_timer_insert(osal_timer_t  *list_head, osal_timer_t  *node)
{
    osal_timer_t  *list_node = list_head;   

    list_node = list_head->next;
    if(list_node == list_head)
    {
        list_head->next = node;
        list_head->prev = node;

        node->next = list_head;
        node->prev = list_head;
    }else
    {
        while(list_node != list_head)
        {
            if( -1 == timestamp_compare(&(node->timestamp), &(list_node->timestamp)))
                break;
        
            list_node = list_node->next;
        }
        node->next = list_node;
        node->prev = list_node->prev;

        list_node->prev->next = node;
        list_node->prev = node;
    }
}

void osal_timer_remove(osal_timer_t  *list_head, osal_timer_t  *node)
{
    if(list_head->next == list_head)
        goto osal_timer_remove_endl;
    
    if(node->next == node)
        goto osal_timer_remove_endl;

    if(node->prev == node)
        goto osal_timer_remove_endl;

    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->next = node;
    node->prev = node;

osal_timer_remove_endl:
    //timer_list_debug(list_head);
    return;
}

void osal_timer_create(osal_timer_t *ins, osal_timer_mode_t mode, osal_timer_cb_t cb)
{
    ins->cb = cb;
    ins->mode = mode;
    ins->next = ins->prev = ins;
}

void osal_timer_start(osal_timer_t  *ins, uint32_t timeout_ms, osal_size_t parm)
{   
    osal_timestamp_t base_timestamp = {
            .timestamp_ms = g_osal_timestamp.timestamp_ms,
            .timestamp_s  = g_osal_timestamp.timestamp_s,
    };
    osal_timer_t  *list_head = NULL;   

    if(OSAL_TIMER_MODE_INVALID == ins->mode)
        return;
    
    if(NULL == ins->cb)
        return;

    ins->next = ins;
    ins->prev = ins;
    ins->period_ms = timeout_ms;
    if(OSAL_TIMER_MODE_SINGLE ==  ins->mode)
    {
        list_head = &g_osal_single_timer_head;
    }
    else
    {
        list_head = &g_osal_repeat_timer_head;
    }

#ifdef OSAL_TIMER_DEBUG
    log_d("pre osal_timer_insert, base_timestamp: [.timestamp_s: %d, .timestamp_ms:%d], ins_timestamp: [.timestamp_s:%d, .timestamp_ms:%d]\n", 
                           base_timestamp.timestamp_s, base_timestamp.timestamp_ms, ins->timestamp.timestamp_s, ins->timestamp.timestamp_ms);
    timer_list_debug(list_head);
#endif

    timestamp_calculate(&(ins->timestamp), &base_timestamp, timeout_ms);
    osal_timer_insert(list_head, ins);

#ifdef OSAL_TIMER_DEBUG
    log_d("post osal_timer_insert, base_timestamp: [.timestamp_s: %d, .timestamp_ms:%d], ins_timestamp: [.timestamp_s:%d, .timestamp_ms:%d]\n", 
                           base_timestamp.timestamp_s, base_timestamp.timestamp_ms, ins->timestamp.timestamp_s, ins->timestamp.timestamp_ms);
    timer_list_debug(list_head);
#endif

    update_next_timestamp();
}


void osal_timer_stop(osal_timer_t  *ins)
{   
    osal_timer_t  *list_head = NULL;   

    if(OSAL_TIMER_MODE_INVALID == ins->mode)
        return;

    if(NULL == ins->cb)
        return;

    if(OSAL_TIMER_MODE_SINGLE ==  ins->mode)
    {
        list_head = &g_osal_single_timer_head;
    }
    else
    {
        list_head = &g_osal_repeat_timer_head;
    }

    osal_timer_remove(list_head, ins);
    ins->next = ins;
    ins->prev = ins;

#ifdef OSAL_TIMER_DEBUG
    timer_list_debug(list_head);
#endif

    update_next_timestamp();
}


int32_t osal_timer_is_running(osal_timer_t *ins)
{
    if((ins == ins->next) && (ins == ins->next))
        return 0;
    else
        return 1;
}

void osal_timer_uninit(osal_timer_t *ins)
{

}

void osal_timer_task(uint32_t parm)
{
    osal_timer_cb_t  timer_cb = NULL;
    osal_size_t      timer_parm;
    osal_timer_t     *list_node = NULL;   
    osal_timestamp_t base_timestamp = {
        .timestamp_ms = g_osal_timestamp.timestamp_ms,
        .timestamp_s  = g_osal_timestamp.timestamp_s,
    };
    
#ifdef    OSAL_TIMER_DEBUG
    log_d("osal_timer_task, base_timestamp: [.s: %d, .ms:%d]n", base_timestamp.timestamp_s, base_timestamp.timestamp_ms);
    timer_list_debug(&g_osal_repeat_timer_head);
    timer_list_debug(&g_osal_single_timer_head);
#endif
    
    if(&g_osal_repeat_timer_head != g_osal_repeat_timer_head.next )
    {
        list_node = g_osal_repeat_timer_head.next;
        while(list_node != &g_osal_repeat_timer_head)
        {
            if(0<timestamp_compare(&(list_node->timestamp), &base_timestamp))
            {
                break;
            }

            osal_timer_remove(&g_osal_repeat_timer_head, list_node);
            timer_cb   = list_node->cb;
            timer_parm = list_node->parm; 
            if(timer_cb)
                timer_cb(timer_parm);

            timestamp_calculate(&(list_node->timestamp), &base_timestamp, list_node->period_ms);
            osal_timer_insert(&g_osal_repeat_timer_head, list_node);
#ifdef    OSAL_TIMER_DEBUG
            timer_list_debug(&g_osal_repeat_timer_head);
#endif
            list_node = g_osal_repeat_timer_head.next;
        }

    }
 

    if(&g_osal_single_timer_head != g_osal_single_timer_head.next )
    {
        list_node = g_osal_single_timer_head.next;
        while(list_node != &g_osal_single_timer_head)
        {
            if(0<timestamp_compare(&(list_node->timestamp), &base_timestamp))
            {
                break;
            }
            osal_timer_remove(&g_osal_single_timer_head, list_node);

            timer_cb   = list_node->cb;
            timer_parm = list_node->parm; 
            if(timer_cb)
                timer_cb(timer_parm);

#ifdef    OSAL_TIMER_DEBUG
            timer_list_debug(&g_osal_single_timer_head);
#endif

            list_node = g_osal_single_timer_head.next;
        }
    }
    update_next_timestamp();
}

