/********************************************************************************************************
 * @file	rapid_json_interface.cpp
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
#include "stdafx.h"
#include "afxmt.h"
#include <iostream>  
#include <string>  
#include <fstream>
#include <stdio.h>
//#include "stdafx.h"
//包含rapidjson必要头文件,rapidjson文件夹拷贝到工程目录，或者设置include路径，或者加入到工程树  
#include "rapidjson/document.h"  
#include "rapidjson/filereadstream.h"  
#include "rapidjson/filewritestream.h" 
#include "rapidjson/prettywriter.h"  
#include "rapidjson/stringbuffer.h"
#include "rapid_json_interface.h"
#include "sig_mesh_json_info.h"
#include "tl_ble_module.h"
#include "tl_ble_moduleDlg.h"
extern int Bin2Text (u8 * lpD, u8 * lpS, int n);
extern int Text2Bin(u8 * lpD, u8 * lpS);
extern int bin2text_normal(u8 * lpD, u8 * lpS, int n);
extern int bin2text_clean(u8* lpD, u8 * lpS, int n);


using namespace std;  
using namespace rapidjson;  //引入rapidjson命名空间  
const char security_char[]="secure";
const char insecurity_char[]="insecure";
char schema_char[]="http://json-schema.org/draft-04/schema#";
char id_char[]="http://www.bluetooth.com/specifications/assigned-numbers/mesh-profile/cdb-schema.json#";
char version_char[] = "1.0.0";
char time_char[] = "2018-12-23T11:45:22Z";
char meshname_char[]="telink_test_room";


int is_buf_zero_win(void *data, unsigned int len){
	u8 *p = (u8*)data;
	for(unsigned int i = 0; i < len; ++i){
		if(*p){
			return 0;
		}
		++p;
	}
	return 1;
}


#define MessageBoxFormat(m1, ...)      do {\
	CString str;\
	str.Format(__VA_ARGS__);\
	MessageBox(NULL, m1, str, MB_OK);\
}while(0)


class FileSafeRw
{
public:
	FileSafeRw(){
		filepath.Empty();
	}
	FileSafeRw(LPCTSTR filepath) : filepath(filepath) {

	}

	int get_file_length() {
		if ( FALSE == PathFileExists(filepath) ){
			return -1;
		}
		mutex.Lock();
		CFileStatus file_status;
		CFile::GetStatus( filepath, file_status );
		int file_length = (int)file_status.m_size;
		mutex.Unlock();
		return file_length;
	}

	int read_all(LPBYTE buffer, int length) {
		ASSERT(!filepath.IsEmpty());
		ASSERT(PathFileExists(filepath));

		if (FALSE == PathFileExists(filepath)) {
			return -1;
		}

		int read_len = -1;
		mutex.Lock();
		for(int i=0;i<10;i++) {
			CFile file;
			BOOL res = file.Open(filepath, CFile::modeRead);
			if (FALSE == res) {
				Sleep(200);
				continue;
			}
			file.SeekToBegin();
			read_len = (int)file.Read(buffer, length);
			file.Close();
			if (read_len > 0) {
				break;
			}
		}
		mutex.Unlock();
		return read_len;
	}

	int write_all(LPBYTE buffer, int length) {
		mutex.Lock();

		int write_len = -1;
		for(int i=0;i<10;i++) {
			CFile file;
			BOOL res = file.Open(filepath, CFile::modeWrite|CFile::modeCreate);
			if (FALSE == res) {
				Sleep(200);
				continue;
			}
			file.SeekToBegin();
			file.Write(buffer, length);
			file.Close();
			write_len = length;
			break;
		}
		mutex.Unlock();

		return write_len;
	}

private:
	static CMutex mutex;
	CString filepath;
};

CMutex FileSafeRw::mutex;


void json_save_file(Document& doc, string file_out)
{
	StringBuffer buffer;  
    PrettyWriter<StringBuffer> pretty_writer(buffer);  //PrettyWriter是格式化的json，如果是Writer则是换行空格压缩后的json  
    doc.Accept(pretty_writer);  
    //输出到文件
	FileSafeRw file(file_out.c_str());
	file.write_all((LPBYTE)buffer.GetString(), (int)(buffer.GetSize()));
}
void json_add_string(Document& doc, char *key,char *content)// test OK
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(key, allocator);
	Value dst = rapidjson::Value(content, allocator);
	doc.AddMember(src,dst,allocator); 
}
void json_add_table(Document& doc, char *num ,unsigned char *p_dat,unsigned int len )
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	//2.添加数组对象  
    Value array1(kArrayType); 
	unsigned int j;
    for(j=0;j<len;j++)  
    {  
        Value int_object(kObjectType);  
        int_object.SetInt(p_dat[j]);  
        array1.PushBack(int_object,allocator);  
    }  
    doc.AddMember(rapidjson::Value(num, allocator),array1,allocator); 
}
void add_mutex_object(Document& doc)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	//3.添加复合对象  
    Value object(kObjectType); 
	char A[]="c++";
	Value dst = rapidjson::Value(A, allocator);
    object.AddMember("language1",dst,allocator);  
    object.AddMember("language2","java",allocator);  
    doc.AddMember("language",object,allocator);  
	
}
void json_add_table_string_mutex(Document& doc)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
    //4.添加对象数组和复合对象的组合  
    Value array2(kArrayType);  
    Value object1(kObjectType);  
    object1.AddMember("hobby","drawing",allocator);  
    array2.PushBack(object1,allocator);  
    Value object2(kObjectType);  
    object2.AddMember("height",1.71,allocator);  
    array2.PushBack(object2,allocator);  
    doc.AddMember("information",array2,allocator);  
	////////////////////////////////
}
void json_modify_string(Document& doc, char * key ,string modify)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(key, allocator);
	if(doc.HasMember(src))
	{
		doc[src].SetString(modify.c_str(),modify.size(),doc.GetAllocator());
	}
	return;
}
void json_remove_string(Document& doc, char *key_src)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(key_src, allocator);
	if(doc.HasMember(src))
	{
		doc.RemoveMember(src);
	}
	//输出到文件 
	return;
}


int json_read_init(Document& doc, const string file_in)
{
	try
	{
		FileSafeRw file(file_in.c_str());

		int file_length = file.get_file_length();
		if (file_length <= 0) {
			throw FALSE;
		}

		shared_ptr<byte> file_data(new byte[file_length+1]);

		int read_len = file.read_all(file_data.get(), file_length);

		if (read_len != file_length) {
			MessageBoxFormat("Error", "File %s occupied!", file_in.c_str());
			throw FALSE;
		}

		file_data.get()[file_length] = 0;
		//解析并打印出来
		//doc.Parse<0>(str_in.c_str());
		doc.Parse<0>((char *)file_data.get(), file_length);

		if (doc.HasParseError()) {
			CString str;
			str.Format(_T("doc Parse Error Code: %d, Offset=%d"), doc.GetParseError(), doc.GetErrorOffset());
			AfxMessageBox(str);
			throw FALSE;
		}
		return 1;
	}
	catch (BOOL result)
	{
		if (!result) {
			doc.SetObject();
		}
		return 0;
	}
}
string json_read_string(Document& doc, string key_part)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(key_part.c_str(), allocator);
	return doc[src].GetString();
}
string json_read_tab_string(Document& doc, string key_par,unsigned char index) 
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(key_par.c_str(), allocator);
	Value &node_info = doc[src];
	if(node_info.IsArray()){
		Value &data=node_info[index]; 
		return data["hobby"].GetString();
	}else{
		return NULL;
	}
}
#define JSON_POINTER_U8		1
#define JSON_POINTER_U16	2
#define JSON_POINTER_U32	4
void rapid_json_get_int(u8 *p_data, u8 type ,rapidjson::Value & p_base)
{
	if(type == JSON_POINTER_U8){
		u8* tmp = p_data;
		*tmp = (u8)p_base.GetInt();
	}else if (type == JSON_POINTER_U16){
		u16 *tmp = (u16*)p_data;
		*tmp = (u16)p_base.GetInt();
	}else if (type == JSON_POINTER_U32){
		u32 *tmp = (u32*)p_data;
		*tmp = (u32)p_base.GetInt();
	}
	return ;
}
void rapid_json_get_table(u8 * p_dat ,u8 type, rapidjson::Value & p_base)
{
	for(u32 i=0;i<p_base.Size();i++){
		if(type == JSON_POINTER_U8){
			u8* tmp = p_dat;
			tmp[i] = (u8)p_base[i].GetInt();
		}else if (type == JSON_POINTER_U16){
			u16 *tmp = (u16*)p_dat;
			tmp[i] = (u16)p_base[i].GetInt();
		}else if (type == JSON_POINTER_U32){
			u32 *tmp = (u32*)p_dat;
			tmp[i] = (u32)p_base[i].GetInt();
		}
	}
	return ;
}

void rapid_json_set(int dat ,rapidjson::Value & p_base)
{
	p_base.SetInt(dat);
	return ;
}

void rapid_json_set_table(Document& doc, u8 * p_dat,u32 len, u8 type,rapidjson::Value & p_base)
{	
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	p_base.Erase(p_base.Begin(),p_base.End());
	for(u32 i=0;i<(len/type);i++){
		Value int_object(kObjectType); 
		if(type == JSON_POINTER_U8){
			int_object.SetInt((u32)(*(p_dat+i)));
			p_base.PushBack(int_object,allocator);

		}else if (type == JSON_POINTER_U16){
			u16 *p_tmp;
			p_tmp = (u16 *)(p_dat);
			int_object.SetInt((u32)(*(p_dat+i)));
			p_base.PushBack(int_object,allocator);
		}else if (type == JSON_POINTER_U32){
			u32 *p_tmp;
			p_tmp = (u32 *)(p_dat);
			int_object.SetInt((u32)(*(p_dat+i)));
			p_base.PushBack(int_object,allocator);
		}
	}
	return ;
}

void write_json_file_doc(const char *p_file)
{
	static CMutex msgCtrlTxMutex ;
	msgCtrlTxMutex.Lock();
	int i=3;
	while(i--){
		Document doc;
		if (json_read_init(doc, p_file) < 0) {
			doc.SetObject();
		}
		json_db_set_mesh_data(doc);
		json_save_file(doc, p_file);//FILE_MESH_DATA_BASE
		if(json_db_get_mesh_data_check(p_file)==1){
			//save the log file use the name by time .
            if (g_module_dlg->m_json_backup) {
			    CString log_file;
			    SYSTEMTIME stUTC;  
			    ::GetLocalTime(&stUTC);
			    log_file.Format("mesh_%d_%d_%d_%d.json",(stUTC.wHour),stUTC.wMinute,stUTC.wSecond,stUTC.wMilliseconds);
			    json_save_file(doc, log_file.GetString());
            }
			msgCtrlTxMutex.Unlock();
			return;
		}else{
			AfxMessageBox("json file write error!");
		}
	}
	msgCtrlTxMutex.Unlock();
	AfxMessageBox("the json can not be write success!");
	
}

void write_json_file_doc_static(const char *p_file)
{
	Document doc;
	json_read_init(doc, p_file);
	json_db_set_mesh_data_static(doc);
	json_save_file(doc, p_file);
}

void json_add_empty_tab(Document& doc, char *num )
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	//2.添加数组对象  
    Value array1(kArrayType); 
    doc.AddMember(rapidjson::Value(num, allocator),array1,allocator); 
}

void json_add_empty_tab_tag(Document& doc, Value & p_base,char *num)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	//2.添加数组对象  
    Value array1(kArrayType); 
    p_base.AddMember(rapidjson::Value(num, allocator),array1,allocator); 
}

void json_add_tab_groups(Document& doc)
{
    json_add_empty_tab(doc, "groups");
}

void json_add_tab_scenes(Document& doc)
{
    json_add_empty_tab(doc, "scenes");
}

void init_json_database_tag(Document& doc)
{
	doc.SetObject();
	json_add_string(doc, "$schema",schema_char);//$schema
	json_add_string(doc, "version",version_char);
	json_add_string(doc, "id",id_char);
	json_add_string(doc, "meshUUID","72C6BE40-444D-2081-BEAA-DDAD4E3CC21C");
	json_add_string(doc, "meshName","Brian and Mary's House");
	json_add_string(doc, "timestamp",time_char);
	json_add_empty_tab(doc, "partial");
	json_add_empty_tab(doc, "netKeys");
	json_add_empty_tab(doc, "appKeys");
	json_add_empty_tab(doc, "provisioners");
	json_add_empty_tab(doc, "nodes");
	json_add_tab_groups(doc);
	json_add_tab_scenes(doc);
	
	return ;
}
int json_db_get_string(Document& doc, rapidjson::Value & p_base,char * p_src,char *p_dst)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	string src_string;
	src_string = p_base[src].GetString();
	memcpy(p_dst,src_string.data(),src_string.size());
	return 1;
}

int json_db_get_string2data(Document& doc, rapidjson::Value & p_base,char * p_src,u8 *dat)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	string src_string;
	char dst_string[256];
	src_string = p_base[src].GetString();
	src_string = src_string +'\0';
	memcpy(dst_string,src_string.data(),src_string.size());
	Text2Bin(dat,(u8 *)dst_string);
	return 1;
}

int json_db_get_string2data_uuid(Document& doc, rapidjson::Value & p_base,char * p_src,u8 *dat)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	string src_string;
	char dst_string[256];
	src_string = p_base[src].GetString();
	src_string = src_string +'\0';
	src_string.erase(std::remove(src_string.begin(), src_string.end(), '-'), src_string.end());
	memcpy(dst_string,src_string.data(),src_string.size());
	Text2Bin(dat,(u8 *)dst_string);
	return 1;
}


int json_db_get_string2unicast(Document& doc, rapidjson::Value & p_base,char * p_src,u16 *p_uni)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	string src_string;
	u8 dst_char[16];
	u8 unicast[2];
	src_string = p_base[src].GetString();
	src_string = src_string +'\0';
	memcpy(dst_char,src_string.data(),src_string.size());
	Text2Bin(unicast,(u8 *)dst_char);
	*p_uni = (unicast[0]<<8)+(unicast[1]);
	return 1;
}

int json_db_get_string2int(Document& doc, rapidjson::Value & p_base,char * p_src,int *p_uni)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	string src_string;
	u8 dst_char[16];
	u8 unicast[4];
	src_string = p_base[src].GetString();
	src_string = src_string +'\0';
	memcpy(dst_char,src_string.data(),src_string.size());
	u8 len = Text2Bin(unicast,(u8 *)dst_char);
	if(len == 4){
        *p_uni = (unicast[0]<<24)+(unicast[1]<<16)+(unicast[2]<<8)+unicast[3];
        return MODEL_VENDOR_FLAG;
	}else if (len == 2){
        *p_uni = (unicast[0]<<8)+(unicast[1]);
        return MODEL_SIG_FLAG;
	}else{}
	
	return 1;
}

int json_db_get_unicast_table(Document& doc, rapidjson::Value & p_base,char * p_src,u16 *p_uni)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	rapidjson::Value & unicast_tab = p_base[src];
	for(u32 i=0;i<unicast_tab.Size();i++){
		string src_string;
		src_string.clear();
		char dst_string[10];
		memset(dst_string,0,sizeof(dst_string));
		u8 unicast[2];
		src_string = unicast_tab[i].GetString();
		memcpy(dst_string,src_string.data(),src_string.size());
		src_string = src_string +'\0';
		Text2Bin(unicast,(u8 *)dst_string);
		p_uni[i] = (unicast[0]<<8)+unicast[1];
	}
	return 1;

}

int json_db_get_boolean(Document& doc, rapidjson::Value & p_base,char * p_src,u8 *p_data)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	*p_data =p_base[src].GetBool();
	return 1;
}
int json_db_get_value(Document& doc, rapidjson::Value & p_base,char * p_src,u8 *p_data,u8 type)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value dst = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(dst)){
		return 0;
	}
	if(type ==  JSON_POINTER_U8){
		*p_data = p_base[dst].GetInt();
	}else if(type == JSON_POINTER_U16){
		int tmp_int;
		tmp_int = p_base[dst].GetInt();
		memcpy(p_data,(u8 *)(&tmp_int),2);
	}else if (type == JSON_POINTER_U32){
		int tmp_int;
		tmp_int = p_base[dst].GetInt();
		memcpy(p_data,(u8 *)(&tmp_int),4);
	}else{}
	return 1;
}

int json_db_get_pro_grp_range(Document& doc, rapidjson::Value & p_base ,
							mesh_provisioners_str *p_pro,char *p_src)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	rapidjson::Value& grp_range_object = p_base[src];
	for(u32 j=0;j<grp_range_object.Size();j++){
		mesh_json_pro_GroupRange *p_grp_range = &(p_pro->groupRange[j]);
		json_db_get_string2unicast(doc, grp_range_object[j],"highAddress",&(p_grp_range->highAddress));
		json_db_get_string2unicast(doc, grp_range_object[j],"lowAddress",&(p_grp_range->lowAddress));
	}
	return 1;
}

int json_db_get_pro_uni_range(Document& doc, rapidjson::Value & p_base ,
							mesh_provisioners_str *p_pro,char *p_src)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	rapidjson::Value& uni_range_object = p_base[src];
	for(u32 j=0;j<uni_range_object.Size();j++){
		mesh_json_pro_UnicastRange *p_uni_range = &(p_pro->unicastRange[j]);
		json_db_get_string2unicast(doc, uni_range_object[j],"highAddress",&(p_uni_range->highAddress));
		json_db_get_string2unicast(doc, uni_range_object[j],"lowAddress",&(p_uni_range->lowAddress));
	}
	return 1;
}

int json_db_get_pro_scene_range(Document& doc, rapidjson::Value & p_base ,
							mesh_provisioners_str *p_pro,char *p_src)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		return 0;
	}
	rapidjson::Value& scene_range_object = p_base[src];
	for(u32 j=0;j<scene_range_object.Size();j++){
		mesh_json_pro_SceneRange *p_scene_range = &(p_pro->sceneRange[j]);
		json_db_get_string2unicast(doc, scene_range_object[j],"firstScene",&(p_scene_range->firstScene));
        json_db_get_string2unicast(doc, scene_range_object[j],"lastScene",&(p_scene_range->lastScene));
	}

	return 1;
}
int json_db_get_provisioners_data(Document& doc, mesh_provisioners_str *p_js_pro)
{
	if(!doc.HasMember("provisioners")){
		return 0;
	}
	rapidjson::Value& pro_object = doc["provisioners"];
	for(u32 i=0;i<pro_object.Size();i++){
		mesh_provisioners_str *p_pro = &(p_js_pro[i]);
		p_pro->valid =1;
		json_db_get_string(doc, pro_object[i],"provisionerName",p_pro->provisionerName);
		json_db_get_string2data_uuid(doc, pro_object[i],"UUID",p_pro->uuid);
		json_db_get_pro_grp_range(doc, pro_object[i],p_pro,"allocatedGroupRange");
		json_db_get_pro_uni_range(doc, pro_object[i],p_pro,"allocatedUnicastRange");
		json_db_get_pro_scene_range(doc, pro_object[i],p_pro,"allocatedSceneRange");
	}
	return 1;
}

int json_db_get_netkey_data(Document& doc, mesh_json_netkeys_str * p_js_netkey)
{
	if(!doc.HasMember("netKeys")){
		return 0;
	}
	rapidjson::Value& doc_object = doc;
	rapidjson::Value& netkeys_object = doc["netKeys"];
	for(u32 i=0;i<netkeys_object.Size();i++){
		mesh_json_netkeys_str *p_netkey = &(p_js_netkey[i]);
		p_netkey->valid =1;
		json_db_get_string(doc, netkeys_object[i],"name",p_netkey->name);
		json_db_get_value(doc, netkeys_object[i],"index",(u8*)&(p_netkey->index),JSON_POINTER_U32);
		json_db_get_string2data(doc, netkeys_object[i],"key",p_netkey->key);
		json_db_get_value(doc, netkeys_object[i], "phase", &(p_netkey->phase), JSON_POINTER_U8);
		if (p_netkey->phase != 0) {
			json_db_get_string2data(doc, netkeys_object[i], "oldKey", p_netkey->oldkey);
		}
		json_db_get_string(doc, netkeys_object[i],"minSecurity",p_netkey->minSecurity);
	}
	return 1;
}

int json_db_get_ivi_idx(Document& doc, u8 *p_ivi)
{
	rapidjson::Value& doc_object = doc;
	json_db_get_string2data(doc, doc_object,"ivIndex",p_ivi);
	return 1;
}
int json_db_get_appkeys_data(Document& doc, mesh_json_appkeys_str *p_js_appkey)
{
	if(!doc.HasMember("appKeys")){
		return 0;
	}
	rapidjson::Value& appkeys_object = doc["appKeys"];
	for(u32 i=0;i<appkeys_object.Size();i++){
		mesh_json_appkeys_str *p_appkey = &(p_js_appkey[i]);
		p_appkey->valid =1;
		json_db_get_string(doc, appkeys_object[i],"name",p_appkey->name);
		json_db_get_value(doc, appkeys_object[i],"index",(u8 *)&(p_appkey->index),JSON_POINTER_U32);
		json_db_get_value(doc, appkeys_object[i],"boundNetKey",(u8 *)&(p_appkey->bound_netkey),JSON_POINTER_U32);
		json_db_get_string2data(doc, appkeys_object[i],"key",p_appkey->key);
		json_db_get_string2data(doc, appkeys_object[i],"oldKey",p_appkey->oldKey);
	}
	return 1;
}


int json_db_get_nodes_feature(Document& doc, rapidjson::Value & p_base ,
										mesh_nodes_feature_str *p_feature)
{
	if(!p_base.HasMember("features")){
		return 0;
	}
	rapidjson::Value& features_object = p_base["features"];
	json_db_get_value(doc, features_object,"relay",
					(u8 *)&(p_feature->relay),JSON_POINTER_U8);
	json_db_get_value(doc, features_object,"proxy",
					(u8 *)&(p_feature->proxy),JSON_POINTER_U8);
	json_db_get_value(doc, features_object,"friend",
					(u8 *)&(p_feature->fri),JSON_POINTER_U8);
	json_db_get_value(doc, features_object,"lowPower",
					(u8 *)&(p_feature->lowpower),JSON_POINTER_U8);
	return 1;
}

int json_db_get_period(Document& doc, rapidjson::Value & p_base ,mesh_models_publish_str *p_pub)
{
	if(!p_base.HasMember("period")){
		return 0;
	}
	rapidjson::Value& retransmit_object = p_base["period"];
	json_db_get_value(doc, retransmit_object,"numberOfSteps",(u8 *)&(p_pub->numberOfSteps),JSON_POINTER_U8);
	json_db_get_value(doc, retransmit_object,"resolution",(u8 *)&(p_pub->resolution),JSON_POINTER_U32);
	return 1;

}

int json_db_get_publish_retransmit(Document& doc, rapidjson::Value & p_base ,mesh_models_publish_str *p_pub)
{
	if(!p_base.HasMember("retransmit")){
		return 0;
	}
	rapidjson::Value& retransmit_object = p_base["retransmit"];
	json_db_get_value(doc, retransmit_object,"count",(u8 *)&(p_pub->count),JSON_POINTER_U8);
	json_db_get_value(doc, retransmit_object,"interval",(u8 *)&(p_pub->interval),JSON_POINTER_U32);
	if(!p_base.HasMember("period")){
		return 0;
	}
	return 1;
}
int json_db_get_models_publish(Document& doc, rapidjson::Value & p_base, mesh_models_publish_str *p_pub)
{
	if(!p_base.HasMember("publish")){
		return 0;
	}
	rapidjson::Value& pub_object = p_base["publish"];
	json_db_get_string2unicast(doc, pub_object,"address",&p_pub->address);	
	json_db_get_value(doc, pub_object,"index",(u8 *)&(p_pub->index),JSON_POINTER_U32);
	json_db_get_value(doc, pub_object,"ttl",(u8 *)&(p_pub->ttl),JSON_POINTER_U8);
	json_db_get_value(doc, pub_object,"credentials",(u8 *)&(p_pub->credentials),JSON_POINTER_U32);
	json_db_get_publish_retransmit(doc, pub_object,p_pub);
	json_db_get_period(doc, pub_object,p_pub);
	return 1;

}

int json_db_get_model_bind(Document& doc, rapidjson::Value & p_base,mesh_nodes_model_str *p_model)
{
	if(!p_base.HasMember("bind"))	{
		return 0;
	}
	
	rapidjson::Value& bind_object = p_base["bind"];
	
	for(u32 i=0;i<bind_object.Size();i++){
		mesh_model_bind_str *p_bind = &(p_model->bind[i]);
		p_bind->valid =1;
		p_bind->bind = bind_object[i].GetInt();
		//json_db_get_value(bind_object[i],"bind",(u8*)&(p_bind->bind),JSON_POINTER_U16);
	}
	return 1;
}

int json_db_get_ele_models(Document& doc, rapidjson::Value & p_base,mesh_nodes_ele_str *p_ele)
{
	if(!p_base.HasMember("models")){
		return 0;
	}
	rapidjson::Value& model_object = p_base["models"];
	for(u32 i=0;i<model_object.Size();i++){
		mesh_nodes_model_str *p_model = &(p_ele->models[i]);
		p_model->valid =1;
		if(json_db_get_string2int(doc, model_object[i],"modelId",&(p_model->modelId))==MODEL_VENDOR_FLAG){
            p_model->vendor_flag = 1;
		}
		json_db_get_unicast_table(doc, model_object[i],"subscribe",p_model->subscribe);
		json_db_get_model_bind(doc, model_object[i],p_model);
		json_db_get_models_publish(doc, model_object[i],&(p_model->publish));
	}
	return 1;
}

int json_db_get_nodes_ele(Document& doc, rapidjson::Value & p_base, mesh_node_str *p_node)
{
	if(!p_base.HasMember("elements")){
		return 0;
	}
	rapidjson::Value& ele_object = p_base["elements"];
	for(u32 i=0;i<ele_object.Size();i++){
		mesh_nodes_ele_str *p_ele = &(p_node->elements[i]);
		p_ele->valid =1;
		json_db_get_value(doc, ele_object[i],"index",
					(u8 *)&(p_ele->index),JSON_POINTER_U32);
		json_db_get_string(doc, ele_object[i],"name",p_ele->name);
		json_db_get_string2unicast(doc, ele_object[i],"location",&(p_ele->location));
		json_db_get_ele_models(doc, ele_object[i],p_ele);

	}
	return 1;

}
int json_db_get_node_networkTransmit(Document& doc, rapidjson::Value & p_base,
								mesh_nodes_networkTransmit_str *p_net_trans)
{
	if(!p_base.HasMember("networkTransmit")){
		return 0;
	}
	rapidjson::Value& network_trans_object = p_base["networkTransmit"];
	json_db_get_value(doc, network_trans_object,"count",(u8 *)&(p_net_trans->count),JSON_POINTER_U8);
	json_db_get_value(doc, network_trans_object,"interval",(u8 *)&(p_net_trans->interval),JSON_POINTER_U16);
	p_net_trans->interval = (p_net_trans->interval)/10-1;
	return 1;
}

int json_db_get_node_relayRetransmit(Document& doc, rapidjson::Value & p_base,
								mesh_nodes_relay_str *p_relay_trans)
{
	if(!p_base.HasMember("relayRetransmit")){
		return 0;
	}
	rapidjson::Value& relay_trans_object = p_base["relayRetransmit"];
	json_db_get_value(doc, relay_trans_object,"count",(u8 *)&(p_relay_trans->count),JSON_POINTER_U8);
	json_db_get_value(doc, relay_trans_object,"interval",(u8 *)&(p_relay_trans->interval),JSON_POINTER_U16);
	p_relay_trans->interval = (p_relay_trans->interval)/10 -1;
	return 1;
}

int json_db_get_node_appkeys(Document& doc, rapidjson::Value & p_base,mesh_node_str *p_node)
{
	if(!p_base.HasMember("appKeys")){
		return 0;
	}
	rapidjson::Value& appkeys_object = p_base["appKeys"];
	for(u32 i=0;i<appkeys_object.Size();i++){
		mesh_nodes_net_appkey_str *p_app = &(p_node->appkeys[i]);
		p_app->valid =1;
		u16 *p_index = &(p_app->index);
		json_db_get_boolean(doc, appkeys_object[i],"updated",&p_app->update);
		json_db_get_value(doc, appkeys_object[i],"index",(u8 *)p_index,JSON_POINTER_U16);
	}
	return 1;
}

int json_db_get_node_netkeys(Document& doc, rapidjson::Value & p_base,mesh_node_str *p_node)
{
	if(!p_base.HasMember("netKeys")){
		return 0;
	}
	rapidjson::Value& netkeys_object = p_base["netKeys"];
	for(u32 i=0;i<netkeys_object.Size();i++){
		mesh_nodes_net_appkey_str *p_net = &(p_node->netkeys[i]);
		p_net->valid =1;
		u16 *p_index = &(p_net->index);
		json_db_get_boolean(doc, netkeys_object[i],"updated",&p_net->update);
		json_db_get_value(doc, netkeys_object[i],"index",(u8 *)p_index,JSON_POINTER_U16);
	}
	return 1;
}

int json_db_get_nodes_data(Document& doc, mesh_node_str *p_js_node)
{
	if(!doc.HasMember("nodes")){
		return 0;
	}
	rapidjson::Value& nodes_object = doc["nodes"];
	for(u32 i=0;i<nodes_object.Size();i++){
		mesh_node_str *p_node = &(p_js_node[i]);
		p_node->valid =1;
		json_db_get_string2data_uuid(doc, nodes_object[i],"UUID",p_node->uuid);
		// mac address need to store by swap 
		json_db_get_string2data(doc, nodes_object[i],"macAddress",p_node->macAddress);
		// need to do endiness swap 
		endianness_swap_u48(p_node->macAddress);
		json_db_get_string(doc, nodes_object[i],"name",p_node->name);
		json_db_get_string2data(doc, nodes_object[i],"deviceKey",p_node->deviceKey);
		json_db_get_string2unicast(doc, nodes_object[i],"unicastAddress",&(p_node->unicastAddress));
		json_db_get_string2int(doc, nodes_object[i],"sno",(int *)&(p_node->sno));
		json_db_get_string(doc, nodes_object[i],"security",p_node->security);
		json_db_get_string2unicast(doc, nodes_object[i],"cid",&(p_node->cid));
		json_db_get_string2unicast(doc, nodes_object[i],"pid",&(p_node->pid));
		json_db_get_string2unicast(doc, nodes_object[i],"vid",&(p_node->vid));
		json_db_get_string2unicast(doc, nodes_object[i],"crpl",&(p_node->crpl));
		json_db_get_nodes_feature(doc, nodes_object[i],&(p_node->features));
		json_db_get_nodes_ele(doc, nodes_object[i], p_node);
		json_db_get_boolean(doc, nodes_object[i],"configComplete",&p_node->configComplete);
		json_db_get_boolean(doc, nodes_object[i],"secureNetworkBeacon",&p_node->secureNetworkBeacon);
		json_db_get_value(doc, nodes_object[i],"defaultTTL",(u8 *)&(p_node->defaultTTL),JSON_POINTER_U8);
		json_db_get_node_networkTransmit(doc, nodes_object[i],&p_node->networkTransmit);
		json_db_get_node_relayRetransmit(doc, nodes_object[i],&p_node->relayRetransmit);
		json_db_get_node_appkeys(doc, nodes_object[i],p_node);
		json_db_get_node_netkeys(doc, nodes_object[i],p_node);
		json_db_get_boolean(doc, nodes_object[i],"excluded",&p_node->excluded);
	}
	return 1;

}

int json_db_get_nodes_data_static(Document& doc, mesh_node_static_str *p_js_node)
{
	if(!doc.HasMember("nodes")){
		return 0;
	}
	rapidjson::Value& nodes_object = doc["nodes"];
	for(u32 i=0;i<nodes_object.Size();i++){
		mesh_node_str *p_node = (mesh_node_str *)&(p_js_node[i]);
		p_node->valid =1;
		json_db_get_string2data_uuid(doc, nodes_object[i],"UUID",p_node->uuid);
		// mac address need to store by swap 
		json_db_get_string2data(doc, nodes_object[i],"macAddress",p_node->macAddress);
		// need to do endiness swap 
		endianness_swap_u48(p_node->macAddress);
		json_db_get_string(doc, nodes_object[i],"name",p_node->name);
		json_db_get_string2data(doc, nodes_object[i],"deviceKey",p_node->deviceKey);
		json_db_get_string2unicast(doc, nodes_object[i],"unicastAddress",&(p_node->unicastAddress));
		json_db_get_string(doc, nodes_object[i],"security",p_node->security);
		json_db_get_string2unicast(doc, nodes_object[i],"cid",&(p_node->cid));
		json_db_get_string2unicast(doc, nodes_object[i],"pid",&(p_node->pid));
		json_db_get_string2unicast(doc, nodes_object[i],"vid",&(p_node->vid));
		json_db_get_string2unicast(doc, nodes_object[i],"crpl",&(p_node->crpl));
		json_db_get_nodes_feature(doc, nodes_object[i],&(p_node->features));
		json_db_get_nodes_ele(doc, nodes_object[i], p_node);
		json_db_get_boolean(doc, nodes_object[i],"configComplete",&p_node->configComplete);
		json_db_get_boolean(doc, nodes_object[i],"secureNetworkBeacon",&p_node->secureNetworkBeacon);
		json_db_get_value(doc, nodes_object[i],"defaultTTL",(u8 *)&(p_node->defaultTTL),JSON_POINTER_U8);
		json_db_get_node_networkTransmit(doc, nodes_object[i],&p_node->networkTransmit);
		json_db_get_node_relayRetransmit(doc, nodes_object[i],&p_node->relayRetransmit);
		json_db_get_node_appkeys(doc, nodes_object[i],p_node);
		json_db_get_node_netkeys(doc, nodes_object[i],p_node);
		json_db_get_boolean(doc, nodes_object[i],"excluded",&p_node->excluded);
	}
	return 1;

}



int json_db_get_groups_data(Document& doc, mesh_group_str *p_js_group)
{
	if(!doc.HasMember("groups")){
		return 0;
	}
	rapidjson::Value& groups_object = doc["groups"];
	for(u32 i=0;i<groups_object.Size();i++){
		mesh_group_str *p_group = &(p_js_group[i]);
		p_group->valid =1;
		json_db_get_string(doc, groups_object[i],"name",p_group->name);
		json_db_get_string2unicast(doc, groups_object[i],"address",&(p_group->address));
		json_db_get_string2unicast(doc, groups_object[i],"parentAddress",&(p_group->parentAddress));
	}
	return 1;
}


int json_db_get_scene_add_tab(Document& doc, rapidjson::Value & p_base,char *p_src,u16 *address)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	json_add_empty_tab_tag(doc, p_base,p_src);
	Value src = rapidjson::Value(p_src, allocator);	
	rapidjson::Value& uni_object = p_base[src];
	for(u32 i=0;i<uni_object.Size();i++){
		if(address[i]!=0){
			Value int_object(kObjectType);
			json_db_get_string2unicast(doc, int_object,"addresses",&(address[i]));
			uni_object.PushBack(int_object,allocator);
		}
	}
	return 1;
}

int json_db_get_scene_data(Document& doc, mesh_scene_str *p_js_scene)
{
	if(!doc.HasMember("scenes")){
		return 0;
	}

	rapidjson::Value& scenes_object = doc["scenes"];
	for(u32 i=0;i<scenes_object.Size();i++){
		mesh_scene_str *p_scene = &(p_js_scene[i]);
		p_scene->valid =1;
		json_db_get_string(doc, scenes_object[i],"name",p_scene->name);
		json_db_get_unicast_table(doc, scenes_object[i],"addresses",p_scene->addresses);
		json_db_get_string2unicast(doc, scenes_object[i],"number",&(p_scene->number));
	}
	return 1;
}


int json_db_get_mesh_data_check(const char *p_file)
{
	Document doc;
	int res = json_read_init(doc, p_file);
	if (res < 0) {
		MessageBoxFormat("Error", "json_read_init error error code=%d", res);
		return res;
	}
	rapidjson::Value& doc_object = doc;
	sig_mesh_json_database *p_db = &json_database_check;
	json_db_get_string(doc, doc_object,"$schema",p_db->schema);
	json_db_get_string(doc, doc_object,"version",p_db->version);
	json_db_get_string2data_uuid(doc, doc_object,"meshUUID",p_db->mesh_uuid);
	json_db_get_string(doc, doc_object,"meshName",p_db->meshName);
	json_db_get_string(doc, doc_object,"timestamp",p_db->timestamp);
	json_db_get_provisioners_data(doc, p_db->provisioners);
	json_db_get_netkey_data(doc, p_db->netKeys);
	json_db_get_appkeys_data(doc, p_db->appkeys);
	json_db_get_nodes_data(doc, p_db->nodes);
	json_db_get_groups_data(doc, p_db->groups);
	json_db_get_scene_data(doc, p_db->scene);
	if(memcmp(json_database.schema,json_database_check.schema,sizeof(json_database.schema))){
		return -1;
	}
	if(memcmp(json_database.version,json_database_check.version,sizeof(json_database.version))){
		return -2;
	}
	if(memcmp(json_database.mesh_uuid,json_database_check.mesh_uuid,sizeof(json_database.mesh_uuid))){
		return -3;
	}
	if(memcmp(json_database.meshName,json_database_check.meshName,sizeof(json_database.meshName))){
		return -4;
	}
	/*
	for(int i=0;i<MAX_PROVISION_NUM;i++){
		mesh_provisioners_str *p_pro = &json_database.provisioners[i];
		mesh_provisioners_str *p_pro_chk = &json_database_check.provisioners[i];
		if(p_pro->valid){
			if(memcmp(p_pro,p_pro_chk,sizeof(mesh_provisioners_str))){
				return -5;
			}
		}
	}
	for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
		mesh_json_netkeys_str *p_net = &json_database.netKeys[i];
		mesh_json_netkeys_str *p_net_chk = &json_database_check.netKeys[i];
		if(p_net->valid){
			if(memcmp(p_net,p_net_chk,sizeof(mesh_json_netkeys_str))){
				return -6;
			}
		}
	}
	for(int i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
		mesh_json_appkeys_str *p_app = &json_database.appkeys[i];
		mesh_json_appkeys_str *p_app_chk = &json_database_check.appkeys[i];
		if(p_app->valid){
			if(memcmp(p_app,p_app,sizeof(mesh_json_appkeys_str))){
				return -7;
			}
		}
	}
	
	for(int i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_node = &json_database.nodes[i];
		mesh_node_str *p_node_chk = &json_database_check.nodes[i];
		if(p_node->valid){
			if(memcmp(p_node,p_node_chk,sizeof(mesh_node_str))){
				return -8;
			}
		}
	}*/

	return 1;
}


