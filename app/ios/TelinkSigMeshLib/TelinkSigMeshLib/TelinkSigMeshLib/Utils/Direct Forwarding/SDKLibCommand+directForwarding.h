/********************************************************************************************************
 * @file     SDKLibCommand+directForwarding.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/9
 *
 * @par     Copyright (c) [2022], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

/// Table 4.184: Values of the Dependent_Origin_List_Size_Indicator and Dependent_Target_List_Size_Indicator fields. size is 2 bits.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.346)
typedef enum : UInt8 {
    ValuesOfListSize_zero       = 0,
    ValuesOfListSize_1octet     = 1,
    ValuesOfListSize_2octet     = 2,
    ValuesOfListSize_prohibited = 3,
} ValuesOfListSize;

/// Table 4.189 defines the values of the Directed_Forwarding field.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.351)
typedef enum : UInt8 {
    /// Disables directed forwarding functionality for the identified subnet.
    DirectedForwarding_disable       = 0,
    /// Enables directed forwarding functionality for the identified subnet.
    DirectedForwarding_enable     = 1,
    //prohibited, 0x02~0xFF
} DirectedForwarding;

/// Table 4.190 defines the values of the Directed_Relay field.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.351)
typedef enum : UInt8 {
    /// Disables directed relay functionality for the identified subnet.
    DirectedRelay_disable       = 0,
    /// Enables directed relay functionality for the identified subnet.
    DirectedRelay_enable     = 1,
    //prohibited, 0x02~0xFF
} DirectedRelay;

/// Table 4.191 defines the values of the Directed_Proxy field.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.352)
typedef enum : UInt8 {
    /// Indicates to disable directed proxy functionality for the identified subnet.
    DirectedProxy_disable       = 0,
    /// Indicates to enable directed proxy functionality for the identified subnet.
    DirectedProxy_enable     = 1,
    //prohibited, 0x02~0xFE
    /// Value of the field is ignored
    DirectedProxy_ignore     = 0xFF,
} DirectedProxy;

/// Table 4.192 defines the values of the Directed_Proxy_Use_Directed_Default field.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.352)
typedef enum : UInt8 {
    /// Indicates the new default value of the Use_Directed parameter of the Directed Proxy Server for the identified subnet.
    DirectedProxyUseDirectedDefault_disable       = 0,
    /// Indicates the new default value of the Use_Directed parameter of the Directed Proxy Server for the identified subnet.
    DirectedProxyUseDirectedDefault_enable     = 1,
    //prohibited, 0x02~0xFE
    /// Value of the field is ignored
    DirectedProxyUseDirectedDefault_ignore     = 0xFF,
} DirectedProxyUseDirectedDefault;

/// Table 4.193 defines the values of the Directed_Friend field.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.352)
typedef enum : UInt8 {
    /// Indicates to disable directed friend functionality for the identified subnet.
    DirectedFriend_disable       = 0,
    /// Indicates to enable directed friend functionality for the identified subnet.
    DirectedFriend_enable     = 1,
    //prohibited, 0x02~0xFE
    /// Value of the field is ignored
    DirectedFriend_ignore     = 0xFF,
} DirectedFriend;

/// Table 4.45: Directed Forwarding state values.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.263)
typedef enum : UInt8 {
    /// Directed forwarding functionality is disabled for a subnet.
    DirectedForwardingState_disable       = 0,
    /// Directed forwarding functionality is enabled for a subnet.
    DirectedForwardingState_enable     = 1,
    //prohibited, 0x02~0xFF
} DirectedForwardingState;

/// Table 4.46: Directed Relay state values.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.264)
typedef enum : UInt8 {
    /// Directed relay functionality is disabled for a subnet.
    DirectedRelayState_disable       = 0,
    /// Directed relay functionality is enabled for a subnet.
    DirectedRelayState_enable     = 1,
    //prohibited, 0x02~0xFF
} DirectedRelayState;

/// Table 4.47: Directed Proxy state values.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.264)
typedef enum : UInt8 {
    /// Directed proxy functionality is disabled for a subnet.
    DirectedProxyState_disable       = 0,
    /// Directed proxy functionality is enabled for a subnet.
    DirectedProxyState_enable     = 1,
    /// Directed proxy functionality is not supported by the node.
    DirectedProxyState_notSupported     = 2,
    //prohibited, 0x03~0xFF
} DirectedProxyState;

/// Table 4.48: Directed Proxy Use Directed Default state values.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.265)
typedef enum : UInt8 {
    /// Use_Directed is set to Disabled for a subnet when connection is created.
    DirectedProxyUseDirectedDefaultState_disable       = 0,
    /// Use_Directed is set to Enabled for a subnet when connection is created.
    DirectedProxyUseDirectedDefaultState_enable     = 1,
    /// Directed proxy functionality is not supported or is disabled for a subnet.
    DirectedProxyUseDirectedDefaultState_notSupported     = 2,
    //prohibited, 0x03~0xFF
} DirectedProxyUseDirectedDefaultState;

/// Table 4.49: Directed Friend state values.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.265)
typedef enum : UInt8 {
    /// Directed friend functionality is disabled for a subnet.
    DirectedFriendState_disable       = 0,
    /// Directed friend functionality is enabled for a subnet.
    DirectedFriendState_enable     = 1,
    /// Directed friend functionality is not supported by the node.
    DirectedFriendState_notSupported     = 2,
    //prohibited, 0x03~0xFF
} DirectedFriendState;


/// The Forwarding Table Entry Header is a 16-bit data structure that encapsulates the flags and the indicators of presence and size of fields of fixed and non-fixed Forwarding Table entries.
/// Table 4.183: Forwarding Table Entry Header parameters.
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.346)
struct ForwardingTableEntryHeader {
    union{
        UInt16 value;
        struct{
            /// Indicates whether the table entry is a fixed path entry (Fixed_Path_Flag = 1) or a non-fixed path entry (Fixed_Path_Flag = 0).
            UInt8 fixedPathFlag      :1;//value的低1个bit
            /// Indicates whether or not the destination of the path is a unicast address
            UInt8 unicastDestinationFlag :1;//val的低位第2个bit
            /// Indicates whether or not the backward path has been validated
            UInt8 backwardPathValidatedFlag :1;//val的低位第3个bit
            /// Indicates the presence or absence of the Bearer_Toward_Path_Origin field
            UInt8 bearerTowardPathOriginIndicator :1;//val的低位第4个bit
            /// Indicates the presence or absence of the Bearer_Toward_Path_Target field
            UInt8 bearerTowardPathTargetIndicator :1;//val的低位第5个bit
            /// Indicates the size of the Dependent_Origin_List field
            ValuesOfListSize dependentOriginListSizeIndicator :2;//val的低位第6/7个bit
            /// Indicates the size of the Dependent_Target_List field
            ValuesOfListSize dependentTargetListSizeIndicator :2;//val的低位第8/9个bit
            /// Prohibited
            UInt8 prohibited :7;//val的低位第10~16个bit
        };
    };
};


#pragma mark 3.4.2.2.1 Unicast address range format


/// A range of unicast addresses may be used in a message, for example, to communicate all the unicast addresses
/// used by a node. In this case, the unicast address range format can be used to efficiently pack the range of unicast
/// addresses. In the unicast address range format, the 15 least significant bits of the starting unicast address of the
/// range and the number of addresses in the range are formatted as in Table 3.6.
/// @note   The sum of the RangeStart field value and the RangeLength field value shall not exceed 0x8000.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.54),
/// 3.4.2.2.1 Unicast address range format.
@interface SigUnicastAddressRangeFormatModel : SigModel
/// Indicates the presence or absence of the RangeLength field.
/// The LengthPresent field indicates whether the address range contains a single address or multiple addresses.
/// The LengthPresent field value shall be set to 1 when the number of addresses in the range of unicast addresses
/// is greater than 1; otherwise, the LengthPresent field value shall be set to 0.
@property (nonatomic, assign) BOOL lengthPresent;
/// 15 least significant bits of the starting unicast address.
/// The RangeStart field shall contain the 15 least significant bits of the starting unicast address of the range of unicast
/// addresses. The RangeStart field value shall not be 0.
@property (nonatomic, assign) UInt16 rangeStart;
/// Number of addresses in the range (0x02 – 0xFF) (C.1)(C.1: If the LengthPresent field value is 1, the RangeLength field
/// shall be present; otherwise, the RangeLength field shall not be present.)
/// If present, the RangeLength field shall contain the number of addresses in the range of unicast addresses. The valid
/// values for the RangeLength field are 0x02 to 0xFF. The values of 0x00 and 0x01 are prohibited.
@property (nonatomic, assign) UInt8 rangeLength;
/// SigUnicastAddressRangeFormatModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   parameters    The hex data of SigUnicastAddressRangeFormatModel object.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   unicastAddressRangeFormat    A range of unicast addresses may be used in a message, for example,
 * to communicate all the unicast addresses used by a node. In this case, the unicast address range format can be used to
 * efficiently pack the range of unicast addresses. In the unicast address range format, the 15 least significant bits of the
 * starting unicast address of the range and the number of addresses in the range are formatted as in Table 3.6.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithOldSigUnicastAddressRangeFormatModel:(SigUnicastAddressRangeFormatModel *)unicastAddressRangeFormat;

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   lengthPresent    Indicates the presence or absence of the RangeLength field.
 * The LengthPresent field indicates whether the address range contains a single address or multiple addresses. The LengthPresent
 * field value shall be set to 1 when the number of addresses in the range of unicast addresses is greater than 1; otherwise, the
 * LengthPresent field value shall be set to 0.
 * @param   rangeStart    15 least significant bits of the starting unicast address.
 * The RangeStart field shall contain the 15 least significant bits of the starting unicast address of the range of unicast addresses.
 * The RangeStart field value shall not be 0.
 * @param   rangeLength    Number of addresses in the range (0x02 – 0xFF) (C.1)(C.1: If the LengthPresent field value is 1, the
 * RangeLength field shall be present; otherwise, the RangeLength field shall not be present.)
 * If present, the RangeLength field shall contain the number of addresses in the range of unicast addresses. The valid values for the
 * RangeLength field are 0x02 to 0xFF. The values of 0x00 and 0x01 are prohibited.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithLengthPresent:(BOOL)lengthPresent rangeStart:(UInt16)rangeStart rangeLength:(UInt8)rangeLength;

/// Get description string.
- (NSString *)getDescription;

@end


/// Table 4.185: Fixed path entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.347)
@interface SigFixedPathEntryFormatModel : SigModel
/// Forwarding Table Entry Header
@property (nonatomic, assign) struct ForwardingTableEntryHeader forwardingTableEntryHeader;
/// Path Origin unicast address range
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *pathOriginUnicastAddrRange;
/// Current number of entries in the list of dependent nodes of the Path Origin (C.1)(C.1: If the value of the
/// Dependent_Origin_List_Size_Indicator field in the Forwarding_Table_Entry_Header field is 0b01 or 0b10,
/// then the Dependent_Origin_List_Size field shall be present; otherwise, the Dependent_Origin_List_Size
/// field shall not be present.)
@property (nonatomic, assign) UInt16 dependentOriginListSize;
/// Index of the bearer toward the Path Origin (C.2)(C.2: If the value of the Bearer_Toward_Path_Origin_Indicator
/// field in the Forwarding_Table_Entry_Header is 1, then the Bearer_Toward_Path_Origin field shall be present;
/// otherwise, the Bearer_Toward_Path_Origin field shall not be present.)
@property (nonatomic, assign) UInt16 bearerTowardPathOrigin;
/// Path Target unicast address range (C.3)(C.3: If the value of the Unicast_Destination_Flag field in the
/// Forwarding_Table_Entry_Header is 1, then the Path_Target_Unicast_Addr_Range field shall be present;
/// otherwise, the Path_Target_Unicast_Addr_Range field shall not be present.)
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *pathTargetUnicastAddrRange;
/// Multicast destination address (C.4)(C.4: If the value of the Unicast_Destination_Flag field in the
/// Forwarding_Table_Entry_Header is 0, then the Multicast_Destination field shall be present; otherwise,
/// the Multicast_Destination field shall not be present.)
@property (nonatomic, assign) UInt16 multicastDestination;
/// Current number of entries in the list of dependent nodes of the Path Target (C.5)(C.5: If the value of the
/// Dependent_Target_List_Size_Indicator field in the Forwarding_Table_Entry_Header is 0b01 or 0b10, then
/// the Dependent_Target_List_Size field shall be present; otherwise, the Dependent_Target_List_Size field
/// shall not be present.)
@property (nonatomic, assign) UInt16 dependentTargetListSize;
/// Index of the bearer toward the Path Target (C.6)(C.6: If the value of the Bearer_Toward_Path_Target_Indicator
/// field in the Forwarding_Table_Entry_Header is 1, then the Bearer_Toward_Path_Target field shall be present;
/// otherwise, the Bearer_Toward_Path_Target field shall not be present.)
@property (nonatomic, assign) UInt16 bearerTowardPathTarget;

/**
 * @brief   Initialize SigFixedPathEntryFormatModel object.
 * @param   parameters    the hex data of SigFixedPathEntryFormatModel.
 * @return  return `nil` when initialize SigFixedPathEntryFormatModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get dictionary from SigFixedPathEntryFormatModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigFixedPathEntryFormatModel;

/// Set dictionary to SigFixedPathEntryFormatModel object.
/// @param dictionary SigFixedPathEntryFormatModel dictionary object.
- (void)setDictionaryToSigFixedPathEntryFormatModel:(NSDictionary *)dictionary;

@end


/// Table 4.186: Non-fixed path entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.348)
@interface SigNonFixedPathEntryFormatModel : SigModel
/// Forwarding Table Entry Header
@property (nonatomic, assign) struct ForwardingTableEntryHeader forwardingTableEntryHeader;
/// Number of lanes in the path
@property (nonatomic, assign) UInt8 laneCounter;
/// The Path_Remaining_Time field represents the remaining path lifetime (in minutes).
@property (nonatomic, assign) UInt16 pathRemainingTime;
/// Path Origin unicast address range
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *pathOriginUnicastAddrRange;
/// Current number of entries in the list of dependent nodes of the Path Origin (C.1)(C.1: If the value of the
/// Dependent_Origin_List_Size_Indicator field in the Forwarding_Table_Entry_Header field is 0b01 or 0b10,
/// then the Dependent_Origin_List_Size field shall be present; otherwise, the Dependent_Origin_List_Size field
/// shall not be present.)
@property (nonatomic, assign) UInt16 dependentOriginListSize;
/// Index of the bearer toward the Path Origin (C.2)(C.2: If the value of the Bearer_Toward_Path_Origin_Indicator
/// field in the Forwarding_Table_Entry_Header is 1, then the Bearer_Toward_Path_Origin field shall be present;
/// otherwise, the Bearer_Toward_Path_Origin field shall not be present.)
@property (nonatomic, assign) UInt16 bearerTowardPathOrigin;
/// Path Target unicast address range (C.3)(C.3: If the value of the Unicast_Destination_Flag field in the
/// Forwarding_Table_Entry_Header is 1, then the Path_Target_Unicast_Addr_Range field shall be present;
/// otherwise, the Path_Target_Unicast_Addr_Range field shall not be present.)
@property (nonatomic, strong) SigUnicastAddressRangeFormatModel *pathTargetUnicastAddrRange;
/// Multicast destination address (C.4)(C.4: If the value of the Unicast_Destination_Flag field in the
/// Forwarding_Table_Entry_Header is 0, then the Multicast_Destination field shall be present; otherwise,
/// the Multicast_Destination field shall not be present.)
@property (nonatomic, assign) UInt16 multicastDestination;
/// Current number of entries in the list of dependent nodes of the Path Target (C.5)(C.5: If the value of the
/// Dependent_Target_List_Size_Indicator field in the Forwarding_Table_Entry_Header is 0b01 or 0b10,
/// then the Dependent_Target_List_Size field shall be present; otherwise, the Dependent_Target_List_Size
/// field shall not be present.)
@property (nonatomic, assign) UInt16 dependentTargetListSize;
/// Index of the bearer toward the Path Target (C.6)(C.6: If the value of the Bearer_Toward_Path_Target_Indicator
/// field in the Forwarding_Table_Entry_Header is 1, then the Bearer_Toward_Path_Target field shall be present;
/// otherwise, the Bearer_Toward_Path_Target field shall not be present.)
@property (nonatomic, assign) UInt16 bearerTowardPathTarget;

/**
 * @brief   Initialize SigNonFixedPathEntryFormatModel object.
 * @param   parameters    the hex data of SigNonFixedPathEntryFormatModel.
 * @return  return `nil` when initialize SigNonFixedPathEntryFormatModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// 4.2.29.2 Forwarding Table Entries
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.268)
@interface SigForwardingTableEntryModel : SigModel
/// Flag indicating whether or not the path is a fixed path.
@property (nonatomic,assign) BOOL fixedPath;
/// Flag indicating whether or not the backward path has been validated.
@property (nonatomic,assign) BOOL backwardPathValidated;
/// Flag indicating whether or not the path is ready for use.
@property (nonatomic,assign) BOOL pathNotReady;
/// Primary element address of the Path Origin.
@property (nonatomic, assign) UInt16 pathOrigin;
/// Number of secondary element addresses of the Path Origin.
@property (nonatomic, assign) UInt8 pathOriginSecondaryElementsCount;
/// List of primary element addresses of dependent nodes of the Path Origin. Each list entry has a corresponding
/// Dependent_Origin_Secondary_Elements_Count_List entry. Size is variable (16 * N1).
@property (nonatomic,strong) NSMutableArray <NSNumber *>*dependentOriginList;
/// List of numbers of secondary elements of dependent nodes of the Path Origin. Each list
/// entry has a corresponding Dependent_Origin_List entry. Size is variable (16 * N1).
@property (nonatomic,strong) NSMutableArray <NSNumber *>*dependentOriginSecondaryElementsCountList;
/// Primary element address of the Path Target, or the group address or the virtual address to which the
/// Path Target is subscribed.
@property (nonatomic, assign) UInt16 destination;
/// Number of secondary elements of the Path Target.
@property (nonatomic, assign) UInt8 pathTargetSecondaryElementsCount;
/// List of primary element addresses of dependent nodes of the Path Target. Each list entry has a corresponding
/// Dependent_Target_Secondary_Elements_Count_List entry. Size is variable (16 * N2).
@property (nonatomic,strong) NSMutableArray <NSNumber *>*dependentTargetList;
/// List of numbers of secondary elements of dependent nodes of the Path Target. Each list
/// entry has a corresponding Dependent_Target_List entry. Size is variable (16 * N1).
@property (nonatomic,strong) NSMutableArray <NSNumber *>*dependentTargetSecondaryElementsCountList;
/// Forwarding number of the Path Origin; if the entry is associated with a fixed path, the value is 0.
@property (nonatomic, assign) UInt8 forwardingNumber;
/// Number of lanes discovered; if the entry is associated with a fixed path, the value is 1.
@property (nonatomic, assign) UInt8 laneCounter;
/// If the node is not the Path Origin, the bearer index to be used for forwarding messages
/// directed to the Path Origin; otherwise, the unassigned bearer index.
@property (nonatomic, assign) UInt16 bearerTowardPathOrigin;
/// If the node is not the Path Target, the bearer index to be used for forwarding messages
/// directed to the Path Target; otherwise, the unassigned bearer index.
@property (nonatomic, assign) UInt16 bearerTowardPathTarget;

@end


/// 4.2.29 Forwarding Table
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.268)
@interface SigForwardingTableModel : SigModel
/// NetKey Index of the NetKey used in the subnet.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// Indicates whether or not the destination of the path is a unicast address.
@property (nonatomic,assign) BOOL unicastDestinationFlag;
/// Indicates whether or not the backward path has been validated.
@property (nonatomic,assign) BOOL backwardPathValidatedFlag;
/// Unicast address range of the Path Origin.
@property (nonatomic, strong, nullable) SigUnicastAddressRangeFormatModel *pathOriginUnicastAddrRange;
/// Unicast address range of the Path Target (C.1)(C.1: If the value of the Unicast_Destination_Flag field is 1,
/// then the Path_Target_Unicast_Addr_Range field shall be present; otherwise,
/// the Path_Target_Unicast_Addr_Range field shall not be present.)
@property (nonatomic, strong, nullable) SigUnicastAddressRangeFormatModel *pathTargetUnicastAddrRange;
/// Multicast destination address (C.2)(C.2: If the value of the Unicast_Destination_Flag field is 0,
/// then the Multicast_Destination field shall be present; otherwise,
/// the Multicast_Destination field shall not be present.)
@property (nonatomic, assign) UInt16 multicastDestination;
/// Index of the bearer toward the Path Origin.
@property (nonatomic, assign) UInt16 bearerTowardPathOrigin;
/// Index of the bearer toward the Path Target.
@property (nonatomic, assign) UInt16 bearerTowardPathTarget;

/// 计算属性，转发表的起始地址
/// Computed attribute, source address of forwarding table
@property (nonatomic, assign) UInt16 tableSource;
/// 计算属性，转发表的目的地址
/// Computed attribute, destination address of forwarding table
@property (nonatomic, assign) UInt16 tableDestination;
/// 存储属性，添加了这个转发表的设备地址
/// Storage attributes, added device address calculation attributes of this forwarding table, starting address of the forwarding table
@property (nonatomic,strong) NSMutableArray <NSNumber *>*entryNodeAddress;


/// The Forwarding Table Update Identifier state is a 16-bit value that changes after each change to
/// the Forwarding Table Entries state (see Section 4.2.29.2). The Forwarding Table Update Identifier
/// state is initialized to 0.
@property (nonatomic,assign) UInt16 forwardingTableUpdateIdentifier;
/// The Forwarding Table Entries state is a list of entries for all paths that contain the node in a given subnet.
@property (nonatomic,strong) NSMutableArray <SigForwardingTableEntryModel *>*forwardingTableEntries;
/// SigForwardingTableModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;

/**
 * @brief   Initialize SigForwardingTableModel object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * @param   unicastDestinationFlag    Indicates whether or not the destination of the path is a unicast address.
 * @param   backwardPathValidatedFlag    Indicates whether or not the backward path has been validated.
 * @param   pathOriginUnicastAddrRange    Unicast address range of the Path Origin.
 * @param   pathTargetUnicastAddrRange    Unicast address range of the Path Target (C.1)(C.1: If the value of the Unicast_Destination_Flag field is 1,
 * then the Path_Target_Unicast_Addr_Range field shall be present; otherwise,
 * the Path_Target_Unicast_Addr_Range field shall not be present.)
 * @param   multicastDestination    Multicast destination address (C.2)(C.2: If the value of the Unicast_Destination_Flag field is 0,
 * then the Multicast_Destination field shall be present; otherwise,
 * the Multicast_Destination field shall not be present.)
 * @param   bearerTowardPathOrigin    Index of the bearer toward the Path Origin.
 * @param   bearerTowardPathTarget    Index of the bearer toward the Path Target.
 * @return  return `nil` when initialize SigForwardingTableModel object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex unicastDestinationFlag:(BOOL)unicastDestinationFlag backwardPathValidatedFlag:(BOOL)backwardPathValidatedFlag pathOriginUnicastAddrRange:(SigUnicastAddressRangeFormatModel * __nullable)pathOriginUnicastAddrRange pathTargetUnicastAddrRange:(SigUnicastAddressRangeFormatModel * __nullable)pathTargetUnicastAddrRange multicastDestination:(UInt16)multicastDestination bearerTowardPathOrigin:(UInt16)bearerTowardPathOrigin bearerTowardPathTarget:(UInt16)bearerTowardPathTarget;

/**
 * @brief   Initialize SigForwardingTableModel object.
 * @param   parameters    the hex data of SigForwardingTableModel.
 * @return  return `nil` when initialize SigForwardingTableModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/**
 * @brief   Initialize SigForwardingTableModel object.
 * @param   forwardingTable    the old SigForwardingTableModel object.
 * @return  return `nil` when initialize SigForwardingTableModel object fail.
 */
