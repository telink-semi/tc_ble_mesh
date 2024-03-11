/********************************************************************************************************
 * @file     OTSBaseModel.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/11/29
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^CompleteCallback)(NSData * __nullable data);
typedef void(^ProgressCallback)(float progress);

/// MARK - OTS协议相关

/// 1.10Application Error Codes
/// Table 1.2: Attribute Protocol Application Error codes defined by this service
/// - seeAlso: OTS_v10.pdf  (page.11)
typedef enum : UInt8 {
    /// An attempt was made to write a value that is invalid or not supported by this
    /// Server for a reason other than the attribute permissions.
    OTSApplicationErrorCode_writeRequestRejected = 0x80,
    /// An attempt was made to read or write to an Object Metadata characteristic
    /// while the Current Object was an Invalid Object (see Section 1.8).
    OTSApplicationErrorCode_objectNotSelected = 0x81,
    /// The Server is unable to service the Read Request or Write Request because
    /// it exceeds the concurrency limit of the service.
    OTSApplicationErrorCode_concurrencyLimitExceeded = 0x82,
    /// The requested object name was rejected because the name was already in use
    /// by an existing object on the Server.
    OTSApplicationErrorCode_objectNameAlreadyExists = 0x83,
} OTSApplicationErrorCode;

/// Table 3.9: Object Action Control Point Procedure Requirements
/// - seeAlso: OTS_v10.pdf  (page.24)
typedef enum : UInt8 {
    OACPOpCode_Create = 0x01,
    OACPOpCode_Delete = 0x02,
    OACPOpCode_CalculateChecksum = 0x03,
    OACPOpCode_Execute = 0x04,
    OACPOpCode_Read = 0x05,
    OACPOpCode_Write = 0x06,
    OACPOpCode_Abort = 0x07,
    OACPOpCode_ResponseCode = 0x60,
    //0x00,0x08-0x5F,0x61-0xFF,Reserved for Future Use.
} OACPOpCode;

/// Table 3.11: List of OACP Result Codes
/// - seeAlso: OTS_v10.pdf  (page.25)
typedef enum : UInt8 {
    /// Response for successful operation.
    OACPResultCode_Success = 0x01,
    /// Response if unsupported Op Code is received.
    OACPResultCode_OpCodeNotSupported = 0x02,
    /// Response if Parameter received does not meet the requirements of the service.
    OACPResultCode_InvalidParameter = 0x03,
    /// Response if the number of octets requested via the value of the Length parameter
    /// or Size parameter (as applicable) exceeds the available memory or processing
    /// capabilities of the Server.
    OACPResultCode_InsufficientResources = 0x04,
    /// Response if the requested OACP procedure cannot be performed because the
    /// Current Object is an Invalid Object.
    OACPResultCode_InvalidObject = 0x05,
    /// Response if the requested procedure could not be performed because an Object
    /// Transfer Channel was not available for use.
    OACPResultCode_ChannelUnavailable = 0x06,
    /// Response if the object type specified in the OACP procedure Type parameter is
    /// not supported by the Server.
    OACPResultCode_UnsupportedType = 0x07,
    /// Response if the requested procedure is not permitted according to the properties
    /// of the Current Object (refer to Section 3.2.8).
    OACPResultCode_ProcedureNotPermitted = 0x08,
    /// Response if the Current Object is temporarily locked by the Server.
    OACPResultCode_ObjectLocked = 0x09,
    /// Response if the requested procedure failed for any reason other than those
    /// enumerated in this table.
    OACPResultCode_OperationFailed = 0x0A,
    /// 0x00,0x0B-0xFF,Reserved for Future Use.
} OACPResultCode;

/// Table 3.22: Object List Control Point Procedure Requirements
/// - seeAlso: OTS_v10.pdf  (page.37)
typedef enum : UInt8 {
    OLCPOpCode_First = 0x01,
    OLCPOpCode_Last = 0x02,
    OLCPOpCode_Previous = 0x03,
    OLCPOpCode_Next = 0x04,
    OLCPOpCode_GoTo = 0x05,
    OLCPOpCode_Order = 0x06,
    OLCPOpCode_RequestNumberOfObjects = 0x07,
    OLCPOpCode_ClearMarking = 0x08,
    OLCPOpCode_ResponseCode = 0x70,
    //0x00,0x09-0x6F,0x71-0xFF,Reserved for Future Use.
} OLCPOpCode;

/// Table 3.23: List Sort Order - Enumeration of Parameter Values
/// - seeAlso: OTS_v10.pdf  (page.39)
typedef enum : UInt8 {
    //Order the list by object name, ascending (Note 1)
    ListSortOrder_objectNameAscending = 0x01,
    //Order the list by object type, ascending
    ListSortOrder_objectTypeAscending = 0x02,
    //Order the list by object current size, ascending
    ListSortOrder_objectCurrentSizeAscending = 0x03,
    //Order the list by object first-created timestamp, ascending
    ListSortOrder_objectFirstCreatedTimestampAscending = 0x04,
    //Order the list by object last-modified timestamp, ascending
    ListSortOrder_objectLastCreatedTimestampAscending = 0x05,
    //Order the list by object name, descending (Note 1)
    ListSortOrder_objectNameDescending = 0x11,
    //Order the list by object type, descending
    ListSortOrder_objectTypeDescending = 0x12,
    //Order the list by object current size, descending
    ListSortOrder_objectCurrentSizeDescending = 0x13,
    //Order the list by object first-created timestamp, descending
    ListSortOrder_objectFirstCreatedTimestampDescending = 0x14,
    //Order the list by object last-modified timestamp, descending
    ListSortOrder_objectLastCreatedTimestampDescending = 0x15,
    //0x00,0x06-0x10,0x16-0xFF,Reserved for Future Use.
} ListSortOrder;

/// Table 3.25: List of OLCP Result Codes and Response Parameters
/// - seeAlso: OTS_v10.pdf  (page.40)
typedef enum : UInt8 {
    /// Response for successful operation.
    OLCPResultCode_Success = 0x01,
    /// Response if unsupported Op Code is received.
    OLCPResultCode_OpCodeNotSupported = 0x02,
    /// Response if Parameter received does not meet the requirements of the service.
    OLCPResultCode_InvalidParameter = 0x03,
    /// Response if the requested procedure failed for a reason other than those enumerated below.
    OLCPResultCode_OperationFailed = 0x04,
    /// Response if the requested procedure attempted to select an object beyond the first object or
    /// beyond the last object in the current list.
    OLCPResultCode_OutOfBounds = 0x05,
    /// Response if the requested procedure failed due to too many objects in the current list.
    OLCPResultCode_TooManyObjects = 0x06,
    /// Response if the requested procedure failed due to there being zero objects in the current list.
    OLCPResultCode_NoObject = 0x07,
    /// Response if the requested procedure failed due to there being no object with the requested Object ID.
    OLCPResultCode_ObjectIDNotFound = 0x08,
    /// 0x00,0x09-0xFF,Reserved for Future Use.
} OLCPResultCode;


/// MARK - 16-bit UUID Numbers Document
///
/// - seeAlso: 16-bit UUID Numbers Document.pdf  (page.12)
typedef enum : UInt16 {

    // Allocation type = GATT Descriptor
    Bluetooth16BitsUUID_PrimaryService = 0x2800,
    Bluetooth16BitsUUID_SecondaryService = 0x2801,
    Bluetooth16BitsUUID_Include = 0x2802,
    Bluetooth16BitsUUID_Characteristic = 0x2803,
    Bluetooth16BitsUUID_CharacteristicExtendedProperties = 0x2900,
    Bluetooth16BitsUUID_CharacteristicUserDescription = 0x2901,
    Bluetooth16BitsUUID_ClientCharacteristicConfiguration = 0x2902,
    Bluetooth16BitsUUID_ServerCharacteristicConfiguration = 0x2903,
    Bluetooth16BitsUUID_CharacteristicPresentationFormat = 0x2904,
    Bluetooth16BitsUUID_CharacteristicAggregateFormat = 0x2905,
    Bluetooth16BitsUUID_ValidRange = 0x2906,
    Bluetooth16BitsUUID_ExternalReportReference = 0x2907,
    Bluetooth16BitsUUID_ReportReference = 0x2908,
    Bluetooth16BitsUUID_NumberOfDigitals = 0x2909,
    Bluetooth16BitsUUID_ValueTriggerSetting = 0x290A,
    Bluetooth16BitsUUID_EnvironmentalSensingConfiguration = 0x290B,
    Bluetooth16BitsUUID_EnvironmentalSensingMeasurement = 0x290C,
    Bluetooth16BitsUUID_EnvironmentalSensingTriggerSetting = 0x290D,
    Bluetooth16BitsUUID_TimeTriggerSetting = 0x290E,
    Bluetooth16BitsUUID_CompleteBR_EDRTransportBlockData = 0x290F,

    // Allocation type = GATT Characteristic and Object Type

    Bluetooth16BitsUUID_DeviceName = 0x2A00,
    Bluetooth16BitsUUID_Appearance = 0x2A01,
    Bluetooth16BitsUUID_PeripheralPrivacyFlag = 0x2A02,
    Bluetooth16BitsUUID_ReconnectionAddress = 0x2A03,
    Bluetooth16BitsUUID_PeripheralPreferredConnectionParameters = 0x2A04,
    Bluetooth16BitsUUID_ServiceChanged = 0x2A05,
    Bluetooth16BitsUUID_AlertLevel = 0x2A06,
    Bluetooth16BitsUUID_TxPowerLevel = 0x2A07,
    Bluetooth16BitsUUID_DateTime = 0x2A08,
    Bluetooth16BitsUUID_DayOfWeek = 0x2A09,
    Bluetooth16BitsUUID_DayDateTime = 0x2A0A,
    Bluetooth16BitsUUID_ExactTime256 = 0x2A0C,
    Bluetooth16BitsUUID_DSTOffset = 0x2A0D,
    Bluetooth16BitsUUID_TimeZone = 0x2A0E,
    Bluetooth16BitsUUID_LocalTimeInformation = 0x2A0F,
    Bluetooth16BitsUUID_TimeWithDST = 0x2A11,
    Bluetooth16BitsUUID_TimeAccuracy = 0x2A12,
    Bluetooth16BitsUUID_TimeSource = 0x2A13,
    Bluetooth16BitsUUID_ReferenceTimeInformation = 0x2A14,
    Bluetooth16BitsUUID_TimeUpdateControlPoint = 0x2A16,
    Bluetooth16BitsUUID_TimeUpdateState = 0x2A17,
    Bluetooth16BitsUUID_GlucoseMeasurement = 0x2A18,
    Bluetooth16BitsUUID_BatteryLevel = 0x2A19,
    Bluetooth16BitsUUID_TemperatureMeasurement = 0x2A1C,
    Bluetooth16BitsUUID_TemperatureType = 0x2A1D,
    Bluetooth16BitsUUID_IntermediateTemperature = 0x2A1E,
    Bluetooth16BitsUUID_MeasurementInterval = 0x2A21,
    Bluetooth16BitsUUID_BootKeyboardInputReport = 0x2A22,
    Bluetooth16BitsUUID_SystemID = 0x2A23,
    Bluetooth16BitsUUID_ModelNumberString = 0x2A24,
    Bluetooth16BitsUUID_SerialNumberString = 0x2A25,
    Bluetooth16BitsUUID_FirmwareRevisionString = 0x2A26,
    Bluetooth16BitsUUID_HardwareRevisionString = 0x2A27,
    Bluetooth16BitsUUID_SoftwareRevisionString = 0x2A28,
    Bluetooth16BitsUUID_ManufacturerNameString = 0x2A29,
    Bluetooth16BitsUUID_IEEE11073_20601RegulatoryCertificationDataList = 0x2A2A,
    Bluetooth16BitsUUID_CurrentTime = 0x2A2B,
    Bluetooth16BitsUUID_ScanRefresh = 0x2A31,
    Bluetooth16BitsUUID_BootKeyboardOutputReport = 0x2A32,
    Bluetooth16BitsUUID_BootMouseInputReport = 0x2A33,
    Bluetooth16BitsUUID_GlucoseMeasurementContext = 0x2A34,
    Bluetooth16BitsUUID_BloodPressureMeasurement = 0x2A35,
    Bluetooth16BitsUUID_IntermediateCuffPressure = 0x2A36,
    Bluetooth16BitsUUID_HeartRateMeasurement = 0x2A37,
    Bluetooth16BitsUUID_BodySensorLocation = 0x2A38,
    Bluetooth16BitsUUID_HeartRateControlPoint = 0x2A39,
    Bluetooth16BitsUUID_AlertStatus = 0x2A3F,
    Bluetooth16BitsUUID_RingerControlPoint = 0x2A40,
    Bluetooth16BitsUUID_RingerSetting = 0x2A41,
    Bluetooth16BitsUUID_AlertCategoryIDBitMask = 0x2A42,
    Bluetooth16BitsUUID_AlertCategoryID = 0x2A43,
    Bluetooth16BitsUUID_AlertNotificationControlPoint = 0x2A44,
    Bluetooth16BitsUUID_UnreadAlertStatus = 0x2A45,
    Bluetooth16BitsUUID_NewAlert = 0x2A46,
    Bluetooth16BitsUUID_SupportedNewAlertCategory = 0x2A47,
    Bluetooth16BitsUUID_SupportedUnreadAlertCategory = 0x2A48,
    Bluetooth16BitsUUID_BloodPressureFeature = 0x2A49,
    Bluetooth16BitsUUID_HIDInformation = 0x2A4A,
    Bluetooth16BitsUUID_ReportMap = 0x2A4B,
    Bluetooth16BitsUUID_HIDControlPoint = 0x2A4C,
    Bluetooth16BitsUUID_Report = 0x2A4D,
    Bluetooth16BitsUUID_ProtocolMode = 0x2A4E,
    Bluetooth16BitsUUID_ScanIntervalWindow = 0x2A4F,
    Bluetooth16BitsUUID_PnPID = 0x2A50,
    Bluetooth16BitsUUID_GlucoseFeature = 0x2A51,
    Bluetooth16BitsUUID_RecordAccessControlPoint = 0x2A52,
    Bluetooth16BitsUUID_RSCMeasurement = 0x2A53,
    Bluetooth16BitsUUID_RSCFeature = 0x2A54,
    Bluetooth16BitsUUID_SCControlPoint = 0x2A55,
    Bluetooth16BitsUUID_Aggregate = 0x2A5A,
    Bluetooth16BitsUUID_CSCMeasurement = 0x2A5B,
    Bluetooth16BitsUUID_CSCFeature = 0x2A5C,
    Bluetooth16BitsUUID_SensorLocation = 0x2A5D,
    Bluetooth16BitsUUID_PLXSpot_CheckMeasurement = 0x2A5E,
    Bluetooth16BitsUUID_PLXContinuousMeasurement = 0x2A5F,
    Bluetooth16BitsUUID_PLXFeatures = 0x2A60,
    Bluetooth16BitsUUID_CyclingPowerMeasurement = 0x2A63,
    Bluetooth16BitsUUID_CyclingPowerVector = 0x2A64,
    Bluetooth16BitsUUID_CyclingPowerFeature = 0x2A65,
    Bluetooth16BitsUUID_CyclingPowerControlPoint = 0x2A66,
    Bluetooth16BitsUUID_LocationAndSpeed = 0x2A67,
    Bluetooth16BitsUUID_Navigation = 0x2A68,
    Bluetooth16BitsUUID_PositionQuality = 0x2A69,
    Bluetooth16BitsUUID_LNFeature = 0x2A6A,
    Bluetooth16BitsUUID_LNControlPoint = 0x2A6B,
    Bluetooth16BitsUUID_Elevation = 0x2A6C,
    Bluetooth16BitsUUID_Pressure = 0x2A6D,
    Bluetooth16BitsUUID_Temperature = 0x2A6E,
    Bluetooth16BitsUUID_Humidity = 0x2A6F,
    Bluetooth16BitsUUID_TrueWindSpeed = 0x2A70,
    Bluetooth16BitsUUID_TrueWindDirection = 0x2A71,
    Bluetooth16BitsUUID_ApparentWindSpeed = 0x2A72,
    Bluetooth16BitsUUID_ApparentWindDirection = 0x2A73,
    Bluetooth16BitsUUID_GustFactor = 0x2A74,
    Bluetooth16BitsUUID_PollenConcentration = 0x2A75,
    Bluetooth16BitsUUID_UVIndex = 0x2A76,
    Bluetooth16BitsUUID_Irradiance = 0x2A77,
    Bluetooth16BitsUUID_Rainfall = 0x2A78,
    Bluetooth16BitsUUID_WindChill = 0x2A79,
    Bluetooth16BitsUUID_HeatIndex = 0x2A7A,
    Bluetooth16BitsUUID_DewPoint = 0x2A7B,
    Bluetooth16BitsUUID_DescriptorValueChanged = 0x2A7D,
    Bluetooth16BitsUUID_AerobicHeartRateLowerLimit = 0x2A7E,
    Bluetooth16BitsUUID_AerobicThreshold = 0x2A7F,
    Bluetooth16BitsUUID_Age = 0x2A80,
    Bluetooth16BitsUUID_AnaerobicHeartRateLowerLimit = 0x2A81,
    Bluetooth16BitsUUID_AnaerobicHeartRateUpperLimit = 0x2A82,
    Bluetooth16BitsUUID_AnaerobicThreshold = 0x2A83,
    Bluetooth16BitsUUID_AerobicHeartRateUpperLimit = 0x2A84,
    Bluetooth16BitsUUID_DateofBirth = 0x2A85,
    Bluetooth16BitsUUID_DateofThresholdAssessment = 0x2A86,
    Bluetooth16BitsUUID_EmailAddress = 0x2A87,
    Bluetooth16BitsUUID_FatBurnHeartRateLowerLimit = 0x2A88,
    Bluetooth16BitsUUID_FatBurnHeartRateUpperLimit = 0x2A89,
    Bluetooth16BitsUUID_FirstName = 0x2A8A,
    Bluetooth16BitsUUID_FiveZoneHeartRateLimits = 0x2A8B,
    Bluetooth16BitsUUID_Gender = 0x2A8C,
    Bluetooth16BitsUUID_HeartRateMax = 0x2A8D,
    Bluetooth16BitsUUID_Height = 0x2A8E,
    Bluetooth16BitsUUID_HipCircumference = 0x2A8F,
    Bluetooth16BitsUUID_LastName = 0x2A90,
    Bluetooth16BitsUUID_MaximumRecommendedHeartRate = 0x2A91,
    Bluetooth16BitsUUID_RestingHeartRate = 0x2A92,
    Bluetooth16BitsUUID_SportTypeForAerobicAndAnaerobicThresholds = 0x2A93,
    Bluetooth16BitsUUID_ThreeZoneHeartRateLimits = 0x2A94,
    Bluetooth16BitsUUID_TwoZoneHeartRateLimits = 0x2A95,
    Bluetooth16BitsUUID_VO2Max = 0x2A96,
    Bluetooth16BitsUUID_WaistCircumference = 0x2A97,
    Bluetooth16BitsUUID_Weight = 0x2A98,
    Bluetooth16BitsUUID_DatabaseChangeIncrement = 0x2A99,
    Bluetooth16BitsUUID_UserIndex = 0x2A9A,
    Bluetooth16BitsUUID_BodyCompositionFeature = 0x2A9B,
    Bluetooth16BitsUUID_BodyCompositionMeasurement = 0x2A9C,
    Bluetooth16BitsUUID_WeightMeasurement = 0x2A9D,
    Bluetooth16BitsUUID_WeightScaleFeature = 0x2A9E,
    Bluetooth16BitsUUID_UserControlPoint = 0x2A9F,
    Bluetooth16BitsUUID_MagneticFluxDensity2D = 0x2AA0,
    Bluetooth16BitsUUID_MagneticFluxDensity3D = 0x2AA1,
    Bluetooth16BitsUUID_Language = 0x2AA2,
    Bluetooth16BitsUUID_BarometricPressureTrend = 0x2AA3,
    Bluetooth16BitsUUID_BondManagementControlPoint = 0x2AA4,
    Bluetooth16BitsUUID_BondManagementFeature = 0x2AA5,
    Bluetooth16BitsUUID_CentralAddressResolution = 0x2AA6,
    Bluetooth16BitsUUID_CGMMeasurement = 0x2AA7,
    Bluetooth16BitsUUID_CGMFeature = 0x2AA8,
    Bluetooth16BitsUUID_CGMStatus = 0x2AA9,
    Bluetooth16BitsUUID_CGMSessionStartTime = 0x2AAA,
    Bluetooth16BitsUUID_CGMSessionRunTime = 0x2AAB,
    Bluetooth16BitsUUID_CGMSpecificOpsControlPoint = 0x2AAC,
    Bluetooth16BitsUUID_IndoorPositioningConfiguration = 0x2AAD,
    Bluetooth16BitsUUID_Latitude = 0x2AAE,
    Bluetooth16BitsUUID_Longitude = 0x2AAF,
    Bluetooth16BitsUUID_LocalNorthCoordinate = 0x2AB0,
    Bluetooth16BitsUUID_LocalEastCoordinate = 0x2AB1,
    Bluetooth16BitsUUID_FloorNumber = 0x2AB2,
    Bluetooth16BitsUUID_Altitude = 0x2AB3,
    Bluetooth16BitsUUID_Uncertainty = 0x2AB4,
    Bluetooth16BitsUUID_LocationName = 0x2AB5,
    Bluetooth16BitsUUID_URI = 0x2AB6,
    Bluetooth16BitsUUID_HTTPHeaders = 0x2AB7,
    Bluetooth16BitsUUID_HTTPStatusCode = 0x2AB8,
    Bluetooth16BitsUUID_HTTPEntityBody = 0x2AB9,
    Bluetooth16BitsUUID_HTTPControlPoint = 0x2ABA,
    Bluetooth16BitsUUID_HTTPSSecurity = 0x2ABB,
    Bluetooth16BitsUUID_TDSControlPoint = 0x2ABC,
    Bluetooth16BitsUUID_OTSFeature = 0x2ABD,
    Bluetooth16BitsUUID_objectName = 0x2ABE,
    Bluetooth16BitsUUID_objectType = 0x2ABF,
    Bluetooth16BitsUUID_objectSize = 0x2AC0,
    Bluetooth16BitsUUID_objectFirstCreated = 0x2AC1,
    Bluetooth16BitsUUID_objectLastModified = 0x2AC2,
    Bluetooth16BitsUUID_objectID = 0x2AC3,
    Bluetooth16BitsUUID_objectProperties = 0x2AC4,
    Bluetooth16BitsUUID_objectActionControlPoint = 0x2AC5,
    Bluetooth16BitsUUID_objectListControlPoint = 0x2AC6,
    Bluetooth16BitsUUID_objectListFilter = 0x2AC7,
    Bluetooth16BitsUUID_objectChanged = 0x2AC8,
    Bluetooth16BitsUUID_ResolvablePrivateAddressOnly = 0x2AC9,
    Bluetooth16BitsUUID_Unspecified = 0x2ACA,
    Bluetooth16BitsUUID_DirectoryListing = 0x2ACB,
    Bluetooth16BitsUUID_FitnessMachineFeature = 0x2ACC,
    Bluetooth16BitsUUID_TreadmillData = 0x2ACD,
    Bluetooth16BitsUUID_CrossTrainerData = 0x2ACE,
    Bluetooth16BitsUUID_StepClimberData = 0x2ACF,
    Bluetooth16BitsUUID_StairClimberData = 0x2AD0,
    Bluetooth16BitsUUID_RowerData = 0x2AD1,
    Bluetooth16BitsUUID_IndoorBikeData = 0x2AD2,
    Bluetooth16BitsUUID_TrainingStatus = 0x2AD3,
    Bluetooth16BitsUUID_SupportedSpeedRange = 0x2AD4,
    Bluetooth16BitsUUID_SupportedInclinationRange = 0x2AD5,
    Bluetooth16BitsUUID_SupportedResistanceLevelRange = 0x2AD6,
    Bluetooth16BitsUUID_SupportedHeartRateRange = 0x2AD7,
    Bluetooth16BitsUUID_SupportedPowerRange = 0x2AD8,
    Bluetooth16BitsUUID_FitnessMachineControlPoint = 0x2AD9,
    Bluetooth16BitsUUID_FitnessMachineStatus = 0x2ADA,
    Bluetooth16BitsUUID_MeshProvisioningDataIn = 0x2ADB,
    Bluetooth16BitsUUID_MeshProvisioningDataOut = 0X2ADC,
    Bluetooth16BitsUUID_MeshProxyDataIn = 0X2ADD,
    Bluetooth16BitsUUID_MeshProxyDataOut = 0X2ADE,
    Bluetooth16BitsUUID_AverageCurrent = 0X2AE0,
    Bluetooth16BitsUUID_AverageVoltage = 0X2AE1,
    Bluetooth16BitsUUID_Boolean = 0X2AE2,
    Bluetooth16BitsUUID_ChromaticDistanceFromPlanckian = 0X2AE3,
    Bluetooth16BitsUUID_ChromaticityCoordinates = 0X2AE4,
    Bluetooth16BitsUUID_ChromaticityInCCTAndDuvValues = 0X2AE5,
    Bluetooth16BitsUUID_ChromaticityTolerance = 0X2AE6,
    Bluetooth16BitsUUID_CIE133_1995ColorRenderingIndex = 0X2AE7,
    Bluetooth16BitsUUID_Coefficient = 0X2AE8,
    Bluetooth16BitsUUID_CorrelatedColorTemperature = 0X2AE9,
    Bluetooth16BitsUUID_Count16 = 0X2AEA,
    Bluetooth16BitsUUID_Count24 = 0X2AEB,
    Bluetooth16BitsUUID_CountryCode = 0X2AEC,
    Bluetooth16BitsUUID_DateUTC = 0X2AED,
    Bluetooth16BitsUUID_ElectricCurrent = 0X2AEE,
    Bluetooth16BitsUUID_ElectricCurrentRange = 0X2AEF,
    Bluetooth16BitsUUID_ElectricCurrentSpecification = 0X2AF0,
    Bluetooth16BitsUUID_ElectricCurrentStatistics = 0X2AF1,
    Bluetooth16BitsUUID_Energy = 0X2AF2,
    Bluetooth16BitsUUID_EnergyInAPeriodOfDay = 0X2AF3,
    Bluetooth16BitsUUID_EventStatistics = 0X2AF4,
    Bluetooth16BitsUUID_FixedString16 = 0X2AF5,
    Bluetooth16BitsUUID_FixedString24 = 0X2AF6,
    Bluetooth16BitsUUID_FixedString36 = 0X2AF7,
    Bluetooth16BitsUUID_FixedString8 = 0X2AF8,
    Bluetooth16BitsUUID_GenericLevel = 0X2AF9,
    Bluetooth16BitsUUID_GlobalTradeItemNumber = 0X2AFA,
    Bluetooth16BitsUUID_Illuminance = 0X2AFB,
    Bluetooth16BitsUUID_LuminousEfficacy = 0X2AFC,
    Bluetooth16BitsUUID_LuminousEnergy = 0X2AFD,
    Bluetooth16BitsUUID_LuminousExposure = 0X2AFE,
    Bluetooth16BitsUUID_LuminousFlux = 0X2AFF,
    Bluetooth16BitsUUID_LuminousFluxRange = 0x2B00,
    Bluetooth16BitsUUID_LuminousIntensity = 0x2B01,
    Bluetooth16BitsUUID_MassFlow = 0x2B02,
    Bluetooth16BitsUUID_PerceivedLightness = 0x2B03,
    Bluetooth16BitsUUID_Percentage8 = 0x2B04,
    Bluetooth16BitsUUID_Power = 0x2B05,
    Bluetooth16BitsUUID_PowerSpecification = 0x2B06,
    Bluetooth16BitsUUID_RelativeRuntimeInACurrentRange = 0x2B07,
    Bluetooth16BitsUUID_RelativeRuntimeInAGenericLevelRange = 0x2B08,
    Bluetooth16BitsUUID_RelativeValueInAVoltageRange = 0x2B09,
    Bluetooth16BitsUUID_RelativeValueInAnIlluminanceRange = 0x2B0A,
    Bluetooth16BitsUUID_RelativeValueInAPeriodOfDay = 0x2B0B,
    Bluetooth16BitsUUID_RelativeValueInATemperatureRange = 0x2B0C,
    Bluetooth16BitsUUID_Temperature8 = 0x2B0D,
    Bluetooth16BitsUUID_Temperature8InAPeriodOfDay = 0x2B0E,
    Bluetooth16BitsUUID_Temperature8Statistics = 0x2B0F,
    Bluetooth16BitsUUID_TemperatureRange = 0x2B10,
    Bluetooth16BitsUUID_TemperatureStatistics = 0x2B11,
    Bluetooth16BitsUUID_TimeDecihour8 = 0x2B12,
    Bluetooth16BitsUUID_TimeExponential8 = 0x2B13,
    Bluetooth16BitsUUID_TimeHour24 = 0x2B14,
    Bluetooth16BitsUUID_TimeMillisecond24 = 0x2B15,
    Bluetooth16BitsUUID_TimeSecond16 = 0x2B16,
    Bluetooth16BitsUUID_TimeSecond8 = 0x2B17,
    Bluetooth16BitsUUID_Voltage = 0x2B18,
    Bluetooth16BitsUUID_VoltageSpecification = 0x2B19,
    Bluetooth16BitsUUID_VoltageStatistics = 0x2B1A,
    Bluetooth16BitsUUID_VolumeFlow = 0x2B1B,
    Bluetooth16BitsUUID_ChromaticityCoordinate = 0x2B1C,
    Bluetooth16BitsUUID_RCFeature = 0x2B1D,
    Bluetooth16BitsUUID_RCSettings = 0x2B1E,
    Bluetooth16BitsUUID_ReconnectionConfigurationControlPoint = 0x2B1F,
    Bluetooth16BitsUUID_IDDStatusChanged = 0x2B20,
    Bluetooth16BitsUUID_IDDStatus = 0x2B21,
    Bluetooth16BitsUUID_IDDAnnunciationStatus = 0x2B22,
    Bluetooth16BitsUUID_IDDFeatures = 0x2B23,
    Bluetooth16BitsUUID_IDDStatusReaderControlPoint = 0x2B24,
    Bluetooth16BitsUUID_IDDCommandControlPoint = 0x2B25,
    Bluetooth16BitsUUID_IDDCommandData = 0x2B26,
    Bluetooth16BitsUUID_IDDRecordAccessControlPoint = 0x2B27,
    Bluetooth16BitsUUID_IDDHistoryData = 0x2B28,
    Bluetooth16BitsUUID_ClientSupportedFeatures = 0x2B29,
    Bluetooth16BitsUUID_DatabaseHash = 0x2B2A,
    Bluetooth16BitsUUID_BSSControlPoint = 0x2B2B,
    Bluetooth16BitsUUID_BSSResponse = 0x2B2C,
    Bluetooth16BitsUUID_EmergencyID = 0x2B2D,
    Bluetooth16BitsUUID_EmergencyText = 0x2B2E,
    Bluetooth16BitsUUID_EnhancedBloodPressureMeasurement = 0x2B34,
    Bluetooth16BitsUUID_EnhancedIntermediateCuffPressure = 0x2B35,
    Bluetooth16BitsUUID_BloodPressureRecord = 0x2B36,
    Bluetooth16BitsUUID_BR_EDRHandoverData = 0x2B38,
    Bluetooth16BitsUUID_BluetoothSIGData = 0x2B39,
    Bluetooth16BitsUUID_ServerSupportedFeatures = 0x2B3A,
    Bluetooth16BitsUUID_PhysicalActivityMonitorFeatures = 0x2B3B,
    Bluetooth16BitsUUID_GeneralActivityInstantaneousData = 0x2B3C,
    Bluetooth16BitsUUID_GeneralActivitySummaryData = 0x2B3D,
    Bluetooth16BitsUUID_CardioRespiratoryActivityInstantaneousData = 0x2B3E,
    Bluetooth16BitsUUID_CardioRespiratoryActivitySummaryData = 0x2B3F,
    Bluetooth16BitsUUID_StepCounterActivitySummaryData = 0x2B40,
    Bluetooth16BitsUUID_SleepActivityInstantaneousData = 0x2B41,
    Bluetooth16BitsUUID_SleepActivitySummaryData = 0x2B42,
    Bluetooth16BitsUUID_PhysicalActivityMonitorControlPoint = 0x2B43,
    Bluetooth16BitsUUID_CurrentSession = 0x2B44,
    Bluetooth16BitsUUID_Session = 0x2B45,
    Bluetooth16BitsUUID_PreferredUnits = 0x2B46,
    Bluetooth16BitsUUID_HighResolutionHeight = 0x2B47,
    Bluetooth16BitsUUID_MiddleName = 0x2B48,
    Bluetooth16BitsUUID_StrideLength = 0x2B49,
    Bluetooth16BitsUUID_Handedness = 0x2B4A,
    Bluetooth16BitsUUID_DeviceWearingPosition = 0x2B4B,
    Bluetooth16BitsUUID_FourZoneHeartRateLimits = 0x2B4C,
    Bluetooth16BitsUUID_HighIntensityExerciseThreshold = 0x2B4D,
    Bluetooth16BitsUUID_ActivityGoal = 0x2B4E,
    Bluetooth16BitsUUID_SedentaryIntervalNotification = 0x2B4F,
    Bluetooth16BitsUUID_CaloricIntake = 0x2B50,
    Bluetooth16BitsUUID_AudioInputState = 0x2B77,
    Bluetooth16BitsUUID_GainSettingsAttribute = 0x2B78,
    Bluetooth16BitsUUID_AudioInputType = 0x2B79,
    Bluetooth16BitsUUID_AudioInputStatus = 0x2B7A,
    Bluetooth16BitsUUID_AudioInputControlPoint = 0x2B7B,
    Bluetooth16BitsUUID_AudioInputDescription = 0x2B7C,
    Bluetooth16BitsUUID_VolumeState = 0x2B7D,
    Bluetooth16BitsUUID_VolumeControlPoint = 0x2B7E,
    Bluetooth16BitsUUID_VolumeFlags = 0x2B7F,
    Bluetooth16BitsUUID_OffsetState = 0x2B80,
    Bluetooth16BitsUUID_AudioLocation = 0x2B81,
    Bluetooth16BitsUUID_VolumeOffsetControlPoint = 0x2B82,
    Bluetooth16BitsUUID_AudioOutputDescription = 0x2B83,
    Bluetooth16BitsUUID_DeviceTimeFeature = 0x2B8E,
    Bluetooth16BitsUUID_DeviceTimeParameters = 0x2B8F,
    Bluetooth16BitsUUID_DeviceTime = 0x2B90,
    Bluetooth16BitsUUID_DeviceTimeControlPoint = 0x2B91,
    Bluetooth16BitsUUID_TimeChangeLogData = 0x2B92,
    Bluetooth16BitsUUID_ConstantToneExtensionEnable = 0x2BAD,
    Bluetooth16BitsUUID_AdvertisingConstantToneExtensionMinimumLength = 0x2BAE,
    Bluetooth16BitsUUID_AdvertisingConstantToneExtensionMinimumTransmitCount = 0x2BAF,
    Bluetooth16BitsUUID_AdvertisingConstantToneExtensionTransmitDuration = 0x2BB0,
    Bluetooth16BitsUUID_AdvertisingConstantToneExtensionInterval = 0x2BB1,
    Bluetooth16BitsUUID_AdvertisingConstantToneExtensionPHY = 0x2BB2,
    Bluetooth16BitsUUID_Mute = 0x2BC3,
} Bluetooth16BitsUUID;


/// Table 3.17: Mode Parameter for OACP Write Op Code
/// - seeAlso: OTS_v10.pdf  (page.32)
struct ModeForOACPWrite {
    union{
        UInt8 value;
        struct{
            /// Static OOB Information is available.
            UInt8 RFU  :1;//value的低1个bit
            UInt8 Truncate  :1;//val的低位第2个bit,The object shall be truncated: 0: False, 1: True
        };
    };
};


/// Table 3.7: Object Properties Definition.
/// - seeAlso: OTS_v10.pdf  (page.22)
struct ObjectProperties {
    union{
        UInt32 value;//大端数据
        struct{
            UInt8 Delete   :1;//value的大端数据的低位第0个bit
            UInt8 Execute  :1;//value的大端数据的低位第1个bit
            UInt8 Read     :1;//value的大端数据的低位第2个bit
            UInt8 Write    :1;//value的大端数据的低位第3个bit
            UInt8 Append   :1;//value的大端数据的低位第4个bit
            UInt8 Truncate :1;//value的大端数据的低位第5个bit
            UInt8 Patch    :1;//value的大端数据的低位第6个bit
            UInt8 Mark     :1;//value的大端数据的低位第7个bit
            /// 8 - 31, Reserved for Future Use.
            UInt32 RFU     :24;//value的大端数据的高24个bit
        };
    };
};


/// Table 3.3: Definition of the bits of the OACP Features field
/// - seeAlso: OTS_v10.pdf  (page.16)
struct OACPFeatures {
    union{
        UInt32 value;//大端数据
        struct{
            UInt8 OACPCreateOpCodeSupported   :1;//value的大端数据的低位第0个bit, OACP Create Op Code Supported, 0 = False, 1 = True
            UInt8 OACPDeleteOpCodeSupported  :1;//value的大端数据的低位第0个bit, OACP Delete Op Code Supported 0 = False, 1 = True
            UInt8 OACPCalculateChecksumOpCodeSupported     :1;//value的大端数据的低位第2个bit, OACP Calculate Checksum Op Code Supported 0 = False, 1 = True
            UInt8 OACPExecuteOpCodeSupported    :1;//value的大端数据的低位第3个bit, OACP Execute Op Code Supported 0 = False, 1 = True
            UInt8 OACPReadOpCodeSupported   :1;//value的大端数据的低位第4个bit, OACP Read Op Code Supported 0 = False, 1 = True
            UInt8 OACPWriteOpCodeSupported :1;//value的大端数据的低位第5个bit, OACP Write Op Code Supported 0 = False, 1 = True
            UInt8 AppendingAdditionalDataToObjectsSupported    :1;//value的大端数据的低位第6个bit, Appending Additional Data to Objects Supported 0 = False, 1 = True
            UInt8 TruncationOfObjectsSupported     :1;//value的大端数据的低位第7个bit, Truncation of Objects Supported 0 = False, 1 = True
            UInt8 PatchingOfObjectsSupported     :1;//value的大端数据的低位第8个bit, Patching of Objects Supported 0 = False, 1 = True
            UInt8 OACPAbortOpCodeSupported     :1;//value的大端数据的低位第9个bit, OACP Abort Op Code Supported 0 = False, 1 = True
            /// 10 - 31, Reserved for Future Use.
            UInt32 RFU     :24;//value的大端数据的高22个bit
        };
    };
};


@interface OTSBaseModel : NSObject
/// uuid of characteristic
@property (nonatomic,strong) NSString *UUIDString;
/// Message parameters as Data.
@property (nonatomic,strong,nullable) NSData *parameters;
/// The message Op Code.
@property (nonatomic,assign) UInt8 opCode;
/// opCode + parameters
@property (nonatomic,strong) NSData *payload;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


@interface OACPBaseModel : OTSBaseModel
@end


@interface ObjectModel : NSObject
/// 3.2.2 Object Name Characteristic
/// The Object Name characteristic exposes the name of the Current Object.
@property (nonatomic, strong) NSString *objectName;
/// 3.2.3 Object Type Characteristic
/// This characteristic exposes the type of the Current Object, identifying the object type by UUID.
/// Relevant object types that are to be used for a given application may be specified by a higher-level specification.
@property (nonatomic, assign) UInt16 objectType;
/// 3.2.4 Object Size Characteristic
/// The Object Size characteristic exposes the current size as well as the allocated size of the Current Object.
/// The structure of the characteristic is defined below:
///             LSO                 MSO
///             Current Size    Allocated Size
/// Octet Order        LSO...MSO     LSO...MSO
/// Data type           UINT32           UINT32
/// Size                    4 octets          4 octets
/// Units                   unitless          unitless
@property (nonatomic, assign) UInt32 objectSize;
/// 3.2.5 Object First-Created Characteristic
/// The Object First-Created characteristic exposes a value representing a date and time when the object
/// contents were first created. Note that this may be an earlier date than the date of the creation of the
/// object on the present Server; it can relate to the original file creation date of a firmware release, for
/// instance, rather than the time that the object was written to the Server.
/// The date and time value shall be represented in Coordinated Universal Time (UTC).
/// Note that, once the value of the object first-created metadata has been set to a valid date and time
/// (i.e., populated with non-zero values), its value is not intended to change any further, since the time
/// of first creation of an object is a constant by definition.
@property (nonatomic, strong) NSData *firstCreatedData;
/// 3.2.6 Object Last-Modified Characteristic
/// The Object Last-Modified characteristic exposes a value representing a date and time when the object
/// content was last modified.
/// The date and time value shall be represented in Coordinated Universal Time (UTC).
@property (nonatomic, strong) NSData *lastModifiedData;
/// 3.2.8 Object Properties Characteristic
/// This characteristic exposes a bit field representing the properties of the Current Object. When a bit is set
/// to 1 (True), the Current Object possesses the associated property as defined in [3].
/// An object’s properties determine the operations that are permitted to be performed on that object, as
/// described in Section 3.3. For example, an attempt to use the OACP Write Op Code when the Write bit of
/// the properties for the Current Object is 0 (False) will result in an error response.
@property (nonatomic, assign) struct ObjectProperties objectProperties;
/// The dictionary of the share mesh network.
@property (nonatomic, strong) NSDictionary *shareMeshDictionary;
/// The SigDataSource object of the share mesh network.
@property (nonatomic, strong) SigDataSource *shareMeshDataSource;
/// The compressed mesh network object data
@property (nonatomic, strong) NSData *objectData;//压缩后的Object数据
/// Mark whether the Object data is locked
@property (nonatomic, assign) BOOL objectLocked;
/// Mark whether Object data is being read.
@property (nonatomic, assign) BOOL isReading;//正在被Read
/// Mark whether Object data is being writing.
@property (nonatomic, assign) BOOL isWriting;//正在被Write

/**
 * @brief   Initialize ObjectModel object.
 * @param   dictionary  The dictionary of the share mesh network.
 * @return  return `nil` when initialize ObjectModel object fail.
 */
