/********************************************************************************************************
 * @file     SDKLibCommand+minor_ENH.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/4/12
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SigModel.h"

NS_ASSUME_NONNULL_BEGIN

@class SigUnicastAddressRangeFormatModel;


/// Table 4.73: Metadata_Entries field entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.286)
@interface SigMetadataEntriesModel : SigModel
/// Size of the Metadata field.
@property (nonatomic, assign) UInt16 metadataLength;
/// Bluetooth assigned number for the Metadata Identifier.
@property (nonatomic, assign) UInt16 metadataID;
/// Model’s metadata.
@property (nonatomic,strong) NSData *metadata;
/// SigUnicastAddressRangeFormatModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;

/**
 * @brief   Initialize SigMetadataEntriesModel object.
 * @param   parameters    the hex data of SigMetadataEntriesModel.
 * @return  return `nil` when initialize SigMetadataEntriesModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get description string.
- (NSString *)getDescription;

@end


/// Table 4.72: SIG_Metadata_Items field entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.286)
@interface SigSIGMetadataItemsModel : SigModel
/// SIG model identifier.
@property (nonatomic, assign) UInt16 modelID;
/// Number of metadata entries.
@property (nonatomic, assign) UInt8 metadataEntriesNumber;
/// List of model’s metadata.
@property (nonatomic, strong) NSMutableArray<SigMetadataEntriesModel *>*metadataEntries;
/// SigSIGMetadataItemsModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;

/**
 * @brief   Initialize SigSIGMetadataItemsModel object.
 * @param   parameters    the hex data of SigSIGMetadataItemsModel.
 * @return  return `nil` when initialize SigSIGMetadataItemsModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get description string.
- (NSString *)getDescription;

@end


/// Table 4.74: Vendor_Metadata_Items field entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.287)
@interface SigVendorMetadataItemsModel : SigModel
/// Vendor model identifier.
@property (nonatomic, assign) UInt32 modelID;
/// Number of metadata entries.
@property (nonatomic, assign) UInt8 metadataEntriesNumber;
/// List of model’s metadata.
@property (nonatomic, strong) NSMutableArray<SigMetadataEntriesModel *>*metadataEntries;
/// SigVendorMetadataItemsModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;

/**
 * @brief   Initialize SigVendorMetadataItemsModel object.
 * @param   parameters    the hex data of SigVendorMetadataItemsModel.
 * @return  return `nil` when initialize SigVendorMetadataItemsModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get description string.
- (NSString *)getDescription;

@end


/// Table 4.71: Metadata_Elements field entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.286)
@interface SigMetadataElementsModel : SigModel
/// Number of metadata items for SIG models in the element.
@property (nonatomic, assign) UInt8 itemsNumberSIGModels;
/// Number of metadata items for vendor models in the element.
@property (nonatomic, assign) UInt8 itemsNumberVendorModels;
/// List of metadata items for SIG models in the element (Optional).
@property (nonatomic, strong) NSMutableArray<SigSIGMetadataItemsModel *>*SIGMetadataItems;
/// List of metadata items for vendor models in the element (Optional).
@property (nonatomic, strong) NSMutableArray<SigVendorMetadataItemsModel *>*vendorMetadataItems;
/// SigMetadataElementsModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;

/**
 * @brief   Initialize SigMetadataElementsModel object.
 * @param   parameters    the hex data of SigMetadataElementsModel.
 * @return  return `nil` when initialize SigMetadataElementsModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get description string.
- (NSString *)getDescription;

@end


/// Table 4.70: Models Metadata Page 0 fields
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.285)
@interface SigModelsMetadataPage0Model : SigModel
/// List of metadata for models for each element.
@property (nonatomic, strong) NSMutableArray<SigMetadataElementsModel *>*metadataElements;
/// SigModelsMetadataPage0Model parameters as Data.
@property (nonatomic,strong) NSData *parameters;

/**
 * @brief   Initialize SigModelsMetadataPage0Model object.
 * @param   parameters    the hex data of SigModelsMetadataPage0Model.
 * @return  return `nil` when initialize SigModelsMetadataPage0Model object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get description string.
- (NSString *)getDescription;

@end


#pragma mark - 4.3.6 On-Demand Private GATT Proxy messages
/*
 On-Demand Private GATT Proxy messages are used to control the On-Demand
 GATT Proxy state (see Section 4.2.47).
 */

#pragma mark 4.3.6.1 ON_DEMAND_PRIVATE_PROXY_GET, opcode:0xB800


/// An ON_DEMAND_PRIVATE_PROXY_GET message is an acknowledged message
/// used to get the current On-Demand Private GATT Proxy state of a node.
/// @note   The response to an ON_DEMAND_PRIVATE_PROXY_GET message is an
/// ON_DEMAND_PRIVATE_PROXY_GET_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.394), 4.3.6.1 ON_DEMAND_PRIVATE_PROXY_GET.
@interface SigOnDemandPrivateProxyGet : SigConfigMessage

@end


#pragma mark 4.3.6.2 ON_DEMAND_PRIVATE_PROXY_SET, opcode:0xB801


/// An ON_DEMAND_PRIVATE_PROXY_SET message is an acknowledged message
/// used to set the On- Demand Private GATT Proxy state of a node.
/// @note   The response to an ON_DEMAND_PRIVATE_PROXY_SET message is an
/// ON_DEMAND_PRIVATE_PROXY_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.394), 4.3.6.2 ON_DEMAND_PRIVATE_PROXY_SET.
@interface SigOnDemandPrivateProxySet : SigConfigMessage
/// New On-Demand Private GATT Proxy state.
/// The On-Demand_Private_GATT_Proxy field contains the new On-Demand Private
/// GATT Proxy state of the node.
@property (nonatomic, assign) UInt8 onDemandPrivateGATTProxy;