- (instancetype)initWithOldSigForwardingTableModel:(SigForwardingTableModel *)forwardingTable;

/// get dictionary from SigForwardingTableModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigForwardingTableModel;

/// Set dictionary to SigForwardingTableModel object.
/// @param dictionary SigForwardingTableModel dictionary object.
- (void)setDictionaryToSigForwardingTableModel:(NSDictionary *)dictionary;

/// Get description string.
- (NSString *)getDescription;

/// Get String of Entry Node Address.
- (NSString *)getEntryNodeAddressString;

@end


#pragma mark - 4.3.5 Directed Forwarding Configuration messages


#pragma mark 4.3.5.3 DIRECTED_CONTROL_GET, opcode:0xBF30


/// A DIRECTED_CONTROL_GET message is an acknowledged message used to get
/// the current Directed Control state of a node (see Section 4.2.26).
/// @note   The response to a DIRECTED_CONTROL_GET message is a
/// DIRECTED_CONTROL_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
/// 4.3.5.3 DIRECTED_CONTROL_GET.
@interface SigDirectControlGet : SigConfigMessage
/// NetKey Index of the NetKey used in the subnet.
/// The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
/// associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;

/**
 * @brief   Initialize SigDirectControlGet object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @return  return `nil` when initialize SigDirectControlGet object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex;

@end


#pragma mark 4.3.5.4 DIRECTED_CONTROL_SET, opcode:0xBF31


/// A DIRECTED_CONTROL_SET message is an acknowledged message used to set
/// the Directed Control state of a subnet (see Section 4.2.26).
/// @note   The response to a DIRECTED_CONTROL_SET message is a
/// DIRECTED_CONTROL_STATUS message.
/// @note   If the Directed_Forwarding field is set to Disable, then the Enable value is
/// Prohibited for any of the Directed_Relay, Directed_Proxy, and Directed_Friend fields.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
/// 4.3.5.4 DIRECTED_CONTROL_SET.
@interface SigDirectControlSet : SigConfigMessage
/// NetKey Index of the NetKey used in the subnet.
/// The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
/// associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// New Directed Forwarding state.
/// The Directed_Forwarding field determines the new Directed Forwarding state for the
/// identified subnet, as defined in Section 4.2.26.1.
@property (nonatomic,assign) DirectedForwarding directedForwarding;
/// New Directed Relay state.
/// The Directed_Relay field determines the new Directed Relay state for the identified subnet,
/// as defined in Section 4.2.26.2.
@property (nonatomic,assign) DirectedRelay directedRelay;
/// New Directed Proxy state or value to ignore.
/// If the value of the Directed_Proxy field is Disable or Enable, then the Directed_Proxy field
/// determines the new Directed Proxy state for the identified subnet, as defined in Section 4.2.26.3.
@property (nonatomic,assign) DirectedProxy directedProxy;
/// New Directed Proxy Use Directed Default state or value to ignore.
/// If the value of the Directed_Proxy_Use_Directed_Default field is Disable or Enable,
/// then the Directed_Proxy_Use_Directed_Default field determines the new Directed Proxy
/// Use Directed Default state for the identified subnet, as defined in Section 4.2.26.4.
@property (nonatomic,assign) DirectedProxyUseDirectedDefault directedProxyUseDirectedDefault;
/// New Directed Friend state or value to ignore.
/// If the value of the Directed_Proxy field is either Do Not Process or Disabled, then the value
/// of the Directed_Proxy_Use_Directed_Default field shall be set to Do Not Process. If the value
/// of the Directed_Proxy field is Enable, then the value of the Directed_Proxy_Use_Directed_Default
/// field shall be set to either Disable or Enable.If the value of the Directed_Friend field is Disable or
/// Enable, then the Directed_Friend field determines the new Directed Friend state for the identified
/// subnet, as defined in Section 4.2.26.5.
@property (nonatomic,assign) DirectedFriend directedFriend;

/**
 * @brief   Initialize SigDirectControlSet object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   directedForwarding    New Directed Forwarding state.
 * The Directed_Forwarding field determines the new Directed Forwarding state for the
 * identified subnet, as defined in Section 4.2.26.1.
 * @param   directedRelay    New Directed Relay state.
 * The Directed_Relay field determines the new Directed Relay state for the identified subnet,
 * as defined in Section 4.2.26.2.
 * @param   directedProxy    New Directed Proxy state or value to ignore.
 * If the value of the Directed_Proxy field is Disable or Enable, then the Directed_Proxy field
 * determines the new Directed Proxy state for the identified subnet, as defined in Section 4.2.26.3.
 * @param   directedProxyUseDirectedDefault    New Directed Proxy Use Directed
 * Default state or value to ignore.
 * If the value of the Directed_Proxy_Use_Directed_Default field is Disable or Enable,
 * then the Directed_Proxy_Use_Directed_Default field determines the new Directed Proxy
 * Use Directed Default state for the identified subnet, as defined in Section 4.2.26.4.
 * @param   directedFriend    New Directed Friend state or value to ignore.
 * If the value of the Directed_Proxy field is either Do Not Process or Disabled, then the value
 * of the Directed_Proxy_Use_Directed_Default field shall be set to Do Not Process. If the value
 * of the Directed_Proxy field is Enable, then the value of the Directed_Proxy_Use_Directed_Default
 * field shall be set to either Disable or Enable.If the value of the Directed_Friend field is Disable or
 * Enable, then the Directed_Friend field determines the new Directed Friend state for the identified
 * subnet, as defined in Section 4.2.26.5.
 * @return  return `nil` when initialize SigDirectControlSet object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex directedForwarding:(DirectedForwarding)directedForwarding directedRelay:(DirectedRelay)directedRelay directedProxy:(DirectedProxy)directedProxy directedProxyUseDirectedDefault:(DirectedProxyUseDirectedDefault)directedProxyUseDirectedDefault directedFriend:(DirectedFriend)directedFriend;

@end


#pragma mark 4.3.5.5 DIRECTED_CONTROL_STATUS, opcode:0xBF32


/// A DIRECTED_CONTROL_STATUS message is an unacknowledged message
/// used to report the current Directed Control state of a subnet (see Section 4.2.26).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
/// 4.3.5.4 DIRECTED_CONTROL_STATUS.
@interface SigDirectControlStatus : SigConfigMessage
/// Status code for the requesting message.
/// The Status field reports the Status code for the most recent operation on the
/// Directed Control state. The Status codes are defined in Section 4.3.14.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// NetKey Index of the NetKey used in the subnet.
/// The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
/// associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// New Directed Forwarding state.
/// The Directed_Forwarding field indicates the current Directed Forwarding state
/// for the identified subnet, as defined in Section 4.2.26.1.
@property (nonatomic,assign) DirectedForwardingState directedForwardingState;
/// New Directed Relay state.
/// The Directed_Relay field indicates the current Directed Relay state
/// for the identified subnet, as defined in Section 4.2.26.2.
@property (nonatomic,assign) DirectedRelayState directedRelayState;
/// New Directed Proxy state or value to ignore.
/// The Directed_Proxy field indicates the current Directed Proxy state for
/// the identified subnet, as defined in Section 4.2.26.3, or reports 0xFF.
@property (nonatomic,assign) DirectedProxyState directedProxyState;
/// New Directed Proxy Use Directed Default state or value to ignore.
/// The Directed_Proxy_Use_Directed_Default field indicates the current Directed Proxy Use
/// Directed Default state for the identified subnet, as defined in Section 4.2.26.4, or reports 0xFF.
@property (nonatomic,assign) DirectedProxyUseDirectedDefaultState directedProxyUseDirectedDefaultState;
/// New Directed Friend state or value to ignore.
/// The Directed_Friend field indicates the current Directed Friend state for the identified subnet,
/// as defined in Section 4.2.26.5, or reports 0xFF.
@property (nonatomic,assign) DirectedFriendState directedFriendState;

/**
 * @brief   Initialize SigDirectControlStatus object.
 * @param   parameters    the hex data of SigDirectControlStatus.
 * @return  return `nil` when initialize SigDirectControlStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// get dictionary from SigDirectControlStatus object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigDirectControlStatus;

/// Set dictionary to SigDirectControlStatus object.
/// @param dictionary SigDirectControlStatus dictionary object.
- (void)setDictionaryToSigDirectControlStatus:(NSDictionary *)dictionary;

@end


#pragma mark 4.3.5.12 FORWARDING_TABLE_ADD, opcode:0xBF39


/// A FORWARDING_TABLE_ADD message is an acknowledged message used to add
/// a fixed path entry to the Forwarding Table state of a node or to update an existing
/// fixed path entry (see Section 4.2.29).
/// @note   The response to a FORWARDING_TABLE_ADD message is a
/// FORWARDING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.360),
/// 4.3.5.12 FORWARDING_TABLE_ADD.
@interface SigForwardingTableAdd : SigConfigMessage
/// Forwarding Table Model object.
@property (nonatomic, strong) SigForwardingTableModel *forwardingTableModel;

/**
 * @brief   Initialize SigForwardingTableAdd object.
 * @param   forwardingTableModel    Forwarding Table Model object.
 * @return  return `nil` when initialize SigForwardingTableAdd object fail.
 */
