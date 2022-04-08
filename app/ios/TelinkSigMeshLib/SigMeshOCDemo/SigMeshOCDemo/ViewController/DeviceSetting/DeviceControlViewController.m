/********************************************************************************************************
 * @file     DeviceControlViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/10
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#import "DeviceControlViewController.h"
#import "ColorManager.h"
#import "OnOffItemCell.h"
#import "NSString+calculate.h"
#import "ColorModelCell.h"
#import "OnOffModelCell.h"
#import "LevelAndSliderCell.h"
#import "BaseTableView.h"

typedef enum : NSUInteger {
    ModelUITypeHSL = 0,
    ModelUITypeOnOff = 1,
    ModelUITypeLum = 2,
    ModelUITypeTemp = 3,
} ModelUIType;

@interface ModelType : NSObject
@property (assign, nonatomic) ModelUIType uiType;
@property (strong, nonatomic) NSMutableArray <NSNumber *>*addresses;
@end
@implementation ModelType
@end


@interface DeviceControlViewController ()<UITableViewDataSource,BaseTableViewDelegate,ColorModelCellDelegate,OnOffModelCellDelegate,LevelAndSliderCellDelegate>
@property (weak, nonatomic) IBOutlet BaseTableView *tableView;
@property (assign, nonatomic) BOOL hadChangeBrightness;
@property (assign, nonatomic) BOOL hasNextBrightness;
@property (assign, nonatomic) UInt8 nextBrightness;
@property (assign, nonatomic) BOOL hadChangeTempareture;
@property (assign, nonatomic) BOOL hasNextTempareture;
@property (assign, nonatomic) UInt8 nextTempareture;
@property (strong, nonatomic) ColorModelCell *colorModelCell;



@property (strong, nonatomic) NSMutableArray <NSNumber *>*onoffStateSource;
@property (strong, nonatomic) NSMutableArray <ModelType *>*dataSource;
@property (strong, nonatomic) UIColor *colorModel;
@property (strong, nonatomic) RGBModel *rgbModel;
@property (strong, nonatomic) HSVModel *hsvModel;
@property (strong, nonatomic) HSLModel *hslModel;
@property (assign, nonatomic) BOOL hasNextHSLCMD;

@end

@implementation DeviceControlViewController

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    
    self.hadChangeBrightness = NO;
    self.hasNextBrightness = NO;
    self.nextBrightness = 0;
    self.hadChangeTempareture = NO;
    self.hasNextTempareture = NO;
    self.nextTempareture = 0;
    
    
    
    
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;//去掉下划线
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.allowsSelection = NO;
    self.tableView.baseTableViewDelegate = self;
    self.dataSource = [NSMutableArray array];
    if (self.model.HSLAddresses.count > 0) {
        self.colorModel = [self getColorWithH:self.model.HSL_Hue100 S:self.model.HSL_Saturation100 L:self.model.HSL_Lightness100];

        ModelType *type = [[ModelType alloc] init];
        type.uiType = ModelUITypeHSL;
        type.addresses = [NSMutableArray arrayWithArray:self.model.HSLAddresses];
        [self.dataSource addObject:type];
        [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ColorModelCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ColorModelCellID];
    }
    if (self.model.onoffAddresses.count > 0) {
        ModelType *type = [[ModelType alloc] init];
        type.uiType = ModelUITypeOnOff;
        type.addresses = [NSMutableArray arrayWithArray:self.model.onoffAddresses];
        self.onoffStateSource = [NSMutableArray arrayWithObject:@(self.model.state == DeviceStateOn)];
        if (type.addresses.count > 1) {
            for (int i = 1; i < type.addresses.count ; i++) {
                [self.onoffStateSource addObject:@(NO)];
            }
        }
        [self.dataSource addObject:type];
        [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_OnOffModelCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_OnOffModelCellID];
    }
    
    //注意：2.8.2发现RGB为255、0、0时，亮度调到100会设备颜色显示异常，暂时屏蔽亮度、色温

    if (self.model.lightnessAddresses.count > 0) {
        ModelType *type = [[ModelType alloc] init];
        type.uiType = ModelUITypeLum;
        type.addresses = [NSMutableArray arrayWithArray:self.model.lightnessAddresses];
        [self.dataSource addObject:type];
        [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_LevelAndSliderCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_LevelAndSliderCellID];
    }
    if (self.model.temperatureAddresses.count > 0) {
        ModelType *type = [[ModelType alloc] init];
        type.uiType = ModelUITypeTemp;
        type.addresses = [NSMutableArray arrayWithArray:self.model.temperatureAddresses];
        [self.dataSource addObject:type];
        [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_LevelAndSliderCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_LevelAndSliderCellID];
    }
    
    //获取当前设备的详细状态数据(亮度、色温、HSL)
    //Attention: app get online status use access_cmd_onoff_get() in publish since v2.7.0, so demo should get light and temprature at node detail vc.
    //modelID 0x1303:Light CTL Get
    BOOL hasCTLGet = [self.model getAddressesWithModelID:@(kSigModel_LightCTLServer_ID)].count > 0;
    //modelID 0x1300:Light Lightness Get
    BOOL hasLightnessGet = [self.model getAddressesWithModelID:@(kSigModel_LightLightnessServer_ID)].count > 0;

    if (self.model.lightnessAddresses.count > 0 && self.model.temperatureAddresses.count > 0 && hasCTLGet && self.model.state != DeviceStateOutOfLine) {
        //get light and temprature
        __weak typeof(self) weakSelf = self;
        [DemoCommand getCTLWithNodeAddress:self.model.address responseMacCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightCTLStatus * _Nonnull responseMessage) {
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            if (isResponseAll) {
                [weakSelf performSelectorOnMainThread:@selector(getHSL) withObject:nil waitUntilDone:YES];
            }
        }];
    }else if (self.model.lightnessAddresses.count > 0 && hasLightnessGet && self.model.state != DeviceStateOutOfLine) {
        //get light
        __weak typeof(self) weakSelf = self;
        [DemoCommand getLumWithNodeAddress:self.model.address responseMacCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightLightnessStatus * _Nonnull responseMessage) {
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            if (isResponseAll) {
                [weakSelf performSelectorOnMainThread:@selector(getHSL) withObject:nil waitUntilDone:YES];
            }
        }];
    }
}

- (void)getHSL{
    BOOL hasHSLGet = [self.model getAddressesWithModelID:@(kSigModel_LightHSLServer_ID)].count > 0;
    //v3.3.0新增逻辑：开灯时才获取HSL，关灯不获取HSL，因为关灯时HSL值都是0，获取HSL没有意义。
    if (self.model.HSLAddresses.count > 0 && hasHSLGet && self.model.state == DeviceStateOn) {
        __weak typeof(self) weakSelf = self;
        [DemoCommand getHSLWithAddress:self.model.address responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightHSLStatus * _Nonnull responseMessage) {
            [weakSelf performSelectorOnMainThread:@selector(HSLCallBack) withObject:nil waitUntilDone:YES];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            
        }];
    }
}

- (void)HSLCallBack{
    self.colorModel = [self getColorWithH:self.model.HSL_Hue100 S:self.model.HSL_Saturation100 L:self.model.HSL_Lightness100];
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (UIColor *)getColorWithH:(UInt8)h S:(UInt8)s L:(UInt8)l{
    //注意：hsl四舍五入取两位小数，理论上hsl中h为[0~100)
    if (h == 100) {
        h = 99;
    }
    HSLModel *hsl = [[HSLModel alloc] init];
    hsl.hue = [NSString stringWithFormat:@"%d",h].div(@"100").floatValue;
    hsl.saturation = [NSString stringWithFormat:@"%d",s].div(@"100").floatValue;
    hsl.lightness = [NSString stringWithFormat:@"%d",l].div(@"100").floatValue;
    return [ColorManager getRGBWithHSLColor:hsl];
}

-(void)dealloc{
    TeLogDebug(@"");
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.dataSource.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    ModelType *type = self.dataSource[indexPath.row];
    UITableViewCell *cell = [[UITableViewCell alloc] init];
    UInt8 lum=0,temp=0;
    switch (type.uiType) {
        case ModelUITypeHSL:
            cell = (ColorModelCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ColorModelCellID forIndexPath:indexPath];
            ((ColorModelCell *)cell).delegate = self;
            [((ColorModelCell *)cell) setColorModel:self.colorModel];
            self.colorModelCell = (ColorModelCell *)cell;
            break;
        case ModelUITypeOnOff:
            cell = (OnOffModelCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_OnOffModelCellID forIndexPath:indexPath];
            [((OnOffModelCell *)cell) setOnoffAddressSource:type.addresses];
            [((OnOffModelCell *)cell) setOnoffStateSource:self.onoffStateSource];
            [((OnOffModelCell *)cell) refreshUI];
            ((OnOffModelCell *)cell).delegate = self;
            break;
        case ModelUITypeLum:
            if (self.model.state == DeviceStateOn) {
                lum = self.model.trueBrightness;
            }
            cell = (LevelAndSliderCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_LevelAndSliderCellID forIndexPath:indexPath];
            ((LevelAndSliderCell *)cell).showLevelLabel.text = [NSString stringWithFormat:@"Lum Level(at ele adr:0x%X):",self.model.address];
            ((LevelAndSliderCell *)cell).showValueLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",lum,self.model.address];
            ((LevelAndSliderCell *)cell).valueSlider.minimumValue = 1;
            ((LevelAndSliderCell *)cell).valueSlider.maximumValue = 100;
            ((LevelAndSliderCell *)cell).valueSlider.value = lum;
            ((LevelAndSliderCell *)cell).delegate = self;
            break;
        case ModelUITypeTemp:
            if (self.model.state == DeviceStateOn) {
                temp = self.model.trueTemperature;
            }
            cell = (LevelAndSliderCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_LevelAndSliderCellID forIndexPath:indexPath];
            ((LevelAndSliderCell *)cell).showLevelLabel.text = [NSString stringWithFormat:@"Temp Level(at ele adr:0x%X):",self.model.temperatureAddresses.firstObject.intValue];
            ((LevelAndSliderCell *)cell).showValueLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",temp,self.model.temperatureAddresses.firstObject.intValue];
            ((LevelAndSliderCell *)cell).valueSlider.minimumValue = 0;
            ((LevelAndSliderCell *)cell).valueSlider.maximumValue = 100;
            ((LevelAndSliderCell *)cell).valueSlider.value = temp;
            ((LevelAndSliderCell *)cell).delegate = self;
            break;
        default:
            break;
    }

    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    ModelType *type = self.dataSource[indexPath.row];
    switch (type.uiType) {
        case ModelUITypeHSL:
            return 692.5;
            break;
        case ModelUITypeOnOff:
            return [OnOffModelCell getOnOffModelCellHightOfItemCount:type.addresses.count];
            break;
        case ModelUITypeLum:
            return 109;
            break;
        case ModelUITypeTemp:
            return 109;
            break;
        default:
            break;
    }
    return 0;
}

#pragma mark - BaseTableViewDelegate

// 目的：触摸到色盘的layer内部时则相应色盘手势，不再响应BaseTableView的滑动手势了。
-(BOOL)baseTableView:(BaseTableView *)baseTableView gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    CGPoint point = [touch locationInView:self.view];
    point = [self.colorModelCell.layer convertPoint:point fromLayer:self.view.layer];
    if ([self.colorModelCell.layer containsPoint:point]) {
        point = [self.colorModelCell.colorPicker.layer convertPoint:point fromLayer:self.colorModelCell.layer];
        if ([self.colorModelCell.colorPicker.layer containsPoint:point]) {
            return NO;
        }
    }
    return YES;
}

#pragma mark - ColorModelCellDelegate

//注意：调节RGB：R=83、G=83~87、B=251，转hsl为66、95、65，返回的HSL数据都是“f00c0a00 01008278 66a666a6 32f3”
- (void)colorModelCell:(ColorModelCell *)cell didChangedColorWithColor:(UIColor *)color rgbModel:(RGBModel *)rgbModel hsvModel:(HSVModel *)hsvModel hslModel:(HSLModel *)hslModel {
    self.colorModel = color;
    self.rgbModel = rgbModel;
    self.hsvModel = hsvModel;
    self.hslModel = hslModel;
    [self sendHSLData];
}

- (void)sendHSLData{
    if ([self canSend]) {
        UInt16 address = self.model.address;
        [DemoCommand changeHSLWithAddress:address hue:self.hslModel.hue saturation:self.hslModel.saturation brightness:self.hslModel.lightness responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightHSLStatus * _Nonnull responseMessage) {
            
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            
        }];
        self.hasNextHSLCMD = NO;
    } else {
        if (!self.hasNextHSLCMD) {
            self.hasNextHSLCMD = YES;
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sendHSLData) object:nil];
                [self performSelector:@selector(sendHSLData) withObject:nil afterDelay:kCMDInterval];
            });
        }
    }
}

// Can send data when collect with kCMDInterval
- (BOOL)canSend{
//    return YES;
    static NSTimeInterval time = 0;
    NSTimeInterval curTime = [[NSDate date] timeIntervalSince1970];
    BOOL tem = NO;
    if (curTime - time >= kCMDInterval) {
        time = curTime;
        tem = YES;
    }
    return tem;
}

#pragma mark - OnOffModelCellDelegate

- (void)onOffModelCell:(OnOffModelCell *)cell indexPath:(nonnull NSIndexPath *)indexPath didChangedValueWithValue:(BOOL)value {
    [self.onoffStateSource replaceObjectAtIndex:indexPath.item withObject:@(value)];
    ModelType *type = self.dataSource[[self.tableView indexPathForCell:cell].item];
    UInt16 elementAddress = [type.addresses[indexPath.item] intValue];
    
    //0.不带渐变且无回包写法：
    //attention: resMax change to 0, because node detail vc needn't response.
//    [DemoCommand switchOnOffWithIsOn:value address:elementAddress responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
    
    //1.不带渐变写法：
    [DemoCommand switchOnOffWithIsOn:value address:elementAddress responseMaxCount:1 ack:YES successCallback:nil resultCallback:nil];
    
    //2.带渐变写法：Sending message:SigGenericOnOffSet->Access PDU, source:(0x0001)->destination: (0x0002) Op Code: (0x8202), accessPdu=820200174100
//        SigTransitionTime *transitionTime = [[SigTransitionTime alloc] initWithSetps:1 stepResolution:SigStepResolution_seconds];
//        [SDKLibCommand genericOnOffSetDestination:elementAddress isOn:value transitionTime:transitionTime delay:0 retryCount:2 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
//            TeLogInfo(@"source=0x%x,destination=0x%x,responseMessage=%@",source,destination,responseMessage);
//        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//        }];
}

#pragma mark - LevelAndSliderCellDelegate

- (void)levelAndSliderCell:(LevelAndSliderCell *)cell didChangedValueWithValue:(CGFloat)value {
    ModelType *type = self.dataSource[[self.tableView indexPathForCell:cell].item];
    if (type.uiType == ModelUITypeLum) {
        cell.showValueLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",(int)value,self.model.address];
        if (!self.hadChangeBrightness) {
            self.nextBrightness = value;
            [self changeBrightnessWithModelType:type];
        } else {
            self.hasNextBrightness = YES;
            self.nextBrightness = value;
        }
        //如果UI是HSL的UI，需要联动修改HSL的滑竿。
        [self changeUIBylightnessOfHSLModel:value/100.0];
    } else if (type.uiType == ModelUITypeTemp) {
        cell.showValueLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",(int)value,self.model.temperatureAddresses.firstObject.intValue];
        if (!self.hadChangeTempareture) {
            self.nextTempareture = value;
            [self changeTemparetureWithModelType:type];
        } else {
            self.hasNextTempareture = YES;
            self.nextTempareture = value;
        }
    }
}

- (void)levelAndSliderCellDidClickedDeleteButton:(LevelAndSliderCell *)cell {
    [self changeLevelWithLevelAndSliderCell:cell isAdd:NO];
}

- (void)levelAndSliderCellDidClickedAddButton:(LevelAndSliderCell *)cell {
    [self changeLevelWithLevelAndSliderCell:cell isAdd:YES];
}

- (void)changeBrightnessWithModelType:(ModelType *)type {
    self.hadChangeBrightness = YES;
    self.hasNextBrightness = NO;
    TeLogInfo(@"self.nextBrightness=%d",self.nextBrightness);
    self.model.brightness = [LibTools lumToLightness:self.nextBrightness];
    UInt16 elementAddress = [type.addresses.firstObject intValue];
    [DemoCommand changeBrightnessWithBrightness100:self.nextBrightness address:elementAddress retryCount:0 responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(changeBrightnessFinishWithModelType:) object:type];
        [self performSelector:@selector(changeBrightnessFinishWithModelType:) withObject:type afterDelay:kCommandInterval];
    });
}

- (void)changeBrightnessFinishWithModelType:(ModelType *)type {
    self.hadChangeBrightness = NO;
    if (self.hasNextBrightness) {
        [self changeBrightnessWithModelType:type];
    }
}

- (void)changeTemparetureWithModelType:(ModelType *)type {
    self.hadChangeTempareture = YES;
    self.hasNextTempareture = NO;
    self.model.temperature = [LibTools temp100ToTemp:self.nextTempareture];
    UInt16 elementAddress = [type.addresses.firstObject intValue];
    [DemoCommand changeTempratureWithTemprature100:self.nextTempareture address:elementAddress retryCount:0 responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(changeTemparetureFinishWithModelType:) object:type];
        [self performSelector:@selector(changeTemparetureFinishWithModelType:) withObject:type afterDelay:kCommandInterval];
    });
}

- (void)changeTemparetureFinishWithModelType:(ModelType *)type {
    self.hadChangeTempareture = NO;
    if (self.hasNextTempareture) {
        [self changeTemparetureWithModelType:type];
    }
}

- (void)changeLevelWithLevelAndSliderCell:(LevelAndSliderCell *)cell isAdd:(BOOL)isAdd {
    SInt16 level = ceil(0xFFFF/10.0);//向上取整
    if (!isAdd) {
        level = -level;
    }
    ModelType *type = self.dataSource[[self.tableView indexPathForCell:cell].item];
    UInt16 address = type.addresses.firstObject.intValue;
    __weak typeof(self) weakSelf = self;
    [DemoCommand changeLevelWithAddress:address level:level responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericLevelStatus * _Nonnull responseMessage) {
        TeLogDebug(@"control level success.");
        dispatch_async(dispatch_get_main_queue(), ^{
            //根据回包进行HSL的滑竿进行联动。
            if (weakSelf.model.HSLAddresses.count > 0) {
                weakSelf.colorModel = [weakSelf getColorWithH:weakSelf.model.HSL_Hue100 S:weakSelf.model.HSL_Saturation100 L:weakSelf.model.HSL_Lightness100];
            }
            [weakSelf.tableView reloadData];
        });
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}

- (void)changeUIBylightnessOfHSLModel:(CGFloat)lightnessOfHSLModel {
    if (self.model.HSLAddresses.count > 0) {
        for (ModelType *m in self.dataSource) {
            if (m.uiType == ModelUITypeHSL) {
                ColorModelCell *cell = (ColorModelCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:[self.dataSource indexOfObject:m] inSection:0]];
                [cell changeUIWithHslModelLightSliderValue:lightnessOfHSLModel];
                break;
            }
        }
    }
}

@end
