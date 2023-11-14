/********************************************************************************************************
 * @file     CMDViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CMDViewController.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"


/**
 Attention: more detail about ini packet can look document 《SIG Mesh iOS APP(OC版本)使用以及开发手册.docx》k.自定义ini指令组包结构说明
 */
@interface CMDViewController()<UITextViewDelegate>
@property (strong, nonatomic) IBOutlet UITextView *inTextView;
@property (strong, nonatomic) IBOutlet UITextView *showTextView;
@property (weak, nonatomic) IBOutlet UIButton *vendorOnButton;
@property (weak, nonatomic) IBOutlet UIButton *vendorOffButton;
@property (weak, nonatomic) IBOutlet UIButton *vendorOnOffGetButton;
@property (weak, nonatomic) IBOutlet UIButton *vendorOnNoackButton;
@property (weak, nonatomic) IBOutlet UIButton *vendorOffNoackButton;
@property (weak, nonatomic) IBOutlet UIButton *sigModelOnButton;
@property (weak, nonatomic) IBOutlet UIButton *devKeyAggregator;
@property (weak, nonatomic) IBOutlet UIButton *appKeyAggregator;
@property (weak, nonatomic) IBOutlet UIButton *sendButton;
@property (nonatomic,strong) NSString *sendString;
@property (nonatomic,strong) NSData *sendData;
@property (nonatomic,strong) NSString *logString;
@end

@implementation CMDViewController

- (IBAction)clickDefaultButton:(UIButton *)sender {
    UInt16 address = 2;
    if (SigDataSource.share.curNodes.count > 0) {
        address = SigDataSource.share.curNodes.firstObject.address;
    }
    switch (sender.tag) {
        case 1:
            self.inTextView.text = @"a3 ff 00 00 00 00 02 00 ff ff c2 11 02 c4 02 01 00";
            break;
        case 2:
            self.inTextView.text = @"a3 ff 00 00 00 00 02 00 ff ff c2 11 02 c4 02 00 01";
            break;
        case 3:
            self.inTextView.text = @"a3 ff 00 00 00 00 02 00 ff ff c1 11 02 c4 00";
            break;
        case 4:
            self.inTextView.text = @"a3 ff 00 00 00 00 02 00 ff ff c3 11 02 00 02 01 03";
            break;
        case 5:
            self.inTextView.text = @"a3 ff 00 00 00 00 02 00 ff ff c3 11 02 00 02 00 04";
            break;
        case 6:
            self.inTextView.text = @"a3 ff 00 00 00 00 02 00 ff ff 82 02 01 00";
            break;
        case 7:
        {
            SigOpcodesAggregatorItemModel *model1 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigDefaultTtlGet alloc] init]];
            SigOpcodesAggregatorItemModel *model2 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigFriendGet alloc] init]];
            SigOpcodesAggregatorItemModel *model3 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigRelayGet alloc] init]];
            NSArray *items = @[model1,model2,model3];
            SigOpcodesAggregatorSequence *message = [[SigOpcodesAggregatorSequence alloc] initWithElementAddress:SigDataSource.share.curNodes.firstObject.address items:items];
            NSString *parameters = [LibTools convertDataToHexStr:message.opCodeAndParameters];
            //默认发送给第一个node的地址。
            NSString *addressString = [NSString stringWithFormat:@"%02X %02X", address & 0xFF, (address >> 8)&0xFF];
            self.inTextView.text = [NSString stringWithFormat:@"a3 ff 00 00 00 00 02 00 %@%@", addressString, parameters];
        }
            break;
        case 8:
        {
            SigOpcodesAggregatorItemModel *model1 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigLightLightnessDefaultGet alloc] init]];
            SigOpcodesAggregatorItemModel *model2 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigLightLightnessRangeGet alloc] init]];
            NSArray *items = @[model1,model2];
            SigOpcodesAggregatorSequence *message = [[SigOpcodesAggregatorSequence alloc] initWithElementAddress:address items:items];
            NSString *parameters = [LibTools convertDataToHexStr:message.opCodeAndParameters];
            //默认发送给第一个node的地址。
            NSString *addressString = [NSString stringWithFormat:@"%02X %02X", address & 0xFF, (address >> 8)&0xFF];
            self.inTextView.text = [NSString stringWithFormat:@"a3 ff 00 00 00 00 02 00 %@%@", addressString, parameters];
            //==========test==========//