/**
 * @brief   Initialize SigOnDemandPrivateProxySet object.
 * @param   onDemandPrivateGATTProxy    New On-Demand Private GATT Proxy state.
 * The On-Demand_Private_GATT_Proxy field contains the new On-Demand Private
 * GATT Proxy state of the node.
 * @return  return `nil` when initialize SigOnDemandPrivateProxySet object fail.
 */
- (instancetype)initWithOnDemandPrivateGATTProxy:(UInt8)onDemandPrivateGATTProxy;

@end


#pragma mark 4.3.6.3 ON_DEMAND_PRIVATE_PROXY_STATUS, opcode:0xB802


/// An ON_DEMAND_PRIVATE_PROXY_STATUS message is an unacknowledged message
/// used to report the current On-Demand Private GATT Proxy state of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.395), 4.3.6.3 ON_DEMAND_PRIVATE_PROXY_STATUS.
@interface SigOnDemandPrivateProxyStatus : SigConfigMessage
/// GATT Proxy state
/// The On-Demand_Private_GATT_Proxy field indicates the current On-Demand Private GATT Proxy state of the node.
@property (nonatomic, assign) UInt8 onDemandPrivateGATTProxy;

/**
 * @brief   Initialize SigOnDemandPrivateProxyStatus object.
 * @param   parameters    the hex data of SigOnDemandPrivateProxyStatus.
 * @return  return `nil` when initialize SigOnDemandPrivateProxyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.7 Solicitation PDU RPL Configuration messages
/*
 Solicitation PDU RPL Configuration messages are used to control the solicitation
 replay protection list of a node (see Section 6.9.2.1).
 */

#pragma mark 4.3.7.1 SOLICITATION_PDU_RPL_ITEMS_CLEAR, opcode:0xB815


/// A SOLICITATION_PDU_RPL_ITEMS_CLEAR message is an acknowledged message
/// used to remove one or more items from the solicitation replay protection list of a node.
/// @note   The response to a SOLICITATION_PDU_RPL_ITEMS_CLEAR message is a
/// SOLICITATION_PDU_RPL_ITEMS_CLEAR_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.395), 4.3.7.1 SOLICITATION_PDU_RPL_ITEMS_CLEAR.
@interface SigSolicitationPduRplItemsClear : SigConfigMessage
/// Unicast address range.
/// The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1)
/// of the solicitation PDU sequences to be removed from the solicitation replay protection
/// list of a node.
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *addressRange;

/**
 * @brief   Initialize SigSolicitationPduRplItemsClear object.
 * @param   addressRange    Unicast address range.
 * The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1)
 * of the solicitation PDU sequences to be removed from the solicitation replay protection
 * list of a node.
 * @return  return `nil` when initialize SigSolicitationPduRplItemsClear object fail.
 */
- (instancetype)initWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange;

@end


#pragma mark 4.3.7.2 SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED, opcode:0xB816


/// A SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED message is an
/// unacknowledged message used to remove one or more items from the solicitation
/// replay protection list of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.396),
/// 4.3.7.2 SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED.
@interface SigSolicitationPduRplItemsClearUnacknowledged : SigConfigMessage
/// Unicast address range.
/// The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1)
/// of the solicitation PDU sequences to be removed from the solicitation replay protection
/// list of a node.
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *addressRange;

/**
 * @brief   Initialize SigSolicitationPduRplItemsClearUnacknowledged object.
 * @param   addressRange    Unicast address range.
 * The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1)
 * of the solicitation PDU sequences to be removed from the solicitation replay protection
 * list of a node.
 * @return  return `nil` when initialize SigSolicitationPduRplItemsClearUnacknowledged
 * object fail.
 */
- (instancetype)initWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange;

@end


#pragma mark 4.3.7.3 SOLICITATION_PDU_RPL_ITEMS_STATUS, opcode:0xB817


/// A SOLICITATION_PDU_RPL_ITEMS_STATUS message is an unacknowledged message
/// used to confirm the removal of one or more items from the solicitation replay protection
/// list of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.396),
/// 4.3.7.3 SOLICITATION_PDU_RPL_ITEMS_STATUS.
@interface SigSolicitationPduRplItemsStatus : SigConfigMessage
/// Unicast address range.
/// The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1)
/// of the solicitation PDU sequences removed from the solicitation replay protection list
/// of a node.
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *addressRange;