- (instancetype)initWithForwardingTableModel:(SigForwardingTableModel *)forwardingTableModel;

@end


#pragma mark 4.3.5.13 FORWARDING_TABLE_DELETE, opcode:0xBF3A


/// A FORWARDING_TABLE_DELETE message is an acknowledged message used to delete
/// all the path entries from a specific Path Origin to a specific destination from the Forwarding
/// Table state of a node (see Section 4.2.29).
/// @note   The response to a FORWARDING_TABLE_DELETE message is a
/// FORWARDING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.362),
/// 4.3.5.13 FORWARDING_TABLE_DELETE.
@interface SigForwardingTableDelete : SigConfigMessage
/// NetKey Index of the NetKey used in the subnet.
/// The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
/// associated with the Forwarding Table state and is encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// Primary element address of the Path Origin.
/// The Path_Origin and Destination fields identify respectively the Path Origin and the destination
/// of the path entries to be deleted from the Forwarding Table state. The unassigned address,
/// group addresses, and virtual addresses are Prohibited for the Path_Origin field. The unassigned
/// address and the all- directed-forwarding-nodes, all-nodes, and all-relays fixed group addresses
/// are Prohibited for the Destination field. The Path_Origin and Destination fields shall not have the
/// same value.
@property (nonatomic, assign) UInt16 pathOrigin;
/// Destination address.
/// The Path_Origin and Destination fields shall not have the same value.
@property (nonatomic, assign) UInt16 pathDestination;