int json_db_get_mesh_data(Document& doc)
{
	rapidjson::Value& doc_object = doc;
	sig_mesh_json_database *p_db = &json_database;
	json_db_get_string(doc, doc_object,"$schema",p_db->schema);
	json_db_get_string(doc, doc_object,"version",p_db->version);
	json_db_get_string2data_uuid(doc, doc_object,"meshUUID",p_db->mesh_uuid);
	json_db_get_string(doc, doc_object,"meshName",p_db->meshName);
	json_db_get_string(doc, doc_object,"timestamp",p_db->timestamp);
	json_db_get_provisioners_data(doc, p_db->provisioners);
	json_db_get_netkey_data(doc, p_db->netKeys);
	json_db_get_appkeys_data(doc, p_db->appkeys);
	json_db_get_nodes_data(doc, p_db->nodes);
	json_db_get_groups_data(doc, p_db->groups);
	json_db_get_scene_data(doc, p_db->scene);
	json_db_get_ivi_idx(doc, p_db->ivi_idx);
	return 1;
}

int json_db_get_mesh_data_static(Document& doc)
{
	rapidjson::Value& doc_object = doc;
	sig_mesh_json_database_static *p_db = &json_db_static;
	json_db_get_string(doc, doc_object,"$schema",p_db->schema);
	json_db_get_string(doc, doc_object,"version",p_db->version);
	json_db_get_string2data_uuid(doc, doc_object,"meshUUID",p_db->mesh_uuid);
	json_db_get_string(doc, doc_object,"meshName",p_db->meshName);
	json_db_get_string(doc, doc_object,"timestamp",p_db->timestamp);
	json_db_get_provisioners_data(doc, p_db->provisioners);
	json_db_get_netkey_data(doc, p_db->netKeys);
	json_db_get_appkeys_data(doc, p_db->appkeys);
	json_db_get_nodes_data_static(doc, p_db->nodes);
	json_db_get_groups_data(doc, p_db->groups);
	json_db_get_scene_data(doc, p_db->scene);
	#if JSON_IVI_INDEX_ENABLE
 	json_db_get_ivi_idx(doc, p_db->ivi_idx,p_db->ivi_en);
	#endif
	return 1;
}

