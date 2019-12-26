/********************************************************************************************************
 * @file     LibHandle.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  LibHandle.m
//  SigMesh
//
//  Created by 石 on 2017/11/10.
//  Copyright © 2017年 Arvin.shi. All rights reserved.
//

#import "LibHandle.h"
#import <mach/mach_time.h>
#import "XHelp.h"

NSString *const NotifyReliableReportBack = @"ReliableReportBack";
NSString *const ReliableReportBackInfoOpcodeKey = @"ReliableReportBackInfoOpcode";
NSString *const ReliableReportBackInfoRspCountKey = @"ReliableReportBackInfoRspCount";
NSString *const ReliableReportBackInfoReqCountKey = @"ReliableReportBackInfoReqCount";
NSString *const NotifyCommandIsBusyOrNot = @"CommandIsBusyOrNot";
NSString *const CommandIsBusyKey = @"IsBusy";

@implementation ResponseModel

- (instancetype)init{
    if (self = [super init]) {
        _rspData = [[NSData alloc] init];
        _clientSubGroups = [[NSMutableArray alloc] init];
        _serverSubGroups = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)updateResponseSubscrbeList:(ResponseModel *)otherModel isClient:(bool)isClient {
    if (isClient) {
        [self.serverSubGroups removeAllObjects];
        for (int i = 0; i<otherModel.clientSubGroups.count; i++) {
            [self.clientSubGroups addObject:otherModel.clientSubGroups[i]];
        }
        TeLog(@"%@", self.clientSubGroups);
    }else{
        [self.serverSubGroups removeAllObjects];
        for (int i = 0; i<otherModel.serverSubGroups.count; i++) {
            [self.serverSubGroups addObject:otherModel.serverSubGroups[i]];
        }
        TeLog(@"%@", self.serverSubGroups);
    }
}

- (void)updateResponsePublishAddress:(ResponseModel *)otherModel isClient:(bool)isClient {
    if (self.clientPubAddress) {
        self.clientPubAddress = otherModel.clientPubAddress;
    }else{
        self.serverPubAddress = otherModel.serverPubAddress;
    }
}

@end

@implementation VendorResponseModel

- (instancetype)initWithResponseData:(NSData *)data{
    if (self = [super init]) {
        _rspData = data;
        Byte *pu = (Byte *)[data bytes];
        unsigned int temp = 0;

        if (_rspData.length >= 1) {
            memcpy(&temp, pu, 1);
            _type = temp;
        }
        if (_rspData.length >= 2) {
            temp = 0;
            memcpy(&temp, pu+1, 1);
            _length = temp;
        }
        if (_rspData.length >= 4) {
            temp = 0;
            memcpy(&temp, pu+2, 2);
            _address = temp;
        }
        if (_rspData.length >= 6) {
            temp = 0;
            memcpy(&temp, pu+4, 2);
            _rspAddress = temp;
        }
        if (_rspData.length >= 9) {
            temp = 0;
            memcpy(&temp, pu+6, 3);
            _vendorOPCode = temp;
        }
        if (_rspData.length > 9) {
            _customData = [data subdataWithRange:NSMakeRange(9, _rspData.length-9)];
        }
    }
    return self;
}

@end

@implementation LibHandle

struct DirectProModel {
    u8  net_work_key[16];
    u8  key_index[2];
    u8  ivi_update_flag;
    u8  iv_index[4];
    u8  unicast_address[2];
};

#pragma About notify deal with
void dealNotifyData(NSData *data) {
    Byte *prcv = (Byte *)[data bytes];
    u16 rcv_len = data.length;
    notify_back_todo(prcv, rcv_len);
}

void notify_back_todo(Byte *prcv, int rcv_len) {
    u8 dat[400]; u16 len;
    dat[0] = DONGLE_REPORT_SPP_DATA;
    memcpy(dat+1, prcv, rcv_len);
    len = rcv_len+1;
    push_notify_into_fifo(dat,len );
}

#pragma mark- Methods of lib need
extern void provision_end_callback(u8 reason) {
    //PROV_NORMAL_RET =0,
    //PROV_TIMEOUT_RET =1,
    TeLog(@"provision_end_callback ->%d\n",reason);

    if (reason == PROV_NORMAL_RET) {
        UInt16 address = [Bluetooth.share getCurrentProvisionAddress];
        NSString *identify = Bluetooth.share.currentPeripheral.identifier.UUIDString;
        
        u8 ele_count = 0;
        gatt_get_node_ele_cnt(address, &ele_count);
        [Bluetooth.share setElementCount:ele_count];
        TeLog(@"gatt_get_node_ele_cnt = %d",ele_count);
        [SigDataSource.share saveLocationProvisionAddress:address+ele_count-1];
        
        u8 deviceKey[16] = {};
        json_get_dev_key_info((u8 *)&deviceKey);
        NSData *devKeyData = [NSData dataWithBytes:&deviceKey length:sizeof(gatt_dev_key)];
        TeLog(@"json_get_dev_key_info = %@",devKeyData);

        SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:identify];
        if (scanModel.address != address && !Bluetooth.share.isRemoteAdd) {
            scanModel.address = address;
            [SigDataSource.share updateScanRspModelToDataSource:scanModel];
        }
        
        SigNodeModel *model = [[SigNodeModel alloc] init];
        [model setAddress:address];
        VC_node_info_t info = model.nodeInfo;
        info.element_cnt = ele_count;
        model.deviceKey = [LibTools convertDataToHexStr:devKeyData];
        model.peripheralUUID = nil;
        //Attention: There isn't scanModel at remote add, so develop need add macAddress in provisionSuccessCallback.
        if (scanModel && !Bluetooth.share.isRemoteAdd) {
            info.cps.page0_head.cid = scanModel.CID;
            info.cps.page0_head.pid = scanModel.PID;
            model.macAddress = scanModel.macAddress;
        }
        model.nodeInfo = info;
        model.UUID = identify;
        [SigDataSource.share saveDeviceWithDeviceModel:model];
        
        //First setp of add device -provision success, SDK need create nodeModel in the above, then do the second setp of add device -key bound.
        if (Bluetooth.share.isRemoteAdd) {
            //SDK needn't disconnect node when app add device in remote add.
            [Bluetooth.share addDevice_provisionResultBack:YES];
            [Bluetooth.share setKeyBindState];
            [Bluetooth.share delayForSetFilterBeforKeyBind];
        } else {
            if (Bluetooth.share.getCurrentKeyBindType == KeyBindTpye_Normal) {
                [Bluetooth.share keyBindAfterProvisionSuccess];
            } else if (Bluetooth.share.getCurrentKeyBindType == KeyBindTpye_Quick) {
                DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:scanModel.CID PID:scanModel.PID];
                if (scanModel && deviceType) {
                    [Bluetooth.share addDevice_provisionResultBack:YES];
                    [Bluetooth.share setKeyBindState];
                    [Bluetooth.share delayForSetFilterBeforKeyBind];
                }else{
                    TeLog(@"waring: this node isn't support fast_bind model, auto do key bound in normal model.");
                    [Bluetooth.share keyBindAfterProvisionSuccess];
                }
            }
        }
    } else if(reason == PROV_TIMEOUT_RET || reason == PROV_COMFIRM_ERR || reason == PROV_FAIL_CMD_ERR){
        //provision fail or timeout
        TeLog(@"provision timeout");
        [Bluetooth.share addDevice_provisionResultBack:NO];
    }
}

/// new callback, this api will callback when ivi_index update.
int mesh_json_update_ivi_index(u8 *p_index){
    NSData *ivIndexData = [NSData dataWithBytes:p_index length:4];
    TeLog(@"mesh_json_update_ivi_index=%@",ivIndexData);
    //Attention: when app call mesh_init_all(), lib will callback the ivi_index in test.bin before app call writeDataSourceToLib. So app should add this if code.
    if (SigDataSource.share.hasWriteDataSourceToLib) {
        [SigDataSource.share updateIvIndexData:ivIndexData];
    }
    return 1;
}

#warning 2019年01月29日11:29:19, new api, use in send and receive data, APP side is not used temporarily.
extern int mesh_rsp_handle_proc_win32(mesh_rc_rsp_t *p_rsp){
    return 1;
}

#warning 2019年03月09日15:47:56, new api, use in set length of app send Bluetooth data.
u16 att_mtu = 23;

int gatt_write_transaction_callback(u8 *p, u16 len, u8 msg_type) {
    u8 tmp[256];
    u16 pkt_no = 0;
    u16 buf_idx = 0;
    u16 total_len;
    u16 vaid_len = att_mtu - 4;// opcode 1 + handle_id 2 + sar 1
    u16 handle = PROXY_WRITE_HANDLER;
    if (MSG_PROVISION_PDU == msg_type) {
        handle = PROVISION_WRITE_HANDLER;
    }
    
    total_len = len;
    pb_gatt_proxy_str *p_notify = (pb_gatt_proxy_str *) (tmp);
    //    provision_flow_Log(0,p,len);
    //can send in one packet
    
    if (len == 0) {
        return 0;
    }
    if (len > vaid_len) {
        while (len) {
            if (!pkt_no) {
                // send the first pkt
                p_notify->sar = SAR_START;
                p_notify->msgType = msg_type;
                memcpy(p_notify->data, p, vaid_len);
                SendPkt(handle, tmp, vaid_len + 1);
                len = len - vaid_len;
                buf_idx += vaid_len;
                pkt_no++;
            } else {
                // the last pkt
                if (buf_idx + vaid_len >= total_len) {
                    p_notify->sar = SAR_END;
                    p_notify->msgType = msg_type;
                    memcpy(p_notify->data, p + buf_idx, total_len - buf_idx);
                    SendPkt(handle, tmp, (unsigned char) (total_len - buf_idx + 1));
                    len = 0;
                } else {
                    // send the continus pkt
                    p_notify->sar = SAR_CONTINUS;
                    p_notify->msgType = msg_type;
                    memcpy(p_notify->data, p + buf_idx, vaid_len);
                    SendPkt(handle, tmp, vaid_len + 1);
                    len = len - vaid_len;
                    buf_idx += vaid_len;
                }
            }
        }
    } else {
        // send the complete pkt
        p_notify->sar = SAR_COMPLETE;
        p_notify->msgType = msg_type;
        memcpy(p_notify->data, p, len);
        SendPkt(handle, tmp, (unsigned char) (len + 1));
    }
    return 0;
}

/// doKeyBind add new parameter isFastBind, 0 means default key bound, 1 means fast bound.
void doKeyBind(u16 address, NSData *appKey, u16 appkeyIndex ,u16 netkeyIndex ,u8 isFastBind) {
    //    refreshKeyBindModelID();
    NSString *str = [NSString stringWithFormat:@"doKeyBind,address:%d,isFastBind=%d",address,isFastBind];
    saveLogData(str);
    
    NSData *data = appKey;
    TeLog(@"doKeyBind,address:%d,getAppkey:%@,isFastBind=%d",address,data,isFastBind);
    if (data.length == 0) {
        TeLog(@"ERROE : doKeyBind getAppkey:%@",data);
    }
    u8 *key = (u8 *)[data bytes];
    u16 add = 0;
    memset(&add, 0, 2);
    memcpy(&add, &address, 2);
    mesh_kc_cfgcl_mode_para_set(appkeyIndex, key, add, netkeyIndex, isFastBind);
}

// flash operation part
extern void flash_write_page(u32 addr, u32 len, const u8 *buf) {

}

// flash operation part
extern void flash_read_page(u32 addr, u32 len, u8 *buf) {
    
}

/**
 report tx_reliable
 
 @param op opcode
 @param rsp_max request count
 @param rsp_cnt reponse count
 */
