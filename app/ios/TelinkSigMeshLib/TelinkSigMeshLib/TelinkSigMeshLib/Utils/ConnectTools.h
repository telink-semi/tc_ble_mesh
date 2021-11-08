/********************************************************************************************************
 * @file     ConnectTools.h
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/4/19
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or  
 *           alter) any information contained herein in whole or in part except as expressly authorized  
 *           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible  
 *           for any claim to the extent arising out of or relating to such deletion(s), modification(s)  
 *           or alteration(s).
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface ConnectTools : NSObject

+ (ConnectTools *)share;


/// demo 自定义连接工具类，用于开始连接指定的节点（逻辑：扫描5秒->扫描到则连接setFilter返回成功，扫描不到则连接已经扫描到的任意设备->setFilter->是则返回成功，不是则setNodeIdentity(多个设备则调用多次)->重复扫描5秒流程。）
/// @param nodeList 需要连接的节点，需要是SigDataSource里面的节点。
/// @param timeout 超时时间
/// @param complete 连接结果回调
- (void)startConnectToolsWithNodeList:(NSArray <SigNodeModel *>*)nodeList timeout:(NSInteger)timeout Complete:(nullable startMeshConnectResultBlock)complete;

/// demo 自定义连接工具类，用于停止连接指定的节点流程并断开当前的连接。
- (void)stopConnectToolsWithComplete:(nullable stopMeshConnectResultBlock)complete;

/// demo 自定义连接工具类，用于停止连接指定的节点流程保持当前的连接。
- (void)endConnectTools;

@end

NS_ASSUME_NONNULL_END