int json_db_set_boolean(Document& doc, rapidjson::Value & p_base ,char * p_src, u8 dat)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		// need to add member
		if(dat!=0){
			Value bool_object(kTrueType);
			p_base.AddMember(src,bool_object,allocator);
		}else{
			Value bool_object(kFalseType);
			p_base.AddMember(src,bool_object,allocator);
		}	
		return 1;
	}
	bool flag;
	if(dat>0){
		flag =true;
	}else{
		flag =false;
	}
	p_base[src].SetBool(flag);
	return 1;
}
int json_db_set_int(Document& doc, rapidjson::Value & p_base ,char * p_src, int dat)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	if(!p_base.HasMember(src)){
		// need to add member
		p_base.AddMember(src,dat,allocator);
		return 1;
	}
	p_base[src].SetInt(dat);
	return 1;
}
int json_db_set_string(Document& doc, rapidjson::Value & p_base,char * p_src,char *p_dst)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	Value dst = rapidjson::Value(p_dst, allocator);
	if(!p_base.HasMember(src)){
		// need to add member
		p_base.AddMember(src,dst,allocator);
		return 1;
	}
	p_base[src].SetString(p_dst,strlen(p_dst),allocator);
	return 1;
}
int json_db_set_data_string(Document& doc, rapidjson::Value & p_base,char * p_src,u8 *p_dst,int len)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	char data_string[256];
	bin2text_clean((u8 *)data_string,p_dst,len);
	Value dst = rapidjson::Value(data_string, allocator);
	if(!p_base.HasMember(src)){
		// need to add member
		p_base.AddMember(src,dst,allocator);
		return 1;
	}
	p_base[src].SetString(data_string,strlen(data_string),allocator);
	return 1;
}