//void mesh_tx_reliable_stop_report(u16 op, u32 rsp_max, u32 rsp_cnt) {
void mesh_tx_reliable_stop_report(u16 op, u16 adr_dst,u32 rsp_max, u32 rsp_cnt) {
    dispatch_async(dispatch_get_main_queue(), ^{
        [Bluetooth.share meshReliableResponseBack:op requestCount:rsp_max responseCount:rsp_cnt];
        [NSNotificationCenter.defaultCenter postNotificationName:(NSString *)NotifyReliableReportBack
                                                          object:nil
                                                        userInfo:@{ReliableReportBackInfoOpcodeKey:@(op),
                                                                   ReliableReportBackInfoReqCountKey:@(rsp_max),
                                                                   ReliableReportBackInfoRspCountKey:@(rsp_cnt)}];
        if (Bluetooth.share.noBusyCallBack) {
            Bluetooth.share.noBusyCallBack();
            Bluetooth.share.noBusyCallBack = nil;
        }
    });
}

// SendPkt interface,need to realize by the master part
extern unsigned char SendPkt(unsigned short handle, unsigned char *p ,unsigned char  len) {
    NSData *data = [NSData dataWithBytes:p length:len];
    if (handle==PROXY_WRITE_HANDLER) {
        [Bluetooth.share writeForPROXYIn:data];
    }else if (handle==PROVISION_WRITE_HANDLER) {
        [Bluetooth.share writeForPBGATTIn:data];
    }
    return 1;
}

u8 proxy_buf[400];
u16 proxy_len = 0;

// the recv part of the VC part
int OnAppendLog_vs(unsigned char *pu, int len) {
    if (len == 0) {
        return 0;
    }
    
    u8 type = pu[0] & 0x7F;
    if (pu[0]==MESH_TX_CMD_RUN_STATUS) {
        TeLog(@"data send fail，fail packet ini data is: %@",[NSData dataWithBytes:pu length:len]);
    }else if(pu[0]==DONGLE_REPORT_SPP_DATA){
        
        anasislyDongleReportSPPData(provision_dispatch_direct(pu+1,len-1,proxy_buf,&proxy_len));
        
    }else if (MESH_CMD_RSP == type){
        //2019年05月27日14:42:03，new change since v2.8.2
        NSMutableData *temData = [NSMutableData dataWithBytes:pu length:len];
        anasislyResponseData(temData);
    }
    return 0;
}


void anasislyDongleReportSPPData(u8 ret_type) {
    if(ret_type == MSG_PROVISION_PDU){
        
        gatt_rcv_pro_pkt_dispatch(proxy_buf,(u8)proxy_len);
        
    }else if (ret_type == MSG_NETWORK_PDU){
        
        u8 bear[400];
        mesh_construct_adv_bear_with_nw(bear, proxy_buf, (u8)proxy_len);
        app_event_handler_adv(bear+1, ADV_FROM_GATT, 0);
        // Attention: log of packet that has been decoded in network layer
        LogMsgModuleDlgBuf((u8 *)bear,proxy_len+1); // network layer decode(the eighth and ninth bytes are node address, the tenth and eleventh bytes are location address.)
    }else if (ret_type == MSG_MESH_BEACON){
        //2018年11月29日14:28:02，new change since v2.7.0
        mesh_cmd_bear_unseg_t bear;
        mesh_construct_adv_bear_with_bc((u8 *)&bear, proxy_buf, (u8)proxy_len);
        if(UNPROVISION_BEACON == bear.beacon.type){
            check_pkt_is_unprovision_beacon(proxy_buf);
        }else if (SECURE_BEACON == bear.beacon.type) {
            mesh_rc_data_beacon_sec(&(bear.len), 0);
        }
    }else if (ret_type == MSG_PROXY_CONFIG) {
        //2019年05月10日10:59:10, new change since v2.8.2
        if (Bluetooth.share.setFilterResponseCallBack) {
            Bluetooth.share.setFilterResponseCallBack();
        }
    }
}

/*
 response data, eg:
 //<f0120200 01008019 00020000 00000000 00000110>
 //<f0120200 01008019 00020000 00000000 00000010>
 //<f0178244 8244802a 008244 00 1000c001 c002c003 c004c005 c0>
 //<f0178244 8244802a 008244 01 1000c001 c002c003 c004c005 c0>
 */

void anasislyResponseData(NSData *responseData) {
    Byte *pu = (Byte *)[responseData bytes];
    ResponseModel *model = [[ResponseModel alloc] init];
    model.rspData = [NSData dataWithData:responseData];
    
    unsigned int temp = 0;
    memcpy(&temp, pu + 2+1, 2);
    model.address = temp;
    
    memcpy(&temp, pu+4+1, 2);
    model.rspAddress = temp;
    
    if (model.address == model.rspAddress) return;
    
    //2019年05月27日14:42:03，new change since v2.8.2
    temp = rf_link_get_op_by_ac(pu+6+1);
    
    model.opcode = temp;
    Opcode op = model.opcode;
    
#if MD_MESH_OTA_EN // VC_DISTRIBUTOR_UPDATE_CLIENT_EN
    u32 size_op = SIZE_OF_OP(temp);
    if(mesh_ota_master_rx((mesh_rc_rsp_t *)(pu+1), temp, size_op)){
        if (temp != FW_INFO_STATUS) {
            return ;
        }
    }
#endif

    switch (op) {
        case OpcodeOnOffStatusResponse:
        {
            temp = 0;
            if (model.rspData.length <= 9+1) {
                //hasn't gradual
                temp = 0;
                memcpy(&temp, pu + 8+1, 1);
                model.currentState = temp;
                break;
            }else if (model.rspData.length >= 11+1){
                //has gradual(it is need to update UI in use the pointState)
                temp = 0;
                memcpy(&temp, pu + 9+1, 1);
                model.currentState = temp;
                model.pointState = temp;
            }
        }   break;
        case OpcodeSubListGetResponse:
        {
            NSString *tip = @"";
            temp = 0;
            memcpy(&temp, pu + 11+1, 2);
            
            int groupsCount = ((int)(responseData.length) - 13-1) / 2;
            if (temp == SIG_MD_G_ONOFF_C) {
                model.isClient = YES;
                tip = @"clientSubGroups->";
                for (int i=0; i<groupsCount; i++) {
                    temp = 0;
                    memcpy(&temp, pu + 13+1 + 2*i, 2);
                    [model.clientSubGroups addObject:@(temp)];
                    tip = [NSString stringWithFormat:@"%@%04x ",tip,temp];
                }
            }else if (temp == SIG_MD_G_ONOFF_S) {
                tip = @"serverSubGroups->";
                model.isClient = NO;
                for (int i=0; i<groupsCount; i++) {
                    temp = 0;
                    memcpy(&temp, pu + 13+1 + 2*i, 2);
                    [model.serverSubGroups addObject:@(temp)];
                    tip = [NSString stringWithFormat:@"%@%04x ",tip,temp];
                }
            }
            TeLog(@"%@",tip);
        }   break;
        case OpcodePublishAddressResponse:
        {
            
        }   break;
        case OpcodeEditSubListResponse:
        {
            
        }   break;
        case OpcodeBrightnessGetResponse:
        {
            temp = 0;
            if (model.rspData.length >= 10+1 && model.rspData.length < 13+1) {
                //hasn't gradual
                memcpy(&temp, pu + 8+1, 2);
                model.currentValue = temp;
                break;
            }else if (model.rspData.length >= 13+1){
                //has gradual(it is need to update UI in use the pointValue)
                memcpy(&temp, pu + 10+1, 2);
                model.currentValue = temp;
                model.pointValue = temp;
            }
        }   break;
        case OpcodeCurrentCTLResponse:
        {
            temp = 0;
            if (model.rspData.length >= 12+1 && model.rspData.length < 16+1) {
                //hasn't gradual
                memcpy(&temp, pu + 8+1, 2);
                //currentValue is brightness
                model.currentValue = temp;
                memcpy(&temp, pu + 10+1, 2);
                //pointValue is color temprature
                model.pointValue = temp;
            }else if (model.rspData.length >= 16+1){
                //has gradual(it is need to update UI in use the pointValue)
                memcpy(&temp, pu + 12+1, 2);
                //currentValue is brightness
                model.currentValue = temp;
                memcpy(&temp, pu + 14+1, 2);
                //pointValue is color temprature
                model.pointValue = temp;
            }
        }   break;
        case OpcodeDelectDeviceResponse:
        {
            //response of delete device
        }   break;
        case OpcodeSetTimeResponse:
        {
            //response of set time
            
        }  break;
        case OpcodeSceneRegisterStatusResponse:
        {
            //response of save scene
            //response of delete scene
            
        }  break;
        case OpcodeRecallSceneResponse:
        {
            //response of recall scene
            
        }  break;
        case OpcodeSetSchedulerResponse:
        {
            //response of set scheduler
            
        }  break;
        case OpcodeHSLNotifyResponse:
        {
            //publish modelID is SIG_MD_LIGHT_HSL_S
            TeLog(@"OpcodeHSLNotifyResponse rspData=%@ address=%d rspAddress=%d", model.rspData,model.address,model.rspAddress);

        }  break;
        case OpcodeChangeLevelResponse:
        {
            //response of set level
            temp = 0;
            if (model.rspData.length <= 10+1) {
                //hasn't gradual
                memcpy(&temp, pu + 8+1, 2);
                model.currentValue = temp;
                break;
            }else if (model.rspData.length >= 13+1){
                //has gradual(it is need to update UI in use the pointValue)
                memcpy(&temp, pu + 10+1, 2);
                model.currentValue = temp;
                model.pointValue = temp;
            }
        }  break;
        case OpcodeCTLTemperatureStatusResponse:
        {
            temp = 0;
            if (model.rspData.length <= 12+1) {
                //hasn't gradual
                memcpy(&temp, pu + 8+1, 2);
                model.currentValue = temp;
                break;
            }else if (model.rspData.length >= 16+1){
                //has gradual(it is need to update UI in use the pointValue)
                memcpy(&temp, pu + 12+1, 2);
                model.currentValue = temp;
                model.pointValue = temp;
            }
        }   break;
        case OpcodeSetUUIDResponse:
        {
            
        }   break;
        case OpcodeGetSchedulerStatusResponse:
        {
            
        }   break;
        case OpcodeGetFwInfoResponse:
        {
            
        }   break;
        default:    {
            TeLog(@"unrecongnized opcode %@", model.rspData);
            [Bluetooth.share responseBackOfVendorID:model];
            return;
        };
    }
    
    [Bluetooth.share responseBack:model];
}

extern unsigned int clock_time_vc_hw() {
    uint64_t time = mach_absolute_time();
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t t = 32 * time * timebase.numer / timebase.denom / 1e3;
    return  (unsigned int)t;
}

int provisionLocal(NSData *netkeydata,uint16_t locationAddress,u16 netkeyIndex) {
    Byte *kbyte = (Byte *)[netkeydata bytes];
    struct ProvisionInfo provsionInfo = SigDataSource.share.provsionInfo;
    memset(provsionInfo.prov_newkey, 0, 16);
    memcpy(provsionInfo.prov_newkey, kbyte, netkeydata.length);
    memset(provsionInfo.prov_key_index, 0, 2);
    memcpy(provsionInfo.prov_key_index, &netkeyIndex, 2);
    memcpy(provsionInfo.prov_unicast_address, &locationAddress, 2);

    TeLog(@"provisionLocal netkeydata:%@ netkeyIndex:%d locationAddress:%d prov_iv_index:%@",netkeydata,netkeyIndex,locationAddress,[NSData dataWithBytes:provsionInfo.prov_iv_index length:4]);
    int tempIndex = mesh_provision_par_set_dir((u8 *)&provsionInfo);
    return tempIndex;
}