/**
 * @brief   Initialize SigSolicitationPduRplItemsStatus object.
 * @param   parameters    the hex data of SigSolicitationPduRplItemsStatus.
 * @return  return `nil` when initialize SigSolicitationPduRplItemsStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// 4.2.48 SAR Transmitter
/// The SAR Transmitter state is a composite state that controls the number and timing of transmissions of segmented messages.
/// A node shall implement the SAR Transmitter state independently of the presence of the SAR Configuration Server model.
/// The SAR Transmitter state includes the SAR Segment Interval Step state, the SAR Unicast Retransmissions Count state,
/// the SAR Unicast Retransmissions Without Progress Count state, the SAR Unicast Retransmissions Interval Step state,
/// the SAR Unicast Retransmissions Interval Increment state, the SAR Multicast Retransmissions Count state, and the SAR
/// Multicast Retransmissions Interval Step state (see Section 4.2.48.7).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.397),
/// 4.3.8.1 SAR_TRANSMITTER_GET.
struct SARTransmitterStructure {
    union{
        UInt32 value;
        struct{
            /// Reserved for Future Use.
            UInt8 RFU      :4;//value的低4个bit
            /// SAR Multicast Retransmissions Interval state.
            /// The SAR_Multicast_Retransmissions_Interval_Step field determines the new SAR Multicast Retransmissions
            /// Interval Step state for the node, as defined in Section 4.2.48.7.
            /// 4.2.48.7 SAR Multicast Retransmissions Interval Step
            /// The SAR Multicast Retransmissions Interval Step state is a 4-bit value that controls the interval between
            /// retransmissions of segments of a segmented message for a destination that is a group address or a virtual address.
            /// The multicast retransmissions interval is the number of milliseconds calculated using the following formula:
            /// multicast retransmissions interval=(SAR Multicast Retransmissions Interval Step+1)×25
            /// The default value of the SAR Multicast Retransmissions Interval Step state should be 0b1001 (250 milliseconds).
            UInt8 SARMulticastRetransmissionsInterval      :4;//value的低4个bit
            /// SAR Multicast Retransmissions Count state.
            /// The SAR_Multicast_Retransmissions_Count field determines the new SAR Multicast Retransmissions
            /// Count state for the node, as defined in Section 4.2.48.6.
            /// 4.2.48.6 SAR Multicast Retransmissions Count
            /// The SAR Multicast Retransmissions Count state is a 4-bit value that controls the maximum number of
            /// transmissions of segments of segmented messages to a group address or a virtual address. The maximum
            /// number of transmissions of a segment is (SAR Multicast Retransmissions Count + 1).
            /// For example, 0b0000 represents a single transmission, and 0b0111 represents 8 transmissions.
            /// The default value of the SAR Multicast Retransmissions Count state should be 0b0010 (3 transmissions).
            UInt8 SARMulticastRetransmissionsCount      :4;//value的低4个bit
            /// SAR Unicast Retransmissions Interval Increment state.
            /// The SAR_Unicast_Retransmissions_Interval_Increment field determines the new SAR Unicast
            /// Retransmissions Interval Increment state for the node, as defined in Section 4.2.48.5.
            /// 4.2.48.5 SAR Unicast Retransmissions Interval Increment
            /// The SAR Unicast Retransmissions Interval Increment state is a 4-bit value that controls the incremental
            /// component of the interval between retransmissions of segments of a segmented message for a destination
            /// that is a unicast address.
            /// The unicast retransmissions interval increment is the number of milliseconds calculated using the following formula:
            /// unicast retransmissions interval increment=(SAR Unicast Retransmissions Interval Increment+1)×25
            /// The default value of the SAR Unicast Retransmissions Interval Increment state should be 0b0001 (50 milliseconds).
            UInt8 SARUnicastRetransmissionsIntervalIncrement      :4;//value的低4个bit
            /// SAR Unicast Retransmissions Interval Step state.
            /// The SAR_Unicast_Retransmissions_Interval_Step field determines the new SAR Unicast Retransmissions
            /// Interval Step state for the node, as defined in Section 4.2.48.4.
            /// 4.2.48.4 SAR Unicast Retransmissions Interval Step
            /// The SAR Unicast Retransmissions Interval Step state is a 4-bit value that controls the interval between
            /// retransmissions of segments of a segmented message for a destination that is a unicast address.
            /// The unicast retransmissions interval step is the number of milliseconds calculated using the following formula:
            /// unicast retransmissions interval step=(SAR Unicast Retransmissions Interval Step+1)×25
            /// The default value of the SAR Unicast Retransmissions Interval Step should be 0b0111 (200 milliseconds) or higher.
            UInt8 SARUnicastRetransmissionsIntervalStep      :4;//value的低4个bit
            /// SAR Unicast Retransmissions Without Progress Count state.
            /// The SAR_Unicast_Retransmissions_Without_Progress_Count field determines the new
            /// SAR Unicast Retransmissions Without Progress Count state for the node, as defined in
            /// Section 4.2.48.3.
            /// 4.2.48.3 SAR Unicast Retransmissions Without Progress Count
            /// The SAR Unicast Retransmissions Without Progress Count state is a 4-bit value that controls
            /// the maximum number of transmissions of segments of segmented messages to a unicast destination
            /// without progress (i.e., without newly marking any segment as acknowledged). The maximum number
            /// of transmissions of a segment without progress is (SAR Unicast Retransmissions Without Progress Count + 1).
            /// For example, 0b0000 represents a single transmission, and 0b0111 represents 8 transmissions.
            /// The default value of the SAR Unicast Retransmissions Without Progress Count state
            /// should be 0b0010 (3 transmissions).
            /// The value of this state should be set to a value greater than the value of the SAR Acknowledgement
            /// Retransmissions Count on a peer node. This helps prevent the SAR transmitter from abandoning
            /// the SAR prematurely.
            UInt8 SARUnicastRetransmissionsWithoutProgressCount      :4;//value的低4个bit
            /// SAR Unicast Retransmissions Count state.
            /// The SAR_Unicast_Retransmissions_Count field determines the new SAR Unicast
            /// Retransmissions Count state for the node, as defined in Section 4.2.48.2.
            /// 4.2.48.2 SAR Unicast Retransmissions Count
            /// The SAR Unicast Retransmissions Count state is a 4-bit value that controls the maximum
            /// number of transmissions of segments of segmented messages to a unicast destination.
            /// The maximum number of transmissions of a segment is (SAR Unicast Retransmissions Count + 1).
            /// For example, 0b0000 represents a single transmission, and 0b0111 represents 8 transmissions.
            /// The default value of the SAR Unicast Retransmissions Count state should be 0b0010 (3 transmissions).
            UInt8 SARUnicastRetransmissionsCount      :4;//value的低4个bit
            /// SAR Segment Interval Step state.
            /// The SAR_Segment_Interval_Step field determines the new SAR Segment Interval
            /// Step state for the node, as defined in Section 4.2.48.1.
            /// 4.2.48.1 SAR Segment Interval Step
            /// The SAR Segment Interval Step state is a 4-bit value that controls the interval
            /// between transmissions of segments of a segmented message.
            /// The segment transmission interval is the number of milliseconds calculated using
            /// the following formula:
            /// segment transmission interval=(SAR Segment Interval Step+1)×10
            /// The default value of the SAR Segment Interval Step state should be 0b0101 (60 milliseconds).
            UInt8 SARSegmentIntervalStep      :4;//value的高4个bit
        };
    };
};


#pragma mark 4.3.8 SAR Configuration messages


#pragma mark 4.3.8.1 SAR_TRANSMITTER_GET, opcode:0xB803


/// A SAR_TRANSMITTER_GET message is an acknowledged message used to
/// get the current SAR Transmitter state of a node.
/// @note   The response to a SAR_TRANSMITTER_GET message is a
/// SAR_TRANSMITTER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.397),
/// 4.3.8.1 SAR_TRANSMITTER_GET.
@interface SigSARTransmitterGet : SigConfigMessage

@end


#pragma mark 4.3.8.2 SAR_TRANSMITTER_SET, opcode:0xB804


/// A SAR_TRANSMITTER_SET message is an acknowledged message used to
/// set the SAR Transmitter state of a node.
/// @note   The response to a SAR_TRANSMITTER_GET message is a
/// SAR_TRANSMITTER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.397),
/// 4.3.8.2 SAR_TRANSMITTER_SET.
@interface SigSARTransmitterSet : SigConfigMessage
/// SAR Transmitter Structure data.
@property (nonatomic, assign) struct SARTransmitterStructure SARTransmitter;

/**
 * @brief   Initialize SigSARTransmitterSet object.
 * @param   SARTransmitter    SAR Transmitter Structure data.
 * @return  return `nil` when initialize SigSARTransmitterSet
 * object fail.
 */