- (instancetype)initWithMeshDictionary:(NSDictionary *)dictionary;

@end


@interface OACPCreateModel : OACPBaseModel
@property (nonatomic, assign) UInt32 size;
/// The gatt_uuid format type defines UUIDs of more than one size, including a large 128-bit UUID.
/// This service supports all UUIDs that conform to the gatt_uuid format. However, note that it will only
/// be possible to create objects with an Object Type represented by a 128-bit UUID via this control
/// point if the Server is able to negotiate an ATT_MTU size with the Client that is 24 octets or larger or
/// else by using the GATT Write Long Characteristic Values sub-procedure to write the value.
@property (nonatomic, strong, nullable) NSData *type;

/**
 * @brief   Initialize OACPCreateModel object.
 * @param   size  The size of the object.
 * @param   type  The gatt_uuid format type defines UUIDs of more than one size, including a large
 * 128-bit UUID. This service supports all UUIDs that conform to the gatt_uuid format. However, note that
 * it will only be possible to create objects with an Object Type represented by a 128-bit UUID via this
 * control point if the Server is able to negotiate an ATT_MTU size with the Client that is 24 octets or larger
 * or else by using the GATT Write Long Characteristic Values sub-procedure to write the value.
 * @return  return `nil` when initialize OACPCreateModel object fail.
 */