void provision(NSData *netkeydata, uint16_t address,u16 netkeyIndex) {
    SigDataSource.share.time = [[NSDate date] timeIntervalSince1970];
    struct ProvisionInfo provsionInfo = SigDataSource.share.provsionInfo;
    TeLog(@"provision netkeydata:%@ address:%d prov_iv_index:%@",netkeydata,address,[NSData dataWithBytes:&provsionInfo.prov_iv_index length:4]);
    Byte *kbyte = (Byte *)[netkeydata bytes];
    memset(provsionInfo.prov_newkey, 0, 16);
    memcpy(provsionInfo.prov_newkey, kbyte, netkeydata.length);
    memset(provsionInfo.prov_key_index, 0, 2);
    memcpy(provsionInfo.prov_key_index, &netkeyIndex, 2);
    memcpy(provsionInfo.prov_unicast_address, &address, 2);
    
    set_provision_key_data(provsionInfo.prov_newkey, provsionInfo.prov_key_index, provsionInfo.flag, provsionInfo.prov_iv_index, provsionInfo.prov_unicast_address);
    start_provision_invite();
}

u8 set_provision_key_data(Byte *prov_newkey, Byte *prov_key_index, Byte prov_ivi_update_flag, Byte *prov_iv_index, Byte *prov_unicast_address) {
    NSData *data = [NSData dataWithBytes:&prov_iv_index length:4];
    TeLog(@"set_provision_key_data=%@",data);

    u8 tmp_dat[25];
    memcpy(tmp_dat,prov_newkey,16);
    memcpy(tmp_dat+16,prov_key_index,2);
    
    tmp_dat[18] = prov_ivi_update_flag;
    memcpy(tmp_dat+19,prov_iv_index,4);
    memcpy(tmp_dat+23,prov_unicast_address,2);
    
    set_app_key_pro_data(tmp_dat,25);
    return 1;
}

#pragma mark- Mesh OTA
u32 new_fw_read(u8 *data_out, u32 max_len) {
    u32 file_size = 0;
    NSData *otaData = MeshOTAManager.share.otaData;
    u8 *byte = (u8 *)otaData.bytes;
    file_size = (u32)otaData.length;
    
    if((file_size > max_len) || (file_size < 48)){
        //AfxMessageBox("new firmware size is too big");
        return 0;
    }
    
    memcpy(data_out,byte,file_size);
    return file_size;
}

void APP_RefreshProgressBar(u16 bk_current, u16 bk_total, u16 chunk_cur, u16 chunk_total, u8 percent) {
//void APP_RefreshProgressBar(u16 bk_current, u16 bk_total, u8 percent) {
    TeLog(@"APP_RefreshProgressBar percent=%d%%",percent);
    [MeshOTAManager.share callBackProgressFromLib:percent];
}

#pragma mark- OTA
void sendData(NSData *data, int index) {
    BOOL isEnd = data.length == 0;
    int countIndex = index;
    Byte *tempBytes = (Byte *)[data bytes];
    Byte resultBytes[20];
    
    memset(resultBytes, 0xff, 20);
    memcpy(resultBytes, &countIndex, 2);
    memcpy(resultBytes+2, tempBytes, data.length);
    uint16_t crc = crc16(resultBytes, isEnd ? 2 : 18);
    memcpy(isEnd ? (resultBytes + 2) : (resultBytes+18), &crc, 2);
    NSData *writeData = [NSData dataWithBytes:resultBytes length:isEnd ? 4 : 20];
    [Bluetooth.share writeOTAData:writeData];
}

/*
 packet of end OTA 6 bytes structure：1byte:0x02 + 1byte:0xff + 2bytes:index + 2bytes:~index
 */
void sendOTAEndData(NSData *data, int index){
    int negationIndex = ~index;
    Byte *tempBytes = (Byte *)[data bytes];
    Byte resultBytes[6];
    
    memset(resultBytes, 0xff, 6);
    memcpy(resultBytes, tempBytes, data.length);
    memcpy(resultBytes+2, &index, 2);
    memcpy(resultBytes+4, &negationIndex, 2);
    NSData *writeData = [NSData dataWithBytes:resultBytes length:6];
    TeLog(@"sendOTAEndData -> %04x ,length:%lu,%@", index,(unsigned long)writeData.length,writeData);
    [Bluetooth.share writeOTAData:writeData];
}

void sendReadFirmwareVersion(){
    uint8_t buf[2] = {0x00,0xff};
    NSData *writeData = [NSData dataWithBytes:buf length:2];
    TeLog(@"sendReadFirmwareVersion -> length:%lu,%@",(unsigned long)writeData.length,writeData);
    [Bluetooth.share writeOTAData:writeData];
}

void sendStartOTA(){
    uint8_t buf[2] = {0x01,0xff};
    NSData *writeData = [NSData dataWithBytes:buf length:2];
    TeLog(@"sendReadStartOTA -> length:%lu,%@",(unsigned long)writeData.length,writeData);
    [Bluetooth.share writeOTAData:writeData];
}

extern unsigned short crc16 (unsigned char *pD, int len) {
    static unsigned short poly[2]={0, 0xa001};
    unsigned short crc = 0xffff;
    int i,j;
    for(j=len; j>0; j--) {
        unsigned char ds = *pD++;
        for(i=0; i<8; i++) {
            crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
    }
    return crc;
}

#pragma mark- Self customizer
NSArray * changeObjToByte(NSString *string) {
    NSArray *tempArr = [string componentsSeparatedByString:@" "];
    NSMutableArray *ar = [NSMutableArray new];
    for (int i=0; i<tempArr.count; i++) {
        if ([tempArr[i] length]>0) {
            unsigned long v = strtoul([tempArr[i] UTF8String], 0, 16);
            [ar addObject:@(v)];
        }
    }
    return ar;
}

void changeStringToByte(NSString *string,Byte *byte) {
    NSArray *arr = changeObjToByte(string);
    NSMutableArray *anew = [NSMutableArray new];
    for (int j=0; j<arr.count; j++) {
        byte[j] = [arr[j] intValue];
        [anew addObject:[NSString stringWithFormat:@"%02x", [arr[j] intValue]]];
    }
}

void logBuf(u8 *buf, int len, NSString *tip) {
    if (!buf) {
        return;
    }
    NSMutableArray *tempArr = [NSMutableArray array];
    for (int i = 0; i<len; i++) {
        [tempArr addObject:[NSString stringWithFormat:@"%02x", buf[i]]];
    }
    TeLog(@"logBuf %@: %@", tip, [tempArr componentsJoinedByString:@" "]);
}

extern void RefreshStatusNotifyByHw(unsigned char *p, int len) {
    TeLog(@"RefreshStatusNotifyByHw");
    logBuf(p, len,@"RefreshStatusNotifyByHw");
}

void LogMsgModuleDlg(char *format, ...) {
    char buff[4069];
    va_list list;
    va_start(list, format);
    vsnprintf(buff, 4069, format, list);
    va_end(list);
    TeLog(@"%s\n", buff);
}

int LogMsgModuleDlg_and_buf(u8 *pbuf,int len,char *log_str,char *format, va_list list)
{
    char buff[4069],buf_tmp[1024];
    unsigned long log_len = 0;
    u32 buf_idx = 0;
    log_len = strlen(log_str);
    memcpy(buff,log_str,log_len);
    buf_idx += log_len;
    //    vsnprintf_s( buf_tmp ,1024, format, list);
    vsnprintf(buf_tmp, 1024, format, list);
    unsigned long len_str = 0;
    len_str = strlen (buf_tmp);
    memcpy(buff+buf_idx,buf_tmp,len_str);
    buf_idx += len_str;
    Byte str[4069];
    unsigned int len_buf;
    len_buf = Bin2Text(str, pbuf, len);
    
    buff[buf_idx]=':';
    memcpy(buff+buf_idx+1,str,len_buf);
    //    g_module_dlg->LogMsg("%s\r\n", buff);
    TeLog(@"%s\r\n", buff);
    return 0;
}

int Bin2Text (Byte *lpD, Byte *lpS, int n)
{
    int i = 0;
    int m = n;
    int d = 0;
    if (m>MAXBSIZE) m = MAXBSIZE;
    
    for (i=0; i<m; i++) {
        
        lpD[d++] = arrb2t [(lpS[i]>>4) & 15];
        lpD[d++] = arrb2t [lpS[i] & 15];
        lpD[d++] = ' ';
        
        if ((i&15)==7) lpD[d++] = ' ';
        else if ((i&15)==15) {
            lpD[d++] = '\r';
            lpD[d++] = '\n';
        }
        
    }
    //lpD[d++] = '\r';
    //lpD[d++] = '\n';
    lpD[d++] = '\0';
    return d;
}

char arrb2t[] = "0123456789abcdef";
extern void mesh_proxy_master_terminate_cmd() {
    TeLog(@"terminate : %s ",__func__);
    [Bluetooth.share cancelConnection:Bluetooth.share.currentPeripheral complete:nil];
}

extern int App_key_bind_end_callback(u8 event) {
    TeLog(@"App_key_bind_end_callback ->event:%d\n",event);
    if (Bluetooth.share.commandHandle.fastProvisionAddDeviceFinishCallBack) {
        TeLog(@"should call back fast provision finish.\n");
        Bluetooth.share.commandHandle.fastProvisionAddDeviceFinishCallBack();
        Bluetooth.share.commandHandle.fastProvisionAddDeviceFinishCallBack = nil;
        return 0;
    }
    if (event == MESH_APP_KEY_BIND_EVENT_SUC) {
        UInt16 address = [Bluetooth.share getCurrentProvisionAddress];
        SigNodeModel *model = [SigDataSource.share getNodeWithAddress:address];

        VC_node_info_t node_info;
        if (Bluetooth.share.getCurrentKeyBindType == KeyBindTpye_Normal) {
            json_get_node_cps_info(address, &node_info);
        } else if (Bluetooth.share.getCurrentKeyBindType == KeyBindTpye_Quick) {
            DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:model.nodeInfo.cps.page0_head.cid PID:model.nodeInfo.cps.page0_head.pid];
            if (deviceType) {
                VC_node_info_t tem = deviceType.defultNodeInfo;
                VC_node_info_t oldNode = model.nodeInfo;
                memcpy(&tem, &oldNode, 22);
                node_info = tem;
            }else{
                TeLog(@"waring: this node isn't support fast_bind model, auto do key bound in normal model, and key bound successful.");
                json_get_node_cps_info(address, &node_info);
            }
        }else{
            json_get_node_cps_info(address, &node_info);
        }
        
        SigAppkeyModel *appkey = [SigDataSource.share curAppkeyModel];
        SigNodeKeyModel *nodeAppkey = [[SigNodeKeyModel alloc] init];
        nodeAppkey.index = appkey.index;
        if (![model.appKeys containsObject:nodeAppkey]) {
            [model.appKeys addObject:nodeAppkey];
        }
        model.nodeInfo = node_info;
        [SigDataSource.share saveLocationData];

        //publish time model after keyBind success.（add since v3.1.0）
        UInt32 option = SIG_MD_TIME_S;
        NSArray *elementAddresses = [model getAddressesWithModelID:@(option)];
        if (elementAddresses.count > 0) {
            TeLog(@"SDK need publish time");
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                UInt16 eleAdr = [elementAddresses.firstObject intValue];
                //周期，20秒上报一次。
                mesh_pub_period_t period;
                period.steps = kTimePublishInterval;//Range：0x01-0x3F
                period.res = 1;
                [Bluetooth.share.commandHandle editPublishListWithExecuteCommand:YES publishAddress:kAllDo_SIGParameters nodeAddress:model.address eleAddress:eleAdr option:option period:period Completation:^(ResponseModel *m) {
                    TeLog(@"publish time callback");
                    PublishResponseModel *pubModel = [[PublishResponseModel alloc] initWithResponseData:m.rspData];
                    if (pubModel.status == 0 && pubModel.elementAddress == eleAdr) {
                        if (pubModel.publishAddress == kAllDo_SIGParameters) {
                            TeLog(@"publish time success");
                            [Bluetooth.share addDevice_keyBindResultBack:YES];
                        }
                    }
                }];
            });
        }else{
            TeLog(@"SDK needn't publish time");
            [Bluetooth.share addDevice_keyBindResultBack:YES];
        }
    } else {
        [Bluetooth.share addDevice_keyBindResultBack:NO];
    }
    if (SigDataSource.share.time > 0) {
        NSString *str = [NSString stringWithFormat:@"====================provision+keybind time:%f====================",[[NSDate date] timeIntervalSince1970]-SigDataSource.share.time];
        saveLogData(str);
        SigDataSource.share.time = 0;
    }
    return 0;
}

