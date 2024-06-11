/********************************************************************************************************
 * @file     LightingControlVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2024/1/15
 *
 * @par     Copyright (c) 2024, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "LightingControlVC.h"
#import "LightControlCell.h"
#import "UIButton+extension.h"
#import "CustomAlertView.h"
#import "NSString+extension.h"

@interface LCShowModel : NSObject
/** 是否展开 */
@property (nonatomic, assign) BOOL isExpand;
@property (strong, nonatomic) NSString *title;
@property (strong, nonatomic) NSString *detail;
@property (assign, nonatomic) NSInteger propertyID;
@property (assign, nonatomic) NSInteger value;
- (instancetype)initWithTitle:(NSString *)title detail:(NSString *)detail propertyID:(NSInteger)propertyID value:(NSInteger)value;
@end
@implementation LCShowModel
- (instancetype)initWithTitle:(NSString *)title detail:(NSString *)detail propertyID:(NSInteger)propertyID value:(NSInteger)value {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _title = title;
        _detail = detail;
        _propertyID = propertyID;
        _value = value;
    }
    return self;
}
@end


@interface LightingControlVC ()
@property (weak, nonatomic) IBOutlet UISwitch *enableLCModeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *enableLCOccupancyModeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *setLCLightOnOffSwitch;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <LCShowModel *>*dataArray;
@property (assign, nonatomic) UInt16 lightLCServerAddress;

@end

@implementation LightingControlVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"Lighting Control";
    self.lightLCServerAddress = [self.model getElementModelWithModelIds:@[@(kSigModel_LightLCServer_ID)]].unicastAddress;
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(LightControlCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(LightControlCell.class)];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.tableView.sectionHeaderTopPadding = 0;
    }

    [self refreshModeUI];
    [self configDefaultData];
    [self getLCStatusAction];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
}

- (void)refreshModeUI {
    self.enableLCModeSwitch.on = self.model.lightControlModeEnable;
    self.enableLCOccupancyModeSwitch.on = self.model.lightControlOccupancyModeEnable;
    self.enableLCOccupancyModeSwitch.enabled = self.model.lightControlModeEnable;
    self.setLCLightOnOffSwitch.on = self.model.lightControlLightOnOffState;
    self.setLCLightOnOffSwitch.enabled = self.model.lightControlModeEnable;
}