- (instancetype)initWithSize:(UInt32)size type:(NSData *)type;

@end


@interface OACPDeleteModel : OACPBaseModel
@end


//这个OTS指令在CDTP里面是可选的指令。
@interface OACPCalculateChecksumModel : OACPBaseModel
/// An Offset parameter is required with the Calculate Checksum Op Code. This parameter is used to request
/// a checksum covering the octets beginning from a specified octet position within the object. The value represents
/// an integer number of octets and is based from zero; the first octet of the object contents has an index of zero,
/// the second octet has an index of one, etc. The octet numbered zero is the least significant octet.
@property (nonatomic, assign) UInt32 offset;
/// A Length parameter is required with the Calculate Checksum Op Code. The object contents included in the
/// calculation shall consist of Length number of octets, starting with the octet identified by the value of the
/// Offset parameter.
@property (nonatomic, assign) UInt32 length;
/// When the Calculate Checksum Op Code is written to the OACP and an error condition does not occur, the Server
/// shall calculate a 32-bit CRC value from the specified octets of the Current Object, using an ISO/IEC 3309
/// compliant 32-bit CRC algorithm [4]. After calculating the checksum, the Server shall respond with the “Success”
/// result code. The newly calculated checksum value shall be provided in the Checksum response parameter.
@property (nonatomic, assign) UInt32 crc32;