//            NSData *randomData = [LibTools createRandomDataWithLength:13];
//            SigMeshPrivateBeacon *beacon = [[SigMeshPrivateBeacon alloc] initWithKeyRefreshFlag:SigDataSource.share.curNetkeyModel.ivIndex.updateActive ivUpdateActive:SigDataSource.share.curNetkeyModel.ivIndex.updateActive ivIndex:[LibTools uint32From16String:SigDataSource.share.ivIndex] randomData:randomData usingNetworkKey:SigDataSource.share.curNetkeyModel];
//            [SigBearer.share sendBlePdu:beacon ofType:SigPduType_meshBeacon];
            //==========test==========//
        }
            break;

        default:
            break;
    }

    [self handleInTextView];
}

- (IBAction)clickSend:(UIButton *)sender {
    [self.inTextView resignFirstResponder];
    //修复不输入点send出现奔溃的问题。
    NSString *s = self.inTextView.text.removeAllSapceAndNewlines;
    if (s && s.length > 0 && [self validateString:s]) {
        [self handleInTextView];
        [self handleSendString];

        [self showNewLogMessage:[NSString stringWithFormat:@"Send: %@",self.sendData]];
        __weak typeof(self) weakSelf = self;
        [SDKLibCommand sendOpINIData:self.sendData successCallback:^(UInt16 source, UInt16 destination, SigMeshMessage * _Nonnull responseMessage) {
            NSString *str = [NSString stringWithFormat:@"Response: opcode=0x%x, parameters=%@",(unsigned int)responseMessage.opCode,responseMessage.parameters];
            TelinkLogVerbose(@"%@",str);
            [weakSelf showNewLogMessage:str];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogVerbose(@"finish");
        }];

//        CBCharacteristic *otaCharacteristic = [SigBluetooth.share getCharacteristicWithUUIDString:kOTA_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral];
//        //注意：以下block使用完成后需要手动置为nil.
//        //test1:接收到蓝牙数据的回调
//        [SigBluetooth.share setBluetoothDidUpdateValueCallback:^(CBPeripheral * _Nonnull peripheral, CBCharacteristic * _Nonnull characteristic, NSError * _Nullable error) {
//            if ([peripheral isEqual:SigBearer.share.getCurrentPeripheral] && [characteristic isEqual:otaCharacteristic]) {
//                if (error == nil) {
//                    TelinkLogDebug(@"CMD界面接收到数据<---0x%@",[LibTools convertDataToHexStr:characteristic.value]);
//                } else {
//                    TelinkLogError(@"CMD界面接收到错误:%@",error);
//                }
//            }
//        }];
//        //test2:发送写蓝牙数据接口
//        [SigBluetooth.share writeValue:self.sendData toPeripheral:SigBearer.share.getCurrentPeripheral forCharacteristic:otaCharacteristic type:CBCharacteristicWriteWithoutResponse];
//        //test3:发送写蓝牙数据接口
//        [SigBluetooth.share readCharachteristic:otaCharacteristic ofPeripheral:SigBearer.share.getCurrentPeripheral];


//        IniCommandModel *ini = [[IniCommandModel alloc] initVendorModelIniCommandWithNetkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyIndex:SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:1 address:2 opcode:0xC4 vendorId:kCompanyID responseOpcode:0xC4 needTid:NO tid:0 commandData:[LibTools nsstringToHex:@"383530333137536D6172745465616D2D50617373776F72642D322E34473034313233343062546573745F46616D696C793034313233343062546573745F46616D696C79313868747470733A2F2F7777772E6C656476616E63652E636F6D3030313368747470733A2F2F3139382E3132372E302E303034323262383031303030313031303432326238"]];
//        IniCommandModel *ini = [[IniCommandModel alloc] initVendorModelIniCommandWithNetkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyIndex:SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:1 address:2 opcode:0xC5 vendorId:kCompanyID responseOpcode:0xC4 tidPosition:2 tid:0 commandData:[LibTools nsstringToHex:@"7900383530333137536D6172745465616D2D50617373776F72642D322E34473034313233343062546573745F46616D696C793034313233343062546573745F46616D696C79313868747470733A2F2F7777772E6C656476616E63652E636F6D3030313368747470733A2F2F3139382E3132372E302E303034323262383031303030313031303432326238"]];
//        [SDKLibCommand sendIniCommandModel:ini successCallback:^(UInt16 source, UInt16 destination, SigMeshMessage * _Nonnull responseMessage) {
//            NSString *str = [NSString stringWithFormat:@"Response: opcode=0x%x, parameters=%@",responseMessage.opCode,responseMessage.parameters];
//            TelinkLogVerbose(@"%@",str);
//            [weakSelf showNewLogMessage:str];
//        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//            TelinkLogVerbose(@"finish");
//        }];

    } else {
        [self showTips:@"Please enter a valid hexadecimal data."];
    }
}

