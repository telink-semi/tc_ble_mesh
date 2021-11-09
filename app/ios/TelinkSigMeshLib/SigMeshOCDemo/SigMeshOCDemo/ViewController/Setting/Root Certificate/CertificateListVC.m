/********************************************************************************************************
 * @file     CertificateListVC.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/10/27
 *
 * @par      
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

#import "CertificateListVC.h"
#import "MeshOTAItemCell.h"
#import "UIButton+extension.h"

@interface CertificateListVC ()
@property (weak, nonatomic) IBOutlet UIButton *saveButton;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*dataArray;
@property (nonatomic, assign) NSInteger selectIndex;

@end

@implementation CertificateListVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Root Certificate";
    self.dataArray = [NSMutableArray arrayWithArray:[LibTools getAllFileNameWithFileType:@"der"]];
    _selectIndex = -1;
    //demo v3.3.4新增certificate-base Provision使用的默认根证书文件名，demo不重新赋值则默认使用PTS的root.der。
    NSString *rootCertificateName = [[NSUserDefaults standardUserDefaults] valueForKey:kRootCertificateName];
    if (rootCertificateName) {
        for (int i=0; i<_dataArray.count; i++) {
            if ([rootCertificateName isEqualToString:_dataArray[i]]) {
                _selectIndex = i;
                break;
            }
        }
    }
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_MeshOTAItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_MeshOTAItemCellID];
}

- (IBAction)clickSaveButton:(UIButton *)sender {
    if (_selectIndex == -1) {
        SigDataSource *tem = [[SigDataSource alloc] init];
        SigDataSource.share.defaultRootCertificateData = tem.defaultRootCertificateData;
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kRootCertificateName];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [self.tableView reloadData];
        [self showTips:@"Change to default Root Certificate successful!"];
    } else {
        NSString *selectName = self.dataArray[self.selectIndex];
        NSData *selectData = [LibTools getDataWithFileName:selectName fileType:nil];
        if (selectData && selectData.length > 0) {
            SigDataSource.share.defaultRootCertificateData = selectData;
            [[NSUserDefaults standardUserDefaults] setValue:selectName forKey:kRootCertificateName];
            [[NSUserDefaults standardUserDefaults] synchronize];
            [self.tableView reloadData];
            [self showTips:@"Change Root Certificate successful!"];
        } else {
            [self showTips:[NSString stringWithFormat:@"Change Root Certificate failed! '%@' is read fail!",selectName]];
        }
    }
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

#pragma mark - UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.dataArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(MeshOTAItemCell.class) forIndexPath:indexPath];
    [self configureCell:cell forRowAtIndexPath:indexPath];
    
    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = (MeshOTAItemCell *)cell;
    __weak typeof(self) weakSelf = self;
    itemCell.titleLabel.text = self.dataArray[indexPath.row];
    itemCell.selectButton.selected = indexPath.row == _selectIndex;
    [itemCell.selectButton addAction:^(UIButton *button) {
        weakSelf.selectIndex = button.selected ? indexPath.row : -1;
        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    }];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
    NSString *rootCertificateName = [[NSUserDefaults standardUserDefaults] valueForKey:kRootCertificateName];
    return [NSString stringWithFormat:@"Last select name:%@",rootCertificateName];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = [tableView cellForRowAtIndexPath:indexPath];
    if (!itemCell.selectButton.selected) {
        _selectIndex = indexPath.row;
    } else {
        _selectIndex = -1;
    }
    [self.tableView reloadData];
}

@end
