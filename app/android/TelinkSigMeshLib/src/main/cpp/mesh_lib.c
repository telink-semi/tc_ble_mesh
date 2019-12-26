/********************************************************************************************************
 * @file     mesh_lib.c
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



#pragma pack(1)

#include "mesh_lib.h"
#include "header/ble_lt_mesh/vendor/common/scene.h"
#include "header/ble_lt_mesh/vendor/common/ev.h"
#include "header/ble_lt_mesh/vendor/common/mesh_ota.h"
#include "header/ble_lt_mesh/vendor/common/lighting_model.h"
#include "header/ble_lt_mesh/vendor/common/mesh_node.h"
#include "header/ble_lt_mesh/proj_lib/ble/crypt/aes/aes_att.h"
#include "header/ble_lt_mesh/vendor/common/app_heartbeat.h"
#include <jni.h>
#include <time.h>
#include <stdio.h>
#include<Android/log.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "TelinkBluetoothSDK-JNI"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG ,__VA_ARGS__)

#define MAXB2TSIZE  1024*64
char arrb2t[] = "0123456789abcdef";

static JavaVM *gJavaVM;
static jobject gCallbackObject = NULL;
static jclass gClass = NULL;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("JNI_OnLoad");
    gJavaVM = vm;
    return JNI_VERSION_1_6;
}

// provision beacon状态回调
extern unsigned char onProvisionChecked(int result) {

    LOGD("onProvisionChecked");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onProvisionBeaconChecked", "(I)V");

    if (mid == NULL) {
        return 0;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, result); //执行方法

    return 1;
}


JNIEXPORT jint JNICALL Java_com_telink_sig_mesh_lib_MeshLib_getClock(JNIEnv *env, jobject obj) {

    return clock_time_vc_hw();
}

// getVersion bridge
JNIEXPORT jint JNICALL Java_com_telink_sig_mesh_lib_MeshLib_getVersion(JNIEnv *env, jobject obj) {
    LOGD("getVersion");
    gCallbackObject = (*env)->NewGlobalRef(env, obj);
    jclass cls = (*env)->GetObjectClass(env, gCallbackObject);
    gClass = (*env)->NewWeakGlobalRef(env, cls);
    return get_lib_version();
}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_testPragmaPack(JNIEnv *env, jobject obj) {
    LOGD("testPragmaPack");
    return test_pragma_pack();
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshLoopProcess(JNIEnv *env, jobject obj) {

    LOGD("meshLoopProcess");
    mesh_loop_process();
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_threadMainProcess(JNIEnv *env, jobject obj) {

//    LOGD("threadMainProcess");
    Thread_main_process();
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_masterClockInit(JNIEnv *env, jobject obj) {

    LOGD("masterClockInit");
    master_clock_init();
}

JNIEXPORT void JNICALL Java_com_telink_sig_mesh_lib_MeshLib_meshInitAll(JNIEnv *env, jobject obj) {
    // memset((u8 *)mesh_key+16,0,192-16);
    LOGD("meshInitAll");

    mesh_init_all();
//    gatt_provision_net_info_callback();
}

JNIEXPORT void JNICALL Java_com_telink_sig_mesh_lib_MeshLib_initMeshKey(JNIEnv *env, jobject obj) {

    LOGD("initMeshKey");
//    init_mesh_key();
}


JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_clockTimeExceed(JNIEnv *env, jobject obj, jint ref,
                                                     jint span_us) {

    LOGD("clockTimeExceed");
    return clock_time_exceed((unsigned int) ref, (unsigned int) span_us);
}

JNIEXPORT jint JNICALL Java_com_telink_sig_mesh_lib_MeshLib_clockTime(JNIEnv *env, jobject obj) {

    LOGD("clockTime");

    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &time1);
    u64 time = (u64) (time1.tv_nsec + time1.tv_sec * 1000 * 1000 * 1000);
//    LOGD("clockTime: %d %d", time1.tv_sec, time1.tv_nsec);
    LOGD("clockTime: %lld", time);


    return clock_time();
}

JNIEXPORT jint JNICALL Java_com_telink_sig_mesh_lib_MeshLib_ivTest(JNIEnv *env, jobject obj) {
    LOGD("ivTest, adr %d", ele_adr_primary);
    if (iv_idx_st.keep_time_s < 0x60000) {
        iv_idx_st.keep_time_s = 0x60000;
    }
    return 1;
}

JNIEXPORT jint JNICALL Java_com_telink_sig_mesh_lib_MeshLib_snoTest(JNIEnv *env, jobject obj) {
    LOGD("snoTest");
    if (mesh_adv_tx_cmd_sno < 0xc10000) {
        mesh_adv_tx_cmd_sno = 0xc10000;
    }
    return 1;
}

JNIEXPORT jint JNICALL Java_com_telink_sig_mesh_lib_MeshLib_ResetLocalAddress(JNIEnv *env, jobject obj, jint address) {
    LOGD("resetLocalAddress");
    if (mesh_adv_tx_cmd_sno < 0xc10000) {
        mesh_adv_tx_cmd_sno = 0xc10000;
    }
    return 1;
}


// 发送灯开关控制指令Deprecated
JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_sendOpParaOnOff(JNIEnv *env, jobject obj, jint addr,
                                                     jbyte rsp_max, jbyte onoff) {

    LOGD("sendOpParaOnOff Deprecated");
//    SendOpParaOnOff(addr, rsp_max, onoff);
}

// 获取level
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdGetLevel(JNIEnv *env, jobject obj, jint addr,
                                                 jbyte rsp_max) {

    LOGD("cmdGetLevel");
    return access_cmd_get_level((u16) addr, (u32) rsp_max);
}

//获取亮度
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdGetLum(JNIEnv *env, jobject obj, jint addr, jbyte rsp_max) {

    LOGD("cmdGetLum");
    return access_get_lum((u16) addr, (u8) rsp_max);
}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdGetOnOff(JNIEnv *env, jobject obj, jint addr,
                                                 jbyte rsp_max) {

    LOGD("cmdGetOnOff");
    return access_cmd_onoff_get((u16) addr, (u8) rsp_max);
}


// 获取色温
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdGetTemp(JNIEnv *env, jobject obj, jint addr,
                                                jbyte rsp_max) {

    LOGD("cmdGetTemp");
    return access_cmd_get_light_ctl_temp((u16) addr, (u32) rsp_max);
}


// 获取CTL 包括色温，亮度
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdGetCtl(JNIEnv *env, jobject obj, jint addr, jbyte rsp_max) {

    LOGD("cmdGetCtl");
    return access_cmd_get_light_ctl((u16) addr, (u32) rsp_max);
}

// 设置level
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSetLevel(JNIEnv *env, jobject obj, jint adr, jbyte rspMax,
                                                 jint level, jint ack) {

    LOGD("cmdSetLevel");
    return access_cmd_set_level((u16) adr, (u8) rspMax, (s16) level, ack, 0);
}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSetDelta(JNIEnv *env, jobject obj, jint adr, jbyte rspMax,
                                                 jint delta, jint ack, jbyteArray transition) {

    LOGD("cmdSetDelta");

    /*u8 trans[sizeof(transition_par_t)] = {0};
    if (transition == NULL){
        trans = NULL;
    } else{
        (*env)->GetByteArrayRegion(env, transition, 0, sizeof(transition_par_t), (jbyte *) trans);
    }*/


    u8 *trans;
    if (transition == NULL) {
        trans = NULL;
    } else {
        int len = 2;
        trans = (u8 *) malloc((size_t) len);
        memset(trans, 0, (size_t) len);

        (*env)->GetByteArrayRegion(env, transition, 0, len, (jbyte *) trans);
    }

    int result = access_cmd_set_delta((u16) adr, (u8) rspMax, delta, ack,
                                      (transition_par_t *) trans);
    free(trans);
    return result;

//    return access_cmd_set_level((u16) adr, (u8) rspMax, (s16) lightness, ack, 0);
}

// 新的设置亮度接口 @date 2017/11/13
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSetLum(JNIEnv *env, jobject obj, jint adr, jbyte rspMax,
                                               jbyte lum, jint ack) {

    LOGD("cmdSetLum");
    return access_set_lum((u16) adr, (u8) rspMax, (u8) lum, ack);
}