int json_db_set_data_string_uuid(Document& doc, rapidjson::Value & p_base,char * p_src,u8 *p_dst,int len)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	char data_string[256];
	bin2text_clean_uuid((u8 *)data_string,p_dst,len);
	Value dst = rapidjson::Value(data_string, allocator);
	if(!p_base.HasMember(src)){
		// need to add member
		p_base.AddMember(src,dst,allocator);
		return 1;
	}
	p_base[src].SetString(data_string,strlen(data_string),allocator);
	return 1;
}


void clear_table_ele(Document& doc, rapidjson::Value & p_base)
{
	if(p_base.Size()>0){
		p_base.Erase(p_base.Begin(),p_base.End());
	}
}

int json_db_set_u16_data(Document& doc, rapidjson::Value & p_base,char *p_src,u16 data)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	//data to string 
	endianness_swap_u16((u8 *)&(data));
	char data_string[256];
	bin2text_clean((u8 *)data_string,(u8 *)&data,2);
	Value dst = rapidjson::Value(data_string, allocator);
	if(!p_base.HasMember(src)){
		// need to add member
		p_base.AddMember(src,dst,allocator);
		return 1;
	}
	p_base[src].SetString(data_string,strlen(data_string),allocator);
	return 1;
}


int json_db_set_u32_data(Document& doc, rapidjson::Value & p_base,char *p_src,u32 data)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	Value src = rapidjson::Value(p_src, allocator);
	//data to string 
	endianness_swap_u32((u8 *)&(data));
	char data_string[256];
	bin2text_clean((u8 *)data_string,(u8 *)&data,4);
	Value dst = rapidjson::Value(data_string, allocator);
	if(!p_base.HasMember(src)){
		// need to add member
		p_base.AddMember(src,dst,allocator);
		return 1;
	}
	p_base[src].SetString(data_string,strlen(data_string),allocator);
	return 1;
}

