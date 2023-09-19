/********************************************************************************************************
 * @file	Sig_mesh_json_info.cpp
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "Sig_mesh_json_info.h"
#include "./lib_file/gatt_provision.h"
#include "rapid_json_interface.h"
extern unsigned char vc_uuid[16];
sig_mesh_json_database json_database;
sig_mesh_json_database json_database_check;
sig_mesh_json_database_static json_db_static;
int node_idx =0;
provison_net_info_str net_info_str;

int database_normal_to_static()
{
	//cpy all the data from the file 
	memcpy(&json_db_static,&json_database,sizeof(json_db_static)-MAX_MESH_NODE_NUM*(sizeof(mesh_node_static_str)));
	//sepcial proc for the node part 
	// set if it have the ivi information part .
	#if JSON_IVI_INDEX_ENABLE
	memcpy(json_db_static.ivi_idx,json_database.ivi_idx,4);
	#endif
	for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_static_str *p_staic_node = &(json_db_static.nodes[i]);
		mesh_node_str *p_node = &(json_database.nodes[i]);
		memcpy(p_staic_node,p_node,sizeof(mesh_node_static_str));
	}
	return 1;
}

int database_static_to_normal()
{
	u32 sno = json_use_uuid_to_get_sno(vc_uuid); // get the sno of the provisioner first 
	memcpy(&json_database,&json_db_static,sizeof(json_db_static)-MAX_MESH_NODE_NUM*(sizeof(mesh_node_static_str)));
	// get the provisioner's sno 
	
	for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_static_str *p_staic_node = &(json_db_static.nodes[i]);
		mesh_node_str *p_node = &(json_database.nodes[i]);
		memcpy(p_node,p_staic_node,sizeof(mesh_node_static_str));
	}
	#if JSON_IVI_INDEX_ENABLE
	memcpy(json_database.ivi_idx,json_db_static.ivi_idx,4);
	#endif
	json_use_uuid_to_set_sno(vc_uuid,sno);
	write_json_file_doc(FILE_MESH_DATA_BASE);//write the file into the json file  
	return 1;
}

u16 json_use_uuid_to_get_unicast(u8 *p_uuid)
{
	int node_idx = provision_find_json_node_by_uuid(p_uuid);
	if(node_idx>=0){
		return json_database.nodes[node_idx].unicastAddress;
	}else{
		return 0;
	}
}

u32 json_use_uuid_to_get_sno(u8 *p_uuid) 
{
	int node_idx = provision_find_json_node_by_uuid(p_uuid);
	return json_database.nodes[node_idx].sno;
}
u32 json_use_uuid_to_set_sno(u8 *p_uuid,u32 sno)
{
	int node_idx = provision_find_json_node_by_uuid(p_uuid);
	json_database.nodes[node_idx].sno = sno;
	return 1;
}

int json_add_net_idx_find(mesh_json_netkeys_str *p_net,u16 netkey_idx)
{
    // find whether it has the same net idx 
	for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
		if(p_net[i].valid){
			if((u16)(p_net[i].index) == (netkey_idx)){
				return -2;
			}	
		}
	}
	// find a blank idx 
	for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
		if(!p_net[i].valid){
			return i;
		}
	}
	return -1;
}

int json_add_network_info(provison_net_info_str *p_pro)
{
	//set the net info 
	int net_idx;
	mesh_json_netkeys_str *p_net = json_database.netKeys;
	net_idx = json_add_net_idx_find(p_net,net_info_str.key_index);
	if(net_idx < 0){
		return  -1;
	}
	memcpy(json_database.ivi_idx,p_pro->iv_index,sizeof(p_pro->iv_index));
	p_net = &(json_database.netKeys[net_idx]);
	p_net->valid =1;
	p_net->index = p_pro->key_index;
	char sec_level[] = "high";
	memcpy(p_net->minSecurity,sec_level,sizeof(sec_level));
	memcpy(p_net->key,p_pro->net_work_key,sizeof(p_pro->net_work_key));
	p_net->phase =0;
	return 1;
}
int json_node_idx_find(u16 unicast)
{
	
	for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_node = &(json_database.nodes[i]);
		if(p_node->valid){
			if(p_node->unicastAddress == unicast){
				return i;
			}
		}
	}
	return -1;
}

int json_find_first_empty_node_idx()
{
	
	for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_node = &(json_database.nodes[i]);
		if(!p_node->valid){
			return i;
		}
	}
	return -1;
}


int json_add_node_info(provison_net_info_str *p_pro ,u8 *p_uuid,u8* p_mac,u8 *p_devkey)
{
	node_idx = json_node_idx_find(p_pro->unicast_address);
	if(node_idx < 0){
	// need to find an empty node info 
	    node_idx = json_find_first_empty_node_idx();
	    if(node_idx<0){
			LOG_MSG_INFO(TL_LOG_WIN32,0,0,"find empty node info ",0);
            return -1;
	    }
	}
	mesh_node_str *p_node = &(json_database.nodes[node_idx]);
	p_node->valid =1;
	memcpy(p_node->macAddress,p_mac,6);
	p_node->unicastAddress = p_pro->unicast_address;
    memcpy(p_node->uuid,p_uuid,sizeof(p_node->uuid));
    // set the node information initial 
    p_node->configComplete =1;
    p_node->secureNetworkBeacon =1;
    p_node->defaultTTL = TTL_DEFAULT;
    p_node->networkTransmit.count = TRANSMIT_CNT_DEF;
    p_node->networkTransmit.interval = TRANSMIT_INVL_STEPS_DEF;
    p_node->relayRetransmit.count = TRANSMIT_CNT_DEF_RELAY;
    p_node->relayRetransmit.interval = TRANSMIT_INVL_STEPS_DEF_RELAY;
    p_node->excluded =0;

    // set the network information 
	mesh_nodes_net_appkey_str *p_net = p_node->netkeys;
	p_net->valid =1;
	p_net->index = p_pro->key_index;
	//get the dev key 
	memcpy(p_node->deviceKey,p_devkey,16);
	return 1;
}

int json_find_prov_index(mesh_provisioners_str *p_prov,u8 *p_uuid)
{
    int i,j;
    int valid_idx = 0;
    for(i=0;i<MAX_PROVISION_NUM;i++){
        if(p_prov[i].valid){
            if(!memcmp(p_prov->uuid,p_uuid,sizeof(p_prov->uuid))){
                return -2;
            }
        }
    }
    for(j=0;j<MAX_PROVISION_NUM;j++){
        if(!(p_prov[j].valid)){
            return j;
        }
    }
    return -1;
}

u16 json_provision_find_max_range(mesh_json_pro_UnicastRange *p_uni)
{
    u16 uni_max =0;
    for(int i=0;i<MAX_PRO_GROUP_RANGE_MAX;i++){
        if(p_uni[i].highAddress > uni_max){
            uni_max = p_uni[i].highAddress;
        }
    }
    return uni_max;

}

u32 json_get_next_range_unicast(u32 unicast)
{
	unicast = unicast /(JSON_PROV_UNICAST_RANGE +1);
	unicast = (unicast +1)*(JSON_PROV_UNICAST_RANGE +1);
	return unicast;
}

u16 json_provisioner_find_range_start(mesh_provisioners_str *p_prov)
{
    u16 unicast_max =0;
    for(int i=0;i<MAX_PROVISION_NUM;i++){
        if(p_prov[i].valid){
            u16 uni_single_max = json_provision_find_max_range(p_prov[i].unicastRange);
            if(uni_single_max > unicast_max){
                unicast_max = uni_single_max;
            }
        }
    }
    if(unicast_max == 0){
        return 1;
    }else{
       return json_get_next_range_unicast(unicast_max);

    }
}

int json_add_provisioner_info(provison_net_info_str *p_net_info,u8 *p_uuid)
{
    char provisioner_name[] = "Telink provisioner";
    mesh_provisioners_str *p_prov = json_database.provisioners;
    int prov_idx = json_find_prov_index(p_prov,p_uuid);
    // find the same provisioner and change the information 
    if(prov_idx >= 0){
        mesh_provisioners_str *p_prov_valid = &(p_prov[prov_idx]);
        p_prov_valid->valid =1;
        
        mesh_json_pro_UnicastRange *p_unicast_rang = p_prov_valid->unicastRange;
        p_unicast_rang->lowAddress = json_provisioner_find_range_start(p_prov);

		if(p_unicast_rang->lowAddress == 1){
			p_unicast_rang->highAddress = JSON_PROV_UNICAST_RANGE;
		}else{
			p_unicast_rang->highAddress = p_unicast_rang->lowAddress + JSON_PROV_UNICAST_RANGE;
		}
        memcpy(p_prov_valid->uuid,p_uuid,sizeof(p_prov_valid->uuid));
        mesh_json_pro_GroupRange *p_grp_range = p_prov_valid->groupRange;
        p_grp_range->lowAddress = 0xc000;
        p_grp_range->highAddress = 0xc0ff;
        mesh_json_pro_SceneRange *p_scene_range = p_prov_valid->sceneRange;
        p_scene_range->firstScene = 1;
        p_scene_range->lastScene = 15;
        memcpy(p_prov_valid->provisionerName,provisioner_name,sizeof(provisioner_name));
    }
    return 1;
}
extern unsigned char ble_moudle_id_is_gateway();
extern unsigned char gw_dev_key[16];
int json_add_net_info_doc(u8 *p_guid,u8 *p_mac)
{
	//get the net information part 
	provison_net_info_str *net_info = &net_info_str;
	json_get_net_info(net_info->net_work_key,&(net_info->key_index),
					net_info->iv_index,&(net_info->flags),&(net_info->unicast_address));
	// set the network info data
	json_add_network_info(net_info);
	// set the node info data 
    if(ble_moudle_id_is_gateway()){
        json_add_node_info(net_info,p_guid,p_mac,gw_dev_key);
		LOG_MSG_INFO(TL_LOG_WIN32,0,0,"json_add_net_info_doc",0);
    }else{
        json_add_node_info(net_info,p_guid,p_mac,gatt_dev_key);
    }
	return 1;
}

int json_add_app_idx_find(u16 app_idx)
{
	
	for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
		mesh_json_appkeys_str *p_appidx = &(json_database.appkeys[i]);
		if(p_appidx->valid){
			if(p_appidx->index == app_idx){
				return i;
			}
		}else{
			return i;
		}
	}
	return -1;
}

int json_set_ele_info_appkeybind(mesh_node_str *p_node,VC_node_info_t *p_vc_info,u16 apk_idx)
{
    int base_model= 0;
	u8 model_nums = 0;
	u8 model_numv = 0;
	mesh_element_model_id *p_ele_vc = &(p_vc_info->cps.model_id);
	u8 *p_ele_buf ;
	p_ele_buf = &(p_ele_vc->id[0]);
	model_nums = p_ele_vc->nums;
	model_numv = p_ele_vc->numv;
	p_node->appkeys[0].valid = 1;
	p_node->appkeys[0].index = apk_idx;
	for(int i=0;i<p_vc_info->element_cnt;i++){
		mesh_nodes_ele_str *p_ele = &(p_node->elements[i]);
		if(!p_ele->valid){
            break;
		}
		for(int k=0;k<model_nums;k++){
			mesh_nodes_model_str *p_model = &(p_ele->models[k]);
			if(p_model->valid){
			    if(p_model->bind[0].valid){
                    break;
			    }
                p_model->bind[0].valid = 1;
                p_model->bind[0].bind  = apk_idx;
			}
		}
		base_model += model_nums*2;
		for(int j=0;j<model_numv;j++){
			mesh_nodes_model_str *p_model = &(p_ele->models[j+model_nums]);
			if(p_model->valid){
			     if(p_model->bind[0].valid){
                    break;
			    }
                p_model->bind[0].valid = 1;
                p_model->bind[0].bind  = apk_idx;
			}	
		}
		base_model += model_numv*4;
		//set the nums and numv for the next element 
		model_nums = p_ele_buf[base_model+2];
		model_numv = p_ele_buf[base_model+3];
		base_model+=4;// loc ,nums ,numv ,total 4 bytes 
	}
	return 1;
}

mesh_node_str * json_get_mesh_node(u8 *p_uuid)
{
    int node_idx = provision_find_json_node_by_uuid(p_uuid);
    if(node_idx <0){
        return NULL;
    }else{
        return &(json_database.nodes[node_idx]);
    }
}

u16 json_del_mesh_info(u8 *p_uuid)
{
     int node_idx = provision_find_json_node_by_uuid(p_uuid);
    if(node_idx <0){
        return 0;
    }else{
        mesh_node_str *p_node = &(json_database.nodes[node_idx]);
        u16 unicast;
        unicast = p_node->unicastAddress;
        memset(p_node,0,sizeof(mesh_node_str));
        write_json_file_doc(FILE_MESH_DATA_BASE);
        return unicast;
    }
}


int json_set_appkey_bind_self_proc(u8 *p_uuid,u16 appkey_idx)
{
    // use the uuid to find the 
    VC_node_info_t vc_node;
    mesh_node_str *p_node = json_get_mesh_node(p_uuid);
    if(p_node == NULL){
        return -1;
    }
	json_get_node_cps_info(p_node->unicastAddress,&vc_node);
	json_set_ele_info_appkeybind(p_node,&vc_node,appkey_idx);
	return 1;
}

int json_set_ele_info(mesh_node_str *p_node,VC_node_info_t *p_vc_info)
{
	mesh_page0_head_t *p_head = &(p_vc_info->cps.page0_head);
	// set id part 
	p_node->cid = p_head->cid;
	p_node->pid = p_head->pid;
	p_node->vid = p_head->vid;
	p_node->crpl = p_head->crpl;
	p_node->defaultTTL = TTL_DEFAULT;
	p_node->secureNetworkBeacon = SEC_NW_BC_BROADCAST_DEFAULT; // (NODE_CAN_SEND_ADV_FLAG) ? SEC_NW_BC_BROADCAST_DEFAULT : NW_BEACON_NOT_BROADCASTING;
    p_node->networkTransmit.count = TRANSMIT_CNT_DEF;
    p_node->networkTransmit.interval = TRANSMIT_INVL_STEPS_DEF;
    p_node->relayRetransmit.count = TRANSMIT_CNT_DEF_RELAY;
    p_node->relayRetransmit.interval = TRANSMIT_INVL_STEPS_DEF_RELAY;

	// set feature part 
	mesh_nodes_feature_str *p_fea = &(p_node->features);
	mesh_page_feature_t *p_vc_fea = &(p_head->feature);
	p_fea->fri = p_vc_fea->frid==FRIEND_SUPPORT_DISABLE?FRIEND_NOT_SUPPORT:FRIEND_SUPPORT_ENABLE;
	p_fea->lowpower = p_vc_fea->low_power==LPN_SUPPORT_DISABLE?LPN_NOT_SUPPORT:LPN_SUPPORT_ENABLE;
	p_fea->proxy = p_vc_fea->proxy==GATT_PROXY_SUPPORT_DISABLE?GATT_PROXY_NOT_SUPPORT:GATT_PROXY_SUPPORT_ENABLE;
	p_fea->relay = p_vc_fea->relay==RELAY_SUPPORT_DISABLE?RELAY_NOT_SUPPORT:RELAY_SUPPORT_ENABLE;

	// ele part
	int base_model= 0;
	u8 model_nums = 0;
	u8 model_numv = 0;
	mesh_element_model_id *p_ele_vc = &(p_vc_info->cps.model_id);
	u8 *p_ele_buf ;
	p_ele_buf = &(p_ele_vc->id[0]);
	model_nums = p_ele_vc->nums;
	model_numv = p_ele_vc->numv;
	for(int i=0;i<p_vc_info->element_cnt;i++){
		mesh_nodes_ele_str *p_ele = &(p_node->elements[i]);
		p_ele->valid =1;
		p_ele->index =i;
		for(int k=0;k<model_nums;k++){
			mesh_nodes_model_str *p_model = &(p_ele->models[k]);
			p_model->valid = 1;
			p_model->vendor_flag = 0;
			p_model->modelId = *(u16 *)&(p_ele_buf[2*k+base_model]);
		}
		base_model += model_nums*2;
		for(int j=0;j<model_numv;j++){
			mesh_nodes_model_str *p_model = &(p_ele->models[j+model_nums]);
			p_model->valid =1;
			p_model->vendor_flag = 1;
			p_model->modelId = *(int *)&(p_ele_buf[base_model+4*j]);	
		}
		base_model += model_numv*4;
		//set the nums and numv for the next element 
		model_nums = p_ele_buf[base_model+2];
		model_numv = p_ele_buf[base_model+3];
		base_model+=4;// loc ,nums ,numv ,total 4 bytes 
		
	}
	return 1;
}
extern u8 json_get_node_cps_info(u16 unicast_adr,VC_node_info_t *p_node);
int json_set_model_doc(mesh_node_str *p_node)
{
	VC_node_info_t vc_node;
	json_get_node_cps_info(p_node->unicastAddress,&vc_node);
	json_set_ele_info(p_node,&vc_node);
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

u8 json_get_node_ele_cnt(mesh_node_str *p_node)
{
    u8 cnt =0;
    mesh_nodes_ele_str *p_ele = p_node->elements;
    for(int i=0;i<MAX_MESH_NODE_ELE_NUM;i++){
        if(p_ele[i].valid){
            cnt++;
        }
    }
    return cnt;
}

u8 json_unicast_is_in_range_or_not(u16 unicast,u8*p_uuid)
{
	// get the unicast adr range first 
	u32 unicast_range_low =0;
	u32 unicast_range_high =0;
	mesh_provisioners_str *p_prov = json_database.provisioners;
	for(int i=0;i<MAX_PROVISION_NUM;i++){
		p_prov = &(json_database.provisioners[i]);
		if(!memcmp(p_prov->uuid,p_uuid,16)){
			unicast_range_low = p_prov->unicastRange[0].lowAddress;
			unicast_range_high = p_prov->unicastRange[0].highAddress;
			break;
		}
	}
	if(unicast >= unicast_range_low && unicast <= unicast_range_high){
		return 1;
	}else{
		return 0;
	}
}

int json_get_next_unicast_adr_val(u8 *p_uuid)
{
    u16 unicast=0;
    u8 ele_cnt =0;
	u16 adr_ret =0;
    for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_node = &(json_database.nodes[i]);
        if(p_node->valid && p_node->unicastAddress >unicast){
			if(json_unicast_is_in_range_or_not(p_node->unicastAddress,p_uuid)){
				unicast = p_node->unicastAddress;
            	ele_cnt = json_get_node_ele_cnt(p_node);
				
			}
        }
    }
	extern u16 get_ini_unicast_max();
	adr_ret = unicast+ele_cnt;
	if(adr_ret<= get_ini_unicast_max()){
		adr_ret = get_ini_unicast_max();
	}
    return (adr_ret);
}


int  provision_find_json_node_by_uuid(u8 *p_uuid)
{
    mesh_node_str *p_node = json_database.nodes;
    for(int i=0;i<MAX_MESH_NODE_NUM;i++){
        if(p_node[i].valid&&!memcmp(p_node[i].uuid,p_uuid,sizeof(p_node[i].uuid))){
            return i;
        }
    }
    return -1;
}

int json_find_valid_prov()
{
    for(int i=0;i<MAX_PROVISION_NUM;i++){
        mesh_provisioners_str *p_prov = &json_database.provisioners[i];
        if(p_prov->valid){
            return i;
        }
    }
    return -1;
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

int  provision_self_get_new_unicast()// we should use the unicast range to get the newest range 
{
	mesh_provisioners_str *p_prov = json_database.provisioners;
	return json_provisioner_find_range_start(p_prov);
}

int  prov_get_net_info_from_json(provison_net_info_str *net_info,int *p_net_idx,int node_idx)
{
	mesh_json_netkeys_str *p_net = json_database.netKeys;
	// init find the first valid network info to use 
	int valid_idx = json_valid_netidx(p_net);
	if(valid_idx < 0){
		return 0;
	}
	*p_net_idx = p_net[valid_idx].index;
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
        return 1;
	}
	
	return 1;
}
int get_app_key_idx_by_net_idx(u16 net_idx,provision_appkey_t *p_appkey)
{
	u16 app_idx=0xffff;
	for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
		mesh_json_appkeys_str *p_appidx = &(json_database.appkeys[i]);
		if(p_appidx->valid){
			if(p_appidx->bound_netkey == net_idx){
				app_idx = i;
				mesh_json_appkeys_str *p_appidx = &(json_database.appkeys[app_idx]);
	            // set the appkey internal 
	            u8 st = mesh_app_key_set(APPKEY_ADD,p_appidx->key, 
				p_appidx->index, net_idx, 1);
				appkey_bind_all(1, p_appidx->index, 0);
				if(p_appkey!=0){
					p_appkey->apk_idx = p_appidx->index;
					memcpy(p_appkey->app_key,p_appidx->key,sizeof(p_appidx->key));
				}
				break;
			}
		}
	}
	if(app_idx == 0xffff){
		return FAILURE;
	}
	return SUCCESS;
}
int set_appkey_from_json()
{
	// use the netkey idx to get the first useful appkey 
	provison_net_info_str *net_info = &(net_info_str);
	u16 net_idx =0;
	net_idx = net_info->key_index;
	get_app_key_idx_by_net_idx(net_idx,0);
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
	p_vc_fea->frid = p_node_fea->fri ==FRIEND_NOT_SUPPORT?FRIEND_SUPPORT_DISABLE:FRIEND_SUPPORT_ENABLE ;
	p_vc_fea->low_power = p_node_fea->lowpower==LPN_NOT_SUPPORT?LPN_SUPPORT_DISABLE:LPN_SUPPORT_ENABLE;
	p_vc_fea->proxy = p_node_fea->proxy==GATT_PROXY_NOT_SUPPORT?GATT_PROXY_SUPPORT_DISABLE:GATT_PROXY_SUPPORT_ENABLE;
	p_vc_fea->relay = p_node_fea->relay==RELAY_NOT_SUPPORT?RELAY_SUPPORT_DISABLE:RELAY_SUPPORT_ENABLE;
	//get the nums and numv part 
	#if GATEWAY_300_NODES_TEST_ENABLE
	p_vc_node->element_cnt = 2;
	#else
	p_vc_node->element_cnt = update_json_ele_info_to_vc(p_node->elements,
	                &(p_vc_node->cps.model_id),&(p_vc_node->cps.len_cps));
	#endif
	return 1;

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
int find_empty_net_idx()
{
	for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
		mesh_json_netkeys_str *p_net = &(json_database.netKeys[i]);
		if(!p_net->valid){
			return i;
		}
	}
	return -1;
}

int find_empty_node_idx()
{
	for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_node = &(json_database.nodes[i]);
		if(!p_node->valid){
			return i;
		}
	}
	return -1;
}
int provision_self_to_json(provison_net_info_str *net_info,int *p_net_idx,u8 *p_guid,u8 *p_devkey)
{
	// add network info part 
	int net_idx = 0;
	u8 *p_netkey = json_get_netkey_by_idx(net_info->key_index);
	if(p_netkey!=0 &&!(memcmp(p_netkey,net_info->net_work_key,sizeof(net_info->net_work_key)))){
		net_idx = mesh_net_key_find(net_info->key_index);
	}else{
		net_idx = find_empty_net_idx();
		if(net_idx<0){
			return -1;
		}
		mesh_json_netkeys_str *p_net = &(json_database.netKeys[net_idx]);
		p_net->valid =1;
		p_net->index = net_info->key_index;
		memcpy(p_net->key,net_info->net_work_key,sizeof(net_info->net_work_key));
	}
	memcpy(json_database.ivi_idx,net_info->iv_index,sizeof(net_info->iv_index));	
	// add the provisioner node info,just need to add the node address part 
	// suppose need to provision self ,so the node net part is empty
	int node_idx =0;
	node_idx = find_empty_node_idx();
	if(node_idx<0){
		return -1;
	}
	mesh_node_str *p_node = &(json_database.nodes[node_idx]);
	p_node->valid =1;
	p_node->unicastAddress = net_info->unicast_address;
	memcpy(p_node->uuid,p_guid,sizeof(p_node->uuid));
	memcpy(p_node->deviceKey,p_devkey,sizeof(p_node->deviceKey));
	mesh_nodes_net_appkey_str *p_netkey_idx = p_node->netkeys;
	p_netkey_idx->valid =1;
	p_netkey_idx->index = net_info->key_index;
	return 1;
}

int bind_self_to_json(u16 net_idx,u8 *p_app_key,u16 app_idx)
{
	mesh_json_appkeys_str *p_appkey = &(json_database.appkeys[0]);
	if(p_appkey->valid){
		return 0;
	}
	p_appkey->valid =1;
	p_appkey->index = app_idx;
	memcpy(p_appkey->key,p_app_key,sizeof(p_appkey->key));
	return 1;
}

mesh_node_str* json_mesh_find_node(u16 unicast)
{
	
	int node_idx;
	mesh_node_str* p_node;
	node_idx = json_node_idx_find(unicast);
	if(node_idx < 0){
		return  NULL;
	}
	p_node = &(json_database.nodes[node_idx]);
	return p_node;
};
u8 * json_get_uuid(u16 src)
{
	mesh_node_str* p_node =0;
	p_node = json_mesh_find_node(src);
	if(p_node == NULL){
		return 0;
	}
    return p_node->uuid;
}

u16 json_del_mesh_vc_node_info(u8 *p_uuid)
{
    u16 unicast = json_del_mesh_info(p_uuid);
    if(unicast > 0){
        //proc the vc node info 
        del_vc_node_info_by_unicast(unicast);
    }
	return unicast;
}

void json_use_adr_to_del_info(u16 adr)
{
    u8 *p_uuid;
    p_uuid = json_get_uuid(adr);
	if(p_uuid!=0){
		json_del_mesh_vc_node_info(p_uuid);
	}
}



mesh_nodes_model_str *json_mesh_find_model(mesh_node_str* p_node,u16 ele_adr,int model_id)
{
    mesh_nodes_ele_str *p_ele;
    u8 ele_idx;
    ele_idx = ele_adr - p_node->unicastAddress;
    p_ele = &(p_node->elements[ele_idx]);
    for(int i=0;i<MAX_MODELS_IN_ELE_CNT;i++){
        mesh_nodes_model_str *p_model = &(p_ele->models[i]);
        if(p_model->valid){
            if(p_model->modelId == model_id){
                return p_model;
            }
        }
    }
    return 0;
}
u8 json_model_app_idx_find_same(mesh_nodes_model_str *p_model,u16 app_idx)
{
    mesh_model_bind_str *p_bind;
    // whether has the same appkey idx 
    for(int i=0;i<MAX_BIND_ADDRESS_NUM;i++){
        p_bind = &p_model->bind[i];
        if(p_bind->valid){
            if(p_bind->bind == app_idx){
                return i;
            }
        }
    }
    return 0;
}

u8 json_model_app_idx_find_empty(mesh_nodes_model_str *p_model,u16 app_idx)
{
    mesh_model_bind_str *p_bind;
    // whether has the same appkey idx 
    for(int i=0;i<MAX_BIND_ADDRESS_NUM;i++){
        p_bind = &p_model->bind[i];
        if(p_bind->valid == 0){
            return i;
        }
    }
    return 0;
}

u8 json_model_app_bind(mesh_nodes_model_str *p_model,u16 app_idx)
{
    u8 idx =0;
    if(!json_model_app_idx_find_same(p_model,app_idx)){
        // can not find the same app_idx
        idx = json_model_app_idx_find_empty(p_model,app_idx);
        mesh_model_bind_str *p_bind = &p_model->bind[idx];
        p_bind->valid =1;
        p_bind->bind = app_idx;
    }
    return 1;
}

u8 json_model_app_unbind(mesh_nodes_model_str *p_model,u16 app_idx)
{
    u8 idx =0;
    idx = json_model_app_idx_find_same(p_model,app_idx);
    if(idx){
        mesh_model_bind_str *p_bind = &p_model->bind[idx];
        memset(p_bind,0,sizeof(mesh_model_bind_str));
    }
    return 1;
}

u8 mesh_model_sub_set_adr_valid(u16 unicast)
{
    if(is_group_adr(unicast)||is_virtual_adr(unicast)){
        return 1;
    }else{
        return 0;
    }
}

int json_mesh_find_the_sub_same(u16 *p_sub,u16 unicast)
{
    int i=0;
    for(i=0;i<MAX_SUBSCRIBE_NUM;i++){
        if(p_sub[i]==unicast){
            return i;
        }
    }
    return -1;
}

int json_mesh_find_blank_sub(u16 *p_sub,u16 unicast)
{
    int i=0;
    for(i=0;i<MAX_SUBSCRIBE_NUM;i++){
        if(p_sub[i]== 0){
            return i;
        }
    }
    return -1;
}

u8 json_mesh_sub_add(u16 *p_sub,u16 unicast)
{
    if(json_mesh_find_the_sub_same(p_sub,unicast)<0){
        int idx =0;
        idx = json_mesh_find_blank_sub(p_sub,unicast);
        if(idx<0){
            return 0;
        }
        p_sub[idx]= unicast;
        return 1;
    }
    return 0;
}

u8 json_mesh_sub_del(u16 *p_sub,u16 unicast)
{
    int idx =0;
    idx = json_mesh_find_the_sub_same(p_sub,unicast);
    if(idx>=0){
        p_sub[idx] = 0;
        return 1;
    }
    return 0;
}

u8 json_mesh_sub_overwrite(u16 *p_sub,u16 unicast)
{
    memset(p_sub,0,2*MAX_SUBSCRIBE_NUM);
    p_sub[0]=unicast;
    return 1;
}

u8 json_mesh_del_all(u16 *p_sub,u16 unicast)
{
    memset(p_sub,0,2*MAX_SUBSCRIBE_NUM);
    return 1;
}
// get the same idx netkey .
int json_mesh_netkey_get_same_idx(u16 idx,mesh_json_netkeys_str *p_net)
{
    int net_idx =0;
    for(net_idx =0;net_idx<MAX_NETKEY_TOTAL_NUM;net_idx++){
        if(p_net[net_idx].valid){
            if(p_net[net_idx].index == idx){
                return net_idx;
            }
        }
    }
    return MAX_NETKEY_TOTAL_NUM;
}

// get the first blank netkey buf
int json_mes_netkey_get_blank_idx(u16 idx,mesh_json_netkeys_str *p_net)
{
    int net_idx =0;
    for(net_idx =0;net_idx<MAX_NETKEY_TOTAL_NUM;net_idx++){
        if(!p_net[net_idx].valid){
            return net_idx;
        }
    }
    return MAX_NETKEY_TOTAL_NUM;
}

u8 json_mesh_node_netkey_add(mesh_node_str* p_node,u16 idx)
{
    // find blank key idx 
    mesh_nodes_net_appkey_str *p_net = p_node->netkeys;
    for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
        if(!p_net->valid){
            p_net[i].valid = 1;
            p_net[i].index = idx;
            return 1;
        }
    }
    return 1;
}

u8 json_mesh_netkey_add(mesh_node_str* p_node,u16 idx,u8 *p_key)
{
    mesh_json_netkeys_str *p_netkey = json_database.netKeys;
    // net dispatch 
    memcpy(json_database.ivi_idx,iv_idx_st.cur,sizeof(iv_idx_st.cur));
    if(json_mesh_netkey_get_same_idx(idx,p_netkey) == MAX_NETKEY_TOTAL_NUM){
        int blank_idx = json_mes_netkey_get_blank_idx(idx,p_netkey);
        if(blank_idx < MAX_NETKEY_TOTAL_NUM){
            mesh_json_netkeys_str *p_dstkey = &json_database.netKeys[blank_idx];
            p_dstkey->valid =1;
            p_dstkey->index = idx;
            memcpy(p_dstkey->key,p_key,sizeof(p_dstkey->key));
        }
    }
    // node dispatch 
    json_mesh_node_netkey_add(p_node,idx);
	return 1;
}

u8 json_mesh_netkey_del(u16 idx,u8 *p_key)
{
    mesh_json_netkeys_str *p_netkey = json_database.netKeys;
    int net_idx =0;
    /* wait to be comfirm
    net_idx = json_mesh_netkey_get_same_idx(idx,p_netkey);
    if(net_idx < MAX_NETKEY_TOTAL_NUM){
        memset(&p_netkey[net_idx],0,sizeof(mesh_json_netkeys_str));
    }
    */
    // add the node proc about the netkey del
    return 1;
}

