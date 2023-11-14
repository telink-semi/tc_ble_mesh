/********************************************************************************************************
 * @file     ScanView.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/11/19
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

#import "ScanView.h"
#import <AVFoundation/AVFoundation.h>
NSString * const SuccessScanQRCodeNotification = @"SuccessScanQRCodeNotification";
NSString * const ScanQRCodeMessageKey = @"ScanQRCodeMessageKey";

@interface ScanView () <AVCaptureMetadataOutputObjectsDelegate>
@property (nonatomic, strong) AVCaptureSession * session;
@property (nonatomic, strong) AVCaptureDeviceInput * input;
@property (nonatomic, strong) AVCaptureMetadataOutput * output;
@property (nonatomic, strong) AVCaptureVideoPreviewLayer * scanView;

@property (nonatomic, strong) CAShapeLayer * maskLayer;
@property (nonatomic, strong) CAShapeLayer * shadowLayer;
@property (nonatomic, strong) CAShapeLayer * scanRectLayer;

@property (nonatomic, assign) CGRect scanRect;
@property (nonatomic, strong) UILabel * remind;
@end

@implementation ScanView

- (void)scanDataViewBackBlock:(void (^)(id content))block {
    self.scanDataBlock = block;
}

- (instancetype)initWithFrame: (CGRect)frame
{
    frame = SCREENBOUNDS;
    if (self = [super initWithFrame: frame]) {
        self.backgroundColor = [UIColor colorWithWhite: 0.f alpha: 0.2f];
        if (self.isCameraAvailable && self.isRearCameraAvailable) {
            [self.layer addSublayer: self.scanView];
        }
        [self setupScanRect];
        [self addSubview: self.remind];
        self.layer.masksToBounds = YES;

    }
    return self;
}

#pragma mark - 摄像头和相册相关的公共类
// 判断设备是否有摄像头
- (BOOL)isCameraAvailable{
    return [UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera];
}

// 前面的摄像头是否可用
- (BOOL)isFrontCameraAvailable{
    return [UIImagePickerController isCameraDeviceAvailable:UIImagePickerControllerCameraDeviceFront];
}

// 后面的摄像头是否可用
- (BOOL)isRearCameraAvailable{
    return [UIImagePickerController isCameraDeviceAvailable:UIImagePickerControllerCameraDeviceRear];
}

#pragma mark - life
/**
 *  释放前停止会话
 */
- (void)dealloc
{
    [self stop];
}


#pragma mark - operate
/**
 *  开始视频会话
 */
- (void)start
{
    if (self.isCameraAvailable && self.isRearCameraAvailable) {
        [self.session startRunning];
    }
}

/**
 *  停止视频会话
 */
- (void)stop {
    if (self.isCameraAvailable && self.isRearCameraAvailable) {
        [self.session stopRunning];
    }
}


#pragma mark - lazy load
#pragma mark >> capture I/O <<
/**
 *  会话对象
 */
- (AVCaptureSession *)session
{
    if (!_session) {
        _session = [AVCaptureSession new];

        [_session setSessionPreset: AVCaptureSessionPreset1920x1080];    //高质量采集
        [self setupIODevice];
    }
    return _session;
}

/**
 *  视频输入设备
 */
- (AVCaptureDeviceInput *)input
{
    if (!_input) {
        AVCaptureDevice * device = [AVCaptureDevice defaultDeviceWithMediaType: AVMediaTypeVideo];
        _input = [AVCaptureDeviceInput deviceInputWithDevice: device error: nil];
    }
    return _input;
}

/**
 *  数据输出对象
 */
- (AVCaptureMetadataOutput *)output
{
    if (!_output) {
        _output = [[AVCaptureMetadataOutput alloc] init];
        [_output setMetadataObjectsDelegate: self queue: dispatch_get_main_queue()];
    }
    return _output;
}

/**
 *  扫描视图
 */
- (AVCaptureVideoPreviewLayer *)scanView
{
    if (!_scanView) {
        _scanView = [AVCaptureVideoPreviewLayer layerWithSession: self.session];
        _scanView.videoGravity = AVLayerVideoGravityResizeAspectFill;
        _scanView.frame = self.bounds;
    }
    return _scanView;
}

#pragma mark >> common <<
/**
 *  扫描范围
 */
- (CGRect)scanRect
{
    if (CGRectEqualToRect(_scanRect, CGRectZero)) {
        CGRect rectOfInterest = self.output.rectOfInterest;
        CGFloat yOffset = rectOfInterest.size.width - rectOfInterest.origin.x;
        CGFloat xOffset = 1 - 2 * SCANSPACEOFFSET;
        _scanRect = CGRectMake(rectOfInterest.origin.y * SCREEN_WIDTH, rectOfInterest.origin.x * SCREEN_HEIGHT, xOffset * SCREEN_WIDTH, yOffset * SCREEN_HEIGHT);
    }
    return _scanRect;
}

/**
 *  提示文本
 */
- (UILabel *)remind
{
    if (!_remind) {
        CGRect textRect = self.scanRect;
        textRect.origin.y += CGRectGetHeight(textRect) + 20;
        textRect.size.height = 50.f;

        _remind = [[UILabel alloc] initWithFrame: textRect];
        _remind.font = [UIFont systemFontOfSize: 15.f * SCREEN_WIDTH / 375.f];
        _remind.textColor = [UIColor whiteColor];
        _remind.textAlignment = NSTextAlignmentCenter;
        _remind.text = @"Put the QR code into the box, and it can be automatically scanned";
        _remind.backgroundColor = [UIColor clearColor];
        _remind.numberOfLines = 0;
    }
    return _remind;
}

#pragma mark >> layer <<
/**
 *  扫描框
 */