/**
 * @brief   Initialize SigForwardingTableDelete object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is associated
 * with the Forwarding Table state and is encoded as defined in Section 4.3.1.1.
 * @param   pathOrigin    Primary element address of the Path Origin.
 * The Path_Origin and Destination fields identify respectively the Path Origin and the destination of
 * the path entries to be deleted from the Forwarding Table state. The unassigned address, group
 * addresses, and virtual addresses are Prohibited for the Path_Origin field. The unassigned address
 * and the all- directed-forwarding-nodes, all-nodes, and all-relays fixed group addresses are Prohibited
 * for the Destination field. The Path_Origin and Destination fields shall not have the same value.
 * @param   pathDestination    Destination address.
 * The Path_Origin and Destination fields shall not have the same value.
 * @return  return `nil` when initialize SigForwardingTableDelete object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex pathOrigin:(UInt16)pathOrigin pathDestination:(UInt16)pathDestination;

@end


#pragma mark 4.3.5.14 FORWARDING_TABLE_STATUS, opcode:0xBF3B


/// A FORWARDING_TABLE_STATUS message is an unacknowledged message used to report the status
/// of the most recent operation performed on the Forwarding Table state of a node (see Section 4.2.29).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.362),
/// 4.3.5.14 FORWARDING_TABLE_STATUS.
@interface SigForwardingTableStatus : SigConfigMessage
/// Status code for the requesting message.
/// The Status field reports the Status code for the most recent operation on the Forwarding Table state.
/// The Status codes are defined in Section 4.3.14.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// NetKey Index of the NetKey used in the subnet.
/// The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is associated with the
/// Forwarding Table state and is encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// Primary element address of the Path Origin.
/// The other fields in Table 4.214 contain the values used in the most recent operation to add or delete an
/// entry in the Forwarding Table state.
/// The Path_Origin and Destination fields identify respectively the Path Origin and the destination of the path
/// entries that have been added to or deleted from the Forwarding Table state in the most recent operation.
/// The unassigned address, group addresses, and virtual addresses are prohibited values for the Path_Origin
/// field. The unassigned address is a prohibited value for the Destination field.
@property (nonatomic, assign) UInt16 pathOrigin;
/// Destination address.
/// The other fields in Table 4.214 contain the values used in the most recent operation to add or delete an
/// entry in the Forwarding Table state.
/// The Path_Origin and Destination fields identify respectively the Path Origin and the destination of the path
/// entries that have been added to or deleted from the Forwarding Table state in the most recent operation.
/// The unassigned address, group addresses, and virtual addresses are prohibited values for the Path_Origin
/// field. The unassigned address is a prohibited value for the Destination field.
@property (nonatomic, assign) UInt16 pathDestination;

/**
 * @brief   Initialize SigForwardingTableStatus object.
 * @param   parameters    the hex data of SigForwardingTableStatus.
 * @return  return `nil` when initialize SigForwardingTableStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// callback about Direct Control Status Message
typedef void(^responseDirectControlStatusMessageBlock)(UInt16 source, UInt16 destination,SigDirectControlStatus *responseMessage);
/// callback about Forwarding Table Status Message
typedef void(^responseForwardingTableStatusMessageBlock)(UInt16 source, UInt16 destination,SigForwardingTableStatus *responseMessage);

@interface SDKLibCommand (directForwarding)


#pragma mark - 4.3.5 Directed Forwarding Configuration messages


#pragma mark 4.3.5.3 DIRECTED_CONTROL_GET, opcode:0xBF30


/**
 * @brief   DIRECTED_CONTROL_GET.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
 * 6.6.6 DIRECTED_CONTROL_GET.
 */