int json_db_set_grp_range(Document& doc, rapidjson::Value & p_base,
							char *p_src,mesh_provisioners_str *p_pro)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	json_add_empty_tab_tag(doc, p_base,p_src);
	Value src = rapidjson::Value(p_src, allocator);
	rapidjson::Value& grp_rang_object = p_base[src];
	for(u32 i=0;i<MAX_PRO_GROUP_RANGE_MAX;i++){
		mesh_json_pro_GroupRange *p_grp = &(p_pro->groupRange[i]);
		if(p_grp->highAddress > p_grp->lowAddress){
			Value grp_object(kObjectType);
			json_db_set_u16_data(doc, grp_object,"highAddress",p_grp->highAddress);
			json_db_set_u16_data(doc, grp_object,"lowAddress",p_grp->lowAddress);
			grp_rang_object.PushBack(grp_object,allocator);
		}
	}
	return 1;
}
int json_db_set_Unicast_range(Document& doc, rapidjson::Value & p_base,
							char *p_src,mesh_provisioners_str *p_pro)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	json_add_empty_tab_tag(doc, p_base,p_src);
	Value src = rapidjson::Value(p_src, allocator);
	rapidjson::Value& uni_rang_object = p_base[src];
	for(u32 i=0;i<MAX_PRO_GROUP_RANGE_MAX;i++){
		mesh_json_pro_UnicastRange *p_uni = &(p_pro->unicastRange[i]);
		if(p_uni->highAddress > p_uni->lowAddress){
			Value uni_object(kObjectType);
			json_db_set_u16_data(doc, uni_object,"highAddress",p_uni->highAddress);
			json_db_set_u16_data(doc, uni_object,"lowAddress",p_uni->lowAddress);
			uni_rang_object.PushBack(uni_object,allocator);
		}
	}
	return 1;
}
int json_db_set_Scene_range(Document& doc, rapidjson::Value & p_base,
							char *p_src,mesh_provisioners_str *p_pro)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	json_add_empty_tab_tag(doc, p_base,p_src);
	Value src = rapidjson::Value(p_src, allocator);
	rapidjson::Value& scene_rang_object = p_base[src];
	for(u32 i=0;i<MAX_PRO_GROUP_RANGE_MAX;i++){
		mesh_json_pro_SceneRange *p_scene = &(p_pro->sceneRange[i]);
		if(p_scene->lastScene > p_scene->firstScene){
			Value scene_object(kObjectType);
            json_db_set_u16_data(doc, scene_object,"firstScene",p_scene->firstScene);
			json_db_set_u16_data(doc, scene_object,"lastScene",p_scene->lastScene);
			scene_rang_object.PushBack(scene_object,allocator);
		}
	}
	return 1;
}
int json_db_set_provision_data(Document& doc, mesh_provisioners_str *p_js_prov)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	if (doc.HasMember("provisioners")) {
		doc.RemoveMember("provisioners");
	}
	rapidjson::Value& pro_object = Value(kArrayType);
	//clean table first 
	for(u32 i=0;i<MAX_PROVISION_NUM;i++){
		mesh_provisioners_str *p_pro = &(p_js_prov[i]);
		if(p_pro->valid){
			Value int_object(kObjectType); 
			json_db_set_string(doc, int_object,"provisionerName",p_pro->provisionerName);
			json_db_set_data_string_uuid(doc, int_object,"UUID",p_pro->uuid,sizeof(p_pro->uuid));
			json_db_set_grp_range(doc, int_object,"allocatedGroupRange",p_pro);
			json_db_set_Unicast_range(doc, int_object,"allocatedUnicastRange",p_pro);
			json_db_set_Scene_range(doc, int_object,"allocatedSceneRange",p_pro);
			pro_object.PushBack(int_object,allocator);
		}
	}
	doc.AddMember("provisioners", pro_object, allocator);
	return 1;
}

