/********************************************************************************************************
 * @file     UIImage+Extension.m
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

#import "UIImage+Extension.h"

@implementation UIImage (Extension)

+(UIImage*) createImageWithColor:(UIColor*) color
{
    CGRect rect=CGRectMake(0.0f, 0.0f, 1.0f, 1.0f);
    UIGraphicsBeginImageContext(rect.size);
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context, [color CGColor]);
    CGContextFillRect(context, rect);
    UIImage *theImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return theImage;
}
+ (UIImage *)createQRImageWithData:(NSData *)data rate:(CGFloat)rate {
    UIImage *resized = nil;

    CIFilter *filter = [CIFilter filterWithName:@"CIQRCodeGenerator"];

    [filter setDefaults];

    [filter setValue:data forKey:@"inputMessage"];

    CIImage *outputImage = [filter outputImage];

    CIContext *context = [CIContext contextWithOptions:nil];

    CGImageRef cgImage = [context createCGImage:outputImage
                                       fromRect:[outputImage extent]];

    UIImage *image = [UIImage imageWithCGImage:cgImage
                                         scale:1.f
                                   orientation:UIImageOrientationUp];

    CGFloat width = image.size.width * rate;
    CGFloat height = image.size.height * rate;

    UIGraphicsBeginImageContext(CGSizeMake(width, height));
    CGContextRef context2 = UIGraphicsGetCurrentContext();
    CGContextSetInterpolationQuality(context2, kCGInterpolationNone);
    [image drawInRect:CGRectMake(0, 0, width, height)];
    resized = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();

    CGImageRelease(cgImage);

    return resized;
}

+ (UIImage *)createQRImageWithString:(NSString *)string rate:(CGFloat)rate{
    return [UIImage createQRImageWithData:[string dataUsingEncoding:NSUTF8StringEncoding] rate:rate];
}

- (UIImage *)imageRotatedOnDegrees:(CGFloat)degrees {
    // Follow ing code can only rotate images on 90, 180, 270.. degrees.
    CGFloat roundedDegrees = (CGFloat)(round(degrees / 90.0) * 90.0);
    BOOL sameOrientationType = ((NSInteger)roundedDegrees) % 180 == 0;
    CGFloat radians = M_PI * roundedDegrees / 180.0;
    CGSize newSize = sameOrientationType ? self.size : CGSizeMake(self.size.height, self.size.width);
    
    UIGraphicsBeginImageContext(newSize);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    CGImageRef cgImage = self.CGImage;
    if (ctx == NULL || cgImage == NULL) {
        UIGraphicsEndImageContext();
        return self;
    }
    
    CGContextTranslateCTM(ctx, newSize.width / 2.0, newSize.height / 2.0);
    CGContextRotateCTM(ctx, radians);
    CGContextScaleCTM(ctx, 1, -1);
    CGPoint origin = CGPointMake(-(self.size.width / 2.0), -(self.size.height / 2.0));
    CGRect rect = CGRectZero;
    rect.origin = origin;
    rect.size = self.size;
    CGContextDrawImage(ctx, rect, cgImage);
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image ?: self;
}

@end