- (instancetype)initWithSARTransmitter:(struct SARTransmitterStructure)SARTransmitter;

@end


#pragma mark 4.3.8.3 SAR_TRANSMITTER_STATUS, opcode:0xB805


/// A SAR_TRANSMITTER_STATUS message is an unacknowledged message used
/// to report the current SAR Transmitter state of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.398),
/// 4.3.8.3 SAR_TRANSMITTER_STATUS.
@interface SigSARTransmitterStatus : SigConfigMessage
/// SAR Transmitter Structure data.
@property (nonatomic, assign) struct SARTransmitterStructure SARTransmitter;

/**
 * @brief   Initialize SigSARTransmitterStatus object.
 * @param   parameters    The hex data of SigSARTransmitterStatus object.
 * @return  return `nil` when initialize SigSARTransmitterStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// 4.2.49 SAR Receiver
/// The SAR Receiver state is a composite state that controls the number and timing of Segment Acknowledgment
/// transmissions and the discarding of reassembly of a segmented message. The node shall implement the SAR
/// Receiver independently of the presence of the SAR Configuration Server model.
/// The SAR Receiver state includes the SAR Segments Threshold state, the SAR Discard Delay state, the SAR
/// Acknowledgment Delay Increment state, the SAR Acknowledgment Retransmissions Count state, and the SAR
/// Receiver Segment Interval Step state.
/// Table 4.258: SAR_RECEIVER_SET message structure
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.396)
struct SARReceiverStructure {
    union{
        UInt32 value;
        struct{
            /// Reserved for Future Use.
            UInt16 RFU8      :8;//value的低8个bit
            /// Reserved for Future Use.
            UInt16 RFU      :6;//value的低6个bit
            /// SAR Acknowledgment Retransmissions Count state.
            /// The SAR_Acknowledgment_Retransmissions_Count field determines the new SAR Acknowledgment Retransmissions Count state for the node, as defined in Section 4.2.49.3.
            /// 4.2.49.3 SAR Acknowledgment Retransmissions Count
            /// The SAR Acknowledgment Retransmissions Count state is a 2-bit value that controls the number of retransmissions of Segment Acknowledgment messages sent by the lower transport layer. The maximum number of transmissions of a Segment Acknowledgment message is (SAR Acknowledgment Retransmissions Count + 1).
            /// For example, 0b00 represents a limit of 1 transmission, and 0b11 represents a limit of 4 transmissions.
            /// The default value of the SAR Acknowledgment Retransmissions Count state should be 0b00 (1 transmission).
            UInt8 SARAcknowledgmentRetransmissionsCount      :2;//value的低4个bit
            /// SAR Receiver Segment Interval Step state.
            /// The SAR_Receiver_Segment_Interval_Step field determines the new SAR Receiver Segment Interval Step state for the node, as defined in Section 4.2.49.5.
            /// 4.2.49.5 SAR Receiver Segment Interval Step
            /// The SAR Receiver Segment Interval Step state is a 4-bit value that indicates the interval between received segments of a segmented message. This is used to control rate of transmission of Segment Acknowledgment messages.
            /// The segment reception interval is the number of milliseconds calculated using the following formula:
            /// segment reception interval=(SAR Receiver Segment Interval Step+1)×10
            /// The default value of the SAR Receiver Segment Interval Step state should be 0b0101 (60 milliseconds).
            UInt8 SARReceiverSegmentIntervalStep      :4;//value的低4个bit
            /// SAR Discard Timeout state.
            /// The SAR_Discard_Timeout field determines the new SAR Discard Timeout state for the node, as defined in Section 4.2.49.4.
            /// 4.2.49.4 SAR Discard Timeout
            /// The SAR Discard Timeout state is a 4-bit value that controls the time that the lower transport layer waits after receiving unique segments of a segmented message before discarding that segmented message.
            /// The discard timeout is the number of seconds calculated using the following formula:
            /// discard timeout=(SAR Discard Timeout+1)×5
            /// The default value of the SAR Discard Timeout state should be 0b0001 (10 seconds) or higher.
            UInt8 SARDiscardTimeout      :4;//value的低4个bit
            /// SAR Acknowledgment Delay Increment state.
            /// The SAR_Acknowledgment_Delay_Increment field determines the new SAR Acknowledgment Delay Increment state for the node, as defined in Section 4.2.49.2.
            /// 4.2.49.2 SAR Acknowledgment Delay Increment
            /// The SAR Acknowledgment Delay Increment state is a 3-bit value that controls the interval between the reception of a new segment of a segmented message for a destination that is a unicast address and the transmission of the Segment Acknowledgment for that message.
            /// The acknowledgment delay increment is calculated using the following formula:
            /// acknowledgment delay increment=SAR Acknowledgment Delay Increment+1.5
            /// The default value of the SAR Acknowledgment Delay Increment state should be 0b001 (2.5 segment transmission interval steps) or higher.
            UInt8 SARAcknowledgmentDelayIncrement      :3;//value的低4个bit
            /// SAR Segments Threshold state.
            /// The SAR_Segments_Threshold field determines the new SAR Segments Threshold state for the node, as defined in Section 4.2.49.1.
            /// 4.2.49.1 SAR Segments Threshold
            /// The SAR Segments Threshold state is a 5-bit value that represents the size of a segmented message in number of segments above which the Segment Acknowledgment messages are enabled.
            /// The default value for the SAR Segments Threshold state should be 0b00011 (3 segments).
            UInt8 SARSegmentsThreshold      :5;//value的低4个bit
        };
    };
};


#pragma mark 4.3.8.4 SAR_RECEIVER_GET, opcode:0xB806


/// A SAR_RECEIVER_GET message is an acknowledged message used to get
/// the current SAR Receiver state of a node.
/// @note   The response to a SAR_RECEIVER_GET message is a
/// SAR_RECEIVER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.400),
/// 4.3.8.4 SAR_RECEIVER_GET.
@interface SigSARReceiverGet : SigConfigMessage

@end


#pragma mark 4.3.8.5 SAR_RECEIVER_SET, opcode:0xB807


/// A SAR_RECEIVER_SET message is an acknowledged message used to set
/// the SAR Receiver state of a node.
/// @note   The response to a SAR_RECEIVER_SET message is a
/// SAR_RECEIVER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.400),
/// 4.3.8.5 SAR_RECEIVER_SET.
@interface SigSARReceiverSet : SigConfigMessage
/// SAR Receiver Structure data.
@property (nonatomic, assign) struct SARReceiverStructure SARReceiver;

/**
 * @brief   Initialize SigSARReceiverSet object.
 * @param   SARReceiver    SAR Receiver Structure data.
 * @return  return `nil` when initialize SigSARReceiverSet
 * object fail.
 */