//显示:大写，去空格，加空格
- (void)handleInTextView{
    NSString *temString = [self.inTextView.text.uppercaseString removeAllSapceAndNewlines];
    temString = [temString insertSpaceNum:1 charNum:2];
    self.inTextView.text = temString;
}

//大写并去空格
- (void)handleSendString{
    self.sendString = [self.inTextView.text.uppercaseString removeAllSapceAndNewlines];
    NSData *data = [LibTools nsstringToHex:self.sendString];
    self.sendData = data;
}

- (BOOL)validateString:(NSString *)str{
    NSString *strRegex = @"^[0-9a-fA-F]{0,}$";
    NSPredicate *strPredicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@",strRegex];
    return [strPredicate evaluateWithObject:str];
}

- (void)showNewLogMessage:(NSString *)msg{
    NSDateFormatter *dformatter = [[NSDateFormatter alloc] init];
    dformatter.dateFormat =@"HH:mm:ss.SSS";
    self.logString = [self.logString stringByAppendingString:[NSString stringWithFormat:@"%@ %@\n",[dformatter stringFromDate:[NSDate date]],msg]];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.showTextView.text = self.logString;
        [self.showTextView scrollRangeToVisible:NSMakeRange(self.showTextView.text.length, 1)];
    });
}

- (void)normalSetting{
    [super normalSetting];
    [self configUI];
    self.logString = @"";
}

- (void)configUI{
    self.vendorOnButton.backgroundColor = UIColor.telinkButtonBlue;
    self.vendorOffButton.backgroundColor = UIColor.telinkButtonBlue;
    self.vendorOnOffGetButton.backgroundColor = UIColor.telinkButtonBlue;
    self.vendorOnNoackButton.backgroundColor = UIColor.telinkButtonBlue;
    self.vendorOffNoackButton.backgroundColor = UIColor.telinkButtonBlue;
    self.sigModelOnButton.backgroundColor = UIColor.telinkButtonBlue;
    self.devKeyAggregator.backgroundColor = UIColor.telinkButtonBlue;
    self.appKeyAggregator.backgroundColor = UIColor.telinkButtonBlue;
    self.sendButton.backgroundColor = UIColor.telinkButtonBlue;
    self.inTextView.layer.borderWidth = 1;
    self.inTextView.layer.borderColor = [[UIColor darkGrayColor] CGColor];
    self.inTextView.delegate = self;
    self.showTextView.layoutManager.allowsNonContiguousLayout = NO;

    UITapGestureRecognizer *gesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(hiddenKeyboard:)];
    [self.view addGestureRecognizer:gesture];
}

- (void)hiddenKeyboard:(UITapGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateEnded) {
        [self.inTextView resignFirstResponder];
    }
    sender.cancelsTouchesInView = NO;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    self.inTextView.text = @"";
    [self.inTextView resignFirstResponder];
}

- (BOOL)textViewShouldEndEditing:(UITextView *)textView{
    if ([self validateString:textView.text.removeAllSapceAndNewlines]) {
        [self handleInTextView];
        return YES;
    }
    [self showTips:@"Please enter a valid hexadecimal data."];
    return NO;
}

-(void)dealloc{
    SigBluetooth.share.bluetoothDidUpdateValueCallback = nil;
    TelinkLogDebug(@"");
}

@end