int json_db_set_netKeys_data(Document& doc, mesh_json_netkeys_str *p_js_netkey)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	if (doc.HasMember("netKeys")) {
		doc.RemoveMember("netKeys");
	}
	rapidjson::Value& netKeys_object = Value(kArrayType);
    // set the ivi index part 
    rapidjson::Value& doc_object = doc;
	//clean table first 
	for(u32 i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
		mesh_json_netkeys_str *p_net = &(p_js_netkey[i]);
		if(p_net->valid){
			Value int_object(kObjectType); 
			json_db_set_string(doc, int_object,"name",p_net->name);
			json_db_set_data_string(doc, int_object,"key",p_net->key,sizeof(p_net->key));
			
			if(is_buf_zero_win(p_net->minSecurity,sizeof(p_net->minSecurity))||
				(	memcmp(p_net->minSecurity,security_char,sizeof(security_char)) && 
					memcmp(p_net->minSecurity,insecurity_char,sizeof(insecurity_char)))){
				memcpy(p_net->minSecurity,security_char,sizeof(security_char));
			}
			json_db_set_string(doc, int_object,"minSecurity",p_net->minSecurity);
			json_db_set_string(doc, int_object, "timestamp", time_char);
			json_db_set_int(doc, int_object,"index",p_net->index);
			json_db_set_int(doc, int_object,"phase",p_net->phase);
			if (p_net->phase != 0) {
				json_db_set_data_string(doc, int_object, "oldKey", p_net->oldkey, sizeof(p_net->oldkey));
			}			
			netKeys_object.PushBack(int_object,allocator);
		}
	}
	doc.AddMember("netKeys", netKeys_object, allocator);
	return 1;
}


int json_db_set_appKeys_data(Document& doc, mesh_json_appkeys_str *p_js_appkey)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	if (doc.HasMember("appKeys")) {
		doc.RemoveMember("appKeys");
	}
	rapidjson::Value& appKeys_object = Value(kArrayType);
	//clean table first 
	for(u32 i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
		mesh_json_appkeys_str *p_appkey = &(p_js_appkey[i]);
		if(p_appkey->valid){
			Value int_object(kObjectType);
			json_db_set_string(doc, int_object,"name",p_appkey->name);
			json_db_set_int(doc, int_object,"index",p_appkey->index);
			json_db_set_int(doc, int_object,"boundNetKey",p_appkey->bound_netkey);
			json_db_set_data_string(doc, int_object,"key",p_appkey->key,sizeof(p_appkey->key));
			json_db_set_data_string(doc, int_object,"oldKey",p_appkey->oldKey,sizeof(p_appkey->oldKey));
			appKeys_object.PushBack(int_object,allocator);
		}
	}
	doc.AddMember("appKeys", appKeys_object, allocator);
	return 1;
}
int json_db_set_feature(Document& doc, rapidjson::Value & p_base,char *p_src,
										mesh_nodes_feature_str *p_fea)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	Value int_object(kObjectType);
	json_db_set_int(doc, int_object,"relay",p_fea->relay);
	json_db_set_int(doc, int_object,"proxy",p_fea->proxy);
	json_db_set_int(doc, int_object,"friend",p_fea->fri);
	json_db_set_int(doc, int_object,"lowPower",p_fea->lowpower);
    p_base.AddMember(rapidjson::Value(p_src, allocator),int_object,allocator); 
	
	return 1;
}

int json_db_set_nodes_relayRetransmit(Document& doc, rapidjson::Value & p_base,
					char *p_src,mesh_nodes_relay_str *p_relay)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	Value int_object(kObjectType);
	json_db_set_int(doc, int_object,"count",p_relay->count);
	json_db_set_int(doc, int_object,"interval",(p_relay->interval+1)*10);
	p_base.AddMember(rapidjson::Value(p_src, allocator),int_object,allocator); 
	return 1;
}

int json_db_set_nodes_networkRetransmit(Document& doc, rapidjson::Value & p_base,
						char *p_src,mesh_nodes_networkTransmit_str *p_network)						
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	Value int_object(kObjectType);
	json_db_set_int(doc, int_object,"count",p_network->count);
	json_db_set_int(doc, int_object,"interval",(p_network->interval+1)*10);
	p_base.AddMember(rapidjson::Value(p_src, allocator),int_object,allocator);
	return 1;
}

int json_db_set_nodes_netkeys(Document& doc, rapidjson::Value & p_base,
						char *p_src,mesh_nodes_net_appkey_str *p_netkeys)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	// create empty tag part 
	json_add_empty_tab_tag(doc, p_base,p_src);
	rapidjson::Value& net_object = p_base[src];
	for(u32 i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
		mesh_nodes_net_appkey_str *p_net = &(p_netkeys[i]);
		if(p_net->valid){
			Value int_object(kObjectType);
			json_db_set_int(doc, int_object,"index",p_net->index);
			json_db_set_boolean(doc, int_object,"updated",p_net->update);
			net_object.PushBack(int_object,allocator);
		}
	}
	return 1;
}

int json_db_set_nodes_appkeys(Document& doc, rapidjson::Value & p_base,
						char *p_src,mesh_nodes_net_appkey_str *p_appkeys)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	// create empty tag part 
	json_add_empty_tab_tag(doc, p_base,p_src);
	rapidjson::Value& app_object = p_base[src];
	for(u32 i=0;i<MAX_APPKEY_TOTAL_NUM;i++){
		mesh_nodes_net_appkey_str *p_app = &(p_appkeys[i]);
		if(p_app->valid){
			Value int_object(kObjectType);
			json_db_set_int(doc, int_object,"index",p_app->index);
			json_db_set_boolean(doc, int_object,"updated",p_app->update);
			app_object.PushBack(int_object,allocator);
		}
	}
	return 1;
}