- (CAShapeLayer *)scanRectLayer
{
    if (!_scanRectLayer) {
        CGRect scanRect = self.scanRect;
        scanRect.origin.x -= 1;
        scanRect.origin.y -= 1;
        scanRect.size.width += 2;
        scanRect.size.height += 2;

        _scanRectLayer = [CAShapeLayer layer];
        _scanRectLayer.path = [UIBezierPath bezierPathWithRect: scanRect].CGPath;
        _scanRectLayer.fillColor = [UIColor clearColor].CGColor;
        _scanRectLayer.strokeColor = [UIColor orangeColor].CGColor;
    }
    return _scanRectLayer;
}

/**
 *  阴影层
 */
- (CAShapeLayer *)shadowLayer
{
    if (!_shadowLayer) {
        _shadowLayer = [CAShapeLayer layer];
        _shadowLayer.path = [UIBezierPath bezierPathWithRect: self.bounds].CGPath;
        _shadowLayer.fillColor = [UIColor colorWithWhite: 0 alpha: 0.75].CGColor;
        _shadowLayer.mask = self.maskLayer;
    }
    return _shadowLayer;
}

/**
 *  遮掩层
 */
- (CAShapeLayer *)maskLayer
{
    if (!_maskLayer) {
        _maskLayer = [CAShapeLayer layer];
        _maskLayer = [self generateMaskLayerWithRect: SCREENBOUNDS exceptRect: self.scanRect];
    }
    return _maskLayer;
}


#pragma mark - generate
/**
 *  生成空缺部分rect的layer
 */
- (CAShapeLayer *)generateMaskLayerWithRect: (CGRect)rect exceptRect: (CGRect)exceptRect
{
    CAShapeLayer * maskLayer = [CAShapeLayer layer];
    if (!CGRectContainsRect(rect, exceptRect)) {
        return nil;
    }
    else if (CGRectEqualToRect(rect, CGRectZero)) {
        maskLayer.path = [UIBezierPath bezierPathWithRect: rect].CGPath;
        return maskLayer;
    }

    CGFloat boundsInitX = CGRectGetMinX(rect);
    CGFloat boundsInitY = CGRectGetMinY(rect);
    CGFloat boundsWidth = CGRectGetWidth(rect);
    CGFloat boundsHeight = CGRectGetHeight(rect);

    CGFloat minX = CGRectGetMinX(exceptRect);
    CGFloat maxX = CGRectGetMaxX(exceptRect);
    CGFloat minY = CGRectGetMinY(exceptRect);
    CGFloat maxY = CGRectGetMaxY(exceptRect);
    CGFloat width = CGRectGetWidth(exceptRect);

    /** 添加路径*/
    UIBezierPath * path = [UIBezierPath bezierPathWithRect: CGRectMake(boundsInitX, boundsInitY, minX, boundsHeight)];
    [path appendPath: [UIBezierPath bezierPathWithRect: CGRectMake(minX, boundsInitY, width, minY)]];
    [path appendPath: [UIBezierPath bezierPathWithRect: CGRectMake(maxX, boundsInitY, boundsWidth - maxX, boundsHeight)]];
    [path appendPath: [UIBezierPath bezierPathWithRect: CGRectMake(minX, maxY, width, boundsHeight - maxY)]];
    maskLayer.path = path.CGPath;

    return maskLayer;
}


#pragma mark - setup
/**
 *  配置输入输出设置
 */
- (void)setupIODevice
{
    if ([self.session canAddInput: self.input]) {
        [_session addInput: _input];
    }
    if ([self.session canAddOutput: self.output]) {
        [_session addOutput: _output];
        _output.metadataObjectTypes = @[AVMetadataObjectTypeQRCode];
    }
}

/**
 *  配置扫描范围
 */
- (void)setupScanRect
{
    CGFloat size = SCREEN_WIDTH * (1 - 2 * SCANSPACEOFFSET);
    CGFloat minY = (SCREEN_HEIGHT - size) * 0.5 / SCREEN_HEIGHT;
    CGFloat maxY = (SCREEN_HEIGHT + size) * 0.5 / SCREEN_HEIGHT;
    self.output.rectOfInterest = CGRectMake(minY, SCANSPACEOFFSET, maxY, 1 - SCANSPACEOFFSET * 2);

    [self.layer addSublayer: self.shadowLayer];
    [self.layer addSublayer: self.scanRectLayer];
}


#pragma mark - touch
///**
// *  点击空白处停止扫描
// */
//- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
//{
//    [self stop];
//    [self removeFromSuperview];
//}


#pragma mark - AVCaptureMetadataOutputObjectsDelegate
/**
 *  二维码扫描数据返回
 */

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputMetadataObjects:(NSArray *)metadataObjects fromConnection:(AVCaptureConnection *)connection {
    if (metadataObjects.count > 0) {
        [self stop];

        AVMetadataMachineReadableCodeObject * metadataObject = metadataObjects[0];
        //        id obj = [metadataObject valueForKeyPath:@"_internal.basicDescriptor"];
        if (metadataObject.type != AVMetadataObjectTypeQRCode) {
            return;
        }
        //        NSData *data = [obj valueForKeyPath:@"BarcodeRawData"];
//        TelinkLogDebug(@"metadataObject->%@", metadataObject.stringValue);
        if (self.scanDataBlock && metadataObject.stringValue) {
            self.scanDataBlock(metadataObject.stringValue);

//            [self removeFromSuperview];
        } else {
            [[NSNotificationCenter defaultCenter] postNotificationName:SuccessScanQRCodeNotification object:self userInfo: @{ ScanQRCodeMessageKey: metadataObject.stringValue }];
        }
    }
}

@end