- (void)configDefaultData {
    self.dataArray = [NSMutableArray array];
    NSArray *titleArray = @[@"Lightness On",@"Lightness Prolong",@"Lightness Standby",@"Lux Level On", @"Lux Level Prolong", @"Lux Level Standby", @"Time Fade On",@"Time Run On",@"Time Fade",@"Time Prolong",@"Time Fade Standby Auto",@"Time Fade Standby Manual",@"Time Occupancy Delay", @"Regulator Accuracy", @"Regulator Kid", @"Regulator Kiu", @"Regulator Kpd", @"Regulator Kpu"];
    NSArray *detailArray = @[@"This property represents the light lightness level of a light or a group of lights in a run state.\nCharacteristic: Perceived Lightness", @"This property represents the light lightness level of a light or a group of lights when in a prolong state.\nCharacteristic: Perceived Lightness", @"This property represents the light lightness level of a light or a group of lights when in a standby state.\nCharacteristic: Perceived Lightness", @"The Light LC Ambient LuxLevel On is a state representing the Ambient LuxLevel level that determines if the controller transitions from the Light Control Standby state.\nCharacteristic: Illuminance", @"The Light LC Ambient LuxLevel Prolong is a state representing the required Ambient LuxLevel level in the Prolong state.\nCharacteristic: Illuminance", @"The Light LC Ambient LuxLevel Standby is a state representing the required Ambient LuxLevel level in the Standby state.\nCharacteristic: Illuminance", @"This property represents the time lights take to transition from a standby state to a run state.\nCharacteristic: Time Millisecond 24", @"This property represents the duration of the run state after last occupancy was detected.\nCharacteristic: Time Millisecond 24", @"This property represents the time a light takes to transition from a run state to a prolong state. The run state is the state when the light is running at normal light level, the prolong state is an intermediate state of a light between the run and the standby state.\nCharacteristic: Time Millisecond 24", @"This property represents the duration of the prolong state, which is the state of a device between its run state and its standby state.\nCharacteristic: Time Millisecond 24", @"This property represents the time lights take to transition from a prolong state to a standby state when the transition is automatic(such as when triggered by an occupancy or light sensor).\nCharacteristic: Time Millisecond 24", @"This property represents the time lights take to transition to a standby state when the transition is triggered by a manual operation(e.g., by a user operating a light switch).\nCharacteristic: Time Millisecond 24", @"This property represents the time delay between receiving a signal from an occupancy sensor and a light controller executing a state change as a result of the signal.\nCharacteristic: Time Millisecond 24", @"The Light LC Regulator Accuracy is a state representing the percentage accuracy of the Light LC PI Feedback Regulator.\nCharacteristic: Percentage 8", @"The Light LC Regulator Kid is a float32 state representing the integral coefficient that determines the integral part of the equation defining the output of the Light LC PI Feedback Regulator, when Light LC Ambient LuxLevel is greater than or equal to the value of the LuxLevel Out state.\nCharacteristic: Coefficient", @"The Light LC Regulator Kiu is a float32 state representing the integral coefficient that determines the integral part of the equation defining the output of the Light LC PI Feedback Regulator, when Light LC Ambient LuxLevel is less than LuxLevel Out.\nCharacteristic: Coefficient", @"The Light LC Regulator Kpd is a float32 state representing the proportional coefficient that determines the proportional part of the equation defining the output of the Light LC PI Feedback Regulator, when Light LC Ambient LuxLevel is greater than or equal to the value of the LuxLevel Out state.\nCharacteristic: Coefficient", @"The Light LC Regulator Kpu is a float32 state representing the proportional coefficient that determines the proportional part of the equation defining the output of the Light LC PI Feedback Regulator, when Light LC Ambient LuxLevel is less than the value of the LuxLevel Out state.\nCharacteristic: Coefficient"];
    NSArray *propertyIDs = @[@(DevicePropertyID_LightControlLightnessOn), @(DevicePropertyID_LightControlLightnessProlong), @(DevicePropertyID_LightControlLightnessStandby), @(DevicePropertyID_LightControlAmbientLuxLevelOn), @(DevicePropertyID_LightControlAmbientLuxLevelProlong), @(DevicePropertyID_LightControlAmbientLuxLevelStandby), @(DevicePropertyID_LightControlTimeFadeOn), @(DevicePropertyID_LightControlTimeRunOn), @(DevicePropertyID_LightControlTimeFade), @(DevicePropertyID_LightControlTimeProlong), @(DevicePropertyID_LightControlTimeFadeStandbyAuto), @(DevicePropertyID_LightControlTimeFadeStandbyManual), @(DevicePropertyID_LightControlTimeOccupancyDelay), @(DevicePropertyID_LightControlRegulatorAccuracy), @(DevicePropertyID_LightControlRegulatorKid), @(DevicePropertyID_LightControlRegulatorKiu), @(DevicePropertyID_LightControlRegulatorKpd), @(DevicePropertyID_LightControlRegulatorKpu)];
    for (int i=0; i<titleArray.count; i++) {
        NSNumber *propertyID = propertyIDs[i];
        NSString *propertyIDKey = [SigHelper.share getUint16String:propertyID.intValue];
        LCShowModel *model = [[LCShowModel alloc] initWithTitle:titleArray[i] detail:detailArray[i] propertyID:propertyID.intValue value:-1];
        if ([self.model.lightControlPropertyDictionary.allKeys containsObject:propertyIDKey]) {
            model.value = [self.model.lightControlPropertyDictionary[propertyIDKey] intValue];
        }
        [self.dataArray addObject:model];
    }    
}

- (void)getLCStatusAction {
    if (self.model.state != DeviceStateOutOfLine) {
        __weak typeof(self) weakSelf = self;
        NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
        [operationQueue addOperationWithBlock:^{
            //这个block语句块在子线程中执行
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [SDKLibCommand lightLCModeGetWithDestination:weakSelf.lightLCServerAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightLCModeStatus * _Nonnull responseMessage) {
                if (source == weakSelf.lightLCServerAddress) {
                    weakSelf.model.lightControlModeEnable = responseMessage.mode;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [weakSelf refreshModeUI];
                    });
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0));
            semaphore = dispatch_semaphore_create(0);
            [SDKLibCommand lightLCOMGetWithDestination:weakSelf.lightLCServerAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightLCOMStatus * _Nonnull responseMessage) {
                if (source == weakSelf.lightLCServerAddress) {
                    weakSelf.model.lightControlOccupancyModeEnable = responseMessage.mode;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [weakSelf refreshModeUI];
                    });
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0));
            [weakSelf lightLCLightOnOffGetAction];
        }];
    }
}

