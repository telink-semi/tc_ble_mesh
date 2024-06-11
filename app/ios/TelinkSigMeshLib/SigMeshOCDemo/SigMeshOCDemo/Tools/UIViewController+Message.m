/********************************************************************************************************
 * @file     UIViewController+Message.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/8/6
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

#import "UIViewController+Message.h"

@implementation UIViewController (Message)

/**
 弹出UIAlertController

 @param title   标题
 @param message 消息
 @param sure    点击确定按钮
 */
- (void)showAlertSureWithTitle:(NSString *)title message:(NSString *)message sure:(void (^) (UIAlertAction *action))sure {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *action = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleDefault handler:sure];
        [alert addAction:action];
        [self presentViewController:alert animated:YES completion:nil];
    });
}

/**
 弹出UIAlerController

 @param title   标题
 @param message 消息
 @param sure    点击确定
 @param cancel  点击取消
 */
- (void)showAlertSureAndCancelWithTitle:(NSString *)title message:(NSString *)message sure:(void (^) (UIAlertAction *action))sure cancel:(void (^) (UIAlertAction *action))cancel {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *action = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleDefault handler:sure];
        UIAlertAction *revoke = [UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:cancel];
        [alert addAction:action];
        [alert addAction:revoke];
        [self presentViewController:alert animated:YES completion:nil];
    });
}

/**
 弹出UIAlertController

 @param actionOneTitle 标题
 @param handlerOne     点击标题的事件
 */
- (void)showSheetOneAction:(NSString *)actionOneTitle handlerOne:(void(^)(UIAlertAction *action))handlerOne {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertSheet = [UIAlertController alertControllerWithTitle:nil message:nil preferredStyle:UIAlertControllerStyleActionSheet];
        UIAlertAction *actionOne = [UIAlertAction actionWithTitle:actionOneTitle style:UIAlertActionStyleDefault handler:handlerOne];
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:nil];
        [alertSheet addAction:actionOne];
        [alertSheet addAction:cancelAction];
        [self presentViewController:alertSheet animated:YES completion:nil];
    });
}

/**
 弹出UIAlerController

 @param actionOneTitle 第一标题
 @param actionTwoTitle 第二个标题
 @param handlerOne     第一个标题点击事件
 @param handlerTwo     第二个标题点击事件
 */
- (void)showSheetTwoAction:(NSString *)actionOneTitle actionTwo:(NSString *)actionTwoTitle handlerOne:(void(^)(UIAlertAction *action))handlerOne handlerTwo:(void (^) (UIAlertAction *action))handlerTwo {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertSheet = [UIAlertController alertControllerWithTitle:nil message:nil preferredStyle:UIAlertControllerStyleActionSheet];
        UIAlertAction *actionOne = [UIAlertAction actionWithTitle:actionOneTitle style:UIAlertActionStyleDefault handler:handlerOne];
        UIAlertAction *actionTwo = [UIAlertAction actionWithTitle:actionTwoTitle style:UIAlertActionStyleDefault handler:handlerTwo];
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:nil];
        [alertSheet addAction:actionOne];
        [alertSheet addAction:actionTwo];
        [alertSheet addAction:cancelAction];
        [self presentViewController:alertSheet animated:YES completion:nil];
    });
}

@end