int json_db_set_model_sub_data(Document& doc, rapidjson::Value & p_base,
								char *p_src,mesh_nodes_model_str *p_model)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	json_add_empty_tab_tag(doc, p_base,p_src);
	rapidjson::Value& sub_object = p_base[src];
	for(u32 i=0;i<MAX_SUBSCRIBE_NUM;i++){
		if(p_model->subscribe[i]>=0xc000){
		    Value int_object(kObjectType);
		    u16 uni_adr = p_model->subscribe[i];
		    endianness_swap_u16((u8 *)(&uni_adr));
	        char data_string[256];
	        bin2text_clean((u8 *)data_string,(u8 *)&uni_adr,2);
	        int_object.SetString(data_string,strlen(data_string),allocator);
			sub_object.PushBack(int_object,allocator);
		}
	}
	return 1;
}

int json_db_set_model_bind_data(Document& doc, rapidjson::Value & p_base,
								char *p_src,mesh_nodes_model_str *p_model)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	json_add_empty_tab_tag(doc, p_base,p_src);
	rapidjson::Value& bind_object = p_base[src];
	for(u32 i=0;i<MAX_BIND_ADDRESS_NUM;i++){
		mesh_model_bind_str *p_bind = &(p_model->bind[i]);
		if(p_bind->valid){
			Value int_object(kObjectType);
			int_object.SetInt(p_bind->bind);
			bind_object.PushBack(int_object,allocator);
		}
	}
	return 1;
}

int json_db_set_period(Document& doc, rapidjson::Value & p_base,
								char *p_src,mesh_models_publish_str *p_pub)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	Value int_object(kObjectType);
	if(p_pub->numberOfSteps == 0){
		p_pub->numberOfSteps = 2;
	}
	if(p_pub->resolution == 0){
		p_pub->resolution = 10000;
	}
	json_db_set_int(doc, int_object,"numberOfSteps",p_pub->numberOfSteps);
	json_db_set_int(doc, int_object,"resolution",p_pub->resolution);
	p_base.AddMember( rapidjson::Value(p_src, allocator),int_object,allocator); 
	return 1;
}


int json_db_set_pub_trans(Document& doc, rapidjson::Value & p_base,
								char *p_src,mesh_models_publish_str *p_pub)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	Value int_object(kObjectType);
	if(p_pub->count== 0){
		p_pub->count =2;
	}
	if(p_pub->interval == 0){
		p_pub->interval = 100;
	}
	json_db_set_int(doc, int_object,"count",p_pub->count);
	json_db_set_int(doc, int_object,"interval",p_pub->interval);
	p_base.AddMember( rapidjson::Value(p_src, allocator),int_object,allocator); 
	return 1;
}

int json_db_set_model_publish_data(Document& doc, rapidjson::Value & p_base,
								char *p_src,mesh_models_publish_str *p_pub)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	Value int_object(kObjectType);
	if(p_pub->address == 0){
		return 0;
	}
	json_db_set_u16_data(doc, int_object,"address",(p_pub->address));
	json_db_set_int(doc, int_object,"index",p_pub->index);
	json_db_set_int(doc, int_object,"ttl",p_pub->ttl);
	json_db_set_int(doc, int_object,"credentials",p_pub->credentials);
	json_db_set_pub_trans(doc, int_object,"retransmit",p_pub);
	json_db_set_period(doc, int_object,"period",p_pub);
	p_base.AddMember(rapidjson::Value(p_src, allocator),int_object,allocator); 
	return 1;
}

int json_db_set_ele_model_data(Document& doc, rapidjson::Value & p_base,
								char *p_src,mesh_nodes_ele_str *p_ele)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	json_add_empty_tab_tag(doc, p_base,p_src);
	rapidjson::Value& model_object = p_base[src];
	for(u32 i=0;i<MAX_MODELS_IN_ELE_CNT;i++){
		mesh_nodes_model_str *p_model = &(p_ele->models[i]);
		if(p_model->valid){
			Value int_object(kObjectType);
			if(p_model->vendor_flag){// vendor model
				json_db_set_u32_data(doc, int_object,"modelId",p_model->modelId);
			}else{
				json_db_set_u16_data(doc, int_object,"modelId",p_model->modelId);
			}
			//subscribe 
			json_db_set_model_sub_data(doc, int_object,"subscribe",p_model);
			// bind
			json_db_set_model_bind_data(doc, int_object,"bind",p_model);
			// publish 
			json_db_set_model_publish_data(doc, int_object,"publish",&(p_model->publish));
			model_object.PushBack(int_object,allocator);
		}
	}
	return 1;

}
int json_db_set_nodes_ele_data(Document& doc, rapidjson::Value & p_base,
						char *p_src,mesh_node_str *p_node)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); 
	Value src = rapidjson::Value(p_src, allocator);
	// create empty tag part 
	json_add_empty_tab_tag(doc, p_base,p_src);
	rapidjson::Value& ele_object = p_base[src];
	for(u32 i=0;i<MAX_MESH_NODE_ELE_NUM;i++){
		mesh_nodes_ele_str *p_ele = &(p_node->elements[i]);
		if(p_ele->valid){
			Value int_object(kObjectType);
			json_db_set_string(doc, int_object,"name",p_ele->name);
			json_db_set_int(doc, int_object,"index",p_ele->index);
			json_db_set_u16_data(doc, int_object,"location",p_ele->location);
			json_db_set_ele_model_data(doc, int_object,"models",p_ele);
			ele_object.PushBack(int_object,allocator);
		}
	}
	return 1;

}

int json_db_set_nodes_data(Document& doc, mesh_node_str  *p_js_node)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	if (doc.HasMember("nodes")) {
		doc.RemoveMember("nodes");
	}

	rapidjson::Value& nodes_object = Value(kArrayType);
	
	for(u32 i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_nodes = &(p_js_node[i]);
		if(p_nodes->valid){
			Value int_object(kObjectType);
			json_db_set_data_string_uuid(doc, int_object,"UUID",p_nodes->uuid,sizeof(p_nodes->uuid));
			// because  it will store by the big endiness .
			u8 mac[6];
			swap48(mac,p_nodes->macAddress);
			json_db_set_data_string(doc, int_object,"macAddress",mac,sizeof(mac));
			json_db_set_string(doc, int_object,"name",p_nodes->name);
			json_db_set_data_string(doc, int_object,"deviceKey",p_nodes->deviceKey,sizeof(p_nodes->deviceKey));
			json_db_set_u16_data(doc, int_object,"unicastAddress",p_nodes->unicastAddress);
			json_db_set_u32_data(doc, int_object,"sno",p_nodes->sno);
			if(is_buf_zero_win(p_nodes->security,sizeof(p_nodes->security))||
				(	memcmp(p_nodes->security,security_char,sizeof(security_char)) && 
					memcmp(p_nodes->security,insecurity_char,sizeof(insecurity_char)))){
				memcpy(p_nodes->security,security_char,sizeof(security_char));
			}
			json_db_set_string(doc, int_object,"security",p_nodes->security);
			json_db_set_u16_data(doc, int_object,"cid",p_nodes->cid);
			json_db_set_u16_data(doc, int_object,"pid",p_nodes->pid);
			json_db_set_u16_data(doc, int_object,"vid",p_nodes->vid);
			json_db_set_u16_data(doc, int_object,"crpl",p_nodes->crpl);

			json_db_set_feature(doc, int_object,"features",&p_nodes->features);
			//networkTransmit
			json_db_set_nodes_relayRetransmit(doc, int_object,
								"relayRetransmit",&(p_nodes->relayRetransmit));
			json_db_set_nodes_networkRetransmit(doc, int_object,
								"networkTransmit",&(p_nodes->networkTransmit));
			//netkey
			json_db_set_nodes_netkeys(doc, int_object,"netKeys",p_nodes->netkeys);
			//appkey
			json_db_set_nodes_appkeys(doc, int_object,"appKeys",p_nodes->appkeys);
			// ele
			json_db_set_nodes_ele_data(doc, int_object,"elements",p_nodes);
			json_db_set_boolean(doc, int_object,"secureNetworkBeacon",p_nodes->secureNetworkBeacon);
			json_db_set_boolean(doc, int_object,"configComplete",p_nodes->configComplete);
			json_db_set_boolean(doc, int_object,"excluded",p_nodes->excluded);
			json_db_set_int(doc, int_object,"defaultTTL",p_nodes->defaultTTL);
			
			nodes_object.PushBack(int_object,allocator);
		}
	}
	doc.AddMember("nodes", nodes_object, allocator);
	return 1;
}