- (void)lightLCLightOnOffGetAction {
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand lightLCLightOnOffGetWithDestination:self.lightLCServerAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightLCLightOnOffStatus * _Nonnull responseMessage) {
        if (source == weakSelf.lightLCServerAddress) {
            weakSelf.model.lightControlLightOnOffState = responseMessage.presentLightOnOff;
            if (responseMessage.presentLightOnOff != responseMessage.targetLightOnOff && weakSelf.model.lightControlModeEnable == YES) {
                [weakSelf addDelayTimer];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                weakSelf.setLCLightOnOffSwitch.on = weakSelf.model.lightControlLightOnOffState;
            });
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        
    }];
}

- (void)addDelayTimer {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(lightLCLightOnOffGetAction) object:nil];
        NSInteger ms = 0;
        NSArray *propertyIDKeys = @[[SigHelper.share getUint16String:DevicePropertyID_LightControlTimeFadeOn], [SigHelper.share getUint16String:DevicePropertyID_LightControlTimeRunOn], [SigHelper.share getUint16String:DevicePropertyID_LightControlTimeFade], [SigHelper.share getUint16String:DevicePropertyID_LightControlTimeProlong], [SigHelper.share getUint16String:DevicePropertyID_LightControlTimeFadeStandbyAuto]];
        NSArray *defaultValue = @[@(2000), @(5000), @(2000), @(4000), @(3000)];
        for (int i=0; i<propertyIDKeys.count; i++) {
            if ([self.model.lightControlPropertyDictionary.allKeys containsObject:propertyIDKeys[i]]) {
                ms = ms + [self.model.lightControlPropertyDictionary[propertyIDKeys[i]] intValue];
            } else {
                ms = ms + [defaultValue[i] intValue];
            }
        }
        [self performSelector:@selector(lightLCLightOnOffGetAction) withObject:nil afterDelay:ms / 1000.0];
    });
}

- (IBAction)clickLCModeSwitch:(UISwitch *)sender {
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand lightLCModeSetWithDestination:self.lightLCServerAddress enable:sender.isOn retryCount:2 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightLCModeStatus * _Nonnull responseMessage) {
        
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (error) {
            sender.on = !sender.isOn;
            [weakSelf showTips:[NSString stringWithFormat:@"Set lightLCMode fail, error=%@", error.localizedDescription]];
        } else {
            weakSelf.model.lightControlModeEnable = sender.isOn;
            weakSelf.enableLCOccupancyModeSwitch.enabled = weakSelf.model.lightControlModeEnable;
            weakSelf.setLCLightOnOffSwitch.enabled = weakSelf.model.lightControlModeEnable;
            [SigDataSource.share saveLocationData];
        }
    }];
}

- (IBAction)clickLCOccupancyModeSwitch:(UISwitch *)sender {
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand lightLCOMSetWithDestination:self.lightLCServerAddress enable:sender.isOn retryCount:2 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightLCOMStatus * _Nonnull responseMessage) {
        
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (error) {
            sender.on = !sender.isOn;
            [weakSelf showTips:[NSString stringWithFormat:@"Set lightLCMode fail, error=%@", error.localizedDescription]];
        } else {
            weakSelf.model.lightControlOccupancyModeEnable = sender.isOn;
            [SigDataSource.share saveLocationData];
        }
    }];
}