u8 json_mesh_netkey_update(u16 idx,u8 *p_key)
{
    mesh_json_netkeys_str *p_netkey = json_database.netKeys;
    int net_idx =0;
    /* wait to be comfirm 
    net_idx = json_mesh_netkey_get_same_idx(idx,p_netkey);
    if(net_idx < MAX_NETKEY_TOTAL_NUM){
        mesh_json_netkeys_str *p_netkey = &p_netkey[net_idx];
        memcpy(p_netkey->oldkey,p_key,sizeof(p_netkey->oldkey));
    }
    */
    // add the node proc about the netkey update.
    return 1;
}
int mesh_json_appkey_find_same(mesh_json_appkeys_str *p_appkey,u16 app_idx)
{
    for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
        if(p_appkey[i].valid){
            if(p_appkey[i].index == app_idx){
                return i;
            }
        }
    }
    return MAX_APPKEY_TOTAL_NUM;
}

int mesh_json_appkey_find_blank(mesh_json_appkeys_str *p_appkey,u16 app_idx)
{
    for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
        if(!p_appkey[i].valid){
            return i;
        }
    }
    return MAX_APPKEY_TOTAL_NUM;
}

u8 mesh_json_node_appkey_add(mesh_node_str* p_node,u16 app_idx)
{
    mesh_nodes_net_appkey_str *p_node_appkey = p_node->appkeys;
    for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
        if(!p_node_appkey[i].valid){
            p_node_appkey[i].valid=1;
            p_node_appkey[i].index = app_idx;
			return 1;
        }
    }
    return 1;

}