// 设置色温
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSetTemp(JNIEnv *env, jobject obj, jint adr, jbyte rspMax,
                                                jbyte temp, jint ack) {

    LOGD("cmdSetTemp");
    return access_cmd_set_light_ctl_temp_100((u16) adr, (u8) rspMax, (u8) temp, ack);
}

// 设置色彩 0-100
// transition 渐变
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSetHSL100(JNIEnv *env, jobject obj, jint adr, jbyte rspMax,
                                                  jbyte hue, jbyte saturation, jbyte lightness,
                                                  jint ack, jbyteArray transition) {

    LOGD("cmdSetHSL100");
//    return access_cmd_set_light_hsl_100(adr, rspMax, lightness, hue, saturation, ack);


    if ((!is_valid_lum(lightness)) || (!is_valid_val_100(hue)) || (!is_valid_val_100(saturation))) {
        return -2;
    }

    u16 l = lum2_lightness(lightness);
    u16 h = lum2_lightness(hue);
    u16 s = lum2_lightness(saturation);


    u8 *p;
    if (transition == NULL) {
        p = NULL;
    } else {
        int len = 2;
        p = (u8 *) malloc(len);
        memset(p, 0, len);

        (*env)->GetByteArrayRegion(env, transition, 0, len, p);
    }

    int result = access_cmd_set_light_hsl(adr, rspMax, l, h, s, ack, p);
    free(p);
    return result;

}

// unit convert utils

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_lum2Level(JNIEnv *env, jobject obj, jbyte lum) {
    LOGD("lum2level");
    return lum2level((u8) lum);
}

JNIEXPORT jbyte JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_level2Lum(JNIEnv *env, jobject obj, jshort level) {
    LOGD("lum2level");
    return level2lum(level);
}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_lum2Lightness(JNIEnv *env, jobject obj, jbyte lum) {

    LOGD("lum2Lightness");
    return lum2_lightness((u8) lum);
}

JNIEXPORT jbyte JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_lightness2Lum(JNIEnv *env, jobject obj, jint lightness) {

    LOGD("lightness2Lum");
    return lightness2_lum((u16) lightness);
}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_temp100ToTemp(JNIEnv *env, jobject obj, jbyte temp100) {

    LOGD("temp100ToTemp");
    return temp100_to_temp((u8) temp100);
}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_tempToTemp100(JNIEnv *env, jobject obj, jint temp) {

    LOGD("tempToTemp100");
    return temp_to_temp100((u16) temp);
}



// 开关灯
// trans 参数 延时 默认为0
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdOnOff(JNIEnv *env, jobject obj, jint addr, jbyte rsp_max,
                                              jbyte onoff, jint ack) {

    LOGD("cmdOnOff");
    return access_cmd_onoff((u16) addr, (u8) rsp_max, (u8) onoff, ack, 0);
}

// 获取节点的 subscription list 状态
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cfgCmdSubGet(JNIEnv *env, jobject obj, jint node_adr,
                                                  jint ele_adr, jint md_id) {

    LOGD("cfgCmdSubGet");
    return cfg_cmd_sub_get(node_adr, ele_adr, md_id);
}

// 设置节点的 subscription
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cfgCmdSubSet(JNIEnv *env, jobject obj, jint op, jint node_adr,
                                                  jint ele_adr, jint sub_adr, jint md_id,
                                                  jbyte sig) {

    LOGD("cfgCmdSubSet");
    return cfg_cmd_sub_set(op, node_adr, ele_adr, sub_adr, md_id, sig);
}

// 获取节点的 public address
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cfgCmdPubGet(JNIEnv *env, jobject obj, jint node_adr,
                                                  jint ele_adr, jint md_id, jbyte sig) {

    LOGD("cfgCmdPubGet");
    return cfg_cmd_pub_get(node_adr, ele_adr, md_id, sig);
}

// 设置节点的 public address
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cfgCmdPubSet(JNIEnv *env, jobject obj, jint node_adr,
                                                  jint ele_adr, jint pub_adr, jint md_id,
                                                  jbyteArray pub_param, jbyte sig) {

    LOGD("cfgCmdPubSet");

    u8 par[sizeof(mesh_model_pub_par_t)] = {0};
    if (pub_param != NULL) {
        (*env)->GetByteArrayRegion(env, pub_param, 0, sizeof(mesh_model_pub_par_t), (jbyte *) par);
    }

    // app_key_index
    return cfg_cmd_pub_set((u16) node_adr, (u16) ele_adr, (u16) pub_adr,
                           (mesh_model_pub_par_t *) par, (u32) md_id, sig);
}

// 设置节点的 relay
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cfgCmdRelaySet(JNIEnv *env, jobject obj, jint node_adr,
                                                    jbyte val) {

    LOGD("cfgCmdRelaySet");


    return cfg_cmd_relay_set((u16) node_adr, (u8) val);
}
// 设置本地 provision 配置信息
JNIEXPORT jbyte JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshProvisionParSetDir(JNIEnv *env, jobject obj,
                                                            jbyteArray data, jint len) {

    LOGD("meshProvisionParSetDir");
    u8 *p;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, data, 0, len, p);
    int result = mesh_provision_par_set_dir(p);
    free(p);
    return result;
}


// 删除对应节点
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdDeleteNode(JNIEnv *env, jobject obj, jint adr) {

    LOGD("cmdDeleteNode");
    return cfg_cmd_reset_node(adr);
}


// 设置provision信息 ， 针对连接的远程设备， 后调用startProvision操作
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_setProvisionData(JNIEnv *env, jobject j, jbyteArray data,
                                                      jint len) {

    LOGD("setProvisionData");
    u8 *p;
    p = (u8 *) malloc(len);
    memset(p, 0, len);

    (*env)->GetByteArrayRegion(env, data, 0, len, p);

    int result = set_app_key_pro_data(p, len);

    free(p);
    return result;
}


JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_startProvisionInvite(JNIEnv *env, jobject j) {
    LOGD("startProvisionInvite");
    start_provision_invite();
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_provisionRandomDataInit(JNIEnv *env, jobject j) {
    LOGD("provisionRandomDataInit");
    provision_random_data_init();
//    start_provision_invite();
}


JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_provisionDispatch(JNIEnv *env, jobject j, jbyteArray data,
                                                       jint len, jobject proxy_buf,
                                                       u16 p_proxy_len) {
//    LOGD("provisionDispatch");

    u8 *p;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, data, 0, len, p);

    u8 *buff_in;
    buff_in = (*env)->GetDirectBufferAddress(env, proxy_buf);

//    jint bufferLen = (*env)->GetDirectBufferCapacity(env, proxy_buf);
//    LOGW("proxy_len: %d", p_proxy_len);
//    LOGW("bufferLen: %d", bufferLen);

    jint ret_type = provision_dispatch_direct(p + 1, len - 1, buff_in, &p_proxy_len);

//    LOGW("proxy_len ret_type: %d", ret_type);

    if (ret_type == MSG_PROVISION_PDU) {
        gatt_rcv_pro_pkt_dispatch(buff_in, (u8) p_proxy_len);
    } else if (ret_type == MSG_NETWORK_PDU) {
        u8 bear[400];
        mesh_construct_adv_bear_with_nw(bear, buff_in, (u8) p_proxy_len);
        LOG_MSG_INFO(TL_LOG_MESH, bear + 1, bear[1], "MSG_NETWORK_PDU:", 0);
        app_event_handler_adv(bear + 1, ADV_FROM_GATT, 0);
    } else if (ret_type == MSG_MESH_BEACON) {

        mesh_cmd_bear_unseg_t bear;
        mesh_construct_adv_bear_with_bc((u8 *) &bear, buff_in, (u8) p_proxy_len);
        if (UNPROVISION_BEACON == bear.beacon.type) {
            check_pkt_is_unprovision_beacon(buff_in);
        } else if (SECURE_BEACON == bear.beacon.type) {
            mesh_rc_data_beacon_sec(&(bear.len), 0);
        }
        /*if (UNPROVISION_BEACON == buff_in[2]) {

            int checkResult = check_pkt_is_unprovision_beacon(buff_in);
            onProvisionChecked(checkResult);
        }*/
    } else if (ret_type == MSG_PROXY_CONFIG) {
        u8 proxy_buf_tmp[400 + 3];
        mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *) proxy_buf_tmp;
        p_bear->trans_par_val = TRANSMIT_DEF_PAR;
        p_bear->len = (u8) (p_proxy_len + 1);
        p_bear->type = MESH_ADV_TYPE_MESSAGE;

        memcpy(&p_bear->nw, buff_in, p_proxy_len);
        mesh_rc_data_cfg_gatt((u8 *) p_bear);
    }

    free(p);
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_provisionPktDispatch(JNIEnv *env, jobject j, jbyteArray data,
                                                          jint len) {

//    LOGD("provisionPktDispatch");

    u8 *p;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, data, 0, len, p);

    gatt_rcv_pro_pkt_dispatch(p, len);
    free(p);
}