- (IBAction)clickLCLightOnOffSwitch:(UISwitch *)sender {
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand lightLCLightOnOffSetWithDestination:self.lightLCServerAddress isOn:sender.isOn retryCount:2 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightLCLightOnOffStatus * _Nonnull responseMessage) {
        
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (error) {
            sender.on = !sender.isOn;
            [weakSelf showTips:[NSString stringWithFormat:@"Set lightLCLightOnOff fail, error=%@", error.localizedDescription]];
        } else {
            weakSelf.model.lightControlLightOnOffState = sender.isOn;
            [SigDataSource.share saveLocationData];
            [weakSelf addDelayTimer];
        }
    }];
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return self.dataArray.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (self.dataArray[section].isExpand) {
        return 1;
    } else {
        return 0;
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return 50.0f;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    UIView *headView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, SCREENWIDTH, 50)];
    UIControl *backView = [[UIControl alloc] initWithFrame:CGRectMake(15, 0, SCREENWIDTH - 30, 50)];
    UIImageView *turnImageView = [[UIImageView alloc] initWithFrame:CGRectMake(SCREENWIDTH - 50, 21, 12, 7)];
    turnImageView.image = [[UIImage imageNamed:@"fb_bottom"] imageWithRenderingMode:1];
    [backView addSubview:turnImageView];

    backView.tag = 1000 + section;
    headView.backgroundColor = [UIColor clearColor];
    backView.backgroundColor = UIColor.telinkBackgroundWhite;

    UILabel *titlelabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 0, SCREENWIDTH - 60 - 120, 50)];
    [backView addSubview:titlelabel];
    titlelabel.font = [UIFont boldSystemFontOfSize:15];
    titlelabel.text = self.dataArray[section].title;
    titlelabel.textColor = UIColor.telinkTitleGray;
    
    UILabel *valueLabel = [[UILabel alloc] initWithFrame:CGRectMake(10+SCREENWIDTH - 60 - 120, 0, 110, 50)];
    [backView addSubview:valueLabel];
    valueLabel.font = kFont(12);
    valueLabel.textAlignment = NSTextAlignmentRight;
    valueLabel.textColor = UIColor.telinkBlue;
    NSInteger value = self.dataArray[section].value;
    if (section > 13) {
        //float32
        NSData *data = [NSData dataWithBytes:&value length:4];
        const unsigned char *byteBuffer = [data bytes];
        Float32 f32 = *(Float32*)byteBuffer;
        valueLabel.text = value == -1 ? @"value: UNKNOWN" : [NSString stringWithFormat:@"value: %.01f", f32];
    } else {
        valueLabel.text = value == -1 ? @"value: UNKNOWN" : [NSString stringWithFormat:@"value: %ld", (long)value];
    }

    if (self.dataArray[section].isExpand) {
        turnImageView.image = [[UIImage imageNamed:@"fb_top"] imageWithRenderingMode:1];
        UIBezierPath *maskPath = [UIBezierPath bezierPathWithRoundedRect:backView.bounds byRoundingCorners:UIRectCornerTopLeft|UIRectCornerTopRight cornerRadii:CGSizeMake(6, 6)];
        CAShapeLayer *maskLayer = [[CAShapeLayer alloc] init];
        maskLayer.frame = backView.bounds;
        maskLayer.path = maskPath.CGPath;
        backView.layer.mask = maskLayer;
    }else{
        turnImageView.image = [[UIImage imageNamed:@"fb_bottom"] imageWithRenderingMode:1];
        UIBezierPath *maskPath = [UIBezierPath bezierPathWithRoundedRect:backView.bounds byRoundingCorners:UIRectCornerAllCorners cornerRadii:CGSizeMake(6, 6)];
        CAShapeLayer *maskLayer = [[CAShapeLayer alloc] init];
        maskLayer.frame = backView.bounds;
        maskLayer.path = maskPath.CGPath;
        backView.layer.mask = maskLayer;
    }
    [headView addSubview:backView];
    [backView addTarget:self action:@selector(didClickedSection:) forControlEvents:(UIControlEventTouchUpInside)];
    return headView;
}

- (void)didClickedSection:(UIControl *)view {
    NSInteger i = view.tag - 1000;
    self.dataArray[i].isExpand = !self.dataArray[i].isExpand;
    NSIndexSet *index = [NSIndexSet indexSetWithIndex:i];
    [_tableView reloadSections:index withRowAnimation:(UITableViewRowAnimationAutomatic)];
}