- (instancetype)initWithSARReceiver:(struct SARReceiverStructure)SARReceiver;

@end


#pragma mark 4.3.8.6 SAR_RECEIVER_STATUS, opcode:0xB808


/// A SAR_RECEIVER_STATUS message is an unacknowledged message used to report
/// the current SAR Receiver state of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.401),
/// 4.3.8.6 SAR_RECEIVER_STATUS.
@interface SigSARReceiverStatus : SigConfigMessage
/// SAR Receiver Structure data.
@property (nonatomic, assign) struct SARReceiverStructure SARReceiver;

/**
 * @brief   Initialize SigSARReceiverStatus object.
 * @param   parameters    The hex data of SigSARReceiverStatus object.
 * @return  return `nil` when initialize SigSARReceiverStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 4.3.10 Large Composition Data messages


#pragma mark 4.3.10.1 LARGE_COMPOSITION_DATA_GET, opcode:0xB811


/// A LARGE_COMPOSITION_DATA_GET message is an acknowledged message
/// used to read a portion of a page of the Composition Data (see Section 4.2.1).
/// @note   The response to a LARGE_COMPOSITION_DATA_GET message is a
/// LARGE_COMPOSITION_DATA_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.404),
/// 4.3.10.1 LARGE_COMPOSITION_DATA_GET.
@interface SigLargeCompositionDataGet : SigConfigMessage
/// Page number of the Composition Data.
/// The Page field shall identify the Composition Data Page number that is being read.
@property (nonatomic, assign) UInt8 page;
/// Offset within the page.
/// The Offset field shall identify the offset within the Composition Data Page in octets.
@property (nonatomic, assign) UInt16 offset;

/**
 * @brief   Initialize SigLargeCompositionDataGet object.
 * @param   page    Page number of the Composition Data.
 * The Page field shall identify the Composition Data Page number that is being read.
 * @param   offset    Offset within the page.
 * The Offset field shall identify the offset within the Composition Data Page in octets.
 * @return  return `nil` when initialize SigLargeCompositionDataGet object fail.
 */
- (instancetype)initWithPage:(UInt8)page offset:(UInt16)offset;

@end


#pragma mark 4.3.10.2 LARGE_COMPOSITION_DATA_STATUS, opcode:0xB812


