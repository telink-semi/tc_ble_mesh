/********************************************************************************************************
 * @file     SceneDetailViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/26
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

#import "SceneDetailViewController.h"
#import "ActionViewController.h"
#import "UIButton+extension.h"
#import "SceneDetailCell.h"
#import "ElementActionView.h"
#import "NSString+extension.h"

@interface SceneDetailViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*allNodes;
@end

@implementation SceneDetailViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SceneDetailCell *cell = (SceneDetailCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SceneDetailCell.class) forIndexPath:indexPath];
    SigNodeModel *node = self.allNodes[indexPath.row];
    [cell updateContent:node];
    NSArray *actionList = [self getActionListOfNode:node];
    for (UIView *view in cell.contentView.subviews) {
        if ([view isMemberOfClass:[ElementActionView class]]) {
            [view removeFromSuperview];
        }
    }
    if (actionList.count > 0) {
        for (int i=0; i<actionList.count; i++) {
            ActionModel *a = actionList[i];
            ElementActionView *view = [[NSBundle mainBundle] loadNibNamed:@"ElementActionView" owner:self options:nil].firstObject;
            view.frame = CGRectMake(20, 89-20+44.0*i, [UIScreen mainScreen].bounds.size.width-40, 44.0);
            view.nameLabel.text = [NSString stringWithFormat:@"element: 0x%04X", a.address];
            view.selectButton.selected = [self.model.actionList containsObject:a];
            __weak typeof(self) weakSelf = self;
            __block SigSceneRegisterStatus *response = nil;
            [view.selectButton addAction:^(UIButton *button) {
                if (a.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
                    if ([weakSelf.model.actionList containsObject:a]) {
                        //移除
                        [ShowTipsHandle.share show:@"delete scene···"];
                        [DemoCommand delSceneWithAddress:a.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                            TelinkLogDebug(@"delSceneWithAddress ResponseModel=%@",responseMessage);
                            response = responseMessage;
                        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                            if (error == nil && response && response.statusCode == SigSceneResponseStatus_success) {
                                [weakSelf removeActionModelWithElementAddress:a.address];
                                [ShowTipsHandle.share show:@"delete scene success"];
                                [ShowTipsHandle.share delayHidden:0.5];
                            } else {
                                [ShowTipsHandle.share hidden];
                                if (error) {
                                    [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
                                } else {
                                    if (response && response.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
                                        [weakSelf showTips:@"Scene Register Full"];
                                    } else if (response && response.statusCode == SigSceneResponseStatus_sceneNotFound) {
                                        [weakSelf showTips:@"Scene Not Found"];
                                    } else {
                                        [weakSelf showTips:[NSString stringWithFormat:@"SigSceneRegisterStatus error code=0x%X", response.statusCode]];
                                    }
                                }
                            }
                        }];
                    } else {
                        //添加
                        [ShowTipsHandle.share show:@"add scene···"];
                        [DemoCommand saveSceneWithAddress:a.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                            TelinkLogDebug(@"saveSceneWithAddress ResponseModel=%@",responseMessage.parameters);
                            response = responseMessage;
                        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                            if (error == nil && response && response.statusCode == SigSceneResponseStatus_success) {
                                [weakSelf addActionModelWithElementAddress:a.address];
                                [ShowTipsHandle.share show:@"add scene success"];
                                [ShowTipsHandle.share delayHidden:0.5];
                            } else {
                                [ShowTipsHandle.share hidden];
                                if (error) {
                                    [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
                                } else {
                                    if (response && response.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
                                        [weakSelf showTips:@"Scene Register Full"];
                                    } else if (response && response.statusCode == SigSceneResponseStatus_sceneNotFound) {
                                        [weakSelf showTips:@"Scene Not Found"];
                                    } else {
                                        [weakSelf showTips:[NSString stringWithFormat:@"SigSceneRegisterStatus error code=0x%X", response.statusCode]];
                                    }
                                }
                            }
                        }];
                    }
                }
            }];
            [view.refreshButton addAction:^(UIButton *button) {
                //更新
                [ShowTipsHandle.share show:@"update scene···"];
                [DemoCommand saveSceneWithAddress:a.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"saveSceneWithAddress ResponseModel=%@",responseMessage.parameters);
                    response = responseMessage;
                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                    if (error == nil && response && response.statusCode == SigSceneResponseStatus_success) {
                        if (![weakSelf.model.actionList containsObject:a]) {
                            [weakSelf addActionModelWithElementAddress:a.address];
                        }
                        [ShowTipsHandle.share show:@"update scene success"];
                        [ShowTipsHandle.share delayHidden:0.5];
                    } else {
                        [ShowTipsHandle.share hidden];
                        if (error) {
                            [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
                        } else {
                            if (response && response.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
                                [weakSelf showTips:@"Scene Register Full"];
                            } else if (response && response.statusCode == SigSceneResponseStatus_sceneNotFound) {
                                [weakSelf showTips:@"Scene Not Found"];
                            } else {
                                [weakSelf showTips:[NSString stringWithFormat:@"SigSceneRegisterStatus error code=0x%X", response.statusCode]];
                            }
                        }
                    }
                }];
            }];
            [cell.contentView addSubview:view];
        }
    }
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.allNodes.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    SigNodeModel *node = self.allNodes[indexPath.row];
    NSArray *actionList = [self getActionListOfNode:node];
    return 89 + 44.0*actionList.count;
}

- (void)addActionModelWithElementAddress:(UInt16)elementAddress {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:elementAddress];
    ActionModel *action = [[ActionModel alloc] initWithNode:node elementAddress:elementAddress];
    [self.model.actionList addObject:action];
    [SigDataSource.share saveLocationData];
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)removeActionModelWithElementAddress:(UInt16)elementAddress {
    for (ActionModel *action in self.model.actionList) {
        if (action.address == elementAddress) {
            [self.model.actionList removeObject:action];
            [SigDataSource.share saveLocationData];
            [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            break;
        }
    }
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)normalSetting{
    [super normalSetting];
    [self setTitle:@"Scene Setting" subTitle:self.model.name];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SceneDetailCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SceneDetailCell.class)];

    self.allNodes = [NSMutableArray arrayWithArray:SigDataSource.share.curNodes];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_edit_bar"] style:UIBarButtonItemStylePlain target:self action:@selector(clickEditNameButton)];
    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)clickEditNameButton {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:@"Update Scene Name" message:@"please input content" preferredStyle: UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new scene name";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *sceneName = [[alertVc textFields] objectAtIndex:0].text;
        sceneName = sceneName.removeHeadAndTailSpacePro;
        TelinkLogInfo(@"new scene is %@", sceneName);
        if (sceneName == nil || sceneName.length == 0) {
            [weakSelf showTips:@"Scene name can not be empty!"];
            return;
        }
        self.model.name = sceneName;
        [SigDataSource.share saveLocationData];
        [self setTitle:@"Scene Setting" subTitle:self.model.name];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
}

- (void)reloadView{
    [self.allNodes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
        return [(SigNodeModel *)obj1 address] > [(SigNodeModel *)obj2 address];
    }];
    [self.tableView reloadData];
}

- (NSArray <ActionModel *>*)getActionListOfNode:(SigNodeModel *)node {
    NSMutableArray *mArray = [NSMutableArray array];
    NSArray *sceneElementAddresses = [NSArray arrayWithArray:node.sceneAddress];
    for (NSNumber *elementAddress in sceneElementAddresses) {
        ActionModel *action = [[ActionModel alloc] initWithNode:node elementAddress:elementAddress.intValue];
        [mArray addObject:action];
    }
    return mArray;
}

-(void)dealloc{
    TelinkLogDebug(@"%s",__func__);
}

@end
