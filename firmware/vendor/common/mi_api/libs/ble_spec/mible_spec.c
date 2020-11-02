#include "mible_spec.h"
#include "common/mible_rxfer.h"

#define MIBLE_MIBLE_SPEC_DATA_QUEUE_SIZE 4
#define MIBLE_SPEC_CALLBACK_MAX  1
#define MIBLE_SPEC_MAX_DATA_LENGTH 1024

static uint16_t s_tid = 0;

static rxfer_cb_t rxfer_cb_spec_recv;
static rxfer_cb_t rxfer_cb_spec_send;

static pt_t pt_spec_send, pt_spec_recv;
static pt_t pt_rxfer_tx, pt_rxfer_rx;

void * rx_timer_id;
void * tx_timer_id;
static uint8_t s_recv_buff[MIBLE_SPEC_MAX_DATA_LENGTH] = {0};
static uint16_t s_recv_buff_len = 0;

static mible_spec_data_t s_send_data;
static mible_spec_data_t s_recv_data;

static uint32_t spec_send_timer_tick;
static uint32_t spec_recv_timer_tick;
static pstimer_t spec_send_timer = {
    .p_curr_tick = &spec_send_timer_tick,
    .tick_interval_ms = MIBLE_SPEC_TIMER_PERIOD,
};
static pstimer_t spec_recv_timer = {
    .p_curr_tick = &spec_recv_timer_tick,
    .tick_interval_ms = MIBLE_SPEC_TIMER_PERIOD,
};

static mible_spec_callback_t spec_callback[MIBLE_SPEC_CALLBACK_MAX] = {0};

static void foreach_callback(mible_spec_command_t state, uint16_t tid, uint8_t p_num, spec_property_t *param)
{
    for (int index = 0; index < MIBLE_SPEC_CALLBACK_MAX; index++) {
        if (spec_callback[index])
            spec_callback[index](state, tid, p_num, param);
    }
}

#if (MIBLE_MIBLE_SPEC_DATA_QUEUE_SIZE > 0 )
static queue_t mi_spec_send_queue;
static mible_spec_data_t spec_send_data_buf[MIBLE_MIBLE_SPEC_DATA_QUEUE_SIZE];