/// A LARGE_COMPOSITION_DATA_STATUS message is an unacknowledged message
/// used to report a portion of a page of the Composition Data (see Section 4.2.1).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.405),
/// 4.3.10.2 LARGE_COMPOSITION_DATA_STATUS.
@interface SigLargeCompositionDataStatus : SigConfigMessage
/// Page number of the Composition Data
/// The Page field shall identify the Composition Data Page number.
@property (nonatomic, assign) UInt8 page;
/// Offset within the page
/// The Offset field shall identify the offset within the Composition Data Page in octets.
@property (nonatomic, assign) UInt16 offset;
/// Total size of the page
/// The Total_Size field shall identify the total size of the Composition Data Page in octets.
@property (nonatomic, assign) UInt16 totalSize;
/// Composition Data for the identified portion of the page
/// The Data field shall contain the identified portion of page of the Composition Data.
@property (nonatomic, strong) NSData *data;

/**
 * @brief   Initialize SigLargeCompositionDataStatus object.
 * @param   parameters    The hex data of SigLargeCompositionDataStatus object.
 * @return  return `nil` when initialize SigLargeCompositionDataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.10.3 MODELS_METADATA_GET, opcode:0xB813


/// A MODELS_METADATA_GET message is an acknowledged message used to
/// read a portion of a page of the Models Metadata state (see Section 4.2.50).
/// @note   The response to a MODELS_METADATA_GET message is a
/// MODELS_METADATA_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.405),
/// 4.3.10.3 MODELS_METADATA_GET.
@interface SigModelsMetadataGet : SigConfigMessage
/// Page number of the Models Metadata.
/// The Metadata_Page field identifies the Models Metadata Page number.
@property (nonatomic, assign) UInt8 metadataPage;
/// Offset within the page.
/// The Offset field identifies the offset within the Models Metadata Page in octets.
@property (nonatomic, assign) UInt16 offset;

/**
 * @brief   Initialize SigLargeCompositionDataGet object.
 * @param   metadataPage    Page number of the Models Metadata.
 * The Metadata_Page field identifies the Models Metadata Page number.
 * @param   offset    Offset within the page.
 * The Offset field identifies the offset within the Models Metadata Page in octets.
 * @return  return `nil` when initialize SigLargeCompositionDataGet object fail.
 */
- (instancetype)initWithMetadataPage:(UInt8)metadataPage offset:(UInt16)offset;

@end


#pragma mark 4.3.10.4 MODELS_METADATA_STATUS, opcode:0xB814


/// A MODELS_METADATA_STATUS message is an unacknowledged message used
/// to report a portion of a page of the Models Metadata state (see Section 4.2.50).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.406),
/// 4.3.10.4 MODELS_METADATA_STATUS.
@interface SigModelsMetadataStatus : SigConfigMessage
/// Page number of the Models Metadata.
/// The Metadata_Page field identifies the Models Metadata Page number.
@property (nonatomic, assign) UInt8 metadataPage;
/// Offset within the page.
/// The Offset field identifies the offset within the Models Metadata Page in octets.
@property (nonatomic, assign) UInt16 offset;
/// Total size of the page.
/// The Total_Size field identifies the total size of the Models Metadata Page in octets.
@property (nonatomic, assign) UInt16 totalSize;
/// Models Metadata for the identified portion of the page.
/// The Data field contains the identified portion of the Models Metadata Page.
@property (nonatomic, strong) NSData *data;