/**
 * @brief   Initialize OACPCalculateChecksumModel object.
 * @param   offset  An Offset parameter is required with the Calculate Checksum Op Code. This parameter is used
 * to request a checksum covering the octets beginning from a specified octet position within the object. The value
 * represents an integer number of octets and is based from zero; the first octet of the object contents has an index of zero,
 * the second octet has an index of one, etc. The octet numbered zero is the least significant octet.
 * @param   length  A Length parameter is required with the Calculate Checksum Op Code. The object contents
 * included in the calculation shall consist of Length number of octets, starting with the octet identified by the value of
 * the Offset parameter.
 * @return  return `nil` when initialize OACPCalculateChecksumModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length;

/**
 * @brief   Initialize OACPCalculateChecksumModel object.
 * @param   offset  An Offset parameter is required with the Calculate Checksum Op Code. This parameter is used
 * to request a checksum covering the octets beginning from a specified octet position within the object. The value
 * represents an integer number of octets and is based from zero; the first octet of the object contents has an index of zero,
 * the second octet has an index of one, etc. The octet numbered zero is the least significant octet.
 * @param   length  A Length parameter is required with the Calculate Checksum Op Code. The object contents
 * included in the calculation shall consist of Length number of octets, starting with the octet identified by the value of
 * the Offset parameter.
 * @param   crc32  When the Calculate Checksum Op Code is written to the OACP and an error condition does not occur,
 * the Server shall calculate a 32-bit CRC value from the specified octets of the Current Object, using an ISO/IEC 3309
 * compliant 32-bit CRC algorithm [4]. After calculating the checksum, the Server shall respond with the “Success” result code.
 * The newly calculated checksum value shall be provided in the Checksum response parameter.
 * @return  return `nil` when initialize OACPCalculateChecksumModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length crc32:(UInt32)crc32;

@end


//这个OTS指令在CDTP里面是被排除的指令。
@interface OACPExecuteModel : OACPBaseModel
@end


@interface OACPReadModel : OACPBaseModel
/// offset of object need to read.
@property (nonatomic, assign) UInt32 offset;
/// length of object need to read.
@property (nonatomic, assign) UInt32 length;

/**
 * @brief   Initialize OACPReadModel object.
 * @param   offset  offset of object need to read.
 * @param   length  length of object need to read.
 * @return  return `nil` when initialize OACPReadModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length;

@end


@interface OACPWriteModel : OACPBaseModel
/// offset of object need to write.
@property (nonatomic, assign) UInt32 offset;
/// length of object need to write.
@property (nonatomic, assign) UInt32 length;
/// mode of object write.
@property (nonatomic, assign) struct ModeForOACPWrite mode;

/**
 * @brief   Initialize OACPWriteModel object.
 * @param   offset  offset of object need to write.
 * @param   length  length of object need to write.
 * @param   truncate  CDTP need use false.
 * @return  return `nil` when initialize OACPWriteModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length truncate:(bool)truncate;

@end


@interface OACPAbortModel : OACPBaseModel
@end


@interface OTSResponseCodeModel : OTSBaseModel
@property (nonatomic,assign) UInt8 requestOpCode;
@property (nonatomic, assign) OACPResultCode resultCode;
@property (nonatomic, strong, nullable) NSData *responseParameter;
/**
 * @brief   Initialize OTSResponseCodeModel object.
 * @param   parameters    the hex data of OTSResponseCodeModel.
 * @return  return `nil` when initialize OTSResponseCodeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;
@end


@interface OACPResponseCodeModel : OTSResponseCodeModel

- (instancetype)initWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode ResultCode:(OACPResultCode)resultCode responseParameter:(NSData * _Nullable)responseParameter;

@end


@interface OLCPBaseModel : OTSBaseModel
@end


@interface OLCPFirstModel : OLCPBaseModel
@end


@interface OLCPLastModel : OLCPBaseModel
@end


@interface OLCPPreviousModel : OLCPBaseModel
@end


@interface OLCPNextModel : OLCPBaseModel
@end


@interface OLCPGoToModel : OLCPBaseModel
//The size is 48 bits.
@property (nonatomic,strong) NSData *objectID;
- (instancetype)initWithObjectID:(NSData *)objectID;
@end


@interface OLCPOrderModel : OLCPBaseModel
@property (nonatomic,assign) ListSortOrder order;
- (instancetype)initWithOrder:(ListSortOrder)order;
@end


@interface OLCPRequestNumberOfObjectsModel : OLCPBaseModel
@end


@interface OLCPClearMarkingModel : OLCPBaseModel
@end


@interface OLCPResponseCodeModel : OTSResponseCodeModel
@end


@interface ChannelModel : NSObject<NSStreamDelegate>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
@property (nonatomic, strong, nullable) CBL2CAPChannel *channel;
#pragma clang diagnostic pop

- (instancetype)initWithChannel:(CBL2CAPChannel *)channel;
- (void)openChannelSuccessAction:(CBL2CAPChannel * _Nullable)channel;
- (void)closeChannelAction;
- (void)openStreamAction;
- (void)closeStreamAction;
- (void)readStreamWithSize:(UInt32)size progress:(ProgressCallback)progress complete:(_Nullable CompleteCallback)block;
- (void)writeStreamWithData:(NSData *)data progress:(ProgressCallback)progress complete:(_Nullable CompleteCallback)block;

@end


@interface ServiceModel : NSObject
@property (nonatomic, strong) CBPeripheral *peripheral;
@property (nonatomic, strong) NSDictionary<NSString *,id> *advertisementData;//扫描到的蓝牙设备广播包完整数据
@property (nonatomic, strong) NSNumber *RSSI;

- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI;
- (NSString *)showName;

@end

NS_ASSUME_NONNULL_END
