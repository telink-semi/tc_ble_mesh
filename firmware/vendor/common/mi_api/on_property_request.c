/*
 * on_property_request.c
 *
 *  Created on: 2020Äê12ÔÂ14ÈÕ
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#if MI_API_ENABLE

void P_128_1_GMT_Offset_doRequest(void)
{
    if(miio_mesh_request_property(SERVER_GMT_OFFSET) != 0){
        MI_LOG_ERROR("send_property_request failed!\n");
    }
}

void P_128_2_Weather_doRequest(void)
{
    if(miio_mesh_request_property(SERVER_WEATHER) != 0){
        MI_LOG_ERROR("send_property_request failed!\n");
    }
}

void P_128_3_UTC_Time_doRequest(void)
{
    if(miio_mesh_request_property(SERVER_UTC_TIME) != 0){
        MI_LOG_ERROR("send_property_request failed!\n");
    }
}
#endif