u8 mesh_json_appkey_add(mesh_node_str* p_node,u8 *app_key,u16 net_idx,u16 app_idx)
{
    mesh_json_appkeys_str *p_appkey = json_database.appkeys;
    int idx =0;
    idx = mesh_json_appkey_find_same(p_appkey,app_idx);
    if(idx == MAX_APPKEY_TOTAL_NUM){
        int appkey_idx = mesh_json_appkey_find_blank(p_appkey,app_idx);
        if(appkey_idx < MAX_APPKEY_TOTAL_NUM){
            mesh_json_appkeys_str  *p_dst_app = &json_database.appkeys[appkey_idx];
            p_dst_app->valid = 1;
            p_dst_app->bound_netkey = net_idx;
            p_dst_app->index =app_idx;
            memcpy(p_dst_app->key,app_key,sizeof(p_dst_app->key));
        }
    }
    // add the proc for the node 
    mesh_json_node_appkey_add(p_node,app_idx);
    // add the appkey str part 
    return 1;
}

u8 mesh_json_appkey_del(u8 *app_key,u16 net_idx,u16 app_idx)
{
    mesh_json_appkeys_str *p_appkey = json_database.appkeys;
    int idx =0;
    /* wait for proc the appkey table part 
    idx = mesh_json_appkey_find_same(p_appkey,app_idx);
    if(idx == MAX_APPKEY_TOTAL_NUM){
        int appkey_idx = mesh_json_appkey_find_blank(p_appkey,app_idx);
        if(appkey_idx < MAX_APPKEY_TOTAL_NUM){
            mesh_json_appkeys_str  *p_dst_app = &json_database.appkeys[appkey_idx];
            memset(p_dst_app,0,sizeof(mesh_json_appkeys_str);
        }
    }
    */
    // add the proc for the node part 
    
    return 1;
}