/**
 * @brief   Initialize SigModelsMetadataStatus object.
 * @param   parameters    The hex data of SigModelsMetadataStatus object.
 * @return  return `nil` when initialize SigModelsMetadataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// Table 6.14: Values of the Use_Directed field of the DIRECTED_PROXY_CONTROL message.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.612)
typedef enum : UInt8 {
    /// Directed forwarding will not be used for Directed Proxy Client messages.
    UseDirected_disable       = 0,
    /// Directed forwarding will be used for Directed Proxy Client messages.
    UseDirected_enable     = 1,
    //prohibited, 0x02~0xFF
} UseDirected;


/// 4.2.26.3 Directed Proxy.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.264)
typedef enum : UInt8 {
    /// Directed proxy functionality is disabled for a subnet.
    SigDirectedProxy_disable       = 0,
    /// Directed proxy functionality is enabled for a subnet.
    SigDirectedProxy_enable     = 1,
    /// Directed proxy functionality is not supported by the node.
    SigDirectedProxy_notSupported     = 2,
    //prohibited, 0x03~0xFF
} SigDirectedProxy;


#pragma mark - 6 Proxyprotocol - 6.6 Proxyconfigurationmessages


#pragma mark 6.6.6 DIRECTED_PROXY_CONTROL, opcode:0x05


/// A DIRECTED_PROXY_CONTROL message is sent by a Directed Proxy Client to set whether or not the Directed
/// Proxy Server uses directed forwarding for Directed Proxy Client messages for a specified range of unicast addresses.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.626),
/// 6.6.6 DIRECTED_PROXY_CONTROL.
@interface SigDirectedProxyControl : SigStaticAcknowledgedProxyConfigurationMessage
/// Indicates whether directed forwarding is used for messages from the Directed Proxy Client.
/// The Use_Directed field determines whether or not the Directed Proxy Server uses directed forwarding for
/// Directed Proxy Client messages.
@property (nonatomic,assign) UseDirected useDirected;
/// Unicast address range of the Directed Proxy Client (C.1)(C.1: If the Use_Directed field is set to Enabled, the
/// Proxy_Client_Unicast_Addr_Range field shall be present; otherwise, the Proxy_Client_Unicast_Addr_Range
/// field shall not be present.)
/// If present, the Proxy_Client_Unicast_Addr_Range field contains the unicast address range (see Section 3.4.2.2.1)
/// for which the Directed Proxy Server will use directed forwarding for all Directed Proxy Client messages.
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *proxyClientUnicastAddressRange;

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   useDirected    Indicates whether directed forwarding is used for messages from the
 * Directed Proxy Client.
 * The Use_Directed field determines whether or not the Directed Proxy Server uses directed forwarding
 * for Directed Proxy Client messages.
 * @param   proxyClientUnicastAddressRange    Unicast address range of the Directed Proxy
 * Client (C.1)(C.1: If the Use_Directed field is set to Enabled, the Proxy_Client_Unicast_Addr_Range field
 * shall be present; otherwise, the Proxy_Client_Unicast_Addr_Range field shall not be present.)
 * If present, the Proxy_Client_Unicast_Addr_Range field contains the unicast address range (see
 * Section 3.4.2.2.1) for which the Directed Proxy Server will use directed forwarding for all Directed
 * Proxy Client messages.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithUseDirected:(UseDirected)useDirected proxyClientUnicastAddressRange:(SigUnicastAddressRangeFormatModel *)proxyClientUnicastAddressRange;

/**
 * @brief   Initialize SigDirectedProxyControl object.
 * @param   parameters    The hex data of SigDirectedProxyControl object.
 * @return  return `nil` when initialize SigDirectedProxyControl object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.6.5 DIRECTED_PROXY_CAPABILITIES_STATUS, opcode:0x04


/// A DIRECTED_PROXY_CAPABILITIES_STATUS message is sent by a Directed Proxy Server to report
/// current Directed Proxy capabilities in a subnet.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.625),
/// 6.6.5 DIRECTED_PROXY_CAPABILITIES_STATUS.
@interface SigDirectedProxyCapabilitiesStatus : SigStaticProxyConfigurationMessage
/// Current Directed Proxy state.
/// The Directed_Proxy field indicates the current Directed Proxy state for the identified subnet, as defined in Section 4.2.26.3.
@property (nonatomic,assign) SigDirectedProxy directedProxy;
/// Indicates whether directed forwarding is used for messages from the Proxy Client.
/// The Use_Directed field indicates whether or not the Directed Proxy Server uses directed forwarding for
/// retransmitting Network PDUs originated from the Proxy Client.
@property (nonatomic,assign) UseDirected useDirected;

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   directedProxy    Current Directed Proxy state.
 * The Directed_Proxy field indicates the current Directed Proxy state for the identified subnet,
 * as defined in Section 4.2.26.3.
 * @param   useDirected    Indicates whether directed forwarding is
 * used for messages from the Proxy Client.
 * The Use_Directed field indicates whether or not the Directed Proxy Server uses directed forwarding
 * for retransmitting Network PDUs originated from the Proxy Client.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithDirectedProxy:(SigDirectedProxy)directedProxy useDirected:(UseDirected)useDirected;

/**
 * @brief   Initialize SigDirectedProxyCapabilitiesStatus object.
 * @param   parameters    The hex data of SigDirectedProxyCapabilitiesStatus object.
 * @return  return `nil` when initialize SigDirectedProxyCapabilitiesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


// callback about On-Demand Private GATT Proxy messages
typedef void(^responseOnDemandPrivateProxyStatusMessageBlock)(UInt16 source, UInt16 destination,SigOnDemandPrivateProxyStatus *responseMessage);
// callback about Solicitation PDU RPL Configuration messages
typedef void(^responseSolicitationPduRplItemsStatusMessageBlock)(UInt16 source, UInt16 destination,SigSolicitationPduRplItemsStatus *responseMessage);
// callback about SAR Configuration messages
typedef void(^responseSARTransmitterStatusMessageBlock)(UInt16 source, UInt16 destination,SigSARTransmitterStatus *responseMessage);
typedef void(^responseSARReceiverStatusMessageBlock)(UInt16 source, UInt16 destination,SigSARReceiverStatus *responseMessage);
// callback about Large Composition Data messages
typedef void(^responseLargeCompositionDataStatusMessageBlock)(UInt16 source, UInt16 destination,SigLargeCompositionDataStatus *responseMessage);
typedef void(^responseModelsMetadataStatusMessageBlock)(UInt16 source, UInt16 destination,SigModelsMetadataStatus *responseMessage);
// callback about Proxy configuration messages
typedef void(^responseDirectedProxyCapabilitiesStatusMessageBlock)(UInt16 source, UInt16 destination,SigDirectedProxyCapabilitiesStatus *responseMessage);

@interface SDKLibCommand (minor_ENH)


#pragma mark - 4.3.6 On-Demand Private GATT Proxy messages
/*
 On-Demand Private GATT Proxy messages are used to control the On-Demand
 GATT Proxy state (see Section 4.2.47).
 */

#pragma mark 4.3.6.1 ON_DEMAND_PRIVATE_PROXY_GET, opcode:0xB800


/**
 * @brief   ON_DEMAND_PRIVATE_PROXY_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.394),
 * 4.3.6.1 ON_DEMAND_PRIVATE_PROXY_GET.
 */
+ (SigMessageHandle *)onDemandPrivateProxyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseOnDemandPrivateProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.6.2 ON_DEMAND_PRIVATE_PROXY_SET, opcode:0xB801


/**
 * @brief   ON_DEMAND_PRIVATE_PROXY_SET.
 * @param   onDemandPrivateGATTProxy    New On-Demand Private GATT Proxy state.
 * The On-Demand_Private_GATT_Proxy field contains the new On-Demand Private GATT Proxy state of the node.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.394),
 * 4.3.6.2 ON_DEMAND_PRIVATE_PROXY_SET.
 */