//JNIEXPORT jint JNICALL
//Java_com_telink_sig_mesh_lib_MeshLib_getProvisionState(JNIEnv *env, jobject j) {
//
////    LOGD("getProvisionState");
//
//    return get_provision_state();
//}


// provision 过程中 处理notify
JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_pushNotifyIntoFifo(JNIEnv *env, jobject j, jbyteArray data,
                                                        jint len) {

    LOGD("pushNotifyIntoFifo:%d", len);
    u8 *p;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, data, 0, len, p);

    u8 dat[400];
    u16 len_d;
    dat[0] = DONGLE_REPORT_SPP_DATA;
    memcpy(dat + 1, p, len);
    len_d = len + 1;

    push_notify_into_fifo(dat, len_d);

    free(p);
}

// deprecated
JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_sendCommand(JNIEnv *env, jobject j, jbyteArray data,
                                                 jint len) {

    LOGD("sendCommand");
    u8 *p;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, data, 0, len, p);

//    write_no_rsps_pkts(p, len, PROXY_WRITE_HANDLER, 0);
    free(p);
}

// send common mesh command
/*
 adr_dst dest address
    rsp_max max response count
    op opcode
    para params
    len param len
 */
JNIEXPORT int JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_sendOpParaDebug(JNIEnv *env, jobject j, jint adr_dst,
                                                     jbyte rsp_max, jint op, jbyteArray para,
                                                     jint len) {

    LOGD("sendOpParaDebug");

    u8 *p;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, para, 0, len, p);

    int result = SendOpParaDebug(adr_dst, rsp_max, op, p, len);
    free(p);
    return result;
}


JNIEXPORT jbyteArray JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_getMeshKey(JNIEnv *env, jobject j) {

    LOGD("getMeshKey");


//    jint len = 192;
    jint len = 256;
    jbyte *jbt = (jbyte *) &mesh_key;


    jbyteArray jarray = (*env)->NewByteArray(env, len);   //byteSize是字符数组大小

    (*env)->SetByteArrayRegion(env, jarray, 0, len, jbt);
    return jarray;

}

/**
filter init
*/
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshProxySetFilterInit(JNIEnv *env, jobject j, jint adr) {

    cache_init(ADR_ALL_NODES);
    LOGD("mesh_proxy_set_filter_init:%d", adr);
    mesh_proxy_set_filter_init((u16) adr);
    mesh_tx_sec_nw_beacon_all_net(1);
    return 1;

}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshSendClProxyBv03(JNIEnv *env, jobject j, jint adr) {

    LOGD("mesh_send_cl_proxy_bv03: %d", adr);


    return mesh_send_cl_proxy_bv03(adr);

}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshSendClProxyBv04(JNIEnv *env, jobject j, jint adr) {

    LOGD("mesh_send_cl_proxy_bv04: %d", adr);


    return mesh_send_cl_proxy_bv04(adr);

}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshSendClProxyBv05(JNIEnv *env, jobject j, jint adr) {

    LOGD("mesh_send_cl_proxy_bv05: %d", adr);


    return mesh_send_cl_proxy_bv05(adr);

}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshSendClProxyBv06(JNIEnv *env, jobject j, jint adr) {

    LOGD("mesh_send_cl_proxy_bv06: %d", adr);


    return mesh_send_cl_proxy_bv06(adr);

}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshSendClProxyBv07(JNIEnv *env, jobject j, jint adr) {

    LOGD("mesh_send_cl_proxy_bv07: %d", adr);


    return mesh_send_cl_proxy_bv07(adr);

}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshConfigModeParaSet(JNIEnv *env, jobject j, jint apk_idx,
                                                           jbyteArray data, jint unicast,
                                                           jint netKeyIndex, jint fastMode) {

    LOGD("meshConfigModeParaSet");
    u8 *p;
    u16 len = 16;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, data, 0, len, (jbyte *) p);


//    write_no_rsps_pkts(p, len, PROXY_WRITE_HANDLER, 0);
    // 0 normal, 1 fast
    mesh_kc_cfgcl_mode_para_set((u16) apk_idx, p, (u16) unicast, (u16) netKeyIndex, (u8) fastMode);
    free(p);
}


JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_onDeviceDisconnect(JNIEnv *env, jobject j) {

    LOGD("master_terminate_ble_callback");

    master_terminate_ble_callback();

}


JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSetScheduler(JNIEnv *env, jobject j, jint addr,
                                                     jint respMax,
                                                     jlong schedulerParam, jint sceneId, jint ack) {

    LOGD("cmdSetScheduler");
//    u16 adr, u32 rsp_max, scheduler_t *p_schd, int ack
//    u8 *p = malloc(sizeof(scheduler_t));
    u8 p[sizeof(scheduler_t)] = {0};
    memcpy(p, &schedulerParam, 8);
    memcpy(p + 8, &sceneId, 2);
    return access_cmd_schd_action_set((u16) addr, (u32) respMax, (scheduler_t *) p, ack);
}


JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdGetScheduler(JNIEnv *env, jobject j, jint addr,
                                                     jint rspMax, jbyte index) {
    LOGD("cmdGetScheduler");
    return access_cmd_schd_action_get((u16) addr, (u32) rspMax, (u8) index);
}

// int access_cmd_time_set(u16 adr, u32 rsp_max, time_status_t *p_set);
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSetTime(JNIEnv *env, jobject j, jint addr,
                                                jint rspMax,
                                                jlong currentTime, jint zoneOffset) {

    LOGD("cmdSetTime");
    u8 p[sizeof(time_status_t)] = {0};
    memcpy(p, &currentTime, 4);
    memcpy(p + 9, &zoneOffset, 1);
    LOG_MSG_DBG (TL_LOG_COMMON, p, sizeof(time_status_t), "time ------- cmd:\r\n", 0);
    return access_cmd_time_set((u16) addr, (u32) rspMax, (time_status_t *) p);
}

// (u16 adr_dst, u32 rsp_max, u16 id, int ack);
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSceneStore(JNIEnv *env, jobject j, jint addr,
                                                   jint respMax,
                                                   jint id, jint ack) {

    LOGD("cmdSceneStore");
    return access_cmd_scene_store((u16) addr, (u32) respMax, (u16) id, ack);
}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSceneDelete(JNIEnv *env, jobject j, jint addr,
                                                    jint respMax,
                                                    jint id, jint ack) {

    LOGD("cmdSceneDelete");
    return access_cmd_scene_del((u16) addr, (u32) respMax, (u16) id, ack);
}

// access_cmd_scene_recall(u16 adr_dst, u32 rsp_max, u16 id, int ack, transition_par_t *trs_par)
JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cmdSceneRecall(JNIEnv *env, jobject j, jint addr,
                                                    jint respMax,
                                                    jint id, jint ack, jbyteArray transition) {

    LOGD("cmdSceneRecall");
    u8 transP[sizeof(transition_par_t)] = {0};
    if (transition != NULL) {
        (*env)->GetByteArrayRegion(env, transition, 0, sizeof(transition_par_t), (jbyte *) transP);
    }
    return access_cmd_scene_recall((u16) addr, (u32) respMax, (u16) id, ack,
                                   (transition_par_t *) transP);
}


// 在java上打印数组
extern unsigned char logArray(unsigned char *p, unsigned char len) {


//    LOGD("logArray");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jbyte *jbt = (jbyte *) p;


    jbyteArray jarray = (*jniEnv)->NewByteArray(jniEnv, len);


    (*jniEnv)->SetByteArrayRegion(jniEnv, jarray, 0, len, jbt);


    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "logArray", "([B)V");

    if (mid == NULL) {
        return 0;
    }


    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, jarray);

    return 1;
}


