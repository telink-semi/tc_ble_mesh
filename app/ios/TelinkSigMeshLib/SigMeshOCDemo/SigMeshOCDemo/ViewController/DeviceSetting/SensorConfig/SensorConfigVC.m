/********************************************************************************************************
 * @file     SensorConfigVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2024/1/18
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

#import "SensorConfigVC.h"
#import "SensorConfigCell.h"
#import "UIButton+extension.h"
#import "CustomAlertView.h"
#import "NSString+extension.h"

@interface SensorConfigVC ()
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UITextField *publishAddressTF;
@property (weak, nonatomic) IBOutlet UITextField *periodTF;
@property (nonatomic, strong) NSMutableArray <SigSensorDataModel *>*source;
@property (nonatomic, assign) UInt16 sensorAddress;

@end

@implementation SensorConfigVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.title = @"Sensor Control";
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SensorConfigCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SensorConfigCell.class)];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.tableView.sectionHeaderTopPadding = 0;
    }
    self.sensorAddress = [self.model getElementModelWithModelIds:@[@(kSigModel_SensorServer_ID)]].unicastAddress;
    self.source = [NSMutableArray arrayWithArray:self.model.sensorDataArray];
    [self refreshPublishUI];
}

- (void)refreshPublishUI {
    SigPublishModel *publish = [self.model getModelIDModelWithModelID:kSigModel_SensorServer_ID].publish;
    _publishAddressTF.text = publish == nil ? @"0000" : publish.address;
    _periodTF.text = publish == nil ? @"0" : [NSString stringWithFormat:@"%ld", publish.period.numberOfSteps * publish.period.resolution];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SensorConfigCell *cell = (SensorConfigCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SensorConfigCell.class) forIndexPath:indexPath];
    SigSensorDataModel *sensorData = self.source[indexPath.row];
    [cell setSensorDataModel:sensorData];
    [cell setSensorDescriptorModel:[self.model getSigSensorDescriptorModelWithPropertyID:sensorData.propertyID]];
    [cell setSensorCadenceModel:[self.model getSigSensorCadenceModelWithPropertyID:sensorData.propertyID]];
    __weak typeof(self) weakSelf = self;
    [cell.getSensorDataButton addAction:^(UIButton *button) {
        [weakSelf clickGetSensorDataButtonWithSigSensorDataModel:sensorData];
    }];
    [cell.getSensorDescriptorButton addAction:^(UIButton *button) {
        [weakSelf clickGetSensorDescriptorButtonWithSigSensorDataModel:sensorData];
    }];
    [cell.getSensorCadenceButton addAction:^(UIButton *button) {
        [weakSelf clickGetSensorCadenceButtonWithSigSensorDataModel:sensorData];
    }];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)clickGetSensorDataButtonWithSigSensorDataModel:(SigSensorDataModel *)model {
    //sensorGet
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand sensorGetWithDestination:self.sensorAddress propertyID:model.propertyID retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSensorStatus * _Nonnull responseMessage) {
        if (weakSelf.sensorAddress == source && responseMessage.sensorDataModelArray) {
            [weakSelf.model updateSigSensorDataModelArray:responseMessage.sensorDataModelArray];
            [SigDataSource.share saveLocationData];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        
    }];
}

- (void)clickGetSensorDescriptorButtonWithSigSensorDataModel:(SigSensorDataModel *)model {
    //sensorDescriptorGet
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand sensorDescriptorGetWithDestination:self.sensorAddress propertyID:model.propertyID retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSensorDescriptorStatus * _Nonnull responseMessage) {
        if (weakSelf.sensorAddress == source && responseMessage.descriptorModels) {
            [weakSelf.model updateSigSensorDescriptorModelArray:responseMessage.descriptorModels];
            [SigDataSource.share saveLocationData];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        
    }];
}

- (void)clickGetSensorCadenceButtonWithSigSensorDataModel:(SigSensorDataModel *)model {
    //sensorCadenceGet
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand sensorCadenceGetWithDestination:self.sensorAddress propertyID:model.propertyID retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSensorCadenceStatus * _Nonnull responseMessage) {
        if (weakSelf.sensorAddress == source && responseMessage.cadenceModel) {
            [weakSelf.model updateSigSensorCadenceModelArray:@[responseMessage.cadenceModel]];
            [SigDataSource.share saveLocationData];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        
    }];
}

- (IBAction)clickGetPublishButton:(UIButton *)sender {
    //ModelPublicationGet
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand configModelPublicationGetWithDestination:self.model.address elementAddress:self.sensorAddress modelIdentifier:self.model.publishModelID companyIdentifier:0 retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
        if (weakSelf.model.address == source && responseMessage.status == SigConfigMessageStatus_success) {
            [weakSelf.model updateSigConfigModelPublicationStatusToDeviceConfig:responseMessage];
            [weakSelf performSelectorOnMainThread:@selector(refreshPublishUI) withObject:nil waitUntilDone:YES];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        
    }];
}

- (IBAction)clickSetPublishButton:(UIButton *)sender {
    NSString *ttlString = self.publishAddressTF.text.removeAllSpace;
    BOOL result = [LibTools validateHex:ttlString];
    if (result == NO || ttlString.length == 0) {
        // need input hexadecimal char.
        [self showTips:@"Please input a hexadecimal string."];
        return;
    }
    UInt16 value = [LibTools uint16From16String:ttlString];
    if (value > 0xFFFF || (value >= 0x8000 && value <= 0xBFFF)) {
        [self showTips:@"The range of publication address is 0~0x7FFF and 0xC000~0xFFFF."];
        return;
    }
    NSString *msString = self.periodTF.text.removeAllSpace;
    result = [LibTools validateNumberString:msString];
    if (result == NO || msString.length == 0) {
        // need input number char.
        [self showTips:@"Please input a number string."];
        return;
    }
    NSInteger ms = [msString integerValue];
    SigPeriodModel *period = [[SigPeriodModel alloc] initWithMillisecond:ms];
    
    [ShowTipsHandle.share show:@"Set Publication Address..."];
    UInt16 elementAddress = [self.model.publishAddress.firstObject intValue];
    TelinkLogInfo(@"send request for edit publish list");
    SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:5 intervalSteps:0];
    SigPublish *publish = [[SigPublish alloc] initWithDestination:value withKeyIndex:SigDataSource.share.curAppkeyModel.index friendshipCredentialsFlag:0 ttl:0xff periodSteps:period.numberOfSteps periodResolution:period.resolution retransmit:retransmit];//ttl=0xFF(表示采用节点默认参数)
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand configModelPublicationSetWithDestination:self.model.address publish:publish elementAddress:elementAddress modelIdentifier:self.model.publishModelID companyIdentifier:0 retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"editPublishList callback");
        if (responseMessage.status == SigConfigMessageStatus_success && responseMessage.elementAddress == elementAddress) {
            [weakSelf.model updateSigConfigModelPublicationStatusToDeviceConfig:responseMessage];
            [SigDataSource.share saveLocationData];
            [weakSelf performSelectorOnMainThread:@selector(refreshPublishUI) withObject:nil waitUntilDone:YES];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set Publication Address failed! error=%@", error]];
        } else {
            [ShowTipsHandle.share show:@"Set Publication Address success!"];
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share delayHidden:0.5];
        });
    }];

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