- (nullable UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section {
    UIView *headView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, SCREENWIDTH, 10)];
    headView.backgroundColor = [UIColor clearColor];
    return headView;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return 10;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    LightControlCell *cell = (LightControlCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(LightControlCell.class) forIndexPath:indexPath];
    LCShowModel *m = self.dataArray[indexPath.section];
    cell.detailLabel.text = m.detail;
    NSString *propertyIDKey = [SigHelper.share getUint16String:m.propertyID];
    __weak typeof(self) weakSelf = self;
    [cell.getButton addAction:^(UIButton *button) {
        [SDKLibCommand lightLCPropertyGetWithDestination:weakSelf.lightLCServerAddress propertyID:m.propertyID retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightLCPropertyStatus * _Nonnull responseMessage) {
            if (responseMessage.lightLCPropertyID == m.propertyID) {
                UInt32 *responseValue = 0;
                Byte *dataByte = (Byte *)responseMessage.lightLCPropertyValue.bytes;
                memcpy(&responseValue, dataByte, responseMessage.lightLCPropertyValue.length);
                m.value = (NSInteger)responseValue;
                weakSelf.model.lightControlPropertyDictionary[propertyIDKey] = @(m.value);
                [SigDataSource.share saveLocationData];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get %@ failed! error=%@", m.title, error]];
            } else {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get %@ success!", m.title]];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:0.5];
            });
        }];
    }];
    [cell.setButton addAction:^(UIButton *button) {
        //Set property parameter
        AlertItemModel *item = [[AlertItemModel alloc] init];
        item.itemType = ItemType_Input;
        item.headerString = @"please input content";
        item.defaultString = @"";
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:[NSString stringWithFormat:@"Set %@", m.title] detail:@"input new value" itemArray:@[item] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                NSString *ttlString = [alertView getTextFieldOfRow:0].text.removeAllSpace;
                if (indexPath.section > 13) {
                    //float32
                    BOOL result = [LibTools validateFloatString:ttlString];
                    if (result == NO || ttlString.length == 0) {
                        [weakSelf showTips:@"Please enter float32 string!"];
                        return;
                    }
                } else {
                    BOOL result = [LibTools validateNumberString:ttlString];
                    if (result == NO || ttlString.length == 0) {
                        [weakSelf showTips:@"Please enter decimal string!"];
                        return;
                    }
                }
                int value = [ttlString intValue];
                int length = 1;//see `+ (UInt8)valueLengthOfDevicePropertyID:(DevicePropertyID)propertyID;`
                if (indexPath.section == 13) {
                    if (value > 0xFF) {
                        [weakSelf showTips:@"The range of value is 0~255(0xFF)."];
                        return;
                    }
                } else if (indexPath.section <= 2) {
                    if (value > 0xFFFF) {
                        [weakSelf showTips:@"The range of value is 0~65535(0xFFFF)."];
                        return;
                    }
                    length = 2;
                } else if (indexPath.section <= 12) {
                    if (value > 0xFFFFFF) {
                        [weakSelf showTips:@"The range of value is 0~16777215(0xFFFFFF)."];
                        return;
                    }
                    length = 3;
                } else {
                    Float32 floatValue = [ttlString floatValue];
                    if (floatValue < 0.0 || floatValue > 1000.0) {
                        [weakSelf showTips:@"The range of value is 0.0~1000.0."];
                        return;
                    }
                    length = 4;
                }
                NSData *data = [NSData dataWithBytes:&value length:length];
                if (indexPath.section > 13) {
                    //float32
                    Float32 floatValue = [ttlString floatValue];
                    data = [NSData dataWithBytes:&floatValue length:4];
                }
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set %@...", m.title]];
                [SDKLibCommand lightLCPropertySetWithDestination:weakSelf.lightLCServerAddress propertyID:m.propertyID propertyValue:data retryCount:2 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightLCPropertyStatus * _Nonnull responseMessage) {
                    TelinkLogInfo(@"lightLCPropertySet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.lightLCServerAddress == source && m.propertyID == responseMessage.lightLCPropertyID) {
                        UInt32 *responseValue = 0;
                        Byte *dataByte = (Byte *)responseMessage.lightLCPropertyValue.bytes;
                        memcpy(&responseValue, dataByte, responseMessage.lightLCPropertyValue.length);
                        m.value = (NSInteger)responseValue;
                        weakSelf.model.lightControlPropertyDictionary[propertyIDKey] = @(value);
                        [SigDataSource.share saveLocationData];
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set %@ failed! error=%@", m.title, error]];
                    } else {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set %@ success!", m.title]];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:0.5];
                    });
                }];
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    }];
    return cell;
}