// 发送数据包
extern unsigned char SendPkt(unsigned short handle, unsigned char *p, unsigned char len) {

    LOGD("SendPkt");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jbyte *jbt = (jbyte *) p;


    jbyteArray jarray = (*jniEnv)->NewByteArray(jniEnv, len);


    (*jniEnv)->SetByteArrayRegion(jniEnv, jarray, 0, len, jbt);

    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "sendPkt", "(I[B)V");

    if (mid == NULL) {
        return 0;
    }

    int type;
    if (handle == PROVISION_WRITE_HANDLER) {
        type = 0x10;
    } else {
        type = 0x11;
    }
    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, type, jarray); //执行方法

    return 1;
}


int onCmdRsp(unsigned char *pu, int len) {
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jbyte *jbt = (jbyte *) pu;


    jbyteArray jarray = (*jniEnv)->NewByteArray(jniEnv, len);


    (*jniEnv)->SetByteArrayRegion(jniEnv, jarray, 0, len, jbt);

    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "onCmdRsp", "([B)V");

    if (mid == NULL) {
        return 0;
    }
    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, jarray); //执行方法

    return 0;
}


// filter init complete
void onFilterInitComplete() {
    LOGD("onFilterInitComplete");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }


    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "onFilterInitComplete", "()V");


    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid);
}


u8 proxyBuf[400] = {0};
u16 proxyLen = 0;

int provisionDispatch(unsigned char *p, int len) {
    jint ret_type = provision_dispatch_direct(p + 1, (u8) (len - 1), proxyBuf, &proxyLen);
    if (ret_type == MSG_PROVISION_PDU) {
        gatt_rcv_pro_pkt_dispatch(proxyBuf, (u8) proxyLen);
    } else if (ret_type == MSG_NETWORK_PDU) {
        u8 bear[400];
        mesh_construct_adv_bear_with_nw(bear, proxyBuf, (u8) proxyLen);
        LOG_MSG_INFO(TL_LOG_MESH, bear + 1, bear[1], "MSG_NETWORK_PDU:", 0);
        app_event_handler_adv(bear + 1, ADV_FROM_GATT, 0);
    } else if (ret_type == MSG_MESH_BEACON) {

        mesh_cmd_bear_unseg_t bear;
        mesh_construct_adv_bear_with_bc((u8 *) &bear, proxyBuf, (u8) proxyLen);
        if (UNPROVISION_BEACON == bear.beacon.type) {
            check_pkt_is_unprovision_beacon(proxyBuf);
        } else if (SECURE_BEACON == bear.beacon.type) {
            mesh_rc_data_beacon_sec(&(bear.len), 0);
        }
        /*if (UNPROVISION_BEACON == buff_in[2]) {

            int checkResult = check_pkt_is_unprovision_beacon(buff_in);
            onProvisionChecked(checkResult);
        }*/
    } else if (ret_type == MSG_PROXY_CONFIG) {
        u8 proxy_buf_tmp[400 + 3];
        mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *) proxy_buf_tmp;
        p_bear->trans_par_val = TRANSMIT_DEF_PAR;
        p_bear->len = (u8) (proxyLen + 1);
        p_bear->type = MESH_ADV_TYPE_MESSAGE;

        memcpy(&p_bear->nw, proxyBuf, proxyLen);
        int err = mesh_rc_data_cfg_gatt((u8 *) p_bear);
        if (err) {
            LOGD("config err");
        } else {
            proxy_config_pdu_sr *config_pdu_sr = (proxy_config_pdu_sr *) p_bear->nw.data;
            if (((config_pdu_sr->opcode) & 0x3F) == PROXY_FILTER_STATUS &&
                config_pdu_sr->para[0] == FILTER_WHITE_LIST && config_pdu_sr->para[2] == 0x02) {
                set_pair_login_ok(1);
                onFilterInitComplete();
            }
        }

    }
}

int onIniCmdError(unsigned char *pu, int len) {
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jbyte *jbt = (jbyte *) pu;


    jbyteArray jarray = (*jniEnv)->NewByteArray(jniEnv, len);


    (*jniEnv)->SetByteArrayRegion(jniEnv, jarray, 0, len, jbt);

    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "onIniCmdError", "([B)V");

    if (mid == NULL) {
        return 0;
    }
    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, jarray); //执行方法

    return 0;
}

extern int OnAppendLog_vs(unsigned char *pu, int len) {
    LOGD("OnAppendLog_vs： %d", len);
//    logArray(pu, (unsigned char) len);
    if (len == 0) {
        return 0;
    }
    if (pu[0] == DONGLE_REPORT_SPP_DATA) {
        provisionDispatch(pu, len);
    } else if ((pu[0] & 0x7f) == MESH_CMD_RSP) {
#if MD_MESH_OTA_EN
        if (len > 9) {
            u16 op = (u16) rf_link_get_op_by_ac(pu + 7);
//            int op = ((pu[7] & 0xFF) << 8) | (pu[8] & 0xFF);
            u32 size_op = SIZE_OF_OP(op);
            if (mesh_ota_master_rx(pu + 1, op, size_op)) {
//                return 0;
            }
        }
#endif
        onCmdRsp(pu, len);
    } else if (pu[0] == 0x78) {
        onIniCmdError(pu, len);
    }
    return 1;
}

// on data get
/*extern int OnAppendLog_vs(unsigned char *pu, int len) {

//    LOGD("OnAppendLog_vs");


}*/

u16 att_mtu = 23;

//extern int gatt_write_transaction_callback(u8 *p,u16 len,u8 msg_type);
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


// time part function
extern unsigned int clock_time_vc_hw() {

    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &time1);
    u64 time = (time1.tv_nsec / 1000) + time1.tv_sec * 1000 * 1000;
    u32 result = (u32) ((time) * 32);
    return result;
}

// flash operation part
extern void flash_write_page(u32 addr, u32 len, const u8 *buf) {

    LOGD("flash_write_page");

    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jbyte *jbt = (jbyte *) buf;

    jbyteArray jarray = (*jniEnv)->NewByteArray(jniEnv, len);

    (*jniEnv)->SetByteArrayRegion(jniEnv, jarray, 0, len, jbt);

    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "flashWrite", "([BII)V");


    if (mid == NULL) {
        return;
    }


    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, jarray, addr, len);
    (*jniEnv)->DeleteLocalRef(jniEnv, jarray);

}

extern void flash_read_page(u32 addr, u32 len, u8 *buf) {

    LOGD("flash_read_page %d", addr);
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jobject byteBuf = (*jniEnv)->NewDirectByteBuffer(jniEnv, buf, len);

    if (byteBuf == NULL) {
        (*jniEnv)->DeleteLocalRef(jniEnv, byteBuf);
        return;
    }

    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        (*jniEnv)->DeleteLocalRef(jniEnv, byteBuf);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "flashRead",
                                           "(Ljava/nio/ByteBuffer;II)V");

    if (mid == NULL) {
        (*jniEnv)->DeleteLocalRef(jniEnv, byteBuf);
        return;
    }

    //执行方法
    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, byteBuf, addr, len);

    (*jniEnv)->DeleteLocalRef(jniEnv, byteBuf);
}

// provision 状态回调
extern void provision_end_callback(u8 reason) {
    LOGD("provision_end_callback: %d", reason);
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }


    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "onProvisionComplete", "(B)V");


    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, reason);

}

extern void mesh_tx_reliable_stop_report(u16 op, u16 destAdr, u32 rsp_max, u32 rsp_cnt) {
    LOGD("mesh_tx_reliable_stop_report");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "onReliableCmdStop", "(III)V");
    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, op, rsp_max, rsp_cnt);

}


extern void RefreshStatusNotifyByHw(unsigned char *p, int len) {

    LOGD("RefreshStatusNotifyByHw");
}

void LogMsgModuleDlg(char *format, ...) {

    char buff[4096];
    va_list list;
    va_start(list, format);
    vsnprintf(buff, 4096, format, list);
    va_end(list);

    LOGD("%s", buff);
}

/*int LogMsgModuleDlg_and_buf(u8 *pbuf, int len, char *log_str, char *format, va_list list) {
    return 0;
}*/
int A_log_too_long = 0;