+ (SigMessageHandle *)directControlGetWithNetKeyIndex:(UInt16)netKeyIndex destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseDirectControlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.5.4 DIRECTED_CONTROL_SET, opcode:0xBF31


/**
 * @brief   DIRECTED_CONTROL_GET.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   directedForwarding    New Directed Forwarding state.
 * The Directed_Forwarding field determines the new Directed Forwarding state for the
 * identified subnet, as defined in Section 4.2.26.1.
 * @param   directedRelay    New Directed Relay state.
 * The Directed_Relay field determines the new Directed Relay state for the identified subnet,
 * as defined in Section 4.2.26.2.
 * @param   directedProxy    New Directed Proxy state or value to ignore.
 * If the value of the Directed_Proxy field is Disable or Enable, then the Directed_Proxy field
 * determines the new Directed Proxy state for the identified subnet, as defined in Section 4.2.26.3.
 * @param   directedProxyUseDirectedDefault    New Directed Proxy Use Directed Default state or value to ignore.
 * If the value of the Directed_Proxy_Use_Directed_Default field is Disable or Enable,
 * then the Directed_Proxy_Use_Directed_Default field determines the new Directed Proxy
 * Use Directed Default state for the identified subnet, as defined in Section 4.2.26.4.
 * @param   directedFriend    New Directed Friend state or value to ignore.
 * If the value of the Directed_Proxy field is either Do Not Process or Disabled, then the value
 * of the Directed_Proxy_Use_Directed_Default field shall be set to Do Not Process. If the value
 * of the Directed_Proxy field is Enable, then the value of the Directed_Proxy_Use_Directed_Default
 * field shall be set to either Disable or Enable.If the value of the Directed_Friend field is Disable or
 * Enable, then the Directed_Friend field determines the new Directed Friend state for the identified
 * subnet, as defined in Section 4.2.26.5.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
 * 4.3.5.4 DIRECTED_CONTROL_SET.
 */