u8 mesh_json_appkey_update(u8 *app_key,u16 net_idx,u16 app_idx)
{
    mesh_json_appkeys_str *p_appkey = json_database.appkeys;
    /* wait for proc the appkey table part 
    int idx =0;
    idx = mesh_json_appkey_find_same(p_appkey,app_idx);
    if(idx < MAX_APPKEY_TOTAL_NUM){
        mesh_json_appkeys_str  *p_dst_app = &json_database.appkeys[idx];
        memcpy(p_dst_app->oldKey,app_key,sizeof(p_dst_app->oldKey);
        return 1;
    }
    */

    // add the proc for the node part 
    return 0;
}

int mesh_json_update_ivi_index(u8 *p_index)
{
	Document doc;
    if(json_file_exist(doc, FILE_MESH_DATA_BASE)){
        memcpy(json_database.ivi_idx,p_index,sizeof(json_database.ivi_idx));
		write_json_file_doc(FILE_MESH_DATA_BASE);
    }
    return 1;
}

u8 mesh_json_set_node_info(u16 adr,u8 *p_mac)
{
    mesh_node_str* p_node =0;
	p_node = json_mesh_find_node(adr);
	if(p_node == NULL){
		return 0;
	}
	memcpy(p_node->macAddress,p_mac,6);
	return json_set_model_doc(p_node);
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
    mesh_json_appkeys_str *p_app = json_database.appkeys;
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
    mesh_json_appkeys_str *p_app = json_database.appkeys;
    for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
        mesh_json_appkeys_str *p_app = &(json_database.appkeys[i]);
        if(p_app->valid && app_key_idx == p_app->index){
            return p_app->key;
        }
    } 
    return NULL;
}
extern mesh_node_str * json_get_mesh_node(u8 *p_uuid);
u8 json_proc_appkey_self(u16 appkey_idx,u8 *p_uuid)
{
    mesh_node_str *p_node = json_get_mesh_node(p_uuid);
    if(p_node == NULL){
        return -1;
    }
    int idx =0;
    // find the same 
    for(idx=0;idx<MAX_APPKEY_TOTAL_NUM;idx++){
        mesh_nodes_net_appkey_str *p_appkey = &p_node->appkeys[idx];
        if(p_appkey->valid && p_appkey->index == appkey_idx){
            return 1;
        }
    }
    // find the first empty 
    for(idx=0;idx<MAX_APPKEY_TOTAL_NUM;idx++){
        mesh_nodes_net_appkey_str *p_appkey = &p_node->appkeys[idx];
        if(!p_appkey->valid ){
            p_appkey->valid = 1;
            p_appkey->index = appkey_idx;
            return 1;
        }
    }
    return 0;
}