extern void LogMsgModuleDlgBuf(unsigned char *bin, int len) {
//    NSData *data = [NSData dataWithBytes:bin length:len];
//    TeLog(@"LogMsgModuleDlgBuf -> %@", data);
}

#warning (Cloud)2018年05月21日17:24:03，when develop add triad-node that need to check in the clouds, develop need realize this callback.
extern int mesh_set_prov_cloud_para(u8 *p_pid,u8 *p_mac){
    return 0;
}

///Remote Add: new callback, response of api:getRemoteProvisioningMaxScannedItemsWithCallback:
void remote_prov_capa_sts_cb(u8 max_item,u8 active_scan){
    TeLog(@"remote_prov_capa_sts_cb，max_item=%d，active_scan=%d",max_item,active_scan);
    
    if (Bluetooth.share.commandHandle.maxScannedItemsCallBack) {
        Bluetooth.share.commandHandle.maxScannedItemsCallBack(max_item,active_scan);
    }
}

///Remote Add: new callback, response of api:startRemoteProvisioningScanWithAddress:
void remote_prov_scan_report_cb(u8 *par,u8 len){
    TeLog(@"remote_prov_scan_report_cb=%@",[NSData dataWithBytes:par length:len]);
    SigRemoteScanRspModel *model = [[SigRemoteScanRspModel alloc] initWithPar:par len:len];
    if (Bluetooth.share.commandHandle.remoteScanRspCallBack) {
        Bluetooth.share.commandHandle.remoteScanRspCallBack(model);
    }
}

///scan unprovision beacon, result callback remote_prov_scan_report_cb(u8 *par,u8 len)
int send_rp_scan_start(u16 address,u8 limit,u8 timeout)
{
    remote_prov_scan_start scan_start;
    scan_start.scannedItemsLimit = limit;
    scan_start.timeout = timeout;
    return SendOpParaDebug(address, 1, REMOTE_PROV_SCAN_START, (u8 *)&scan_start, 2);
}



#warning 2019年06月18日12:07:55，v2.9.0新增callback
void mesh_heartbeat_cb_data(u16 src, u16 dst,u8 *p_hb) {
    LOG_MSG_INFO(TL_LOG_COMMON,p_hb,sizeof(mesh_hb_msg_t)+1,
                 "heartbeat src adr is 0x%04x,dst adr is 0x%04x",src,dst);
}

#warning 2019年06月18日12:07:55，v2.9.0新增callback
void mesh_provision_para_init() {

}

#warning 2019年06月18日12:07:55，v2.9.0新增callback
u8 win32_proved_state() {
    return 1;
}

///Mesh Add: return GATT node address.（add since v2.9.0）
u16 APP_get_GATT_connect_addr() {
    return SigDataSource.share.getCurrentConnectedNode.address;
}

///Mesh OTA: callback when node status changed.（add since v2.9.0）
void APP_report_mesh_ota_apply_status(u16 adr_src, fw_update_status_t *p) {
    if (p->st == 0) {
        //adr_src had mesh ota success.
        [MeshOTAManager.share callBackMeshOTASuccessAddress:adr_src];
    }
}

///fast provision: callback address that can provision next node.（add since v2.8.0, return 2; use since v3.0.0）
int set_win32_prov_unicast_adr(u16 adr){
    TeLog(@"fast provision call back: add success, address=%d.",adr);
    [SigDataSource.share saveLocationProvisionAddress:adr-1];
    return 1;
}

///fast provision: return next address to add device.（add since v2.8.0, return 2; use since v3.0.0）
u16 get_win32_prov_unicast_adr() {
    u16 provisionAddress = SigDataSource.share.provisionAddress;
    if (provisionAddress == 0) {
        TeLog(@"warning: address has run out.");
    }else if (provisionAddress > SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.hightIntAddress) {
        TeLog(@"warning: address out of range.");
    }
    return provisionAddress;
}

///fast provision: call back single device that had provision and keybind success.
void mesh_fast_prov_node_info_callback(u8 *dev_key, u16 node_addr, u16 pid) {
    if (Bluetooth.share.commandHandle.fastProvisionAddSingleDeviceSuccessCallBack) {
        NSData *data = [NSData dataWithBytes:dev_key length:16];
        TeLog(@"dev_key=%@",data);
        Bluetooth.share.commandHandle.fastProvisionAddSingleDeviceSuccessCallBack(data, (UInt16)node_addr, (UInt16)pid);
    }
}

///fast provision: return element count of pid.
u8 mesh_fast_prov_get_ele_cnt_callback(u16 pid) {
    TeLog(@"pid=%d",pid);
    return [SigDataSource.share getElementCountOfPid:pid];
}

#define JSON_DATABASE_STRING_MAX    50
#define MAX_NETKEY_TOTAL_NUM    400
#define MAX_APPKEY_TOTAL_NUM    400
#define MAX_PROVISION_NUM         200
//#define MAX_MESH_NODE_NUM        32767
#define MAX_MESH_NODE_NUM        255
#define MAX_MESH_NODE_ELE_NUM    4
typedef struct{
    u8 valid;
    char name[JSON_DATABASE_STRING_MAX];
    int index;
    u8 phase;
    u8 key[16];
    char minSecurity[JSON_DATABASE_STRING_MAX];
    u8 oldkey[16];
    u32 timestamps;
}mesh_json_netkeys_str;

typedef struct{
    u8 valid;
    char name[JSON_DATABASE_STRING_MAX];
    int index;
    int bound_netkey;
    u8 key[16];
    u8 oldKey[16];
}mesh_json_appkeys_str;

typedef struct{
    u16 highAddress;
    u16 lowAddress;
}mesh_json_pro_GroupRange;
#define MAX_PRO_GROUP_RANGE_MAX     100

typedef struct{
    u16 lowAddress;
    u16 highAddress;
}mesh_json_pro_UnicastRange;

typedef struct{
    u16 firstScene;
    u16 lastScene;
}mesh_json_pro_SceneRange;
#define JSON_PROV_UNICAST_RANGE 0xff
typedef struct{
    u8 valid;
    char provisionerName[JSON_DATABASE_STRING_MAX];
    u8 uuid[16];
    mesh_json_pro_UnicastRange unicastRange[MAX_PRO_GROUP_RANGE_MAX];
    mesh_json_pro_GroupRange groupRange[MAX_PRO_GROUP_RANGE_MAX];
    mesh_json_pro_SceneRange sceneRange[MAX_PRO_GROUP_RANGE_MAX];
}mesh_provisioners_str;

#define MAX_GROUP_NUM     400
typedef struct{
    u8 valid;
    char name[JSON_DATABASE_STRING_MAX];
    u16 address;
    u16 parentAddress;
}mesh_group_str;

#define MAX_SCENE_NUM     MAX_GROUP_NUM
typedef struct{
    u8 valid;
    char name[JSON_DATABASE_STRING_MAX];
    u16 addresses[MAX_GROUP_NUM];
    int number;
}mesh_scene_str;

// nodes part
typedef struct{
    u8 relay;
    u8 proxy;
    u8 fri;
    u8 lowpower;
}mesh_nodes_feature_str;

typedef struct{
    u8 count;
    u8 interval;
}mesh_nodes_networkTransmit_str;

typedef struct{
    u16 address;
    int index;
    u8 ttl;
    int period;
    u8 credentials;
    u8 count;
    u8 interval;
}mesh_models_publish_str;

#define MAX_SUBSCRIBE_NUM    8
#define MAX_BIND_ADDRESS_NUM    10
typedef struct{
    u8 valid;
    u16 bind;
}mesh_model_bind_str;

typedef struct{
    u8 valid;
    u8 vendor_flag;
    int modelId;
    u16 subscribe[MAX_SUBSCRIBE_NUM];
    mesh_model_bind_str bind[MAX_BIND_ADDRESS_NUM];
    mesh_models_publish_str publish;
}mesh_nodes_model_str;

#define MAX_MODELS_IN_ELE_CNT    80
typedef struct{
    u8 valid;
    char name[JSON_DATABASE_STRING_MAX];
    int index;
    u16 location;
    mesh_nodes_model_str models[MAX_MODELS_IN_ELE_CNT];
}mesh_nodes_ele_str;

typedef struct{
    u8 count;
    u8 interval;
}mesh_nodes_relay_str;

typedef struct{
    u8 valid;
    u16 index;
}mesh_nodes_net_appkey_str;
typedef struct{
    u8 valid;
    u8 macAddress[6];
    u8 uuid[16];
    int sno;
    u16 unicastAddress;
    u8 deviceKey[16];
    char security[JSON_DATABASE_STRING_MAX];
    mesh_nodes_net_appkey_str netkeys[MAX_NETKEY_TOTAL_NUM];
    u8 configComplete;
    char name[JSON_DATABASE_STRING_MAX];
    u16 cid;
    u16 pid;
    u16 vid;
    u16 crpl;
    mesh_nodes_feature_str features;
    u8  secureNetworkBeacon;
    u8  defaultTTL;
    mesh_nodes_networkTransmit_str networkTransmit;
    mesh_nodes_relay_str relayRetransmit;
    mesh_nodes_net_appkey_str appkeys[MAX_APPKEY_TOTAL_NUM];
    mesh_nodes_ele_str elements[MAX_MESH_NODE_ELE_NUM];
    u8 blacklisted;
}mesh_node_str;

typedef struct{
    char schema[JSON_DATABASE_STRING_MAX];
    char version[JSON_DATABASE_STRING_MAX];
    u8 mesh_uuid[16];
    char meshName[JSON_DATABASE_STRING_MAX];
    u32 timestamp;
    mesh_provisioners_str     provisioners[MAX_PROVISION_NUM];
    u8 ivi_idx[4];
    mesh_json_netkeys_str     netKeys[MAX_NETKEY_TOTAL_NUM];
    mesh_json_appkeys_str     appkeys[MAX_APPKEY_TOTAL_NUM];
    mesh_node_str            nodes[MAX_MESH_NODE_NUM];
    mesh_group_str            groups[MAX_GROUP_NUM];
    mesh_scene_str          scene[MAX_SCENE_NUM];
}sig_mesh_json_database;

unsigned char vc_uuid[16];
sig_mesh_json_database json_database;