+ (SigMessageHandle *)directControlSetWithNetKeyIndex:(UInt16)netKeyIndex directedForwarding:(DirectedForwarding)directedForwarding directedRelay:(DirectedRelay)directedRelay directedProxy:(DirectedProxy)directedProxy directedProxyUseDirectedDefault:(DirectedProxyUseDirectedDefault)directedProxyUseDirectedDefault directedFriend:(DirectedFriend)directedFriend destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseDirectControlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.5.12 FORWARDING_TABLE_ADD, opcode:0xBF39


/**
 * @brief   FORWARDING_TABLE_ADD.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   unicastDestinationFlag    Indicates whether or not the destination of the path is a unicast address.
 * @param   backwardPathValidatedFlag    Indicates whether or not the backward path has been validated.
 * @param   pathOriginUnicastAddrRange    Unicast address range of the Path Origin.
 * @param   pathTargetUnicastAddrRange     Unicast address range of the Path Target (C.1)(C.1: If the value of the Unicast_Destination_Flag field is 1,
 * then the Path_Target_Unicast_Addr_Range field shall be present; otherwise,
 * the Path_Target_Unicast_Addr_Range field shall not be present.)
 * @param   multicastDestination    Multicast destination address (C.2)(C.2: If the value of the Unicast_Destination_Flag field is 0,
 * then the Multicast_Destination field shall be present; otherwise,
 * the Multicast_Destination field shall not be present.)
 * @param   bearerTowardPathOrigin    Index of the bearer toward the Path Origin.
 * @param   bearerTowardPathTarget    Index of the bearer toward the Path Target.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.360),
 * 4.3.5.12 FORWARDING_TABLE_ADD.
 */