int LogMsgModuleDlg_and_buf(u8 *pbuf, int len, char *log_str, char *format, va_list list) {
    char buff[4096], buf_tmp[1024];
    unsigned int log_len = 0;
    u32 buf_idx = 0;
    log_len = strlen(log_str);
    if (log_len > 512) {
        A_log_too_long++;
        log_len = 512;
        LOGE("log_len too long %s", log_str);
    }
    memcpy(buff, log_str, log_len);
    buf_idx += log_len;
    vsnprintf(buf_tmp, 1024, format, list);
    unsigned int len_str = 0;
    len_str = strlen(buf_tmp);
    if (len_str > 512) {
        A_log_too_long++;
        len_str = 512;
        LOGE("len_str too long ");
    }
    memcpy(buff + buf_idx, buf_tmp, len_str);
    buf_idx += len_str;
    char str[4096];
    unsigned int len_buf;
    len_buf = Bin2Text(str, pbuf, len);
    buff[buf_idx] = ':';
    memcpy(buff + buf_idx + 1, str, len_buf);
    LOGD("%s\r\n", buff);

    return 0;
}

int Bin2Text(jbyte *lpD, jbyte *lpS, int n) {
    int i = 0;
    int m = n;
    int d = 0;
    if (m > MAXB2TSIZE) m = MAXB2TSIZE;

    for (i = 0; i < m; i++) {

        lpD[d++] = arrb2t[(lpS[i] >> 4) & 15];
        lpD[d++] = arrb2t[lpS[i] & 15];
        lpD[d++] = ' ';

        if ((i & 15) == 7) lpD[d++] = ' ';
        else if ((i & 15) == 15) {
            lpD[d++] = '\r';
            lpD[d++] = '\n';
        }

    }
    //lpD[d++] = '\r';
    //lpD[d++] = '\n';
    lpD[d++] = '\0';
    return d;
}


void mesh_proxy_master_terminate_cmd() {
    LOGD("mesh_proxy_master_terminate_cmd");

    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "terminate", "()V");

    if (mid == NULL) {
        return;
    }


    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid); //执行方法

}

// bind end
int App_key_bind_end_callback(u8 event) {
    LOGD("App_key_bind_end_callback: %d", event);
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 1;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 1;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 1;
    }


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onAppKeyBindEnd", "(B)V");

    if (mid == NULL) {
        return 1;
    }


    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, event);

    return 1;

}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_getElementCnt(JNIEnv *env, jobject j, jint adr) {
    LOGD("getElementCnt: %d", adr);
    u8 cnt[1] = {0};
    gatt_get_node_ele_cnt((u16) adr, cnt);
    return *cnt;
}

// u8 json_get_node_cps_info(u16 unicast_adr,VC_node_info_t *p_node)
JNIEXPORT jbyteArray JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_getNodeInfo(JNIEnv *env, jobject j, jint adr) {

    LOGD("getNodeInfo: %d", adr);
    u8 node_info[sizeof(VC_node_info_t)] = {0};
    json_get_node_cps_info((u16) adr, (VC_node_info_t *) node_info);

    jbyte *jbt = (jbyte *) node_info;

    jbyteArray jarray = (*env)->NewByteArray(env, sizeof(VC_node_info_t));

    (*env)->SetByteArrayRegion(env, jarray, 0, sizeof(VC_node_info_t), jbt);
    return jarray;
}


JNIEXPORT int JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_sendOpByINI(JNIEnv *env, jobject j,
                                                 jbyteArray data, jint len) {

    LOGD("sendOpByINI");
    u8 *p;
//    u16 len = 16;
    p = (u8 *) malloc(len);
    memset(p, 0, len);
    (*env)->GetByteArrayRegion(env, data, 0, len, p);
    int result = SendOpByINI(p, len);
    free(p);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_isOpReliable(JNIEnv *env, jobject j,
                                                  jint opcode) {

    jboolean re = (jboolean) (get_op_rsp((u16) opcode) != STATUS_NONE);
    LOGD("isOpReliable: %d", re);
    return re;
}


JNIEXPORT int JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_resetVendorId(JNIEnv *env, jobject j,
                                                   jshort vendorId) {
    LOGD("resetVendorId");
    APP_reset_vendor_id(vendorId);
}



/******************************************************************************************
 * provision cloud interface
 *******************************************************************************************/
/**
 add at 2018/05/21
 provsion with cloud operations
 **/

/**
 *
 * @param p_pid 4 bytes
 * @param p_mac 6 bytes
 *
 * @return 0 err; 1 success
 * @deprecated
 *
 */
extern int mesh_set_prov_cloud_para(u8 *p_pid, u8 *p_mac) {

    return 1;
}

/**
 *
 * 在cloud enable
 * @param p_comfirm  16 bytes
 * @param p_comfirm_key 16 bytes
 * @param pro_random 16 bytes
 * @return 0 err; 1 success
 *
 *
 * provision 计算confirm值会调用到， 需填入计算结果到 p_comfirm，
 * 都是全局变量， 需要等待结果
 * lib会获取confirm值
 *
 */
extern int mesh_sec_prov_cloud_comfirm(u8 *p_comfirm, u8 *p_comfirm_key, u8 *pro_random) {
    LOGD("mesh_sec_prov_cloud_comfirm");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

//    jbyte *confirm = (jbyte *) p_comfirm;
    jbyteArray confirmArray = (*jniEnv)->NewByteArray(jniEnv, 16);
//    (*jniEnv)->SetByteArrayRegion(jniEnv, confirmArray, 0, 16, confirm);

    jbyte *confirm_key = (jbyte *) p_comfirm_key;
    jbyteArray confirmKeyArray = (*jniEnv)->NewByteArray(jniEnv, 16);
    (*jniEnv)->SetByteArrayRegion(jniEnv, confirmKeyArray, 0, 16, confirm_key);

    jbyte *random = (jbyte *) pro_random;
    jbyteArray randomArray = (*jniEnv)->NewByteArray(jniEnv, 16);
    (*jniEnv)->SetByteArrayRegion(jniEnv, randomArray, 0, 16, random);

    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "setProvCloudConfirm", "([B[B[B)I");

    if (mid == NULL) {
        return 0;
    }

    int result = (*jniEnv)->CallIntMethod(jniEnv, gCallbackObject, mid, confirmArray,
                                          confirmKeyArray, randomArray);

    (*jniEnv)->GetByteArrayRegion(jniEnv, confirmArray, 0, 16, (jbyte *) p_comfirm);

    (*jniEnv)->DeleteLocalRef(jniEnv, confirmArray);
    (*jniEnv)->DeleteLocalRef(jniEnv, confirmKeyArray);
    (*jniEnv)->DeleteLocalRef(jniEnv, randomArray);
    return result;
}

/**
 *
 * @param p_comfirm_key 16 bytes
 * @param p_dev_random 16 bytes
 * @param p_dev_comfirm 16 bytes
 * @return 0 err; 1 success
 *
 * 在provision random阶段check confirm值 （设备端confirm值， 与云端confirm值比较）
 * 用后两个参数计算出结果， 对比p_comfirm_key， 返回对比结果
 * 需同步状态
 */
extern int mesh_cloud_dev_comfirm_check(u8 *p_comfirm_key, u8 *p_dev_random,
                                        u8 *p_dev_comfirm) {
    LOGD("mesh_cloud_dev_comfirm_check");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jbyte *confirm_key = (jbyte *) p_comfirm_key;
    jbyteArray confirmKeyArray = (*jniEnv)->NewByteArray(jniEnv, 16);
    (*jniEnv)->SetByteArrayRegion(jniEnv, confirmKeyArray, 0, 16, confirm_key);

    jbyte *dev_random = (jbyte *) p_dev_random;
    jbyteArray devRandomArray = (*jniEnv)->NewByteArray(jniEnv, 16);
    (*jniEnv)->SetByteArrayRegion(jniEnv, devRandomArray, 0, 16, dev_random);

    jbyte *dev_confirm = (jbyte *) p_dev_comfirm;
    jbyteArray devConfirmArray = (*jniEnv)->NewByteArray(jniEnv, 16);
    (*jniEnv)->SetByteArrayRegion(jniEnv, devConfirmArray, 0, 16, dev_confirm);


    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "cloudDevConfirmCheck", "([B[B[B)I");

    if (mid == NULL) {
        return 0;
    }

    int result = (*jniEnv)->CallIntMethod(jniEnv, gCallbackObject, mid, confirmKeyArray,
                                          devRandomArray, devConfirmArray);
    (*jniEnv)->DeleteLocalRef(jniEnv, confirmKeyArray);
    (*jniEnv)->DeleteLocalRef(jniEnv, devRandomArray);
    (*jniEnv)->DeleteLocalRef(jniEnv, devConfirmArray);
    return result;
}