u8 *json_get_first_prov_uuid()
{
    for(int i=0;i<MAX_PROVISION_NUM;i++){
        mesh_provisioners_str *p_prov = &json_database.provisioners[i];
        if(p_prov->valid){
            return p_prov->uuid;
        }
    }
    return NULL;
}

u8 json_update_node_adr_devicekey(u16 new_adr,u16 old_adr,u8* devkey)
{
	
    for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_node = &json_database.nodes[i];
        if(p_node->valid&& p_node->unicastAddress == old_adr){
            p_node->unicastAddress = new_adr;
			memcpy(p_node->deviceKey,devkey,16);
			write_json_file_doc(FILE_MESH_DATA_BASE);
			return 1;
        }
    }	
	return 0;
}

int mesh_rsp_handle_proc_fun(mesh_rc_rsp_t *p_rsp,mesh_tx_reliable_t *p_tx_rela)
{
	u16 tx_op = p_tx_rela->mat.op;
	// rx data
	u16 rx_op = rf_link_get_op_by_ac(p_rsp->data);
	u32 size_op = SIZE_OF_OP(rx_op);
    u8 *par = p_rsp->data + size_op;
    u16 par_len = GET_PAR_LEN_FROM_RSP(p_rsp->len, size_op);
	// src and the dst is compare or not 
	if(p_tx_rela->mat.adr_src != p_rsp->dst){
		return 0;
	}
#if GATEWAY_300_NODES_TEST_ENABLE
	if(is_group_adr(p_tx_rela->mat.adr_src)) {
		return 0;
	}
#endif
	mesh_node_str* p_node =0;
	p_node = json_mesh_find_node(p_rsp->src);
	if(p_node == NULL){
		return 0;
	}
	switch(rx_op)
	{
	    case COMPOSITION_DATA_STATUS:
	        if(tx_op == COMPOSITION_DATA_GET){
                json_set_model_doc(p_node);
	        }
	        break;
	    case HEALTH_ATTENTION_STATUS:
			if(tx_op == HEALTH_ATTENTION_SET){
				// not need to change the json file 
			}
			break;
		case HEALTH_PERIOD_STATUS:
			if(tx_op == HEALTH_PERIOD_SET){

			}
			break;
		case HEARTBEAT_SUB_STATUS:
			if(tx_op == HEARTBEAT_SUB_SET){

			}
			break;
		case NODE_ID_STATUS:
			if(tx_op == NODE_ID_SET){
				
			}
			break;
		case CFG_BEACON_STATUS:
			if(tx_op == CFG_BEACON_SET){
				p_node->secureNetworkBeacon =par[0];
			}
			break;
			
		case CFG_DEFAULT_TTL_STATUS:
			if(tx_op == CFG_DEFAULT_TTL_SET){
				p_node->defaultTTL = par[0];
			}else if (tx_op == CFG_DEFAULT_TTL_GET){
				mesh_tx_en_devkey_candi(0);// clear the force candiate send status .
			}
			break;
		case CFG_FRIEND_STATUS:
			if(tx_op == CFG_FRIEND_SET){
				mesh_nodes_feature_str *p_fea = &p_node->features;
				p_fea->fri = par[0];
			}
			break;
		case CFG_GATT_PROXY_STATUS:
			if(tx_op == CFG_GATT_PROXY_SET){
				mesh_nodes_feature_str *p_fea = &p_node->features;
				p_fea->proxy = par[0];
			}
			break;
		case CFG_NW_TRANSMIT_STATUS:
			if(tx_op == CFG_NW_TRANSMIT_SET){
			    mesh_transmit_t *p_transmit = (mesh_transmit_t *)(par);
				p_node->networkTransmit.count = p_transmit->count;
				p_node->networkTransmit.interval = p_transmit->invl_steps;
			}
			break;
		case CFG_RELAY_STATUS:
			if(tx_op == CFG_RELAY_SET){
			    mesh_transmit_t *p_transmit = (mesh_transmit_t *)(par);
				p_node->relayRetransmit.count = p_transmit->count;
				p_node->relayRetransmit.interval = p_transmit->invl_steps;
			}
			break;
		case APPKEY_STATUS:
			{
				mesh_appkey_status_t *st = (mesh_appkey_status_t *)par;
				u8 *p_cmd_idx = p_tx_rela->ac_par;
				if(	(st->st != ST_SUCCESS)||
					(memcmp(st->net_app_idx,p_cmd_idx,sizeof(st->net_app_idx))) ){
					break;
				}
				// get the pointer about the json 
				u16 net_idx = GET_NETKEY_INDEX(st->net_app_idx);
                u16 app_idx = GET_APPKEY_INDEX(st->net_app_idx);
				mesh_appkey_set_t *p_set = (mesh_appkey_set_t *)(p_cmd_idx);
				if(tx_op == APPKEY_ADD){
				    mesh_json_appkey_add(p_node,p_set->appkey,net_idx,app_idx);
				}else if (tx_op == APPKEY_UPDATE){
                    mesh_json_appkey_update(p_set->appkey,net_idx,app_idx); 
				}else if (tx_op == APPKEY_DEL){
                    mesh_json_appkey_del(p_set->appkey,net_idx,app_idx);
				}
				mesh_key_retrieve();
			}
			break;
		case MODE_APP_STATUS:
			{
				// find the model pointer
				mesh_app_bind_t bind = {0};
				mesh_app_bind_t *p_bind = &bind;
				int len_st = min2(par_len-1, sizeof(bind));
				memcpy(p_bind,par+1,len_st);
				mesh_nodes_model_str *p_model;
				p_model = json_mesh_find_model(p_node,p_bind->ele_adr,p_bind->model_id);
				if(p_model==0){
                    break;
				}
				if(tx_op == MODE_APP_BIND){
					json_model_app_bind(p_model,p_bind->index);
				}else if (tx_op == MODE_APP_UNBIND){
					json_model_app_unbind(p_model,p_bind->index);
				}
			}
			break;
		case CFG_KEY_REFRESH_PHASE_STATUS:
			if(tx_op == CFG_KEY_REFRESH_PHASE_SET){
				
			}
			break;
		case CFG_MODEL_SUB_STATUS:
			{
				mesh_cfg_model_sub_set_t *p_sub_sts = (mesh_cfg_model_sub_set_t *)(par+1);
				if(!mesh_model_sub_set_adr_valid(p_sub_sts->sub_adr)){
					break;
				}
				mesh_nodes_model_str *p_model;
				p_model = json_mesh_find_model(p_node,p_sub_sts->ele_adr,p_sub_sts->model_id);
                if(p_model == 0){
                    break;
                }
				if( tx_op == CFG_MODEL_SUB_ADD || 
					tx_op ==CFG_MODEL_SUB_VIRTUAL_ADR_ADD){
					json_mesh_sub_add(p_model->subscribe ,p_sub_sts->sub_adr);
				}else if (  tx_op == CFG_MODEL_SUB_DEL || 
							tx_op == CFG_MODEL_SUB_VIRTUAL_ADR_DEL){
					json_mesh_sub_del(p_model->subscribe ,p_sub_sts->sub_adr);
				}else if (  tx_op == CFG_MODEL_SUB_OVER_WRITE ||
							tx_op == CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE){
					json_mesh_sub_overwrite(p_model->subscribe ,p_sub_sts->sub_adr);
				}else if (tx_op == CFG_MODEL_SUB_DEL_ALL){
					json_mesh_del_all(p_model->subscribe, p_sub_sts->sub_adr);
				}
			}
			break;
		case CFG_MODEL_PUB_STATUS:
			if(tx_op == CFG_MODEL_PUB_SET || tx_op == CFG_MODEL_PUB_VIRTUAL_ADR_SET){
                mesh_cfg_model_pub_set_t *p_pub_sts = (mesh_cfg_model_pub_set_t *)(par+1);
                mesh_nodes_model_str *p_model;
		        p_model = json_mesh_find_model(p_node,p_pub_sts->ele_adr,p_pub_sts->model_id);
		        if(p_model == 0){
                    break;
		        }
		        mesh_models_publish_str *p_model_pub = &(p_model->publish);
		        mesh_model_pub_par_t *p_cmd_pub = &(p_pub_sts->pub_par);
		        p_model_pub->address = p_pub_sts->pub_adr;
		        p_model_pub->count = p_cmd_pub->transmit.count;
		        p_model_pub->index = p_cmd_pub->appkey_idx;
		        p_model_pub->interval = p_cmd_pub->transmit.invl_steps;
				p_model_pub->numberOfSteps = p_cmd_pub->period.steps;
				if(p_cmd_pub->period.res == 0){
					p_model_pub->resolution = 100;
				}else if (p_cmd_pub->period.res == 1){
					p_model_pub->resolution = 1000;
				}else if (p_cmd_pub->period.res == 2){
					p_model_pub->resolution = 10000;
				}else if(p_cmd_pub->period.res == 3){
					p_model_pub->resolution = 600000;
				}else{}
		        p_model_pub->ttl = p_cmd_pub->ttl;
			}
			break;
		case NETKEY_STATUS:
			{
				mesh_netkey_status_t *p_netkey_st = (mesh_netkey_status_t *)par;
				mesh_netkey_set_t *p_netkey_cmd = (mesh_netkey_set_t *)p_tx_rela->ac_par;
                if(p_netkey_st->idx != p_netkey_cmd->idx){
                    break;
                }
				if(tx_op == NETKEY_ADD){
                    json_mesh_netkey_add(p_node,p_netkey_cmd->idx,p_netkey_cmd->key);
				}else if (tx_op == NETKEY_DEL){
                    json_mesh_netkey_del(p_netkey_cmd->idx,p_netkey_cmd->key);
				}else if (tx_op == NETKEY_UPDATE){
                    // need to update the netkey part 
                    json_mesh_netkey_update(p_netkey_cmd->idx,p_netkey_cmd->key);
				}
				mesh_key_retrieve();
			}
			break;
		
		case NODE_RESET_STATUS:
			if(tx_op == NODE_RESET){
				// clear node info 
				memset(p_node,0,sizeof(mesh_node_str));
			}
			break;
		default:
			return 1;	// nothing to handle, if not it will cause Tool interrupt when wtire json file.
			break;
	}
    write_json_file_doc(FILE_MESH_DATA_BASE);
    return 1;

}

int mesh_rsp_handle_proc_win32(mesh_rc_rsp_t *p_rsp)
{
	// tx data
	#if !JSON_FILE_ENABLE
    return 1;
	#endif
	extern unsigned char ble_moudle_id_is_gateway();
	if(ble_moudle_id_is_gateway()){
        return 1;
	}
    return mesh_rsp_handle_proc_fun(p_rsp,&mesh_tx_reliable);
}

