/********************************************************************************************************
 * @file     DemoCommand.m 
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
//  DemoCommand.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DemoCommand.h"

@implementation DemoCommand

/// Is get online status from private uuid, YES main callback onoffCallback、brighrnessCallback、tempratureCallback、HSLCallback、levelCallback from OnlineStatusCharacteristic.
+ (BOOL)isPrivatelyGetOnlineStatus {
    NSNumber *online = [[NSUserDefaults standardUserDefaults] valueForKey:kGetOnlineStatusType];
    BOOL hasOTACharacteristic = [SDKLibCommand getCharacteristicWithUUIDString:kOnlineStatusCharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral] != nil;
    return online.boolValue && hasOTACharacteristic;
}

/// New API on demo since V2.8.1: Get Online device, publish use SigGenericOnOffGet, private use OnlineStatusCharacteristic
+ (BOOL)getOnlineStatusWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    /*Attention: when demo open funcation getOnlinestatus on Mesh Info ViewController, and current node has OnlineStatusCharacteristic, that API will get online status privately; if not, that API will get online status publish. */
    if ([self isPrivatelyGetOnlineStatus]) {
        NSError *error = [SDKLibCommand telinkApiGetOnlineStatueFromUUIDWithResponseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        if (error) {
            TeLogInfo(@"send request from uuid, but error:%@.",error.domain);
            return NO;
        } else {
            TeLogInfo(@"send request from uuid");
            return YES;
        }
    }else{
        if (SigMeshLib.share.isBusyNow) {
            TeLogInfo(@"send request for onlinestatus, but busy now.");
            if (resultCallback) {
                NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
                resultCallback(NO,error);
            }
            return NO;
        } else {
            TeLogInfo(@"send request for onlinestatus");
            [SDKLibCommand genericOnOffGetWithDestination:kMeshAddress_allNodes retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
            return YES;
        }
    }
}

/// Get Lum
+ (BOOL)getLumWithNodeAddress:(UInt16)nodeAddress responseMacCount:(int)responseMacCount successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get lum, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get lim");
        [SDKLibCommand lightLightnessGetWithDestination:nodeAddress retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMacCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Get CTL, (Lum and Temprature)
+ (BOOL)getCTLWithNodeAddress:(UInt16)nodeAddress responseMacCount:(int)responseMacCount successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get CTL, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get CTL");
        [SDKLibCommand lightCTLGetWithDestination:nodeAddress retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMacCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

///change subscribe list（develop can see mesh_node.h line129 to get more detail of option）
+ (BOOL)editSubscribeListWithWithDestination:(UInt16)destination isAdd:(BOOL)isAdd groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for edit subscribe list, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        SigGroupModel *group = [SigDataSource.share getGroupModelWithGroupAddress:groupAddress];
        if (!group) {
            TeLogInfo(@"send request for edit subscribe list, but group is not exist.");
            if (resultCallback) {
                NSError *error = [NSError errorWithDomain:kSigMeshLibGroupAddressNoExistErrorMessage code:kSigMeshLibGroupAddressNoExistErrorCode userInfo:nil];
                resultCallback(NO,error);
            }
            return NO;
        }
        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:destination];
        SigModelIDModel *modelIDModel = [node getModelIDModelWithModelID:modelIdentifier andElementAddress:elementAddress];
        if (!modelIDModel) {
            TeLogInfo(@"send request for edit subscribe list, but modelID is not exist.");
            if (resultCallback) {
                NSError *error = [NSError errorWithDomain:kSigMeshLibModelIDModelNoExistErrorMessage code:kSigMeshLibModelIDModelNoExistErrorCode userInfo:nil];
                resultCallback(NO,error);
            }
            return NO;
        }

        TeLogInfo(@"send request for edit subscribe list");
        if (isAdd) {
            [SDKLibCommand configModelSubscriptionAddWithDestination:destination toGroupAddress:groupAddress elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:retryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        } else {
            [SDKLibCommand configModelSubscriptionDeleteWithDestination:destination groupAddress:groupAddress elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:retryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        }
        return YES;
    }
}

/// Change publish list
+ (BOOL)editPublishListWithPublishAddress:(UInt16)publishAddress nodeAddress:(UInt16)nodeAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for edit publish list, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:nodeAddress];
        SigModelIDModel *modelIDModel = [node getModelIDModelWithModelID:modelIdentifier andElementAddress:elementAddress];
        if (!modelIDModel) {
            TeLogInfo(@"send request for edit publish list, but modelID is not exist.");
            if (resultCallback) {
                NSError *error = [NSError errorWithDomain:kSigMeshLibModelIDModelNoExistErrorMessage code:kSigMeshLibModelIDModelNoExistErrorCode userInfo:nil];
                resultCallback(NO,error);
            }
            return NO;
        }

        TeLogInfo(@"send request for edit publish list");
        SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:5 intervalSteps:2];
        SigPublish *publish = [[SigPublish alloc] initWithDestination:publishAddress withKeyIndex:SigDataSource.share.curAppkeyModel.index friendshipCredentialsFlag:0 ttl:0xff periodSteps:periodSteps periodResolution:periodResolution retransmit:retransmit];//ttl=0xFF(表示采用节点默认参数)
        [SDKLibCommand configModelPublicationSetWithDestination:nodeAddress publish:publish elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:retryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Get publish address
+ (BOOL)getPublishAddressWithNodeAddress:(UInt16)nodeAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for edit publish list, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:nodeAddress];
        SigModelIDModel *modelIDModel = [node getModelIDModelWithModelID:modelIdentifier andElementAddress:elementAddress];
        if (!modelIDModel) {
            TeLogInfo(@"send request for edit publish list, but modelID is not exist.");
            if (resultCallback) {
                NSError *error = [NSError errorWithDomain:kSigMeshLibModelIDModelNoExistErrorMessage code:kSigMeshLibModelIDModelNoExistErrorCode userInfo:nil];
                resultCallback(NO,error);
            }
            return NO;
        }

        TeLogInfo(@"send request for edit publish list");
        [SDKLibCommand configModelPublicationGetWithDestination:nodeAddress elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:retryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Turn on or turn off the lights part
+ (BOOL)switchOnOffWithIsOn:(BOOL)isOn address:(UInt16)address responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for switch onoff, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for switch onoff value:%d",isOn);
        [SDKLibCommand genericOnOffSetWithDestination:address isOn:isOn retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Change brightness100
+ (BOOL)changeBrightnessWithBrightness100:(UInt8)brightness100 address:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for change brightness100, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        UInt16 lightness = [SigHelper.share getUint16LightnessFromUInt8Lum:brightness100];
        TeLogInfo(@"send request for change brightness100 value:%d,UInt16 lightness=%d",brightness100,lightness);
        [SDKLibCommand lightLightnessSetWithDestination:address lightness:lightness retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Change temprature
+ (BOOL)changeTempratureWithTemprature100:(UInt8)temprature100 address:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for change temprature100, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        UInt16 temperature = [SigHelper.share getUint16TemperatureFromUInt8Temperature100:temprature100];
        TeLogInfo(@"send request for change temprature100 value:%d,UInt16 temperature=%d",temprature100,temperature);
        [SDKLibCommand lightCTLTemperatureSetWithDestination:address temperature:temperature deltaUV:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];// deltaUV comfirm later
        return YES;
    }
}

/// Get temprature
+ (BOOL)getTempratureWithAddress:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get temprature100, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get temprature100");
        [SDKLibCommand lightCTLTemperatureGetWithDestination:address retryCount:retryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Change HSL
+ (BOOL)changeHSLWithAddress:(UInt16)address hue:(float)hue saturation:(float)saturation brightness:(float)brightness responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for change HSL100, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for change HSL100 h100:%d,s100:%d,l100:%d,UInt16 hue=0x%x,saturation=0x%x,brightness=0x%x",(int)(hue*100),(int)(saturation*100),(int)(brightness*100),(UInt16)(hue*0xFFFF),(UInt16)(saturation*0xFFFF),(UInt16)(brightness*0xFFFF));
        [SDKLibCommand lightHSLSetWithDestination:address HSLLight:(UInt16)(brightness*0xFFFF) HSLHue:(UInt16)(hue*0xFFFF) HSLSaturation:(UInt16)(saturation*0xFFFF) retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Get HSL
+ (BOOL)getHSLWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get HSL, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get HSL");
        [SDKLibCommand lightHSLGetWithDestination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Change level
+ (BOOL)changeLevelWithAddress:(UInt16)address level:(SInt16)level responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for change level, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for change level value:%d",level);
        [SDKLibCommand genericDeltaSetWithDestination:address delta:level retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Get level
+ (BOOL)getLevelWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get level, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get level address:%d",address);
        [SDKLibCommand genericLevelGetWithDestination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Kick out
+ (BOOL)kickoutDevice:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for kick out, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for kick out address:%d",address);
        [SDKLibCommand resetNodeWithDestination:address retryCount:retryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Set nodes's time to app's time
+ (BOOL)setNowTimeWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for set time, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        UInt64 second = [LibTools secondsFrome2000];
        [NSTimeZone resetSystemTimeZone];//重置手机系统的时区
        NSInteger offset = [NSTimeZone localTimeZone].secondsFromGMT;
        UInt8 zone_offset = offset/60/15+64;//时区=分/15+64
        TeLogInfo(@"send request for set time, address=0x%04x,second=%llu,zone_offset=%d.",address,second,zone_offset);
        SigTimeModel *timeModel = [[SigTimeModel alloc] initWithTAISeconds:second subSeconds:0 uncertainty:0 timeAuthority:0 TAI_UTC_Delta:0 timeZoneOffset:zone_offset];
        [SDKLibCommand timeSetWithDestination:address timeModel:timeModel retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// status NowTime to node, without response
+ (BOOL)statusNowTimeWithAddress:(UInt16)address successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for status time, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        UInt64 second = [LibTools secondsFrome2000];
        [NSTimeZone resetSystemTimeZone];//重置手机系统的时区
        NSInteger offset = [NSTimeZone localTimeZone].secondsFromGMT;
        UInt8 zone_offset = offset/60/15+64;//时区=分/15+64
        TeLogInfo(@"send request for status time, address=0x%04x,second=%llu,zone_offset=%d.",address,second,zone_offset);
        SigTimeModel *timeModel = [[SigTimeModel alloc] initWithTAISeconds:second subSeconds:0 uncertainty:0 timeAuthority:0 TAI_UTC_Delta:0 timeZoneOffset:zone_offset];
        [SDKLibCommand timeStatusWithDestination:address timeModel:timeModel retryCount:0 responseMaxCount:0 successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Save scene
+ (BOOL)saveSceneWithAddress:(UInt16)address sceneId:(UInt16)sceneId responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for save scene, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for save scene, address=0x%04x, sceneId:%d",address,sceneId);
        [SDKLibCommand sceneStoreWithDestination:address sceneNumber:sceneId retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Recall scene
+ (BOOL)recallSceneWithAddress:(UInt16)address sceneId:(UInt16)sceneId responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for recall scene, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for recall scene, address=0x%04x, sceneId:%d",address,sceneId);
        [SDKLibCommand sceneRecallWithDestination:address sceneNumber:sceneId transitionTime:nil delay:0 retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Delete scene
+ (BOOL)delSceneWithAddress:(UInt16)address sceneId:(UInt16)sceneId responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for delete scene, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for delete scene, address=0x%04x, sceneId:%d",address,sceneId);
        [SDKLibCommand sceneDeleteWithDestination:address sceneNumber:sceneId retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Get scene register status
+ (BOOL)getSceneRegisterStatusWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get scene register status, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get scene register status, address=0x%04x",address);
        [SDKLibCommand sceneRegisterGetWithDestination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Get scheduler status
+ (BOOL)getSchedulerStatusWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get Scheduler Status, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get Scheduler Status, address=0x%04x",address);
        [SDKLibCommand schedulerGetWithDestination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// Set scheduler action
+ (BOOL)setSchedulerActionWithAddress:(UInt16)address schedulerModel:(SchedulerModel *)schedulerModel responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for set Scheduler Action, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for set Scheduler Action, address=0x%04x, schedulerModel.schedulerData:%@",address,[SigHelper.share getUint64String:schedulerModel.schedulerData]);
        [SDKLibCommand schedulerActionSetWithDestination:address schedulerModel:schedulerModel retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

/// get scheduler action
+ (BOOL)getSchedulerActionWithAddress:(UInt16)address schedulerModelID:(UInt8)schedulerModelID responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for get Scheduler Action, but busy now.");
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:kSigMeshLibIsBusyErrorMessage code:kSigMeshLibIsBusyErrorCode userInfo:nil];
            resultCallback(NO,error);
        }
        return NO;
    } else {
        TeLogInfo(@"send request for get Scheduler Action, address=0x%04x,schedulerModelID=%d",address,schedulerModelID);
        [SDKLibCommand schedulerActionGetWithDestination:address schedulerIndex:schedulerModelID retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMaxCount successCallback:successCallback resultCallback:resultCallback];
        return YES;
    }
}

@end