static mible_status_t sent_data_enque(uint8_t* addr, uint16_t len)
{
    mible_spec_data_t data;
    data.addr = addr;
    data.len  = len;

    if(enqueue(&mi_spec_send_queue, &data) == MI_ERR_NO_MEM) {
        MI_LOG_ERROR("mi_spec_send_queue full.  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        //free(addr);
        return MI_ERR_RESOURCES;
    }
    
    MI_LOG_DEBUG("sent_data_enque OK.\n");
    
    return  MI_SUCCESS;
}


static void sent_data_queue_flush()
{
    mible_spec_data_t data;
    while(dequeue(&mi_spec_send_queue, &data) == MI_SUCCESS){}
        //free(data.addr);
}
#endif

static int spec_data_send(pt_t * pt)
{    
    uint8_t stat;

    PT_BEGIN(pt);    
    
    if (dequeue(&mi_spec_send_queue, &s_send_data) == MI_ERR_NOT_FOUND) {
        goto EXIT;
    }
    
    MI_LOG_DEBUG("in spec_data_send %d\n", s_send_data.len);
    MI_HEXDUMP(s_send_data.addr, s_send_data.len);
    
    format_tx_cb(&rxfer_cb_spec_send, s_send_data.addr, s_send_data.len);
    PT_SPAWN(pt, &pt_rxfer_tx, stat = rxfer_tx_thd(&pt_rxfer_tx, &rxfer_cb_spec_send, PASS_THROUGH));
    
    if (stat == PT_EXITED) {
        //free(s_send_data.addr);
        s_send_data.addr = NULL;
        goto EXIT;
    }
            
    MI_LOG_DEBUG("data really send OK\n");  
		
    //free(s_send_data.addr); 
    s_send_data.addr = NULL;		
    PT_END(pt);
    
EXIT:
    PT_EXIT(pt);
}


static void mible_spec_send_timer_callback(void * arg)
{
    spec_send_timer_tick++;
    uint8_t stat = spec_data_send(&pt_spec_send);
    if (stat == PT_ENDED) {
        MI_LOG_DEBUG("send OK \n");              
        PT_INIT(&pt_spec_send);
    }
    else if(stat == PT_EXITED) {              
        PT_INIT(&pt_spec_send);
    }
}

static uint8_t is_valid_type_length(uint8_t type, uint16_t length)
{
    switch(type) {
        case 0x00:
        case 0x01:
        case 0x02:
            if(1 == length)
                return 1;
            else
            {
                MI_LOG_ERROR("value type %d should has length 1 bytes, now %d bytes\n", type, length);
                return 0;
            }
            //break;
        case 0x03:
        case 0x04:
            if(2 == length)
                return 1;
            else
            {
                MI_LOG_ERROR("value type %d should has length 2 bytes, now %d bytes\n", type, length);
                return 0;
            }
            //break;
        case 0x05:
        case 0x06:
        case 0x09:
            if(4 == length)
                return 1;
            else
            {
                MI_LOG_ERROR("value type %d should has length 4 bytes, now %d bytes\n", type, length);
                return 0;
            }
            //break;
        case 0x07:
        case 0x08:
            if(8 == length)
                return 1;
            else
            {
                MI_LOG_ERROR("value type %d should has length 8 bytes, now %d bytes\n", type, length);
                return 0;
            }
            //break;
        case 0x0A:
            if(length > MIBLE_SPEC_MAX_DATA_LENGTH)
            {
                MI_LOG_ERROR("value type %d should has length less than %d bytes, now %d bytes\n", type, MIBLE_SPEC_MAX_DATA_LENGTH, length);
                return 0;
            }
            else
                return 1;
            //break;
        default:
            MI_LOG_ERROR("value type %d ERROR!\n", type);
            break;        
    }
    return 0;    
}


/***********************spec data send func***************************/
uint8_t mible_rxfer_buf[MIBLE_SPEC_MAX_DATA_LENGTH];
mible_status_t mible_set_properties_rsp(uint16_t tid, uint8_t p_num, spec_property_t* array)
{
    MI_LOG_DEBUG("set_properties rsp, tid = %2X\n", tid);
    
    //uint8_t* buff = (uint8_t*)malloc(MIBLE_SPEC_MAX_DATA_LENGTH * sizeof(uint8_t));
    uint8_t* buff  = mible_rxfer_buf;
    if(NULL == buff)
    {
        MI_LOG_ERROR("malloc fail  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_NO_MEM;    
    }

    uint16_t idx = 0;

    //Header
    uint16_t header = 0;
    memset(&header, 0, sizeof(header));
    header |= MIBLE_SPEC_VERSION << 12;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));    
    idx += sizeof(header);
    
    //tid
    memcpy(buff+idx, &tid, sizeof(tid));
    MI_LOG_DEBUG("tid:\n");
    MI_LOG_HEXDUMP(&tid, sizeof(tid));
    idx += sizeof(tid);
    
    //opcode
    uint8_t opcode = MIBLE_SPEC_OP_SET_PROPERTY_RSP;
    memcpy(buff+idx, &opcode, sizeof(opcode));
    MI_LOG_DEBUG("opcode:\n");
    MI_LOG_HEXDUMP(&opcode, sizeof(opcode));
    idx += sizeof(opcode);
    
    //properties-num 
    memcpy(buff+idx, &p_num, sizeof(p_num));
    MI_LOG_DEBUG("p_num:\n");
    MI_LOG_HEXDUMP(&p_num, sizeof(p_num));
    idx += sizeof(p_num);
    
    //properties and code
    for(uint8_t i=0; i<p_num; i++)
    {
        //siid
        memcpy(buff+idx, &(array[i].siid), sizeof(array[i].siid));
        MI_LOG_DEBUG("siid:\n");
        MI_LOG_HEXDUMP(&(array[i].siid), sizeof(array[i].siid));
        idx += sizeof(array[i].siid);
        
        //piid
        memcpy(buff+idx, &(array[i].piid), sizeof(array[i].piid));
        MI_LOG_DEBUG("piid:\n");
        MI_LOG_HEXDUMP(&(array[i].piid), sizeof(array[i].piid));
        idx += sizeof(array[i].piid);
        
        //code
        memcpy(buff+idx, (uint8_t*)&(array[i].code), sizeof(array[i].code));
        MI_LOG_DEBUG("code:\n");
        MI_LOG_HEXDUMP(&(array[i].code), sizeof(array[i].code));
        idx += sizeof(array[i].code);
    }
    
    if(idx > MIBLE_SPEC_MAX_DATA_LENGTH-6)
    {
        MI_LOG_ERROR("send msg too long  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }
    
    //new header
    header += idx;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));
    memcpy(buff, &header, sizeof(header));
    
    MI_LOG_HEXDUMP(buff, idx);
    
    mi_session_encrypt(buff, idx, buff);
    //MI_LOG_HEXDUMP(buff, idx+6);    

    //enqueue spec data
    mible_status_t ret = sent_data_enque(buff, idx+6);
    if(ret != MI_SUCCESS)
    {
        MI_LOG_ERROR("sent_data_enque error ! ret = %d, %s:%d\n", ret, (uint32_t)__FILE__, __LINE__);
        return ret;
    }

    return MI_SUCCESS;
        
}