+ (SigMessageHandle *)onDemandPrivateProxySetWithOnDemandPrivateGATTProxy:(UInt8)onDemandPrivateGATTProxy destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseOnDemandPrivateProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.7 Solicitation PDU RPL Configuration messages
/*
 Solicitation PDU RPL Configuration messages are used to control the solicitation
 replay protection list of a node (see Section 6.9.2.1).
 */

#pragma mark 4.3.7.1 SOLICITATION_PDU_RPL_ITEMS_CLEAR, opcode:0xB815


/**
 * @brief   SOLICITATION_PDU_RPL_ITEMS_CLEAR.
 * @param   addressRange    Unicast address range.
 * The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1) of the solicitation PDU sequences to be
 * removed from the solicitation replay protection list of a node.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.395),
 * 4.3.7.1 SOLICITATION_PDU_RPL_ITEMS_CLEAR.
 */
+ (SigMessageHandle *)solicitationPduRplItemsClearWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSolicitationPduRplItemsStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.7.2 SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED, opcode:0xB816


/**
 * @brief   SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED.
 * @param   addressRange    Unicast address range.
 * The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1) of the solicitation PDU sequences to be
 * removed from the solicitation replay protection list of a node.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.395),
 * 4.3.7.2 SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED.
 */
+ (SigMessageHandle *)solicitationPduRplItemsClearUnacknowledgedWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.8 SAR Configuration messages


#pragma mark 4.3.8.1 SAR_TRANSMITTER_GET, opcode:0xB803


/**
 * @brief   SAR_TRANSMITTER_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.397),
 * 4.3.8.1 SAR_TRANSMITTER_GET.
 */
+ (SigMessageHandle *)SARTransmitterGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARTransmitterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.8.2 SAR_TRANSMITTER_SET, opcode:0xB804


/**
 * @brief   SAR_TRANSMITTER_SET.
 * @param   SARTransmitter    SAR Transmitter Structure data.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.397),
 * 4.3.8.2 SAR_TRANSMITTER_SET.
 */
+ (SigMessageHandle *)SARTransmitterSetWithSARTransmitter:(struct SARTransmitterStructure)SARTransmitter destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARTransmitterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.8.4 SAR_RECEIVER_GET, opcode:0xB806

/**
 * @brief   SAR_RECEIVER_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.400),
 * 4.3.8.4 SAR_RECEIVER_GET.
 */
+ (SigMessageHandle *)SARReceiverGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARReceiverStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.8.4 SAR_RECEIVER_SET, opcode:0xB807


/**
 * @brief   SAR_RECEIVER_SET.
 * @param   SARReceiver    SAR Receiver Structure data.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.400),
 * 4.3.8.4 SAR_RECEIVER_SET.
 */
+ (SigMessageHandle *)SARReceiverSetWithSARReceiver:(struct SARReceiverStructure)SARReceiver destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARReceiverStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 4.3.10 Large Composition Data messages


#pragma mark 4.3.10.1 LARGE_COMPOSITION_DATA_GET, opcode:0xB811


/**
 * @brief   LARGE_COMPOSITION_DATA_GET.
 * @param   page    Page number of the Composition Data.
 * The Page field shall identify the Composition Data Page number that is being read.
 * @param   offset    Offset within the page.
 * The Offset field shall identify the offset within the Composition Data Page in octets.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.404),
 * 4.3.10.1 LARGE_COMPOSITION_DATA_GET.
 */
+ (SigMessageHandle *)largeCompositionDataGetWithPage:(UInt8)page offset:(UInt16)offset destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLargeCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.10.3 MODELS_METADATA_GET, opcode:0xB813

/**
 * @brief   MODELS_METADATA_GET.
 * @param   metadataPage    Page number of the Models Metadata.
 * The Metadata_Page field identifies the Models Metadata Page number.
 * @param   offset    Offset within the page.
 * The Offset field identifies the offset within the Models Metadata Page in octets.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.405),
 * 4.3.10.3 MODELS_METADATA_GET.
 */
+ (SigMessageHandle *)modelsMetadataGetWithMetadataPage:(UInt8)metadataPage offset:(UInt16)offset destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseModelsMetadataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6 Proxyprotocol - 6.6 Proxyconfigurationmessages


#pragma mark 6.6.6 DIRECTED_PROXY_CONTROL, opcode:0x05


/**
 * @brief   DIRECTED_PROXY_CONTROL.
 * @param   useDirected    Indicates whether directed forwarding is used for messages from the
 * Directed Proxy Client.
 * The Use_Directed field determines whether or not the Directed Proxy Server uses directed forwarding
 * for Directed Proxy Client messages.
 * @param   proxyClientUnicastAddressRange    Unicast address range of the Directed Proxy
 * Client (C.1)(C.1: If the Use_Directed field is set to Enabled, the Proxy_Client_Unicast_Addr_Range field
 * shall be present; otherwise, the Proxy_Client_Unicast_Addr_Range field shall not be present.)
 * If present, the Proxy_Client_Unicast_Addr_Range field contains the unicast address range (see
 * Section 3.4.2.2.1) for which the Directed Proxy Server will use directed forwarding for all Directed
 * Proxy Client messages.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.405),
 * 6.6.6 DIRECTED_PROXY_CONTROL.
 */
+ (SigMessageHandle *)directedProxyControlUseDirected:(UseDirected)useDirected proxyClientUnicastAddressRange:(SigUnicastAddressRangeFormatModel *)proxyClientUnicastAddressRange successCallback:(responseDirectedProxyCapabilitiesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

@end

NS_ASSUME_NONNULL_END