/** 设置分区圆角 */
- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {

    //    if ([cell isKindOfClass:[LZRowTableViewCell class]]) {

    // 圆角弧度半径
    CGFloat cornerRadius = 6.f;
    // 设置cell的背景色为透明，如果不设置这个的话，则原来的背景色不会被覆盖
    cell.backgroundColor = UIColor.clearColor;

    // 创建一个shapeLayer
    CAShapeLayer *layer = [[CAShapeLayer alloc] init];
    CAShapeLayer *backgroundLayer = [[CAShapeLayer alloc] init]; //显示选中
    // 创建一个可变的图像Path句柄，该路径用于保存绘图信息
    CGMutablePathRef pathRef = CGPathCreateMutable();
    // 获取cell的size
    // 第一个参数,是整个 cell 的 bounds, 第二个参数是距左右两端的距离,第三个参数是距上下两端的距离
    CGRect bounds = CGRectInset(cell.bounds, 15, 0);

    // CGRectGetMinY：返回对象顶点坐标
    // CGRectGetMaxY：返回对象底点坐标
    // CGRectGetMinX：返回对象左边缘坐标
    // CGRectGetMaxX：返回对象右边缘坐标
    // CGRectGetMidX: 返回对象中心点的X坐标
    // CGRectGetMidY: 返回对象中心点的Y坐标

    // 这里要判断分组列表中的第一行，每组section的第一行，每组section的中间行

    // CGPathAddRoundedRect(pathRef, nil, bounds, cornerRadius, cornerRadius);
    //        if ([tableView numberOfRowsInSection:indexPath.section] == 1) {
    //            CGPathMoveToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds));
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMinY(bounds), CGRectGetMidX(bounds), CGRectGetMinY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMinY(bounds), CGRectGetMaxX(bounds), CGRectGetMidY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMaxY(bounds), CGRectGetMidX(bounds), CGRectGetMaxY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds), CGRectGetMinX(bounds), CGRectGetMidY(bounds), cornerRadius);
    //            CGPathAddLineToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds));
    //        }
    //        else if (indexPath.row == 0) {
    //            // 初始起点为cell的左下角坐标
    //            CGPathMoveToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds));
    //            // 起始坐标为左下角，设为p，（CGRectGetMinX(bounds), CGRectGetMinY(bounds)）为左上角的点，设为p1(x1,y1)，(CGRectGetMidX(bounds), CGRectGetMinY(bounds))为顶部中点的点，设为p2(x2,y2)。然后连接p1和p2为一条直线l1，连接初始点p到p1成一条直线l，则在两条直线相交处绘制弧度为r的圆角。
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMinY(bounds), CGRectGetMidX(bounds), CGRectGetMinY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMinY(bounds), CGRectGetMaxX(bounds), CGRectGetMidY(bounds), cornerRadius);
    //            // 终点坐标为右下角坐标点，把绘图信息都放到路径中去,根据这些路径就构成了一块区域了
    //            CGPathAddLineToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMaxY(bounds));
    //
    //        }

    if (indexPath.row == [tableView numberOfRowsInSection:indexPath.section]-1) {
        // 初始起点为cell的左上角坐标
        CGPathMoveToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMinY(bounds));
        CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds), CGRectGetMidX(bounds), CGRectGetMaxY(bounds), cornerRadius);
        CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMaxY(bounds), CGRectGetMaxX(bounds), CGRectGetMidY(bounds), cornerRadius);
        // 添加一条直线，终点坐标为右下角坐标点并放到路径中去
        CGPathAddLineToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMinY(bounds));
    } else {
        // 添加cell的rectangle信息到path中（不包括圆角）
        CGPathAddRect(pathRef, nil, bounds);
    }
    // 把已经绘制好的可变图像路径赋值给图层，然后图层根据这图像path进行图像渲染render
    layer.path = pathRef;
    backgroundLayer.path = pathRef;
    // 注意：但凡通过Quartz2D中带有creat/copy/retain方法创建出来的值都必须要释放
    CFRelease(pathRef);
    // 按照shape layer的path填充颜色，类似于渲染render
    // layer.fillColor = [UIColor colorWithWhite:1.f alpha:0.8f].CGColor;
    layer.fillColor = UIColor.telinkBackgroundWhite.CGColor;

    // view大小与cell一致
    UIView *roundView = [[UIView alloc] initWithFrame:bounds];
    // 添加自定义圆角后的图层到roundView中
    [roundView.layer insertSublayer:layer atIndex:0];
    roundView.backgroundColor = UIColor.clearColor;
    // cell的背景view
    cell.backgroundView = roundView;

    // 以上方法存在缺陷当点击cell时还是出现cell方形效果，因此还需要添加以下方法
    // 如果你 cell 已经取消选中状态的话,那以下方法是不需要的.
    //    UIView *selectedBackgroundView = [[UIView alloc] initWithFrame:bounds];
    //    backgroundLayer.fillColor = [UIColor colorWithRed:0.90 green:0.90 blue:0.90 alpha:1].CGColor;
    //    [selectedBackgroundView.layer insertSublayer:backgroundLayer atIndex:0];
    //    selectedBackgroundView.backgroundColor = UIColor.clearColor;
    //    cell.selectedBackgroundView = selectedBackgroundView;
    //    }
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