#warning 2019年05月27日15:13:51，v2.9.0新增callback
int mesh_par_retrieve_store_win32(u8 *in_out, u32 *p_adr, u32 adr_base, u32 size,u8 flag) {
    int err =-1;
    switch(adr_base){
//        case FLASH_ADR_LIST_CFG_S:
        case FLASH_ADR_PROVISION_CFG_S:
        case FLASH_ADR_FRIEND_SHIP:
        case FLASH_ADR_RESET_CNT:
        case FLASH_ADR_MESH_TYPE_FLAG:
        case FLASH_ADR_SW_LEVEL:
            break;
        case FLASH_ADR_MESH_KEY:
            // get the json file's net key and the dev key to update
            if(flag == MESH_PARA_STORE_VAL){
                //err = mesh_key_store_win32((mesh_key_save_t *)in_out);
            }else if(flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_key_retrieve_win32((mesh_key_save_t *)in_out);
            }
            break;
        case FLASH_ADR_MISC:
            // update the sno ,and move the sno to the json file part
            if(flag == MESH_PARA_STORE_VAL){
                mesh_misc_store_win32((misc_save_t *)in_out);
            }else if (flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_misc_retrieve_win32((misc_save_t *)in_out);
            }
            break;
        case FLASH_ADR_VC_NODE_INFO:
            // update the vc node info to json
            if(flag == MESH_PARA_STORE_VAL){
                // json file is the newest ,no need to change by the store part .
            }else if (flag == MESH_PARA_RETRIEVE_VAL){
                err = update_VC_info_from_json((int)SigDataSource.share.curNetkeyModel.index ,(VC_node_info_t *)in_out);
            }
            break;
            // model proc part
        case FLASH_ADR_MD_CFG_S:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                TeLog(@"FLASH_ADR_MD_CFG_S -> MESH_PARA_RETRIEVE_VAL");
                err = mesh_model_cfg_retrieve_win32((model_sig_cfg_s_t *) in_out);
            }
            break;
        case FLASH_ADR_MD_HEALTH:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_model_health_retrieve_win32((model_health_t *) in_out);
            }
            break;
        case FLASH_ADR_MD_G_ONOFF_LEVEL:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_model_onoff_retrieve_win32((model_g_onoff_level_t *)in_out);
            }
            break;
        case FLASH_ADR_MD_TIME_SCHEDULE:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if STRUCT_MD_TIME_SCHEDULE_EN
                err = mesh_model_time_retrieve_win32((model_time_schedule_t *)in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_LIGHTNESS:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if MD_LIGHT_CONTROL_EN
                err = mesh_model_lightness_retrieve_win32((model_lightness_t *)in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_LIGHT_CTL:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if (LIGHT_TYPE_CT_EN)
                err = mesh_model_light_ctl_retrieve_win32((model_light_ctl_t*)in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_LIGHT_LC:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if MD_LIGHT_CONTROL_EN
                err = mesh_model_light_lc_retrieve_win32((model_light_lc_t *)in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_LIGHT_HSL:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if LIGHT_TYPE_HSL_EN
                err = mesh_model_light_hsl_retrieve_win32((model_light_hsl_t *)in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_SENSOR:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if(MD_SENSOR_EN || MD_BATTERY_EN || MD_LOCATION_EN)
                err = mesh_model_sensor_retrieve_win32((model_sensor_t *)in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_G_POWER_ONOFF:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if STRUCT_MD_DEF_TRANSIT_TIME_POWER_ONOFF_EN
                err = mesh_model_power_onoff_win32((model_g_power_onoff_trans_time_t *)in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_SCENE:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if MD_SCENE_EN
                err = mesh_model_scene_win32((model_scene_t *) in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_MESH_OTA:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if MD_MESH_OTA_EN
                err = mesh_model_mesh_ota_win32((model_mesh_ota_t *) in_out);
#endif
            }
            break;
        case FLASH_ADR_MD_REMOTE_PROV:
            if(flag == MESH_PARA_RETRIEVE_VAL){
#if MD_REMOTE_PROV
                err = mesh_model_remote_prov_win32((model_remote_prov_t *) in_out);
#endif
            }
            break;
        default:
            break;
    }
    return err;
}

#warning 2019年05月27日15:13:51，v2.9.0新增callback
void user_system_time_proc() {

}

#warning 2019年05月27日15:13:51，v2.9.0新增callback
void user_node_oob_set() {

}

#warning 2019年05月27日15:13:51，v2.9.0新增callback
void user_mesh_cps_init() {

}

int  prov_get_net_info_from_json(provison_net_info_str *net_info,int *p_net_idx,int node_idx)
{
    mesh_json_netkeys_str *p_net = json_database.netKeys;
    // init find the first valid network info to use
    int valid_idx = json_valid_netidx(p_net);
    if(valid_idx < 0){
        return 0;
    }
    *p_net_idx = valid_idx;
    memcpy(net_info->iv_index,json_database.ivi_idx,sizeof(json_database.ivi_idx));
    // get the provision net information part
    p_net = &(json_database.netKeys[valid_idx]);
    net_info->flags =0;
    net_info->key_index = p_net->index;
    memcpy(net_info->net_work_key,p_net->key,sizeof(p_net->key));

    if(node_idx>=0){
        // has the same provisioner about self
        net_info->unicast_address = json_database.nodes[node_idx].unicastAddress;
    }else{
        // find an unused unicast adr ,should suppose the provisioner is a node too
        int uni_adr = provision_self_get_new_unicast();
        if(uni_adr >0){
            net_info->unicast_address = uni_adr;
        }
        return 0;
    }

    return 1;
}

int json_valid_netidx(mesh_json_netkeys_str *p_net)
{
    for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
        mesh_json_netkeys_str *p_id_net = &(p_net[i]);
        if(p_id_net->valid){
            return i;
        }
    }
    return -1;
}

int  provision_self_get_new_unicast()
{
    u16 adr_max =0;
    for(int i=0;i<MAX_MESH_NODE_NUM;i++){
        mesh_node_str *p_node = &(json_database.nodes[i]);
        if(p_node->valid){
            int ele_cnt = json_mesh_get_ele_cnt_by_node(p_node);
            if((p_node->unicastAddress+ele_cnt)>adr_max){
                adr_max = p_node->unicastAddress+ele_cnt;
            }
        }
    }
    if(adr_max == 0){
        adr_max = -1;
    }
    return adr_max;
}

int  json_mesh_get_ele_cnt_by_node(mesh_node_str *p_node)
{
    int ele_cnt =0;
    for(int i=0;i<MAX_MESH_NODE_ELE_NUM;i++){
        mesh_nodes_ele_str *p_ele = p_node->elements;
        if(p_ele->valid){
            ele_cnt++;
        }
    }
    return ele_cnt;
}

int init_json(void)
{
    TeLog(@"init_json");
    //load json to lib
    memcpy(&json_database.schema, SigDataSource.share.$schema.UTF8String, JSON_DATABASE_STRING_MAX);
    memcpy(&json_database.version, SigDataSource.share.version.UTF8String, JSON_DATABASE_STRING_MAX);
    memcpy(&json_database.mesh_uuid, [LibTools nsstringToHex:SigDataSource.share.meshUUID].bytes, 16);
    memcpy(&json_database.meshName, SigDataSource.share.meshName.UTF8String, JSON_DATABASE_STRING_MAX);
    memcpy(&json_database.timestamp, [LibTools nsstringToHex:SigDataSource.share.timestamp].bytes, 4);
    if (SigDataSource.share.provisioners.count > 0) {
        for (int i=0; i<SigDataSource.share.provisioners.count; i++) {
            SigProvisionerModel *provisioner = SigDataSource.share.provisioners[i];
            mesh_provisioners_str provisioners_struct = {};
            provisioners_struct.valid = 1;
            memcpy(&provisioners_struct.provisionerName, provisioner.provisionerName.UTF8String, JSON_DATABASE_STRING_MAX);
            memcpy(&provisioners_struct.uuid, [LibTools nsstringToHex:provisioner.UUID].bytes, 16);
            if (provisioner.allocatedUnicastRange.count > 0) {
                for (int a=0; a<provisioner.allocatedUnicastRange.count; a++) {
                    SigRangeModel *rangeModel = provisioner.allocatedUnicastRange[a];
                    mesh_json_pro_UnicastRange unicastRange = {};
                    unicastRange.lowAddress = rangeModel.lowIntAddress;
                    unicastRange.highAddress = rangeModel.hightIntAddress;
                    memcpy(&provisioners_struct.unicastRange[a], &unicastRange, sizeof(unicastRange));
                }
            }
            if (provisioner.allocatedGroupRange.count > 0) {
                for (int a=0; a<provisioner.allocatedGroupRange.count; a++) {
                    SigRangeModel *rangeModel = provisioner.allocatedGroupRange[a];
                    mesh_json_pro_UnicastRange unicastRange = {};
                    unicastRange.lowAddress = rangeModel.lowIntAddress;
                    unicastRange.highAddress = rangeModel.hightIntAddress;
                    memcpy(&provisioners_struct.groupRange[a], &unicastRange, sizeof(unicastRange));
                }
            }
            if (provisioner.allocatedSceneRange.count > 0) {
                for (int a=0; a<provisioner.allocatedSceneRange.count; a++) {
                    SigSceneRangeModel *rangeModel = provisioner.allocatedSceneRange[a];
                    mesh_json_pro_UnicastRange unicastRange = {};
                    unicastRange.lowAddress = rangeModel.lowAddress;
                    unicastRange.highAddress = rangeModel.highAddress;
                    memcpy(&provisioners_struct.sceneRange[a], &unicastRange, sizeof(unicastRange));
                }
            }
            memcpy(&json_database.provisioners[i], &provisioners_struct, sizeof(provisioners_struct));
        }
    }
    memcpy(&json_database.ivi_idx, [LibTools nsstringToHex:SigDataSource.share.ivIndex].bytes, 4);
    if (SigDataSource.share.netKeys.count > 0) {
        for (int i=0; i<SigDataSource.share.netKeys.count; i++) {
            SigNetkeyModel *netkeyModel = SigDataSource.share.netKeys[i];
            mesh_json_netkeys_str netkeys_struct = {};
            netkeys_struct.valid = 1;
            memcpy(&netkeys_struct.name, netkeyModel.name.UTF8String, JSON_DATABASE_STRING_MAX);
            netkeys_struct.index = (int)netkeyModel.index;
            netkeys_struct.phase = netkeyModel.phase;
            memcpy(&netkeys_struct.key, [LibTools nsstringToHex:netkeyModel.key].bytes, 16);
            memcpy(&netkeys_struct.minSecurity, netkeyModel.minSecurity.UTF8String, JSON_DATABASE_STRING_MAX);
            memcpy(&netkeys_struct.oldkey, [LibTools nsstringToHex:netkeyModel.oldKey].bytes, 16);
            memcpy(&netkeys_struct.timestamps, [LibTools nsstringToHex:netkeyModel.timestamp].bytes, 4);
            memcpy(&json_database.netKeys[i], &netkeys_struct, sizeof(netkeys_struct));
        }
    }
    if (SigDataSource.share.appKeys.count > 0) {
        for (int i=0; i<SigDataSource.share.appKeys.count; i++) {
            SigAppkeyModel *appkeyModel = SigDataSource.share.appKeys[i];
            mesh_json_appkeys_str appkeys_struct = {};
            appkeys_struct.valid = 1;
            memcpy(&appkeys_struct.name, appkeyModel.name.UTF8String, JSON_DATABASE_STRING_MAX);
            appkeys_struct.index = (int)appkeyModel.index;
            appkeys_struct.bound_netkey = (int)appkeyModel.boundNetKey;
            memcpy(&appkeys_struct.key, [LibTools nsstringToHex:appkeyModel.key].bytes, 16);
            memcpy(&appkeys_struct.oldKey, [LibTools nsstringToHex:appkeyModel.oldKey].bytes, 16);
            memcpy(&json_database.appkeys[i], &appkeys_struct, sizeof(appkeys_struct));
        }
    }
    if (SigDataSource.share.nodes.count > 0) {
        for (int i=0; i<SigDataSource.share.nodes.count; i++) {
            SigNodeModel *nodeModel = SigDataSource.share.nodes[i];
            mesh_node_str nodes_struct = {};
            nodes_struct.valid = 1;
            if(nodeModel.macAddress) memcpy(&nodes_struct.macAddress, [LibTools nsstringToHex:nodeModel.macAddress].bytes, 6);
            if(nodeModel.UUID) memcpy(&nodes_struct.uuid, [LibTools nsstringToHex:nodeModel.UUID].bytes, 16);
            if ([nodeModel.UUID isEqualToString:SigDataSource.share.curProvisionerModel.UUID]) {
                nodes_struct.sno = SigDataSource.share.getLocationSno;
            } else {
                nodes_struct.sno = 0;
            }
            nodes_struct.unicastAddress = nodeModel.address;
            if(nodeModel.deviceKey) memcpy(&nodes_struct.deviceKey, [LibTools nsstringToHex:nodeModel.deviceKey].bytes, 16);
            if(nodeModel.security) memcpy(&nodes_struct.security, nodeModel.security.UTF8String, JSON_DATABASE_STRING_MAX);
            if (nodeModel.netKeys.count > 0) {
                for (int j=0; j<nodeModel.netKeys.count; j++) {
                    SigNodeKeyModel *nodekeyModel = nodeModel.netKeys[j];
                    mesh_nodes_net_appkey_str nodesNetkey_struct = {};
                    nodesNetkey_struct.valid = 1;
                    nodesNetkey_struct.index = nodekeyModel.index;
                    memcpy(&nodes_struct.netkeys[j], &nodesNetkey_struct, sizeof(nodesNetkey_struct));
                }
            }
            nodes_struct.configComplete = nodeModel.configComplete?1:0;
            if(nodeModel.name) memcpy(&nodes_struct.name, nodeModel.name.UTF8String, JSON_DATABASE_STRING_MAX);
            if(nodeModel.cid) nodes_struct.cid = [LibTools uint16From16String:nodeModel.cid];
            if(nodeModel.pid) nodes_struct.pid = [LibTools uint16From16String:nodeModel.pid];
            if(nodeModel.vid) nodes_struct.vid = [LibTools uint16From16String:nodeModel.vid];
            if(nodeModel.crpl) nodes_struct.crpl = [LibTools uint16From16String:nodeModel.crpl];
            mesh_nodes_feature_str features = {};
            features.relay = nodeModel.features.relay;
            features.proxy = nodeModel.features.proxy;
            features.fri = nodeModel.features.friend;
            features.lowpower = nodeModel.features.lowPower;
            memcpy(&nodes_struct.features, &features, sizeof(features));
            nodes_struct.secureNetworkBeacon = nodeModel.secureNetworkBeacon?1:0;
            nodes_struct.defaultTTL = nodeModel.defaultTTL;
            mesh_nodes_networkTransmit_str networkTransmit = {};
            networkTransmit.count = nodeModel.networkTransmit.count;
            networkTransmit.interval = nodeModel.networkTransmit.interval;
            memcpy(&nodes_struct.networkTransmit, &networkTransmit, sizeof(networkTransmit));
            mesh_nodes_relay_str relayRetransmit = {};
            relayRetransmit.count = nodeModel.relayRetransmit.count;
            relayRetransmit.interval = nodeModel.relayRetransmit.interval;
            memcpy(&nodes_struct.relayRetransmit, &relayRetransmit, sizeof(relayRetransmit));
            if (nodeModel.appKeys.count > 0) {
                for (int j=0; j<nodeModel.appKeys.count; j++) {
                    SigNodeKeyModel *nodeAppKeyModel = nodeModel.appKeys[j];
                    mesh_nodes_net_appkey_str appkey_struct = {};
                    appkey_struct.valid = 1;
                    appkey_struct.index = nodeAppKeyModel.index;
                    memcpy(&nodes_struct.appkeys[j], &appkey_struct, sizeof(appkey_struct));
                }
            }
            if (nodeModel.elements.count > 0) {
                for (int j=0; j<nodeModel.elements.count; j++) {
                    SigElementModel *eleModel = nodeModel.elements[j];
                    mesh_nodes_ele_str ele_struct = {};
                    ele_struct.valid = 1;
                    if(eleModel.name) memcpy(&ele_struct.name, eleModel.name.UTF8String, JSON_DATABASE_STRING_MAX);
                    ele_struct.index = (int)eleModel.index;
                    if(eleModel.location) ele_struct.location = [LibTools uint16From16String:eleModel.location];
                    if (eleModel.models.count > 0) {
                        for (int k=0; k<eleModel.models.count; k++) {
                            SigModelIDModel *modelIDModel = eleModel.models[k];
                            mesh_nodes_model_str modelID_struct = {};
                            modelID_struct.valid = 1;
                            modelID_struct.vendor_flag = modelIDModel.modelId.length==8;
                            modelID_struct.modelId = modelIDModel.getIntModelID;
                            if (modelIDModel.subscribe.count > 0) {
                                for (int l=0; l<modelIDModel.subscribe.count; l++) {
                                    NSString *subscribe = modelIDModel.subscribe[l];
                                    u16 subscribeU16 = (u16)[LibTools uint16From16String:subscribe];
                                    modelID_struct.subscribe[l] = subscribeU16;
                                }
                            }
                            if (modelIDModel.bind.count > 0) {
                                for (int l=0; l<modelIDModel.bind.count; l++) {
                                    NSNumber *bind = modelIDModel.bind[l];
                                    mesh_model_bind_str bind_struct = {};
                                    bind_struct.valid = 1;
                                    bind_struct.bind = (u16)bind.intValue;
                                    modelID_struct.bind[l] = bind_struct;
                                }
                            }
                            mesh_models_publish_str publish = {};
                            if(modelIDModel.publish) {
                                if (modelIDModel.publish.address) {
                                    publish.address = [LibTools uint16From16String:modelIDModel.publish.address];
                                }
                                publish.index = (int)modelIDModel.publish.index;
                                publish.ttl = modelIDModel.publish.ttl;
                                publish.period = (int)modelIDModel.publish.period;
                                publish.credentials = modelIDModel.publish.credentials;
                                publish.count = modelIDModel.publish.retransmit.count;
                                publish.interval = modelIDModel.publish.retransmit.interval;
                            }
                            memcpy(&modelID_struct.publish, &publish, sizeof(publish));
                            memcpy(&ele_struct.models[k], &modelID_struct, sizeof(modelID_struct));
                        }
                    }
                    memcpy(&nodes_struct.elements[j], &ele_struct, sizeof(ele_struct));
                }
            }
            nodes_struct.blacklisted = nodeModel.blacklisted?1:0;
            memcpy(&json_database.nodes[i], &nodes_struct, sizeof(nodes_struct));
        }
    }
    if (SigDataSource.share.groups.count > 0) {
        for (int i=0; i<SigDataSource.share.groups.count; i++){
            SigGroupModel *groupModel = SigDataSource.share.groups[i];
            mesh_group_str group_struct = {};
            group_struct.valid = 1;
            memcpy(&group_struct.name, groupModel.name.UTF8String, JSON_DATABASE_STRING_MAX);
            group_struct.address = groupModel.intAddress;
            if(groupModel.parentAddress && groupModel.parentAddress.length > 0) group_struct.parentAddress = [LibTools uint16From16String:groupModel.parentAddress];
            memcpy(&json_database.groups[i], &group_struct, sizeof(group_struct));
        }
    }
    if (SigDataSource.share.scenes.count > 0) {
        for (int i=0; i<SigDataSource.share.scenes.count; i++){
            SigSceneModel *sceneModel = SigDataSource.share.scenes[i];
            mesh_scene_str scene_struct = {};
            scene_struct.valid = 1;
            if(sceneModel.name) memcpy(&scene_struct.name, sceneModel.name.UTF8String, JSON_DATABASE_STRING_MAX);
            if (sceneModel.addresses.count) {
                for (int j=0; j<sceneModel.addresses.count; j++){
                    NSString *address = sceneModel.addresses[j];
                    scene_struct.addresses[j] = [LibTools uint16From16String:address];
                }
            }
            scene_struct.number = (int)sceneModel.number;
            memcpy(&json_database.scene[i], &scene_struct, sizeof(scene_struct));
        }
    }
    
    NSData *uuidData = [LibTools nsstringToHex:SigDataSource.share.curLocationNodeModel.UUID];
    memcpy(&vc_uuid, uuidData.bytes, sizeof(vc_uuid));
    
    return 1;
}

int mesh_misc_store_win32(misc_save_t *p_misc)
{
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    p_node->sno = p_misc->sno;
    memcpy(json_database.ivi_idx,p_misc->iv_index,sizeof(p_misc->iv_index));

    [SigDataSource.share setLocationSno:p_misc->sno];
    NSData *ivIndexData = [NSData dataWithBytes:p_misc->iv_index length:4];
    if (SigDataSource.share.hasWriteDataSourceToLib) {
        [SigDataSource.share updateIvIndexData:ivIndexData];
    }

    write_json_file_doc();
    return 0;
}

void write_json_file_doc()
{
    [SigDataSource.share saveLocationData];
}

int mesh_key_retrieve_win32(mesh_key_save_t *p_key_save)
{
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    u8 netkey_idx =0;
    u8 appkey_idx =0;
    memcpy(p_key_save->dev_key,p_node->deviceKey,sizeof(p_node->deviceKey));
    mesh_net_key_save_t *p_net = p_key_save->net_key;

    // proc json file's netkey part
    for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
        mesh_nodes_net_appkey_str *p_json_net = &(p_node->netkeys[i]);
        appkey_idx =0;
        if(p_json_net->valid){
            p_net[netkey_idx].valid =1;
            p_net[netkey_idx].node_identity =1;
            p_net[netkey_idx].index = p_json_net[i].index;
            // use the netkey idx to find the netkey value part
            u8 *p_net_json = json_get_netkey_by_idx(p_json_net[i].index);
            if(p_net_json!=NULL){
                memcpy(p_net[netkey_idx].key,p_net_json,16);
            }
            // proc json file's appkey part
            mesh_app_key_t *p_appkey_save = p_net[netkey_idx].app_key;
            for(int j=0;j<MAX_APPKEY_TOTAL_NUM;j++){
                mesh_nodes_net_appkey_str *p_json_app = &(p_node->appkeys[j]);
                if( p_json_app->valid &&
                   json_use_appkey_get_netkey(p_json_app->index)== p_net[netkey_idx].index){
                    // valid appkey
                    p_appkey_save[appkey_idx].valid =1;
                    p_appkey_save[appkey_idx].index = p_json_app->index;
                    u8 *p_app_val = json_use_appkey_get_key_val(p_json_app->index);
                    if(p_app_val!= NULL){
                        memcpy(p_appkey_save[appkey_idx].key,p_app_val,16);
                    }
                    appkey_idx++;
                    if(appkey_idx>=APP_KEY_MAX){
                        break;
                    }
                }
            }
            netkey_idx++;
            if(netkey_idx>=NET_KEY_MAX){
                break;
            }
        }
    }
    return 0;
}

int mesh_misc_retrieve_win32(misc_save_t *p_misc)
{
    memset(p_misc,0,sizeof(misc_save_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    p_misc->sno = p_node->sno;
    memcpy(p_misc->iv_index,json_database.ivi_idx,sizeof(p_misc->iv_index));
    return 0;
}

mesh_node_str * json_get_mesh_node(u8 *p_uuid)
{
    int node_idx = provision_find_json_node_by_uuid(vc_uuid);
    if(node_idx <0){
        return NULL;
    }else{
        return &(json_database.nodes[node_idx]);
    }
}

int  provision_find_json_node_by_uuid(u8 *p_uuid)
{
    mesh_node_str *p_node = json_database.nodes;
    for(int i=0;i<MAX_MESH_NODE_NUM;i++){
        if(p_node->valid&&!memcmp(p_node[i].uuid,p_uuid,sizeof(p_node[i].uuid))){
            return i;
        }
    }
    return -1;
}

int update_VC_info_from_json(int net_idx,VC_node_info_t *p_vc_node)
{
    // update the node suit for the net_idx,and update to vc node info part
    int vc_node_idx =0;
    for(int i=0;i<MAX_MESH_NODE_NUM;i++){
        mesh_node_str *p_node = &(json_database.nodes[i]);
        if(p_node->valid && json_node_netkey_indx_suit(net_idx,p_node)){
            //set node info part
            update_json_info_to_vc(p_node,&(p_vc_node[vc_node_idx]));
            vc_node_idx++;
        }
    }
    return 1;
}

int json_node_netkey_indx_suit(int net_idx,mesh_node_str *p_node)
{
    for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
        mesh_nodes_net_appkey_str *p_net = &(p_node->netkeys[i]);
        if(p_net->valid && p_net->index == net_idx){
            return 1;
        }
    }
    return 0;
}

int update_json_info_to_vc(mesh_node_str *p_node,VC_node_info_t *p_vc_node)
{
    memset(p_vc_node,0,sizeof(VC_node_info_t));
    p_vc_node->node_adr = p_node->unicastAddress;
    memcpy(p_vc_node->dev_key,p_node->deviceKey,sizeof(p_node->deviceKey));
    mesh_page0_head_t *p_vc_page_head = &(p_vc_node->cps.page0_head);
    p_vc_page_head->cid = p_node->cid;
    p_vc_page_head->pid = p_node->pid;
    p_vc_page_head->crpl = p_node->crpl;
    p_vc_page_head->vid = p_node->vid;
    mesh_page_feature_t *p_vc_fea = &(p_vc_page_head->feature);
    mesh_nodes_feature_str *p_node_fea = &(p_node->features);
    p_vc_fea->frid = p_node_fea->fri;
    p_vc_fea->low_power = p_node_fea->lowpower;
    p_vc_fea->proxy = p_node_fea->proxy;
    p_vc_fea->relay = p_node_fea->relay;
    //get the nums and numv part
    p_vc_node->element_cnt = update_json_ele_info_to_vc(p_node->elements,
                                                        &(p_vc_node->cps.model_id),&(p_vc_node->cps.len_cps));
    return 1;

}

int update_json_ele_info_to_vc(mesh_nodes_ele_str *p_ele,mesh_element_model_id *p_id,u16 *p_len)
{
    int idx =0;
    int ele_cnt =0;
    u8 *p_nums = &(p_id->nums);
    u8 *p_numv = &(p_id->numv);
    for(int j=0;j<MAX_MESH_NODE_ELE_NUM;j++){
        mesh_nodes_ele_str * p_ele_dst = (p_ele+j);
        if(p_ele_dst->valid){
            ele_cnt++;
            for(int i=0;i<MAX_MODELS_IN_ELE_CNT;i++){
                mesh_nodes_model_str *p_node_md = &(p_ele_dst->models[i]);
                if(p_node_md->valid){
                    // suppose is sig model ,not dispatch the vendor model now
                    if(p_node_md->vendor_flag){
                        (*p_numv)++;
                        memcpy(p_id->id+idx,(u8 *)&(p_node_md->modelId),4);
                        idx+=4;
                    }else{
                        (*p_nums)++;
                        memcpy(p_id->id+idx,(u8 *)&(p_node_md->modelId),2);
                        idx +=2;
                    }
                }
            }
            //local proc
            p_id->id[idx] =0 ;
            p_id->id[idx+1] =0 ;
            p_nums = &p_id->id[idx+2];
            p_numv = &p_id->id[idx+3];
            idx = idx+4;
        }else{
            break;
        }
    }
    *p_len = 14+idx-4;
    return ele_cnt;
}

int mesh_model_cfg_retrieve_win32(model_sig_cfg_s_t *p_cfg)
{
    memset(p_cfg,0,sizeof(model_sig_cfg_s_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    mesh_common_model_retrieve_win(&p_cfg->com,p_node,SIG_MD_CFG_SERVER,0);
    p_cfg->sec_nw_beacon = p_node->secureNetworkBeacon;
    p_cfg->ttl_def = p_node->defaultTTL;
    p_cfg->gatt_proxy = p_node->features.proxy;
    p_cfg->frid = p_node->features.fri;
    p_cfg->relay = p_node->features.relay;
    p_cfg->nw_transmit.count = p_node->networkTransmit.count;
    p_cfg->nw_transmit.invl_steps = p_node->networkTransmit.interval;
    p_cfg->relay_retransmit.count = p_node->relayRetransmit.count;
    p_cfg->relay_retransmit.invl_steps = p_node->relayRetransmit.interval;
    return 0;
}

void mesh_common_model_retrieve_win(model_common_t *p_common,mesh_node_str *p_node,u32 mode_id,u8 idx)
{
    // use the mode_id to get the model pointer ,and the element idx
    u16 ele_adr;
    u8 bind_idx =0;
    mesh_nodes_model_str *p_json_model = json_get_model_pointer_by_model_id(p_node,mode_id,&ele_adr,idx);
    if(p_json_model ==NULL){
        return ;
    }
    p_common->ele_adr = ele_adr;
    for(int i=0;i<MAX_BIND_ADDRESS_NUM;i++){
        mesh_model_bind_str *p_bind = &(p_json_model->bind[i]);
        if(p_bind->valid){
            p_common->bind_key[bind_idx].bind_ok =1;
            p_common->bind_key[bind_idx].idx = p_bind->bind;
            p_common->bind_key[bind_idx].rsv =0;
            bind_idx++;
            if(bind_idx>=BIND_KEY_MAX){
                break;
            }
        }
    }
    //sub proc part
    memcpy(p_common->sub_list,p_json_model->subscribe,sizeof(p_common->sub_list));
    // pub proc part
    mesh_model_pub_par_t *p_pub_common = &(p_common->pub_par);
    mesh_models_publish_str *p_pub_json = &(p_json_model->publish);
    p_common->pub_adr = p_pub_json->address;
    p_pub_common->appkey_idx = p_pub_json->index;
    p_pub_common->credential_flag = p_pub_json->credentials;
    p_pub_common->period.res = (p_pub_json->period)>>6;
    p_pub_common->period.steps = (p_pub_json->period)&0x3f;
    p_pub_common->ttl = p_pub_json->ttl;
    p_pub_common->transmit.count = p_pub_json->count;
    p_pub_common->transmit.invl_steps = p_pub_json->interval;
}

mesh_nodes_model_str *json_get_model_pointer_by_model_id(mesh_node_str *p_node,u32 mode_id,u16 *p_ele_adr,u8 idx)
{
    u8 node_model_idx =0;
    for(int i=0;i<MAX_MESH_NODE_ELE_NUM;i++){
        mesh_nodes_ele_str *p_ele = &p_node->elements[i];
        for(int j=0;j<MAX_MODELS_IN_ELE_CNT;j++){
            mesh_nodes_model_str *p_model = &p_ele->models[j];
            if(p_model->valid && p_model->modelId == mode_id){
                if((node_model_idx++) == idx){
                    *p_ele_adr = p_node->unicastAddress + i;
                    return p_model;
                }
            }
        }
    }
    return NULL;
}

u8 * json_get_netkey_by_idx(u16 idx)
{
    for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
        mesh_json_netkeys_str *p_netkey = &json_database.netKeys[i];
        if(p_netkey->valid && p_netkey->index == idx){
            return p_netkey->key;
        }
    }
    return NULL;
}

int json_use_appkey_get_netkey(u16 app_key_idx)
{
    for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
        mesh_json_appkeys_str *p_app = &(json_database.appkeys[i]);
        if(p_app->valid && app_key_idx == p_app->index){
            return p_app->bound_netkey;
        }
    }
    return -1;
}

u8 *json_use_appkey_get_key_val(u16 app_key_idx)
{
    for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
        mesh_json_appkeys_str *p_app = &(json_database.appkeys[i]);
        if(p_app->valid && app_key_idx == p_app->index){
            return p_app->key;
        }
    }
    return NULL;
}

int mesh_model_health_retrieve_win32(model_health_t * p_health)
{
    memset(p_health,0,sizeof(model_health_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    mesh_common_model_retrieve_win(&(p_health->clnt.com),p_node,SIG_MD_HEALTH_CLIENT,0);
    mesh_common_model_retrieve_win(&(p_health->srv.com),p_node,SIG_MD_HEALTH_SERVER,0);
    return 0;
}

int mesh_model_onoff_retrieve_win32(model_g_onoff_level_t *p_onoff)
{
    memset(p_onoff,0,sizeof(model_g_onoff_level_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_light =&(p_onoff->onoff_srv[i]);
        mesh_common_model_retrieve_win(&(p_light->com),p_node,SIG_MD_G_ONOFF_S,i);
    }
#if MD_LEVEL_EN
    for(int j=0;j<LIGHT_CNT * ELE_CNT_EVERY_LIGHT;j++){
        model_g_light_s_t *p_level = &(p_onoff->level_srv[j]);
        mesh_common_model_retrieve_win(&(p_level->com),p_node,SIG_MD_G_LEVEL_S,j);
    }
#endif
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_onoff_cli = (p_onoff->onoff_clnt);
    mesh_common_model_retrieve_win(&(p_onoff_cli->com),p_node,SIG_MD_G_ONOFF_C,0);
#if MD_LEVEL_EN
    model_client_common_t *p_level_cli = (p_onoff->level_clnt);
    mesh_common_model_retrieve_win(&(p_level_cli->com),p_node,SIG_MD_G_LEVEL_C,0);
#endif
#endif
    return 0;

}

int mesh_model_time_retrieve_win32(model_time_schedule_t *p_timer)
{
    memset(p_timer,0,sizeof(model_time_schedule_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
#if MD_TIME_EN
    model_g_light_s_t *p_time_srv = p_timer->time_srv;
    model_g_light_s_t *p_time_setup = p_timer->time_setup;
    mesh_common_model_retrieve_win(&(p_time_srv->com),p_node,SIG_MD_TIME_S,0);
    mesh_common_model_retrieve_win(&(p_time_setup->com),p_node,SIG_MD_TIME_SETUP_S,0);
#endif
#if MD_SCHEDULE_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_sch_srv = &(p_timer->sch_srv[i]);
        model_g_light_s_t *p_sch_setup = &(p_timer->sch_setup[i]);
        mesh_common_model_retrieve_win(&(p_sch_srv->com),p_node,SIG_MD_SCHED_S,i);
        mesh_common_model_retrieve_win(&(p_sch_setup->com),p_node,SIG_MD_SCHED_SETUP_S,i);
    }
#endif
#endif
#if MD_CLIENT_EN
#if MD_TIME_EN
    model_client_common_t *p_time_clnt = (p_timer->time_clnt);
    mesh_common_model_retrieve_win(&(p_time_clnt->com),p_node,SIG_MD_TIME_C,0);
#endif
#if MD_SCHEDULE_EN
    model_client_common_t *p_sch_clnt = (p_timer->sch_clnt);
    mesh_common_model_retrieve_win(&(p_sch_clnt->com),p_node,SIG_MD_SCHED_C,0);
#endif
#endif
    return 0;

}

int mesh_model_lightness_retrieve_win32(model_lightness_t *p_light)
{
    memset(p_light,0,sizeof(model_lightness_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_LIGHTNESS_EN
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_light->srv[i]);
        model_g_light_s_t *p_setup = &(p_light->setup[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHTNESS_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHTNESS_SETUP_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_light->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHTNESS_C,0);
#endif
#endif
    return 0;
}

int mesh_model_light_ctl_retrieve_win32(model_light_ctl_t *p_ctl)
{
    memset(p_ctl,0,sizeof(model_light_ctl_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_ctl->srv[i]);
        model_g_light_s_t *p_setup = &(p_ctl->setup[i]);
        model_g_light_s_t *p_temp = &(p_ctl->temp[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHT_CTL_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHT_CTL_SETUP_S,i);
        mesh_common_model_retrieve_win(&(p_temp->com),p_node,SIG_MD_LIGHT_CTL_TEMP_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_ctl->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHT_CTL_C,0);
#endif
    return 0;

}

int mesh_model_light_lc_retrieve_win32(model_light_lc_t *p_lc)
{
    memset(p_lc,0,sizeof(model_light_lc_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &p_lc->srv[i];
        model_g_light_s_t *p_setup = &p_lc->setup[i];
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHT_LC_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHT_LC_SETUP_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_lc->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHT_LC_C,0);
#endif
    return 0;
}

int mesh_model_light_hsl_retrieve_win32(model_light_hsl_t *p_hsl)
{
    memset(p_hsl,0,sizeof(model_light_hsl_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_hsl->srv[i]);
        model_g_light_s_t *p_setup = &(p_hsl->setup[i]);
        model_g_light_s_t *p_hue = &(p_hsl->hue[i]);
        model_g_light_s_t *p_sat = &(p_hsl->sat[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHT_HSL_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHT_HSL_SETUP_S,i);
        mesh_common_model_retrieve_win(&(p_hue->com),p_node,SIG_MD_LIGHT_HSL_HUE_S,i);
        mesh_common_model_retrieve_win(&(p_sat->com),p_node,SIG_MD_LIGHT_HSL_SAT_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_hsl->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHT_HSL_C,0);
#endif
    return 0;
}

int mesh_model_sensor_retrieve_win32(model_sensor_t *p_sensor)
{
    memset(p_sensor,0,sizeof(model_sensor_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
#if MD_SENSOR_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_sensor_srv = &p_sensor->sensor_srv[i];
        model_g_light_s_t *p_sensor_setup = &p_sensor->sensor_setup[i];
        mesh_common_model_retrieve_win(&(p_sensor_srv->com),p_node,SIG_MD_SENSOR_S,i);
        mesh_common_model_retrieve_win(&(p_sensor_setup->com),p_node,SIG_MD_SENSOR_SETUP_S,i);
    }
    // not proc the sensor state part
#endif
#if MD_BATTERY_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_bat_srv = &(p_sensor->battery_srv[i]);
        mesh_common_model_retrieve_win(&(p_bat_srv->com),p_node,SIG_MD_G_BAT_S,i);
    }
#endif
#if MD_LOCATION_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_loc_srv = &(p_sensor->location_srv[i]);
        model_g_light_s_t *p_loc_setup = &(p_sensor->location_setup[i]);
        mesh_common_model_retrieve_win(&(p_loc_srv->com),p_node,SIG_MD_G_LOCATION_S,i);
        mesh_common_model_retrieve_win(&(p_loc_setup->com),p_node,SIG_MD_G_LOCATION_SETUP_S,i);
    }
#endif
#endif

#if MD_CLIENT_EN
#if MD_SENSOR_EN
    model_client_common_t *p_sensor_clnt = p_sensor->sensor_clnt;
    mesh_common_model_retrieve_win(&(p_sensor_clnt->com),p_node,SIG_MD_SENSOR_C,0);
#endif
#if MD_BATTERY_EN
    model_client_common_t *p_bat_clnt = p_sensor->battery_clnt;
    mesh_common_model_retrieve_win(&(p_bat_clnt->com),p_node,SIG_MD_G_BAT_C,0);
#endif
#if MD_LOCATION_EN
    model_client_common_t *p_loc_clnt = p_sensor->location_clnt;
    mesh_common_model_retrieve_win(&(p_loc_clnt->com),p_node,SIG_MD_G_LOCATION_C,0);
#endif
#endif
    return 0;
}

int mesh_model_power_onoff_win32(model_g_power_onoff_trans_time_t * p_power)
{
    memset(p_power,0,sizeof(model_g_power_onoff_trans_time_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
#if MD_POWER_ONOFF_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_pw_onoff_srv = &(p_power->pw_onoff_srv[i]);
        model_g_light_s_t *p_pw_onoff_setup = &(p_power->pw_onoff_setup[i]);
        mesh_common_model_retrieve_win(&(p_pw_onoff_srv->com),p_node,SIG_MD_G_POWER_ONOFF_S,i);
        mesh_common_model_retrieve_win(&(p_pw_onoff_setup->com),p_node,SIG_MD_G_POWER_ONOFF_SETUP_S,i);
    }
#endif
#if MD_DEF_TRANSIT_TIME_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_trans_time_srv = &(p_power->def_trans_time_srv[i]);
        mesh_common_model_retrieve_win(&(p_trans_time_srv->com),p_node,SIG_MD_G_DEF_TRANSIT_TIME_S,i);
    }
#endif
#endif
#if MD_CLIENT_EN
#if MD_POWER_ONOFF_EN
    model_client_common_t *p_onoff_clnt = p_power->pw_onoff_clnt;
    mesh_common_model_retrieve_win(&(p_onoff_clnt->com),p_node,SIG_MD_G_POWER_ONOFF_C,0);
#endif
#if MD_DEF_TRANSIT_TIME_EN
    model_client_common_t *p_trans_time_clnt = p_power->def_trans_time_clnt;
    mesh_common_model_retrieve_win(&(p_trans_time_clnt->com),p_node,SIG_MD_G_DEF_TRANSIT_TIME_C,0);
#endif
#endif
    return 0;
}

int mesh_model_scene_win32(model_scene_t * p_scene)
{
    memset(p_scene,0,sizeof(model_scene_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_scene->srv[i]);
        model_g_light_s_t *p_setup = &(p_scene->setup[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_SCENE_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_SCENE_SETUP_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_scene->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_SCENE_C,0);
#endif
    return 0;
}

int mesh_model_mesh_ota_win32(model_mesh_ota_t *p_ota)
{
    memset(p_ota,0,sizeof(model_mesh_ota_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if 1 // MD_SERVER_EN
    model_g_light_s_t *p_fw_distr_srv = &(p_ota->fw_distr_srv);
    model_g_light_s_t *p_fw_update_srv = &(p_ota->fw_update_srv);
    model_g_light_s_t *p_obj_trans_srv = &(p_ota->obj_trans_srv);
    model_client_common_t *p_fw_update_clnt = &(p_ota->fw_update_clnt);
    model_client_common_t *p_obj_trans_clnt = &(p_ota->obj_trans_clnt);
    mesh_common_model_retrieve_win(&(p_fw_distr_srv->com),p_node,SIG_MD_FW_DISTRIBUT_S,0);
    mesh_common_model_retrieve_win(&(p_fw_update_srv->com),p_node,SIG_MD_FW_UPDATE_S,0);
    mesh_common_model_retrieve_win(&(p_obj_trans_srv->com),p_node,SIG_MD_OBJ_TRANSFER_S,0);
    mesh_common_model_retrieve_win(&(p_fw_update_clnt->com),p_node,SIG_MD_FW_UPDATE_C,0);
    mesh_common_model_retrieve_win(&(p_obj_trans_clnt->com),p_node,SIG_MD_OBJ_TRANSFER_C,0);
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_fw_distr_clnt = &(p_ota->fw_distr_clnt);
    mesh_common_model_retrieve_win(&(p_fw_distr_clnt->com),p_node,SIG_MD_FW_DISTRIBUT_C,0);
#endif
    return 0;
}

int mesh_model_remote_prov_win32(model_remote_prov_t *p_remote)
{
    memset(p_remote,0,sizeof(model_remote_prov_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    model_rp_server_common_t *p_srv = p_remote->srv;
    mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_REMOTE_PROV_SERVER,0);
#endif
#if MD_CLIENT_EN
    model_rp_client_common_t *p_clnt = p_remote->client;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_REMOTE_PROV_CLIENT,0);
#endif
    return 0;
}





/*
 sample data:
 caculate the comfirm
 p_comfirm:              5f 7a 53 23 07 9d 99 e7 80 ee 21 4f 68 eb f7 05
 p_comfirm_key:       31 5c a9 09 bd e6 0a 8d 97 dd b0 f0 85 20 76 32
 pro_random:            8b 19 ac 31 d5 8b 12 4c 94 62 09 b5 db 10 21 b9
 */
extern int mesh_sec_prov_cloud_comfirm(u8* p_comfirm,u8 *p_comfirm_key,u8 *pro_random){
    /*static OOB 示范代码，添加天猫的设备需要*/
//    unsigned char s[32];
//    unsigned char aligenieAuthValue[16] = {0};
//    memcpy (s, pro_random, 16);
//    memcpy (s+16, aligenieAuthValue, 16);//oob
//    tn_aes_cmac (p_comfirm_key, s, 32, p_comfirm);
//    return 1;

    
    return 0;
    //该回调需要同步等待HTTP的值返回才往下走。且当前线程不是主线程。
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    TeLog(@"开始请求");
    LibHandle *tem = [[LibHandle alloc] init];
    [tem requestHTTP:^(id result, NSError *err) {
        if (err) {
            TeLog(@"%@",err.localizedDescription);
        } else {
            //校验成功
            //赋值到p_comfirm
            TeLog(@"校验成功");
        }
        dispatch_semaphore_signal(semaphore);
    }];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    TeLog(@"HTTP请求返回完成，继续其他代码");
}

/*
 sample data:
 check the comfirm value data
 p_comfirm_key:    31 5c a9 09 bd e6 0a 8d 97 dd b0 f0 85 20 76 32
 p_dev_random:     55 a2 a2 bc a0 4c d3 2f f6 f3 46 bd 0a 0c 1a 3a
 p_dev_comfirm:    2d ee 6f 21 a0 a0 21 14 f2 a3 ab bd b3 1b 49 14
 */
extern int mesh_cloud_dev_comfirm_check(u8 *p_comfirm_key,u8* p_dev_random,u8*p_dev_comfirm){
    /*static OOB 示范代码，添加天猫的设备需要*/
//    unsigned char s[32];
//    unsigned char aligenieAuthValue[16] = {0};
//    memcpy (s, p_dev_random, 16);
//    memcpy (s+16, aligenieAuthValue, 16);//oob
//    unsigned char comfirm[16];
//    tn_aes_cmac (p_comfirm_key, s, 32, comfirm);
//    return memcmp(comfirm, p_dev_comfirm, 16)==0?1:0;

    
    return 0;
    //该回调需要同步等待HTTP的值返回才往下走。参考代码同上。
}

typedef void (^myBlock)(id result,NSError *err);
- (void)requestHTTP:(myBlock)block{
    TeLog(@"requestHTTP");
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        if (block) {
            TeLog(@"block");
            block(nil,nil);
        }
    });
}

u16 master_filter_list[52]={};

extern void refreshKeyBindModelID(){
    NSArray *tem = SigDataSource.share.keyBindModelIDs;
    TeLog(@"refreshKeyBindModelID,list:%@",tem);
    for (int index = 0; index < tem.count; index ++) {
        NSNumber *modelID = tem[index];
        master_filter_list[index] = (u16)modelID.intValue;
    }
}

u8 model_need_key_bind_whitelist(u16 *key_bind_list_buf,u8 *p_list_cnt,u8 max_cnt)
{
    //app don't use whitelist, return 1.
    *p_list_cnt =0;
    return 1;
    
    //app use whitelist, need to comment the above two lines of code.
    NSInteger count = SigDataSource.share.keyBindModelIDs.count;
    *p_list_cnt = count;
    memcpy((u8 *)key_bind_list_buf,(u8 *)master_filter_list,count * 2);
    return 1;
}

extern void gatt_provision_net_info_callback(){
    return;
    u8 netkey[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    u16 key_idx = 0x0001;
    u8 flag =0;
    u8 ivi[4]={0x22,0x33,0x44,0x55};
    u16 unicast_adr = 0x6677;
    set_gatt_provision_net_info(netkey,&key_idx,&flag,ivi,&unicast_adr);
}

u8 win32_create_rand(){
    u8 key = arc4random() % 256;
    return key;
}

@end