int json_db_set_nodes_data_static(Document& doc, mesh_node_static_str  *p_js_node)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	if (doc.HasMember("nodes")) {
		doc.RemoveMember("nodes");
	}
	rapidjson::Value& nodes_object = Value(kArrayType);

	for(u32 i=0;i<MAX_MESH_NODE_NUM;i++){
		mesh_node_str *p_nodes = (mesh_node_str *)&(p_js_node[i]);
		if(p_nodes->valid){
			Value int_object(kObjectType);
			json_db_set_data_string_uuid(doc, int_object,"UUID",p_nodes->uuid,sizeof(p_nodes->uuid));
			// because  it will store by the big endiness .
			u8 mac[6];
			swap48(mac,p_nodes->macAddress);
			//json_db_set_data_string(doc, int_object,"macAddress",mac,sizeof(mac));
			json_db_set_string(doc, int_object,"name",p_nodes->name);
			json_db_set_data_string(doc, int_object,"deviceKey",p_nodes->deviceKey,sizeof(p_nodes->deviceKey));
			json_db_set_u16_data(doc, int_object,"unicastAddress",p_nodes->unicastAddress);
			json_db_set_string(doc, int_object,"security",p_nodes->security);
			json_db_set_u16_data(doc, int_object,"cid",p_nodes->cid);
			json_db_set_u16_data(doc, int_object,"pid",p_nodes->pid);
			json_db_set_u16_data(doc, int_object,"vid",p_nodes->vid);
			json_db_set_u16_data(doc, int_object,"crpl",p_nodes->crpl);

			json_db_set_feature(doc, int_object,"features",&p_nodes->features);
			//networkTransmit
			json_db_set_nodes_relayRetransmit(doc, int_object,
								"relayRetransmit",&(p_nodes->relayRetransmit));
			json_db_set_nodes_networkRetransmit(doc, int_object,
								"networkTransmit",&(p_nodes->networkTransmit));
			//netkey
			json_db_set_nodes_netkeys(doc, int_object,"netKeys",p_nodes->netkeys);
			//appkey
			json_db_set_nodes_appkeys(doc, int_object,"appKeys",p_nodes->appkeys);
			// ele
			json_db_set_nodes_ele_data(doc, int_object,"elements",p_nodes);
			json_db_set_boolean(doc, int_object,"secureNetworkBeacon",p_nodes->secureNetworkBeacon);
			json_db_set_boolean(doc, int_object,"configComplete",p_nodes->configComplete);
			json_db_set_boolean(doc, int_object,"excluded",p_nodes->excluded);
			json_db_set_int(doc, int_object,"defaultTTL",p_nodes->defaultTTL);
			
			nodes_object.PushBack(int_object,allocator);
		}
	}
	doc.AddMember("nodes", nodes_object, allocator);
	return 1;
}

void json_groups_data_init(mesh_group_str *p_js_grp)
{
    char groups_name[8][20]={"Living room","Kitchen","Master bedroom","Secondary bedroom",
                               "Balcony","Bathroom","Hallway","others"};
    for(int i=0;i<8;i++){
        mesh_group_str *p_grp = &(p_js_grp[i]);
        p_grp->valid=1;
        p_grp->address =0xc000+i;
        memcpy(p_grp->name,groups_name[i],20);
    }
    return ;
}
int json_db_set_groups_data(Document& doc, mesh_group_str *p_js_grp)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	if (doc.HasMember("groups")) {
		doc.RemoveMember("groups");
	}
	rapidjson::Value& groups_object = Value(kArrayType);
	//clean table first 
	clear_table_ele(doc, groups_object);
	json_groups_data_init(p_js_grp);
	for(u32 i=0;i<MAX_GROUP_NUM;i++){
		mesh_group_str *p_grp = &(p_js_grp[i]);
		if(p_grp->valid){
			Value int_object(kObjectType);
			json_db_set_string(doc, int_object,"name",p_grp->name);
			json_db_set_u16_data(doc, int_object,"address",p_grp->address);
			json_db_set_u16_data(doc, int_object,"parentAddress",p_grp->parentAddress);
			groups_object.PushBack(int_object,allocator);
		}
	}
	doc.AddMember("groups", groups_object, allocator);
	return 1;	
}

int json_db_set_scene_add_tab(Document& doc, rapidjson::Value & p_base,char *p_src,u16 *address)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
	json_add_empty_tab_tag(doc, p_base,p_src);
	Value src = rapidjson::Value(p_src, allocator);	
	rapidjson::Value& uni_object = p_base[src];
	for(u32 i=0;i<MAX_GROUP_NUM;i++){
		if(address[i]!=0){
			Value int_object(kObjectType);
			u16 uni_adr = address[i];
		    endianness_swap_u16((u8 *)(&uni_adr));
	        char data_string[256];
	        bin2text_clean((u8 *)data_string,(u8 *)&uni_adr,2);
	        int_object.SetString(data_string,strlen(data_string),allocator);
			uni_object.PushBack(int_object,allocator);
		}
	}
	return 1;
}
int json_db_set_scene_data(Document& doc, mesh_scene_str *p_js_scene)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
    Value src = rapidjson::Value("scenes", allocator);
    if(!doc.HasMember(src)){
        json_add_tab_scenes(doc);
    }
	rapidjson::Value& scene_object = doc["scenes"];
	//clean table first 
	clear_table_ele(doc, scene_object);
	for(u32 i=0;i<MAX_SCENE_NUM;i++){
		mesh_scene_str *p_scene = &(p_js_scene[i]);
		if(p_scene->valid){
			Value int_object(kObjectType);
			json_db_set_string(doc, int_object,"name",p_scene->name);
			json_db_set_u16_data(doc, int_object,"number",p_scene->number);
			json_db_set_scene_add_tab(doc, int_object,"addresses",p_scene->addresses);
			scene_object.PushBack(int_object,allocator);
		}
	}
	return 1;
}

int json_db_set_ivi_data(Document& doc, u8 * p_js_ivi,u8 len)
{
	Document::AllocatorType &allocator=doc.GetAllocator(); //获取分配器
    rapidjson::Value& doc_object = doc;
    json_db_set_data_string(doc, doc_object,"ivIndex",p_js_ivi,len);
	return 1;
}


int json_db_set_mesh_data(Document& doc)
{
	rapidjson::Value& doc_object = doc;
	sig_mesh_json_database *p_db = &json_database;

	memcpy(p_db->schema,schema_char,sizeof(schema_char));
	memcpy(p_db->version,version_char,sizeof(version_char));
	memcpy(p_db->id,id_char,sizeof(id_char));
	memcpy(p_db->meshName,meshname_char,sizeof(meshname_char));
	memcpy(p_db->timestamp,time_char,sizeof(time_char));
	p_db->partial =0;// disable the partial part.
	json_db_set_string(doc, doc_object,"$schema",p_db->schema);
	json_db_set_string(doc, doc_object,"timestamp",p_db->timestamp);
	json_db_set_string(doc, doc_object,"version",p_db->version);
	json_db_set_string(doc, doc_object,"meshName",p_db->meshName);
	json_db_set_string(doc, doc_object,"id",p_db->id);
	json_db_set_boolean(doc, doc_object,"partial",p_db->partial);
	json_db_set_data_string_uuid(doc, doc_object,"meshUUID",p_db->mesh_uuid,sizeof(p_db->mesh_uuid));
	json_db_set_provision_data(doc, p_db->provisioners);
	json_db_set_netKeys_data(doc, p_db->netKeys);
	json_db_set_appKeys_data(doc, p_db->appkeys);
	json_db_set_nodes_data(doc, p_db->nodes);
	json_db_set_groups_data(doc, p_db->groups);
	json_db_set_scene_data(doc, p_db->scene);
	json_db_set_ivi_data(doc, p_db->ivi_idx,sizeof(p_db->ivi_idx));
	return 1;
}

int json_db_set_mesh_data_static(Document& doc)
{
	rapidjson::Value& doc_object = doc;
	sig_mesh_json_database_static *p_db = &json_db_static;
	memcpy(p_db->schema,schema_char,sizeof(schema_char));
	memcpy(p_db->version,version_char,sizeof(version_char));
	memcpy(p_db->id,id_char,sizeof(id_char));
	memcpy(p_db->meshName,meshname_char,sizeof(meshname_char));
	memcpy(p_db->timestamp,time_char,sizeof(time_char));
	p_db->partial =0;// disable the partial part.
	// keep order the same as JSON before output should be better
	json_db_set_string(doc, doc_object,"$schema",p_db->schema);
	json_db_set_string(doc, doc_object,"timestamp",p_db->timestamp);
	json_db_set_string(doc, doc_object,"version",p_db->version);
	json_db_set_string(doc, doc_object,"meshName",p_db->meshName);
	json_db_set_string(doc, doc_object,"id",p_db->id);
	json_db_set_boolean(doc, doc_object,"partial",p_db->partial);
	json_db_set_data_string_uuid(doc, doc_object,"meshUUID",p_db->mesh_uuid,sizeof(p_db->mesh_uuid));
	json_db_set_nodes_data_static(doc, p_db->nodes);
	json_db_set_provision_data(doc, p_db->provisioners);
	json_db_set_netKeys_data(doc, p_db->netKeys);
	json_db_set_appKeys_data(doc, p_db->appkeys);
	json_db_set_scene_data(doc, p_db->scene);
	json_db_set_groups_data(doc, p_db->groups);
	#if JSON_IVI_INDEX_ENABLE
	// when input the db should set to invalid ivi
	json_db_set_ivi_data(doc, p_db->ivi_idx,sizeof(p_db->ivi_idx),0);
	#endif
	return 1;
}

int json_file_exist(Document& doc, const char *p_json)
{
    if(json_read_init(doc, p_json)>0){
        return 1;
    }else{
        return 0;
    }
}
int init_json(const char *p_json,u8 mode)  
{
	Document doc;
    if(json_file_exist(doc, p_json)){
		if(mode){
			json_db_get_mesh_data_static(doc);
		}else{
			json_db_get_mesh_data(doc);
		}
        return 1;
    }else{
        // need to init doc file 
        set_ini_unicast_max(0);
		init_json_database_tag(doc);
    }
    return 0;  
}  

