/********************************************************************************************************
 * @file     ColorModelCell.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/11/23
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

#import "ColorModelCell.h"

@interface ColorModelCell ()<ColorPickerViewDelegate>
@property (weak, nonatomic) IBOutlet UIImageView *currentColorImageView;
@property (weak, nonatomic) IBOutlet UILabel *showVLabel;
@property (weak, nonatomic) IBOutlet UISlider *VSlider;//H:0~1,S:0~1,V:0~1。
@property (weak, nonatomic) IBOutlet UILabel *showRLabel;
@property (weak, nonatomic) IBOutlet UISlider *RSlider;
@property (weak, nonatomic) IBOutlet UILabel *showGLabel;
@property (weak, nonatomic) IBOutlet UISlider *GSlider;
@property (weak, nonatomic) IBOutlet UILabel *showBLabel;
@property (weak, nonatomic) IBOutlet UISlider *BSlider;
@property (weak, nonatomic) IBOutlet UILabel *showHLabel;
@property (weak, nonatomic) IBOutlet UISlider *HSlider;
@property (weak, nonatomic) IBOutlet UILabel *showSLabel;
@property (weak, nonatomic) IBOutlet UISlider *SSlider;
@property (weak, nonatomic) IBOutlet UILabel *showLLabel;
@property (weak, nonatomic) IBOutlet UISlider *LSlider;
@end

@implementation ColorModelCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    //cornerRadius
    self.currentColorImageView.layer.cornerRadius = 8;
    //borderWidth
    self.currentColorImageView.layer.borderWidth = 1;
    //borderColor
    self.currentColorImageView.layer.borderColor = [UIColor lightGrayColor].CGColor;
    self.colorPicker.delegate = self;
    [self setColorModel:[UIColor whiteColor]];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];
}

- (IBAction)changedValueOfV:(UISlider *)sender {
    self.hsvModel.value = sender.value;
    [self setHsvModel:self.hsvModel];
    [self callbackCurrentColor];
}

- (IBAction)changedValueOfR:(UISlider *)sender {
    self.rgbModel.red = sender.value;
    [self setRgbModel:self.rgbModel];
    [self callbackCurrentColor];
}

- (IBAction)changedValueOfG:(UISlider *)sender {
    self.rgbModel.green = sender.value;
    [self setRgbModel:self.rgbModel];
    [self callbackCurrentColor];
}

- (IBAction)changedValueOfB:(UISlider *)sender {
    self.rgbModel.blud = sender.value;
    [self setRgbModel:self.rgbModel];
    [self callbackCurrentColor];
}

- (IBAction)changedValueOfH:(UISlider *)sender {
    //self.hslModel.hue为0~1，需装换为(0~359)/360，即hue不为360。
    CGFloat hue = (sender.value * 359.999999)/360.0;
    self.hslModel.hue = hue;
    [self setHslModel:self.hslModel];
    [self callbackCurrentColor];
}

- (IBAction)changedValueOfS:(UISlider *)sender {
    self.hslModel.saturation = sender.value;
    [self setHslModel:self.hslModel];
    [self callbackCurrentColor];
}

- (IBAction)changedValueOfL:(UISlider *)sender {
    self.hslModel.lightness = sender.value;
    [self setHslModel:self.hslModel];
    [self callbackCurrentColor];
}

- (void)colorPickerView:(ColorPickerView *)colorPickerView didPickColorWithColor:(UIColor *)color hue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness {
    //注意：以ColorPickerView返回的hue、saturation加上当前界面的brightness组成新的color，才是发送给设备的color。
    color = [UIColor colorWithHue:hue saturation:saturation brightness:self.hsvModel.value alpha:self.hsvModel.alpha];
    [self setColorModel:color];
    [self refreshUI];
    [self callbackCurrentColor];
}

- (void)setColorModel:(UIColor *)colorModel {
    _colorModel = colorModel;
    _rgbModel = [ColorManager getRGBWithColor:colorModel];
    _hslModel = [ColorManager getHSLWithColor:colorModel];
    _hsvModel = [ColorManager getHSVWithColor:colorModel];
    [self refreshUI];
}

- (void)setRgbModel:(RGBModel *)rgbModel {
    _colorModel = [UIColor colorWithRed:rgbModel.red green:rgbModel.green blue:rgbModel.blud alpha:rgbModel.alpha];;
    _rgbModel = rgbModel;
    _hslModel = [ColorManager getHSLWithColor:_colorModel];
    _hsvModel = [ColorManager getHSVWithColor:_colorModel];
    [self refreshUI];
}

- (void)setHslModel:(HSLModel *)hslModel {
    _colorModel = [ColorManager getRGBWithHSLColor:hslModel];;
    _rgbModel = [ColorManager getRGBWithColor:_colorModel];
    _hslModel = hslModel;
    _hsvModel = [ColorManager getHSVWithColor:_colorModel];
    [self refreshUI];
}

- (void)changeUIWithHslModelLightSliderValue:(CGFloat)lightSliderValue {
    self.hslModel.lightness = lightSliderValue;
    [self setHslModel:self.hslModel];
}

- (void)setHsvModel:(HSVModel *)hsvModel {
    _colorModel = [ColorManager getUIColorWithHSVColor:hsvModel];;
    _rgbModel = [ColorManager getRGBWithColor:_colorModel];
    _hslModel = [ColorManager getHSLWithColor:_colorModel];
    _hsvModel = hsvModel;
    [self refreshUI];
}

- (void)refreshUI {
    self.currentColorImageView.backgroundColor = self.colorModel;
    if (!self.VSlider.isTracking) {
        self.VSlider.value = self.hsvModel.value;
    }
    self.showVLabel.text = [NSString stringWithFormat:@"V(%.0f)",(self.hsvModel.value*100)];

    if (!self.RSlider.isTracking) {
        self.RSlider.value = self.rgbModel.red;
    }
    if (!self.GSlider.isTracking) {
        self.GSlider.value = self.rgbModel.green;
    }
    if (!self.BSlider.isTracking) {
        self.BSlider.value = self.rgbModel.blud;
    }
    self.showRLabel.text = [NSString stringWithFormat:@"R(%.0f)",(self.rgbModel.red*255)];
    self.showGLabel.text = [NSString stringWithFormat:@"G(%.0f)",(self.rgbModel.green*255)];
    self.showBLabel.text = [NSString stringWithFormat:@"B(%.0f)",(self.rgbModel.blud*255)];

    if (!self.HSlider.isTracking) {
        self.HSlider.value = self.hslModel.hue;
    }
    if (!self.SSlider.isTracking) {
        self.SSlider.value = self.hslModel.saturation;
    }
    if (!self.LSlider.isTracking) {
        self.LSlider.value = self.hslModel.lightness;
    }
    self.showHLabel.text = [NSString stringWithFormat:@"H(%.0f)",(self.hslModel.hue*360)];
    self.showSLabel.text = [NSString stringWithFormat:@"S(%.0f)",(self.hslModel.saturation*100)];
    self.showLLabel.text = [NSString stringWithFormat:@"L(%.0f)",(self.hslModel.lightness*100)];
}

- (void)callbackCurrentColor {
    if ([self.delegate respondsToSelector:@selector(colorModelCell:didChangedColorWithColor:rgbModel:hsvModel:hslModel:)]) {
        [self.delegate colorModelCell:self didChangedColorWithColor:self.colorModel rgbModel:self.rgbModel hsvModel:self.hsvModel hslModel:self.hslModel];
    }
}

@end
