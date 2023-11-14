/********************************************************************************************************
 * @file     OOBListVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/5/12
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

#import "OOBListVC.h"
#import "OOBItemCell.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"
#import "EditOOBVC.h"
#import "ChooseOOBFileVC.h"
#import "NSString+extension.h"

#define CellIdentifiers_OOBItemCellID  @"OOBItemCell"

@interface OOBListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigOOBModel *>*sourceArray;

@end

@implementation OOBListVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"OOB List";
    self.sourceArray = [NSMutableArray arrayWithArray:SigDataSource.share.OOBList];
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_OOBItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_OOBItemCellID];
//    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd:)];
    UIButton *but = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 20, 20)];
    [but setBackgroundImage:[UIImage imageNamed:@"ic_add"] forState:UIControlStateNormal];
    [but addTarget:self action:@selector(clickAdd:) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithCustomView:but];
    UIBarButtonItem *rightItem2 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemTrash target:self action:@selector(clickDeleteAll)];
    self.navigationItem.rightBarButtonItems = @[rightItem1,rightItem2];
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
}

- (void)clickAdd:(UIButton *)button{
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"Select mode" message:nil preferredStyle:UIAlertControllerStyleActionSheet];

    UIAlertAction *alertT = [UIAlertAction actionWithTitle:@"Manual Input" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [weakSelf addOobByManualInput];
    }];
    UIAlertAction *alertT2 = [UIAlertAction actionWithTitle:@"Import from file" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [weakSelf addOobByImportFromFile];
    }];
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"Cancel");
    }];
    [actionSheet addAction:alertT];
    [actionSheet addAction:alertT2];
    [actionSheet addAction:alertF];
    actionSheet.popoverPresentationController.sourceView = button;
    actionSheet.popoverPresentationController.sourceRect =  button.frame;

    [self presentViewController:actionSheet animated:YES completion:nil];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)addOobByManualInput {
    EditOOBVC *vc = [[EditOOBVC alloc] init];
    vc.isAdd = YES;
    __weak typeof(self) weakSelf = self;
    [vc setBackOobModel:^(SigOOBModel * _Nonnull oobModel) {
        [SigDataSource.share addAndUpdateSigOOBModel:oobModel];
        [weakSelf.sourceArray addObject:oobModel];
        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)addOobByImportFromFile {
    ChooseOOBFileVC *vc = [[ChooseOOBFileVC alloc] init];
    __weak typeof(self) weakSelf = self;
    [vc setBackOobFileInfo:^(NSData * _Nonnull oobFileData, NSString * _Nonnull oobFileName) {
        [weakSelf handleOobData:oobFileData oobFileName:oobFileName];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)handleOobData:(NSData *)oobData oobFileName:(NSString *)oobFileName {
    NSString *oobString = [[NSString alloc] initWithData:oobData encoding:NSUTF8StringEncoding];
    NSArray *arr = [oobString componentsSeparatedByString:@"\n"];
    NSMutableArray *validArray = [NSMutableArray array];
    for (NSString *str in arr) {
        if ([str rangeOfString:@"//"].location == NSNotFound) {
            if ([str rangeOfString:@" "].location != NSNotFound) {
                NSArray *array = [str componentsSeparatedByString:@" "];
                if (array && array.count == 2) {
                    NSString *uuidString = array.firstObject;
                    NSString *oobString = array.lastObject;
                    uuidString = uuidString.removeHeadAndTailSpacePro;
                    oobString = oobString.removeHeadAndTailSpacePro;
                    if (uuidString.length == 32 && (oobString.length == 32 || oobString.length == 64)) {
                        SigOOBModel *oobModel = [[SigOOBModel alloc] initWithSourceType:OOBSourceTypeImportFromFile UUIDString:uuidString OOBString:oobString];
                        [validArray addObject:oobModel];
                    }
                }
            }
        }
    }
    [SigDataSource.share addAndUpdateSigOOBModelList:validArray];
    for (SigOOBModel *model in validArray) {
        if ([self.sourceArray containsObject:model]) {
            [self.sourceArray replaceObjectAtIndex:[self.sourceArray indexOfObject:model] withObject:model];
        } else {
            [self.sourceArray addObject:model];
        }
    }
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)clickDeleteAll {
    __weak typeof(self) weakSelf = self;
    [self showAlertSureAndCancelWithTitle:@"Hits" message:@"Wipe all oob info?" sure:^(UIAlertAction *action) {
        [SigDataSource.share deleteAllSigOOBModel];
        weakSelf.sourceArray = [NSMutableArray array];
        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    } cancel:^(UIAlertAction *action) {

    }];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            SigOOBModel *model = self.sourceArray[indexPath.row];
            TelinkLogDebug(@"%@",indexPath);
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete oob data, UUID:%@",model.UUIDString];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                [SigDataSource.share deleteSigOOBModel:model];
                [weakSelf.sourceArray removeObject:model];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            } cancel:^(UIAlertAction *action) {

            }];
        }
    }
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sourceArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    OOBItemCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(OOBItemCell.class) forIndexPath:indexPath];
    SigOOBModel *model = self.sourceArray[indexPath.row];
    [cell setModel:model];
    __weak typeof(self) weakSelf = self;
    [cell.editButton addAction:^(UIButton *button) {
        EditOOBVC *vc = [[EditOOBVC alloc] init];
        vc.isAdd = NO;
        [vc setOobModel:model];
        [vc setBackOobModel:^(SigOOBModel * _Nonnull oobModel) {
            if (![model.UUIDString isEqualToString:oobModel.UUIDString]) {
                [SigDataSource.share deleteSigOOBModel:model];
            }
            [SigDataSource.share addAndUpdateSigOOBModel:oobModel];
            [weakSelf.sourceArray replaceObjectAtIndex:indexPath.row withObject:oobModel];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }];
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 55;
}

@end