mible_status_t mible_get_properties_rsp(uint16_t tid, uint8_t p_num, spec_property_t* array)
{
    MI_LOG_DEBUG("get_properties rsp, tid = %2X\n", tid);
    
   // uint8_t* buff = malloc(MIBLE_SPEC_MAX_DATA_LENGTH * sizeof(uint8_t));
   uint8_t* buff  = mible_rxfer_buf;
    if(NULL == buff)
    {
        MI_LOG_ERROR("malloc fail  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_NO_MEM;    
    }

    uint16_t idx = 0;
    
    //Header
    uint16_t header = 0;
    memset(&header, 0, sizeof(header));
    header |= MIBLE_SPEC_VERSION << 12;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));    
    idx += sizeof(header);
    
    //tid
    memcpy(buff+idx, &tid, sizeof(tid));
    MI_LOG_DEBUG("tid:\n");
    MI_LOG_HEXDUMP(&tid, sizeof(tid));
    idx += sizeof(tid);
    
    //opcode
    uint8_t opcode = MIBLE_SPEC_OP_GET_PROPERTY_RSP;
    memcpy(buff+idx, &opcode, sizeof(opcode));
    MI_LOG_DEBUG("opcode:\n");
    MI_LOG_HEXDUMP(&opcode, sizeof(opcode));
    idx += sizeof(opcode);
    
    //properties-num 
    memcpy(buff+idx, &p_num, sizeof(p_num));
    MI_LOG_DEBUG("p_num:\n");
    MI_LOG_HEXDUMP(&p_num, sizeof(p_num));
    idx += sizeof(p_num);
    
    //properties
    for(uint8_t i=0; i<p_num; i++)
    {
        //siid
        memcpy(buff+idx, &(array[i].siid), sizeof(array[i].siid));
        MI_LOG_DEBUG("siid:\n");
        MI_LOG_HEXDUMP(&(array[i].siid), sizeof(array[i].siid));
        idx += sizeof(array[i].siid);
        
        //piid
        memcpy(buff+idx, &(array[i].piid), sizeof(array[i].piid));
        MI_LOG_DEBUG("piid:\n");
        MI_LOG_HEXDUMP(&(array[i].piid), sizeof(array[i].piid));
        idx += sizeof(array[i].piid);
        
        //code
        memcpy(buff+idx, (uint8_t*)&(array[i].code), sizeof(array[i].code));
        MI_LOG_DEBUG("code:\n");
        MI_LOG_HEXDUMP(&(array[i].code), sizeof(array[i].code));
        idx += sizeof(array[i].code);
        
        //type and len
        if(is_valid_type_length(array[i].type, array[i].len) != 1)
        {
            MI_LOG_ERROR("invalid type and length!!!  %s:%d\n", (uint32_t)__FILE__, __LINE__);
            return MI_ERR_INVALID_LENGTH;
        }
        uint16_t temp = 0;
        memset(&temp, 0, sizeof(temp));
        temp |= array[i].type << 12;
        temp += array[i].len;
        MI_LOG_DEBUG("temp:\n");
        MI_LOG_HEXDUMP(&temp, sizeof(temp));        
        memcpy(buff+idx, &temp, sizeof(temp));
        idx += sizeof(temp);
        
        //value
        memcpy(buff+idx, array[i].val, array[i].len);
        MI_LOG_DEBUG("val:\n");
        MI_LOG_HEXDUMP(array[i].val, array[i].len);
        idx += array[i].len;
    }
    
    if(idx > MIBLE_SPEC_MAX_DATA_LENGTH-6)
    {
        MI_LOG_ERROR("send msg too long  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }
    
    //new header
    header += idx;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));
    memcpy(buff, &header, sizeof(header));
    
    MI_LOG_HEXDUMP(buff, idx);
    
    mi_session_encrypt(buff, idx, buff);
    MI_LOG_HEXDUMP(buff, idx+6);    
     
    //enqueue spec data
    mible_status_t ret = sent_data_enque(buff, idx+6);
    if(ret != MI_SUCCESS)
    {
        MI_LOG_ERROR("sent_data_enque error ! ret = %d, %s:%d\n", ret, (uint32_t)__FILE__, __LINE__);
        return ret;
    }

    return MI_SUCCESS;
    
}