+ (SigMessageHandle *)forwardingTableAddWithNetKeyIndex:(UInt16)netKeyIndex unicastDestinationFlag:(BOOL)unicastDestinationFlag backwardPathValidatedFlag:(BOOL)backwardPathValidatedFlag pathOriginUnicastAddrRange:(SigUnicastAddressRangeFormatModel *)pathOriginUnicastAddrRange pathTargetUnicastAddrRange:(SigUnicastAddressRangeFormatModel *)pathTargetUnicastAddrRange multicastDestination:(UInt16)multicastDestination bearerTowardPathOrigin:(UInt16)bearerTowardPathOrigin bearerTowardPathTarget:(UInt16)bearerTowardPathTarget destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseForwardingTableStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.5.13 FORWARDING_TABLE_DELETE, opcode:0xBF3A


/**
 * @brief   FORWARDING_TABLE_DELETE.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   pathOrigin    Primary element address of the Path Origin.
 * The Path_Origin and Destination fields identify respectively the Path Origin and the destination
 * of the path entries to be deleted from the Forwarding Table state. The unassigned address,
 * group addresses, and virtual addresses are Prohibited for the Path_Origin field. The unassigned
 * address and the all- directed-forwarding-nodes, all-nodes, and all-relays fixed group addresses
 * are Prohibited for the Destination field. The Path_Origin and Destination fields shall not have the
 * same value.
 * @param   pathDestination    Destination address.
 * The Path_Origin and Destination fields shall not have the same value.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
 * 4.3.5.13 FORWARDING_TABLE_DELETE.
 */
+ (SigMessageHandle *)forwardingTableDeleteWithNetKeyIndex:(UInt16)netKeyIndex pathOrigin:(UInt16)pathOrigin pathDestination:(UInt16)pathDestination destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseForwardingTableStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


@end

NS_ASSUME_NONNULL_END