/**
 * should call set_gatt_provision_net_info
 * void set_gatt_provision_net_info(u8 p_netkey[16],u16*p_key_index,u8*p_flag,u8 p_ivi[4],u16*p_unicast)
 * confirm 完成后会调用到， 需要调用 set_gatt_provision_net_info
 */
void gatt_provision_net_info_callback() {
    LOGD("gatt_provision_net_info_callback");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }


    if (!gClass) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, gClass, "gattProvisionNetInfoCallback", "()V");

    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid);
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_setGattProvisionNetInfo(JNIEnv *env, jobject j,
                                                             jbyteArray networkKey,
                                                             jobject keyIndex, jobject flag,
                                                             jbyteArray ivIndex, jobject unicast) {

    LOGD("setGattProvisionNetInfo");
    u8 net_key[16] = {0};
    u8 *p_net_key = NULL;
    jint net_key_len = 16;
    if (networkKey != NULL) {

        //p_net_key = (u8 *) malloc(net_key_len);
        //memset(p_net_key, 0, net_key_len);

        p_net_key = net_key;

        (*env)->GetByteArrayRegion(env, networkKey, 0, net_key_len, p_net_key);
    }

    u16 *p_key_index = NULL;
    if (keyIndex != NULL) {
        jclass shortCls = (*env)->GetObjectClass(env, keyIndex);
        jmethodID intValue = (*env)->GetMethodID(env, shortCls, "shortValue",
                                                 "()S");
        if (intValue == NULL) {
            p_key_index = NULL;
        } else {
            jshort value = (*env)->CallShortMethod(env, keyIndex, intValue);
            p_key_index = (u16 *) &value;
            LOGD("value: %d", value);
        }
    }

    u8 *p_flag = NULL;
    if (flag != NULL) {
        jclass byteCls = (*env)->GetObjectClass(env, flag);
        jmethodID byteValue = (*env)->GetMethodID(env, byteCls, "byteValue",
                                                  "()B");
        if (byteValue != NULL) {
            jbyte value = (*env)->CallByteMethod(env, flag, byteValue);
            p_flag = (u8 *) &value;
        }
    }


    u8 *p_iv_index = NULL;
    jint iv_index_len = 4;
    if (ivIndex != NULL) {
        p_iv_index = (u8 *) malloc(iv_index_len);
        memset(p_iv_index, 0, iv_index_len);
        (*env)->GetByteArrayRegion(env, ivIndex, 0, iv_index_len, p_iv_index);
    }


    u16 *p_unicast = NULL;
    if (unicast != NULL) {
        jclass shortCls = (*env)->GetObjectClass(env, unicast);

        jmethodID shortValue = (*env)->GetMethodID(env, shortCls, "shortValue",
                                                   "()S");
        if (shortValue != NULL) {
            jshort value = (*env)->CallShortMethod(env, unicast, shortValue);
            p_unicast = (u16 *) &value;
        }
    }
    set_gatt_provision_net_info(p_net_key, p_key_index, p_flag, p_iv_index, p_unicast);

    /*free(p_net_key);
    free(p_key_index);
    free(p_flag);
    free(p_iv_index);
    free(p_unicast);*/
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_setGattProCloudEn(JNIEnv *env, jobject j, jbyte en) {

//    LOGD("setGattProCloudEn");
    set_gatt_pro_cloud_en((u8) en);
}

u16 *bindModels;
u8 modelCnt;

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_setKeyBindList(JNIEnv *env, jobject j, jshortArray models,
                                                    jbyte cnt) {

    LOGD("set modelList: %d", cnt);
    bindModels = (u16 *) (*env)->GetShortArrayElements(env, models, 0);

    /*for (int i = 0; i < cnt; i++) {
        LOGD("set modelList: %d", bindModels[i]);
    }*/

    modelCnt = (u8) cnt;
}

/**
 * set model binding white list
 * @param key_bind_list_buf
 * @param p_list_cnt
 * @param max_cnt
 * @return
 */
u8 model_need_key_bind_whitelist(u16 *key_bind_list_buf, u8 *p_list_cnt, u8 max_cnt) {

//    p_list_cnt = 0;
//    return 1;
//    LOGD("get modelList, max: %d  cnt: %d", max_cnt, modelCnt);

    *p_list_cnt = 0;

    return 1;
    /*if ((modelCnt) >= max_cnt) {
        return 0;
    }

    memcpy(key_bind_list_buf, bindModels, (size_t) (modelCnt * 2));

    *p_list_cnt = modelCnt;

//    for (int i = 0; i < modelCnt; ++i) {
//        LOGD("get modelList, buffer: %d", key_bind_list_buf[i]);
//    }

//    LOGD("get modelList, p_list_cnt: %d ", *p_list_cnt);
    return 1;*/
}

/**
 * rand
 * @return
 */
u8 win32_create_rand() {
//    LOGD("create rand");
    return (u8) rand();
}


JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_resetAppKey(JNIEnv *env, jobject j, jint appKeyIndex,
                                                 jint netKeyIndex, jbyteArray appKey) {

    LOGD("resetAppKey");

    u8 app_key_data[16] = {0};
//    jint len = (*env)->GetArrayLength(env, appKey);
    cache_init(ADR_ALL_NODES);
    (*env)->GetByteArrayRegion(env, appKey, 0, 16,
                               (jbyte *) &app_key_data);

    mesh_app_key_set(APPKEY_ADD, app_key_data, (u16) appKeyIndex, (u16) netKeyIndex, 1);
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_reattachNodes(JNIEnv *env, jobject j, jobjectArray nodes) {
    LOGD("reattachNodes");
    if (nodes == NULL) {
        for (int i = 0; i < MESH_NODE_MAX_NUM; i++) {
            memset(&VC_node_info[i], 0, sizeof(VC_node_info_t));
        }
        return;
    }

    jint len = (*env)->GetArrayLength(env, nodes);
    LOGD("reattachNodes: %d", len);

    for (int i = 0; i < MESH_NODE_MAX_NUM; i++) {
//        VC_node_info_t node_data = {0};
        if (i < len) {
            jbyteArray nodeInfo = (jbyteArray) (*env)->GetObjectArrayElement(env, nodes, i);
            (*env)->GetByteArrayRegion(env, nodeInfo, 0, sizeof(VC_node_info_t),
                                       (jbyte *) &VC_node_info[i]);
        } else {
            memset(&VC_node_info[i], 0, sizeof(VC_node_info_t));
        }
    }

    flash_write_page(FLASH_ADR_VC_NODE_INFO, sizeof(VC_node_info), (u8 *) VC_node_info);
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_cacheInit(JNIEnv *env, jobject j) {
    LOGD("cacheInit");
    cache_init(ADR_ALL_NODES);
}


JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_addNodeInfo(JNIEnv *env, jobject j, jint address,
                                                 jbyteArray nodeInfo) {
    LOGD("addNodeInfo");
    if (address == 0)return;

    int position = -1;
    for (int i = 0; i < MESH_NODE_MAX_NUM; i++) {
        if (VC_node_info[i].node_adr == address) {
            position = i;
            break;
        }
        if (VC_node_info[i].node_adr == 0 && position == -1) {
            position = i;
        }
    }

    if (position != -1) {
        u8 node_info_data[sizeof(VC_node_info_t)] = {0};
        (*env)->GetByteArrayRegion(env, nodeInfo, 0, sizeof(VC_node_info_t),
                                   (jbyte *) &node_info_data);
        memcpy(&VC_node_info[position], &node_info_data, sizeof(VC_node_info_t));
        flash_write_page(FLASH_ADR_VC_NODE_INFO, sizeof(VC_node_info), (u8 *) VC_node_info);
    }
    return;
}


JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_removeNodeInfo(JNIEnv *env, jobject j, jint address) {
    LOGD("removeNodeInfo");

    for (int i = 0; i < MESH_NODE_MAX_NUM; i++) {
        if (VC_node_info[i].node_adr == address) {
            memset(&VC_node_info[i], 0, sizeof(VC_node_info_t));
        }
    }
    flash_write_page(FLASH_ADR_VC_NODE_INFO, sizeof(VC_node_info), (u8 *) VC_node_info);
}


JNIEXPORT jbyteArray JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_getCurrentDeviceKey(JNIEnv *env, jobject j) {
    u8 dev_key[16] = {0};
    json_get_dev_key_info((u8 *) &dev_key);
    jbyteArray result;
    result = (*env)->NewByteArray(env, 16);
    (*env)->SetByteArrayRegion(env, result, 0, 16, (jbyte *) &dev_key);
    return result;

}

// 直接return 1
extern int mesh_rsp_handle_proc_win32(mesh_rc_rsp_t *p_rsp) {
    return 1;
}

// iv index 更新后， 存到本地
extern int mesh_json_update_ivi_index(u8 *p_index) {

    LOGD("onIvUpdated");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onIvUpdated", "([B)V");

    if (mid == NULL) {
        return 0;
    }

    int len = 4;
    jbyte *jbt = (jbyte *) p_index;

    jbyteArray jarray = (*jniEnv)->NewByteArray(jniEnv, len);

    (*jniEnv)->SetByteArrayRegion(jniEnv, jarray, 0, len, jbt);

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, jarray); //执行方法

    return 1;
}

/*************************************************************************************
 ** remote provision **
 **************************************************************************************/

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_initRemoteProvision(JNIEnv *env, jobject j,
                                                         jint address, jbyteArray uuid) {
    LOGD("initRemoteProvisionSetting");
    u8 par[16] = {0};
    if (uuid != NULL) {
        (*env)->GetByteArrayRegion(env, uuid, 0, 16, (jbyte *) par);
    }
    mesh_rp_start_settings((u16) address, par);
}

/**
 * remote provision
 * 获取到capability
 * 回调
 *
 */
void remote_prov_capa_sts_cb(u8 max_item, u8 active_scan) {

    LOGD("remote_prov_capa_sts_cb");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onRemoteProvisionCapabilityGet", "(II)V");

    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, max_item, active_scan); //执行方法
    return;
}

/**
 * 上报report
 * 回调
 * @param par
 * @param len
 */
void remote_prov_scan_report_cb(u8 *par, u8 len) {
    LOGD("remote_prov_scan_report_cb");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onRemoteProvisionScanReport", "([B)V");

    if (mid == NULL) {
        return;
    }

    jbyte *jbt = (jbyte *) par;

    jbyteArray jarray = (*jniEnv)->NewByteArray(jniEnv, len);

    (*jniEnv)->SetByteArrayRegion(jniEnv, jarray, 0, len, jbt);

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, jarray); //执行方法
}


/*************************************************************************************
 ** online status **
 **************************************************************************************/


JNIEXPORT jbyteArray JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_decryptOnlineStatusData(JNIEnv *env, jobject j,
                                                             jbyteArray data) {
    LOGD("decryptOnlineStatusData");

    u8 *p_data = NULL;
    int len = 0;
    if (data != NULL) {
        len = (*env)->GetArrayLength(env, data);
        p_data = (u8 *) malloc((size_t) len);
        memset(p_data, 0, (size_t) len);
        (*env)->GetByteArrayRegion(env, data, 0, len, (jbyte *) p_data);
    }

    int decResult = online_st_gatt_dec(p_data, (u8) len);
    if (decResult != 0) {
        return NULL;
    }

    jbyteArray result;
    result = (*env)->NewByteArray(env, sizeof(online_st_report_t));
    (*env)->SetByteArrayRegion(env, result, 0, sizeof(online_st_report_t), (jbyte *) p_data);

    free(p_data);
    return result;

}



/*************************************************************************************
 ** mesh OTA **
 **************************************************************************************/
/**
 * read firmware
 * @param data_out
 * @param max_len
 * @return
 */
u32 new_fw_read(u8 *data_out, u32 max_len) {

    LOGD("new_fw_read");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onMeshOTAFirmwareRequest", "(I)[B");

    if (mid == NULL) {
        return 0;
    }

    jbyteArray fwData = (*jniEnv)->CallObjectMethod(jniEnv, gCallbackObject, mid, max_len);

    if (fwData == NULL) {
        return 0;
    }
    u32 dataLen = (u32) (*jniEnv)->GetArrayLength(jniEnv, fwData);
    if (dataLen > max_len || dataLen < 48) {
        return 0;
    }

    (*jniEnv)->GetByteArrayRegion(jniEnv, fwData, 0, dataLen, (jbyte *) data_out);

    return dataLen;
}

/**
 * ota progress update
 * 0: prepare
 * 1-99: sending packets
 * 100: ota complete
 * @param bk_current current block
 * @param bk_total
 * @param percent 0-100
 */

void
APP_RefreshProgressBar(u16 bk_current, u16 bk_total, u16 chunk_cur, u16 chunk_total, u8 percent) {
    LOGD("APP_RefreshProgressBar: cur--%d total--%d per--%d", bk_current, bk_total, percent);
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onMeshOTAProgressUpdate", "(III)V");

    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, bk_current, bk_total, percent); //执行方法
    return;
}

/**
 * continue mesh ota when device disconnected previous
 * @param env
 * @param j
 * @return
 */
JNIEXPORT jboolean JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_continueMeshOta(JNIEnv *env, jobject j) {
    LOGD("continueMeshOta");
    APP_set_mesh_ota_pause_flag(0);
    return 1;
}

JNIEXPORT jboolean JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_pauseMeshOta(JNIEnv *env, jobject j) {
    LOGD("pauseMeshOta");
    APP_set_mesh_ota_pause_flag(1);
    return 1;
}

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_meshRetrieveAll(JNIEnv *env, jobject j) {
    LOGD("meshRetrieveAll");
    mesh_flash_retrieve();
}

void APP_report_mesh_ota_apply_status(u16 adr_src, fw_update_status_t *p) {
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }
    int len = sizeof(fw_update_status_t);
    jbyteArray meshData = (*jniEnv)->NewByteArray(jniEnv, len);

    (*jniEnv)->SetByteArrayRegion(jniEnv, meshData, 0, len, (const jbyte *) p);


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onMeshOtaApplyStatusNotify", "(I[B)V");

    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, adr_src, meshData);
}

u16 APP_get_GATT_connect_addr() {
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onConnectedAdrRequest", "()I");

    if (mid == NULL) {
        return 0;
    }

    int re = (*jniEnv)->CallIntMethod(jniEnv, gCallbackObject, mid);

    LOGD("onConnectedAdrRequest: %d", re);
    return (u16) re;
}

const int STORAGE_TYPE_MESH_KEY = 0x1005;
const int STORAGE_TYPE_MISC = 0x1006;
const int STORAGE_TYPE_NODE_INFO = 0x1007;
const int STORAGE_TYPE_CONFIG_MODEL = 0x1008;
const int STORAGE_TYPE_MESH_OTA = 0x1009;

int saveMeshInfo(u8 *buf, int len, int type) {
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jbyteArray meshData = (*jniEnv)->NewByteArray(jniEnv, len);   //byteSize是字符数组大小

    (*jniEnv)->SetByteArrayRegion(jniEnv, meshData, 0, len, buf);


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onMeshStorageUpdate", "([BI)V");

    if (mid == NULL) {
        return 0;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, meshData, type);

    return 1;
}

int loadMeshInfo(u8 *buf, int len, int type) {
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onMeshStorageRetrieve", "(II)[B");

    if (mid == NULL) {
        return 0;
    }

    jbyteArray fwData = (*jniEnv)->CallObjectMethod(jniEnv, gCallbackObject, mid, len, type);

    if (fwData == NULL) {
        return 0;
    }
    u32 dataLen = (u32) (*jniEnv)->GetArrayLength(jniEnv, fwData);

    /*if (type == STORAGE_TYPE_NODE_INFO && dataLen % sizeof(VC_node_info_t) == 0){

        return 1;
    }*/
    if (type != STORAGE_TYPE_NODE_INFO && dataLen != len) {
        return 0;
    }

    (*jniEnv)->GetByteArrayRegion(jniEnv, fwData, 0, dataLen, (jbyte *) buf);

    if (type == STORAGE_TYPE_MESH_KEY) {
        mesh_key_save_t *meshKeySave = (mesh_key_save_t *) buf;
        LOGD("MESHKEY");
    }

    if (type == STORAGE_TYPE_MESH_OTA) {
        model_mesh_ota_t *configS = (model_mesh_ota_t *) buf;
        LOGD("load mesh ota info: %d", sizeof(model_mesh_ota_t));
    }
    return 1;
}

/**
 * retrieve data from storage, or store data to storage
 */
/**
 *
 * @param in_out buffer
 * @param p_adr
 * @param adr_base flash address
 * @param size
 * @param flag retrieve/store
 * @return
 */


int mesh_par_retrieve_store_win32(u8 *in_out, u32 *p_adr, u32 adr_base, u32 size, u8 flag) {
//    LOGD("mesh_par_retrieve_store_win32 %x %d", adr_base, flag);
    int err = -1;
    switch (adr_base) {
//        case FLASH_ADR_LIST_CFG_S:
        case FLASH_ADR_PROVISION_CFG_S:
        case FLASH_ADR_FRIEND_SHIP:
        case FLASH_ADR_RESET_CNT:
        case FLASH_ADR_MESH_TYPE_FLAG:
        case FLASH_ADR_SW_LEVEL:
            break;
        case FLASH_ADR_MESH_KEY:
            // get the json file's net key and the dev key to update

            if (flag == MESH_PARA_STORE_VAL) {
//                saveMeshInfo(in_out, sizeof(mesh_key_save_t), STORAGE_TYPE_MESH_KEY);
                // //err = mesh_key_store_win32((mesh_key_save_t *)in_out);
            } else if (flag == MESH_PARA_RETRIEVE_VAL) {
                loadMeshInfo(in_out, sizeof(mesh_key_save_t), STORAGE_TYPE_MESH_KEY);
                err = 0;
//                err = mesh_key_retrieve_win32((mesh_key_save_t *)in_out);
            }
            break;
        case FLASH_ADR_MISC:
            // update the sno ,and move the sno to the json file part
            if (flag == MESH_PARA_STORE_VAL) {
                saveMeshInfo(in_out, sizeof(misc_save_t), STORAGE_TYPE_MISC);
                err = 0;
//                mesh_misc_store_win32((misc_save_t *)in_out);
            } else if (flag == MESH_PARA_RETRIEVE_VAL) {
//                err = mesh_misc_retrieve_win32((misc_save_t *)in_out);
                loadMeshInfo(in_out, sizeof(misc_save_t), STORAGE_TYPE_MISC);
                err = 0;
            }
            break;
        case FLASH_ADR_VC_NODE_INFO:
            // update the vc node info to json
            if (flag == MESH_PARA_STORE_VAL) {
                // json file is the newest ,no need to change by the store part .
            } else if (flag == MESH_PARA_RETRIEVE_VAL) {
//                err = update_VC_info_from_json(netidx ,(VC_node_info_t *)in_out);
                loadMeshInfo(in_out, 0, STORAGE_TYPE_NODE_INFO);
                err = 0;
            }
            break;
            // model proc part
        case FLASH_ADR_MD_CFG_S:
            if (flag == MESH_PARA_RETRIEVE_VAL) {
//                err = mesh_model_cfg_retrieve_win32((model_sig_cfg_s_t *) in_out);

                loadMeshInfo(in_out, sizeof(model_sig_cfg_s_t), STORAGE_TYPE_CONFIG_MODEL);
                err = 0;
            }
            break;

        case FLASH_ADR_MD_MESH_OTA:
            if (flag == MESH_PARA_RETRIEVE_VAL) {
                // model_mesh_ota_t: 1156
                // model_common_t
                loadMeshInfo(in_out, sizeof(model_mesh_ota_t), STORAGE_TYPE_MESH_OTA);
                err = 0;
//                err = mesh_model_mesh_ota_win32((model_mesh_ota_t *) in_out);
            }
            break;
            /*case FLASH_ADR_MD_HEALTH:
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
                break;*/
        default:
            break;
    }
    return err;
}


/*
 * mesh heart beat callback
 */
void mesh_heartbeat_cb_data(u16 src, u16 dst, u8 *p_hb) {
    // len = sizeof(mesh_hb_msg_t)+1
    LOGD("mesh_heartbeat_cb_data");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jbyteArray hbData = (*jniEnv)->NewByteArray(jniEnv,
                                                sizeof(mesh_hb_msg_t) + 1);   //byteSize是字符数组大小

    (*jniEnv)->SetByteArrayRegion(jniEnv, hbData, 0, sizeof(mesh_hb_msg_t) + 1,
                                  (const jbyte *) p_hb);


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onHeartbeatNotify", "(II[B)V");

    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, src, dst, hbData);

    /**
     void mesh_heartbeat_cb_data(u16 src, u16 dst,u8 *p_hb)
{
    LOG_MSG_INFO(TL_LOG_COMMON,p_hb,sizeof(mesh_hb_msg_t)+1,
        "heartbeat src adr is 0x%04x,dst adr is 0x%04x",src,dst);
}
     */
}

/**
 * @return if provisioner is provisioned
 */
u8 win32_proved_state() {
//    LOGD("win32_proved_state");
    return 1;
}


JNIEXPORT jbyteArray JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_getLocalCpsData(JNIEnv *env, jobject j) {
    LOGD("getLocalCpsData");

    int len = 384;
    jbyteArray result = (*env)->NewByteArray(env, len);
    (*env)->SetByteArrayRegion(env, result, 0, len, (jbyte *) gp_page0);

    return result;

}

JNIEXPORT jint JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_getCallbackLen(JNIEnv *env, jobject obj) {
    return sizeof(cb_pub_st_t);
}

JNIEXPORT jbyteArray JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_getConfigInfo(JNIEnv *env, jobject obj) {
    int len = sizeof(model_sig_cfg_s_t);
    jbyteArray result = (*env)->NewByteArray(env, len);
    (*env)->SetByteArrayRegion(env, result, 0, len, (jbyte *) &model_sig_cfg_s);
    return result;
}


/*************************************************************************************
 ** fast provision **
 **************************************************************************************/

/***
* fast provision flow
*/

// mesh_fast_prov_start

// get_win32_prov_unicast_adr, adr

// mesh_fast_prov_node_info_callback, save info

// app_bind_key_end

JNIEXPORT void JNICALL
Java_com_telink_sig_mesh_lib_MeshLib_startFastProv(JNIEnv *env, jobject obj, jint pid) {
    mesh_fast_prov_start((u16) pid);
}


// callback when provision unicast address required
u16 get_win32_prov_unicast_adr() {
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return 0;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return 0;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onFastProvAdrRequest", "()I");

    if (mid == NULL) {
        return 0;
    }

    int re = (*jniEnv)->CallIntMethod(jniEnv, gCallbackObject, mid);

    LOGD("onFastProvAdrRequest: %d", re);
    return (u16) re;
}

// callback when provision unicast address used, implementation is not needed
int set_win32_prov_unicast_adr(u16 adr) {

}


void mesh_fast_prov_node_info_callback(u8 *dev_key, u16 node_addr, u16 pid) {
    LOGD("mesh_fast_prov_node_info_callback");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    jbyteArray devKeyData = (*jniEnv)->NewByteArray(jniEnv, 16);

    (*jniEnv)->SetByteArrayRegion(jniEnv, devKeyData, 0, 16,
                                  (const jbyte *) dev_key);


    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onFastProvNodeInfoCallback", "([BII)V");

    if (mid == NULL) {
        return;
    }

    (*jniEnv)->CallVoidMethod(jniEnv, gCallbackObject, mid, devKeyData, node_addr, pid);
}

u8 mesh_fast_prov_get_ele_cnt_callback(u16 pid) {
    LOGD("mesh_fast_prov_get_ele_cnt_callback");
    int status;
    JNIEnv *jniEnv;
    int isAttached = 0;

    if (!gCallbackObject) return -1;

    if ((status = (*gJavaVM)->GetEnv(gJavaVM, (void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
        if ((status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &jniEnv, NULL)) < 0) {
            return -1;
        }
        isAttached = 1;
    }

    jclass cls = (*jniEnv)->GetObjectClass(jniEnv, gCallbackObject);
    if (!cls) {
        if (isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return -1;
    }

    jmethodID mid = (*jniEnv)->GetMethodID(jniEnv, cls, "onFastProvEleCntRequire", "(I)I");

    if (mid == NULL) {
        return -1;
    }

    u8 re = (u8) (*jniEnv)->CallIntMethod(jniEnv, gCallbackObject, mid, pid);
    LOGD("fast provision element cnt: %d", re);
    return re;
}