mible_status_t mible_action_rsp(uint16_t tid, int16_t code)
{
    MI_LOG_DEBUG("action rsp, tid = %2X\n", tid);
    
   // uint8_t* buff = malloc(MIBLE_SPEC_MAX_DATA_LENGTH * sizeof(uint8_t));
   uint8_t* buff  = mible_rxfer_buf;
    if(NULL == buff)
    {
        MI_LOG_ERROR("malloc fail  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_NO_MEM;    
    }

    uint16_t idx = 0;
    
    //Header
    uint16_t header = 0;
    memset(&header, 0, sizeof(header));
    header |= MIBLE_SPEC_VERSION << 12;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));    
    idx += sizeof(header);
    
    //tid
    memcpy(buff+idx, &tid, sizeof(tid));
    MI_LOG_DEBUG("tid:\n");
    MI_LOG_HEXDUMP(&tid, sizeof(tid));
    idx += sizeof(tid);
    
    //opcode
    uint8_t opcode = MIBLE_SPEC_OP_ACTION_RSP;
    memcpy(buff+idx, &opcode, sizeof(opcode));
    MI_LOG_DEBUG("opcode:\n");
    MI_LOG_HEXDUMP(&opcode, sizeof(opcode));
    idx += sizeof(opcode);
    
    //code
    memcpy(buff+idx, &code, sizeof(code));
    MI_LOG_DEBUG("code:\n");
    MI_LOG_HEXDUMP(&code, sizeof(code));
    idx += sizeof(code);
    
    //new header
    header += idx;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));
    memcpy(buff, &header, sizeof(header));
    
    if(idx != 7)
    {
        MI_LOG_ERROR("mible_action_rsp length error!  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }
    
    MI_LOG_HEXDUMP(buff, idx);
    
    mi_session_encrypt(buff, idx, buff);
    MI_LOG_HEXDUMP(buff, idx+6);     
     
    //enqueue spec data
    mible_status_t ret = sent_data_enque(buff, idx+6);
    if(ret != MI_SUCCESS)
    {
        MI_LOG_ERROR("sent_data_enque error ! ret = %d, %s:%d\n", ret, (uint32_t)__FILE__, __LINE__);
        return ret;
    }

    return MI_SUCCESS;
    
}

mible_status_t mible_properties_changed(uint8_t p_num, spec_property_t* array)
{
    MI_LOG_DEBUG("mible_properties_changed\n");
    
    //uint8_t* buff = malloc(MIBLE_SPEC_MAX_DATA_LENGTH * sizeof(uint8_t));
    uint8_t* buff  = mible_rxfer_buf;
    if(NULL == buff)
    {
        MI_LOG_ERROR("malloc fail  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_NO_MEM;    
    }

    uint16_t idx = 0;
    
    //Header
    uint16_t header = 0;
    memset(&header, 0, sizeof(header));
    header |= MIBLE_SPEC_VERSION << 12;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));    
    idx += sizeof(header);
    
    //tid
    uint16_t tid = s_tid + 1;
    MI_LOG_DEBUG("tid:\n");
    MI_LOG_HEXDUMP(&tid, sizeof(tid));
    memcpy(buff+idx, &tid, sizeof(tid));
    idx += sizeof(tid);
    
    //opcode
    uint8_t opcode = MIBLE_SPEC_OP_PROPERTY_CHANGED;
    memcpy(buff+idx, &opcode, sizeof(opcode));
    MI_LOG_DEBUG("opcode:\n");
    MI_LOG_HEXDUMP(&opcode, sizeof(opcode));
    idx += sizeof(opcode);
    
    //properties-num 
    memcpy(buff+idx, &p_num, sizeof(p_num));
    MI_LOG_DEBUG("p_num:\n");
    MI_LOG_HEXDUMP(&p_num, sizeof(p_num));
    idx += sizeof(p_num);
    
    //properties
    for(uint8_t i=0; i<p_num; i++)
    {
        //siid
        memcpy(buff+idx, &(array[i].siid), sizeof(array[i].siid));
        MI_LOG_DEBUG("siid:\n");
        MI_LOG_HEXDUMP(&(array[i].siid), sizeof(array[i].siid));
        idx += sizeof(array[i].siid);
        
        //piid
        memcpy(buff+idx, &(array[i].piid), sizeof(array[i].piid));
        MI_LOG_DEBUG("piid:\n");
        MI_LOG_HEXDUMP(&(array[i].piid), sizeof(array[i].piid));
        idx += sizeof(array[i].piid);
        
        //type and len
        if(is_valid_type_length(array[i].type, array[i].len) != 1)
        {
            MI_LOG_ERROR("invalid type and length!!!  %s:%d\n", (uint32_t)__FILE__, __LINE__);
            return MI_ERR_INVALID_LENGTH;
        }
        uint16_t temp = 0;
        memset(&temp, 0, sizeof(temp));
        temp |= array[i].type << 12;
        temp += array[i].len;
        MI_LOG_DEBUG("temp:\n");
        MI_LOG_HEXDUMP(&temp, sizeof(temp));        
        memcpy(buff+idx, &temp, sizeof(temp));
        idx += sizeof(temp);
        
        //value
        memcpy(buff+idx, array[i].val, array[i].len);
        MI_LOG_DEBUG("val:\n");
        MI_LOG_HEXDUMP(array[i].val, array[i].len);
        idx += array[i].len;
    }
    
    if(idx > MIBLE_SPEC_MAX_DATA_LENGTH-6)
    {
        MI_LOG_ERROR("send msg too long  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }
    
    //new header
    header += idx;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));
    memcpy(buff, &header, sizeof(header));
    
    MI_LOG_HEXDUMP(buff, idx);
    
    mi_session_encrypt(buff, idx, buff);    
    MI_LOG_HEXDUMP(buff, idx+6);
    
    //enqueue spec data
    mible_status_t ret = sent_data_enque(buff, idx+6);
    if(ret != MI_SUCCESS)
    {
        MI_LOG_ERROR("sent_data_enque error ! ret = %d, %s:%d\n", ret, (uint32_t)__FILE__, __LINE__);
        return ret;
    }

    return MI_SUCCESS;
    
}

mible_status_t mible_event_occured(uint8_t siid, uint8_t eiid, uint8_t p_num, spec_property_t* array)
{
    MI_LOG_DEBUG("mible_event_occured\n");
    
    //uint8_t* buff = malloc(MIBLE_SPEC_MAX_DATA_LENGTH * sizeof(uint8_t));
	uint8_t* buff  = mible_rxfer_buf;
    if(NULL == buff)
    {
        MI_LOG_ERROR("malloc fail  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_NO_MEM;    
    }
		
    uint16_t idx = 0;
    
    //Header
    uint16_t header = 0;
    memset(&header, 0, sizeof(header));
    header |= MIBLE_SPEC_VERSION << 12;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));    
    idx += sizeof(header);
    
    //tid
    uint16_t tid = s_tid + 1;
    memcpy(buff+idx, &tid, sizeof(tid));
    MI_LOG_DEBUG("tid:\n");
    MI_LOG_HEXDUMP(&tid, sizeof(tid));
    idx += sizeof(tid);
    
    //opcode
    uint8_t opcode = MIBLE_SPEC_OP_EVENT_OCCURED;
    memcpy(buff+idx, &opcode, sizeof(opcode));
    MI_LOG_DEBUG("opcode:\n");
    MI_LOG_HEXDUMP(&opcode, sizeof(opcode));
    idx += sizeof(opcode);
    
    //siid
    memcpy(buff+idx, &siid, sizeof(siid));
    MI_LOG_DEBUG("siid:\n");
    MI_LOG_HEXDUMP(&siid, sizeof(siid));
    idx += sizeof(siid);
    
    //eiid
    memcpy(buff+idx, &eiid, sizeof(eiid));
    MI_LOG_DEBUG("eiid:\n");
    MI_LOG_HEXDUMP(&eiid, sizeof(eiid));
    idx += sizeof(eiid);
    
    //properties-num 
    memcpy(buff+idx, &p_num, sizeof(p_num));
    MI_LOG_DEBUG("p_num:\n");
    MI_LOG_HEXDUMP(&p_num, sizeof(p_num));
    idx += sizeof(p_num);
    
    //properties
    for(uint8_t i=0; i<p_num; i++)
    {
        //piid
        memcpy(buff+idx, &(array[i].piid), sizeof(array[i].piid));
        MI_LOG_DEBUG("piid:\n");
        MI_LOG_HEXDUMP(&p_num, sizeof(p_num));
        idx += sizeof(array[i].piid);
        
        //type and len
        if(is_valid_type_length(array[i].type, array[i].len) != 1)
        {
            MI_LOG_ERROR("invalid type and length!!!  %s:%d\n", (uint32_t)__FILE__, __LINE__);
            return MI_ERR_INVALID_LENGTH;
        }
        uint16_t temp = 0;
        memset(&temp, 0, sizeof(temp));
        temp |= array[i].type << 12;
        temp += array[i].len;
        MI_LOG_DEBUG("temp:\n");
        MI_LOG_HEXDUMP(&temp, sizeof(temp));        
        memcpy(buff+idx, &temp, sizeof(temp));
        idx += sizeof(temp);
        
        //value
        memcpy(buff+idx, array[i].val, array[i].len);
        MI_LOG_DEBUG("val:\n");
        MI_LOG_HEXDUMP(array[i].val, array[i].len);
        idx += array[i].len;
    }
    
    if(idx > MIBLE_SPEC_MAX_DATA_LENGTH-6)
    {
        MI_LOG_ERROR("send msg too long  %s:%d\n", (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }
    
    //new header
    header += idx;
    MI_LOG_DEBUG("header:\n");
    MI_LOG_HEXDUMP(&header, sizeof(header));
    memcpy(buff, &header, sizeof(header));
    
    MI_LOG_HEXDUMP(buff, idx);
    
    mi_session_encrypt(buff, idx, buff);    
    MI_LOG_HEXDUMP(buff, idx+6);
    
    //enqueue spec data
    mible_status_t ret = sent_data_enque(buff, idx+6);
    if(ret != MI_SUCCESS)
    {
        MI_LOG_ERROR("sent_data_enque error ! ret = %d, %s:%d\n", ret, (uint32_t)__FILE__, __LINE__);
        return ret;
    }

    return MI_SUCCESS;
    
}


/***********************spec data recv func***************************/

static mible_status_t mible_handle_set_properties(uint8_t *pdata, uint16_t ilen, uint16_t tid)
{
    MI_LOG_DEBUG("recv set_properties CMD\n");
    MI_LOG_HEXDUMP(pdata, ilen);
    
    uint8_t p_num = 0;
    uint16_t idx = 0;
    
    memcpy(&p_num, pdata+idx, sizeof(p_num));
    idx += sizeof(p_num);
    MI_LOG_DEBUG("porperty num %d \n", p_num);
    
    spec_property_t array[p_num];
    
    for(uint8_t i=0; i<p_num; i++)
    {
        memcpy(&(array[i].siid), pdata+idx, sizeof(array[i].siid));
        idx += sizeof(array[i].siid);
        MI_LOG_DEBUG("siid %d \n", array[i].siid);
        
        memcpy(&(array[i].piid), pdata+idx, sizeof(array[i].piid));
        idx += sizeof(array[i].piid);
        MI_LOG_DEBUG("piid %d \n", array[i].piid);
        
        uint16_t tmp = 0;
        memcpy(&tmp, pdata+idx, sizeof(tmp));
        idx += sizeof(tmp);
        MI_LOG_DEBUG("temp : %2X \n", tmp);
        
        array[i].type = (uint8_t)(tmp >> 12);
        array[i].len = (uint16_t)(tmp & 0x0FFF);
        MI_LOG_DEBUG("type : %X \n", array[i].type);
        MI_LOG_DEBUG("len : %d \n", array[i].len);
        
        if(is_valid_type_length(array[i].type, array[i].len) != 1)
        {
            MI_LOG_ERROR("recv: invalid type and length!!!  %s:%d\n", (uint32_t)__FILE__, __LINE__);
            return MI_ERR_INVALID_LENGTH;
        }
        
        array[i].val = pdata + idx;
        MI_LOG_DEBUG("property value:\n");
        MI_LOG_HEXDUMP(array[i].val, array[i].len);
        
        idx += array[i].len;
    }
    
    if(idx != ilen)
    {
        MI_LOG_ERROR("len : %d, index : %d,  %s:%d \n", ilen, idx, (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }

    MI_LOG_DEBUG("set_properties parse OK\n");
    
    foreach_callback(MIBLE_SPEC_CMD_SET_PROPERTY, tid, p_num, array);
    
    return MI_SUCCESS;
    
}

static mible_status_t mible_handle_get_properties(uint8_t *pdata, uint16_t ilen, uint16_t tid)
{
    MI_LOG_DEBUG("recv get_properties CMD\n");
    MI_LOG_HEXDUMP(pdata, ilen);
    
    uint8_t p_num = 0;
    uint16_t idx = 0;
    
    memcpy(&p_num, pdata+idx, sizeof(p_num));
    idx += sizeof(p_num);
    MI_LOG_DEBUG("porperty num %d \n", p_num);
    
    spec_property_t array[p_num];
    
    for(uint8_t i=0; i<p_num; i++)
    {
        memcpy(&(array[i].siid), pdata+idx, sizeof(array[i].siid));
        idx += sizeof(array[i].siid);
        MI_LOG_DEBUG("siid %d \n", array[i].siid);
        
        memcpy(&(array[i].piid), pdata+idx, sizeof(array[i].piid));
        idx += sizeof(array[i].piid);
        MI_LOG_DEBUG("piid %d \n", array[i].piid);
    }
    
    if(idx != ilen)
    {
        MI_LOG_ERROR("len : %d, index : %d,  %s:%d \n", ilen, idx, (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }
    
    MI_LOG_DEBUG("get_properties parse OK\n");
    
    foreach_callback(MIBLE_SPEC_CMD_GET_PROPERTY, tid, p_num, array);
    
    return MI_SUCCESS;
    
}

static mible_status_t mible_handle_action(uint8_t *pdata, uint16_t ilen, uint16_t tid)
{
    MI_LOG_DEBUG("recv action CMD\n");
    MI_LOG_HEXDUMP(pdata, ilen);
    
    uint8_t p_num = 0;
    uint8_t siid = 0;
    uint8_t aiid = 0;
    uint16_t idx = 0;
    
    memcpy(&siid, pdata+idx, sizeof(siid));
    idx += sizeof(siid);
    MI_LOG_DEBUG("siid %d \n", siid);
    
    memcpy(&aiid, pdata+idx, sizeof(aiid));
    idx += sizeof(aiid);
    MI_LOG_DEBUG("aiid %d \n", aiid);
    
    memcpy(&p_num, pdata+idx, sizeof(p_num));
    idx += sizeof(p_num);
    MI_LOG_DEBUG("porperty num %d \n", p_num);
    
    spec_property_t array[p_num];
    
    for(uint8_t i=0; i<p_num; i++)
    {        
        memcpy(&(array[i].piid), pdata+idx, sizeof(array[i].piid));
        idx += sizeof(array[i].piid);
        MI_LOG_DEBUG("piid %d \n", array[i].piid);
        
        uint16_t tmp = 0;
        memcpy(&tmp, pdata+idx, sizeof(tmp));
        idx += sizeof(tmp);
        MI_LOG_DEBUG("temp : %2X \n", tmp);
        
        array[i].type = (uint8_t)(tmp >> 12);
        array[i].len = (uint16_t)(tmp & 0x0FFF);
        MI_LOG_DEBUG("type : %X \n", array[i].type);
        MI_LOG_DEBUG("len : %d \n", array[i].len);
                
        if(is_valid_type_length(array[i].type, array[i].len) != 1)
        {
            MI_LOG_ERROR("recv: invalid type and length!!!  %s:%d\n", (uint32_t)__FILE__, __LINE__);
            return MI_ERR_INVALID_LENGTH;
        }
        
        array[i].val = pdata + idx;
        MI_LOG_DEBUG("property value:\n");
        MI_LOG_HEXDUMP(array[i].val, array[i].len);
        
        idx += array[i].len;
    }
    
    if(idx != ilen)
    {
        MI_LOG_ERROR("len : %d, index : %d,  %s:%d \n", ilen, idx, (uint32_t)__FILE__, __LINE__);
        return MI_ERR_INVALID_LENGTH;
    }

    MI_LOG_DEBUG("action parse OK\n");
    
    foreach_callback(MIBLE_SPEC_CMD_ACTION, tid, p_num, array);
        
    return MI_SUCCESS;
}


static void spec_data_parse(uint8_t *pdata, uint16_t ilen)
{
    uint16_t idx = 0;
    uint16_t header = 0;
    uint16_t tid = 0;
    uint8_t opcode = 0;
    
    memcpy(&header, pdata+idx, sizeof(header));
    idx += sizeof(header);
    MI_LOG_DEBUG("header %2X\n", header);
    
    memcpy(&tid, pdata+idx, sizeof(tid));
    idx += sizeof(idx);
    MI_LOG_DEBUG("tid %2X\n", tid);
    s_tid = tid;
    
    memcpy(&opcode, pdata+idx, sizeof(opcode));
    idx += sizeof(opcode);
    MI_LOG_DEBUG("opcode %X\n", opcode);
    
    switch (opcode)
    {
        case MIBLE_SPEC_OP_SET_PROPERTY:            
            mible_handle_set_properties(pdata+idx, ilen-idx, tid);
            break;
        case MIBLE_SPEC_OP_GET_PROPERTY:
            mible_handle_get_properties(pdata+idx, ilen-idx, tid);
            break;
        case MIBLE_SPEC_OP_ACTION:
            mible_handle_action(pdata+idx, ilen-idx, tid);
            break;
        default:
            MI_LOG_ERROR("Error opcode! \n");
    }
    
}


static int spec_data_recv(pt_t * pt)
{    
    uint8_t stat;
    s_recv_buff_len = 0;

    PT_BEGIN(pt);
    
    format_rx_cb(&rxfer_cb_spec_recv, s_recv_buff, sizeof(s_recv_buff));
    PT_SPAWN(pt, &pt_rxfer_rx, stat = rxfer_rx_thd(&pt_rxfer_rx, &rxfer_cb_spec_recv, PASS_THROUGH, &s_recv_buff_len));
    
    if (stat == PT_EXITED) {
        goto EXIT;
  }
        
    PT_END(pt);

EXIT:
    PT_EXIT(pt);
}

static void spec_data_recv_handler(void *arg)
{
    mi_session_decrypt(s_recv_data.addr, s_recv_data.len, s_recv_data.addr);
    MI_HEXDUMP(s_recv_data.addr, s_recv_data.len);
    
    spec_data_parse(s_recv_data.addr, s_recv_data.len - 6);

   // free(s_recv_data.addr);
}

uint8_t recv_buff[256];
static void mible_spec_recv_timer_callback(void * arg)
{
    spec_recv_timer_tick++;
    uint8_t stat = spec_data_recv(&pt_spec_recv);
    if (stat == PT_ENDED) {            
        PT_INIT(&pt_spec_recv);
            
        MI_LOG_DEBUG("recv %d bytes\n", s_recv_buff_len);
        MI_HEXDUMP(s_recv_buff, s_recv_buff_len);

        uint8_t* recv_buff = recv_buff;
        if(NULL == recv_buff)
        {
            MI_LOG_ERROR("malloc fail  %s:%d\n", (uint32_t)__FILE__, __LINE__);
            return;
        }

        memcpy(recv_buff, s_recv_buff, s_recv_buff_len);

        s_recv_data.addr = recv_buff;
        s_recv_data.len  = s_recv_buff_len;

        // post to user context
        mible_task_post(spec_data_recv_handler, NULL);

        memset(s_recv_buff, 0, MIBLE_SPEC_MAX_DATA_LENGTH);
        s_recv_buff_len = 0;
    }
    else if(stat == PT_EXITED)
    {
        PT_INIT(&pt_spec_recv);
        memset(s_recv_buff, 0, MIBLE_SPEC_MAX_DATA_LENGTH);
        s_recv_buff_len = 0;
    }
    
}

void spec_data_trans_init(uint16_t conn_handle, uint16_t srv_handle, uint16_t read_handle, uint16_t write_handle)
{
    /* Init parameters for Segmentation And Reassembly */
    rxfer_init(&rxfer_cb_spec_recv, &spec_recv_timer);
    rxfer_cb_spec_recv.conn_handle  = conn_handle;
    rxfer_cb_spec_recv.srv_handle   = srv_handle;
    rxfer_cb_spec_recv.value_handle = read_handle;
    
    rxfer_init(&rxfer_cb_spec_send, &spec_send_timer);
    rxfer_cb_spec_send.conn_handle  = conn_handle;
    rxfer_cb_spec_send.srv_handle   = srv_handle;
    rxfer_cb_spec_send.value_handle = write_handle;
    
    MI_LOG_DEBUG("spec_data_trans_init OK\n");

    /* Init protothread objects */
    PT_INIT(&pt_spec_send);
    PT_INIT(&pt_spec_recv);
    
    /* Init and enable timer */
    if (NULL != rx_timer_id) {
        mible_timer_delete(rx_timer_id);
        rx_timer_id = NULL;
    }
        
    //rx timer
    int ret = mible_timer_create(&rx_timer_id, mible_spec_recv_timer_callback,
                                 MIBLE_TIMER_REPEATED);
    if (MI_SUCCESS != ret) {
        MI_LOG_ERROR("Fail to create rx timer. ret = %d\n", ret);
    }
        
    mible_timer_start(rx_timer_id, 50, NULL);
        
    //tx timer
    if (NULL != tx_timer_id) {
        mible_timer_delete(tx_timer_id);
        tx_timer_id = NULL;
    }
                
    ret = mible_timer_create(&tx_timer_id, mible_spec_send_timer_callback,
                             MIBLE_TIMER_REPEATED);
    if (MI_SUCCESS != ret) {
        MI_LOG_ERROR("Fail to create tx timer. ret = %d\n", ret);
    }
        
    mible_timer_start(tx_timer_id, 50, NULL);

    //init mi_spec_send_queue
#if (MIBLE_MIBLE_SPEC_DATA_QUEUE_SIZE > 0)
    queue_init(&mi_spec_send_queue, (void*)spec_send_data_buf, MIBLE_MIBLE_SPEC_DATA_QUEUE_SIZE, sizeof(spec_send_data_buf[0]));
#endif
}

void spec_data_trans_deinit(void)
{
    mible_timer_stop(tx_timer_id);
    mible_timer_stop(rx_timer_id);
    mible_timer_delete(tx_timer_id);
    mible_timer_delete(rx_timer_id);

    sent_data_queue_flush();
}

void spec_recv_data_process(uint8_t *pdata, uint16_t len)
{
    rxfer_process(&rxfer_cb_spec_recv, pdata, len);    
}

void spec_write_data_process(uint8_t *pdata, uint16_t len)
{    
    rxfer_process(&rxfer_cb_spec_send, pdata, len);    
}

mible_status_t mible_spec_callback_register(mible_spec_callback_t cb)
{
    int index;
    mible_status_t ret;

    if (NULL == cb) {
        return MI_ERR_INVALID_PARAM;
    }

    CRITICAL_SECTION_ENTER();
    for (index = 0; index < MIBLE_SPEC_CALLBACK_MAX; ++index) {
        if (cb == spec_callback[index]) {
            ret = MI_SUCCESS;
            goto end;
        }
    }

    for (index = 0; index < MIBLE_SPEC_CALLBACK_MAX; ++index) {
        if (NULL == spec_callback[index]) {
            spec_callback[index] = cb;
            ret = MI_SUCCESS;
            goto end;
        }
    }

    ret = MI_ERR_RESOURCES;

end:
    CRITICAL_SECTION_EXIT();

    return ret;
}

mible_status_t mible_spec_callback_unregister(mible_spec_callback_t cb)
{
    int index;

    if (NULL == cb) {
        return MI_ERR_INVALID_PARAM;
    }

    CRITICAL_SECTION_ENTER();
    for (index = 0; index < MIBLE_SPEC_CALLBACK_MAX; ++index) {
        if (cb == spec_callback[index]) {
            spec_callback[index] = NULL;
            break;
        }
    }
    CRITICAL_SECTION_EXIT();

    return MI_SUCCESS;
}

