package com.telink.ble.mesh.model.json; /********************************************************************************************************
 * @file MeshStorageTest.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************//*

package com.telink.ble.mesh.model.json;

import com.google.gson.Gson;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

*/
/**
 * Created by kee on 2018/9/10.
 *//*


public class MeshStorageTest {
    private static final String jsonData = "{\n" +
            "    \"$schema\": \"test\",\n" +
            "    \"version\": \"\",\n" +
            "    \"meshUUID\": \"00000000000000000000000000000000\",\n" +
            "    \"meshName\": \"\",\n" +
            "    \"timestamp\": \"0000400000005a8dfa8e\",\n" +
            "    \"netKeys\": [\n" +
            "        {\n" +
            "            \"ivi_idx\": \"11223344\",\n" +
            "            \"name\": \"\",\n" +
            "            \"key\": \"1122c2c3c4c5c6c7d8d9dadbdcdddedf\",\n" +
            "            \"oldKey\": \"00000000000000000000000000000000\",\n" +
            "            \"minSecurity\": \"high\",\n" +
            "            \"index\": 0,\n" +
            "            \"phase\": 0\n" +
            "        }\n" +
            "    ],\n" +
            "    \"appKeys\": [\n" +
            "        {\n" +
            "            \"name\": \"\",\n" +
            "            \"index\": 0,\n" +
            "            \"boundNetKey\": 0,\n" +
            "            \"key\": \"60964771734fbd76e3b40519d1d94a48\",\n" +
            "            \"oldKey\": \"00000000000000000000000000000000\"\n" +
            "        }\n" +
            "    ],\n" +
            "    \"provisioners\": [\n" +
            "        {\n" +
            "            \"provisionerName\": \"\",\n" +
            "            \"UUID\": \"00000000000000000000000000000000\",\n" +
            "            \"allocatedGroupRange\": [],\n" +
            "            \"allocatedUnicastRange\": [],\n" +
            "            \"allocatedSceneRange\": []\n" +
            "        }\n" +
            "    ],\n" +
            "    \"nodes\": [\n" +
            "        {\n" +
            "            \"UUID\": \"00000000000000000000000000000000\",\n" +
            "            \"macAddress\":\"112233445566\",\n" +
            "            \"name\": \"\",\n" +
            "            \"deviceKey\": \"00000000000000000000000000000000\",\n" +
            "            \"unicastAddress\": \"0003\",\n" +
            "            \"security\": \"\",\n" +
            "            \"cid\": \"0000\",\n" +
            "            \"pid\": \"0000\",\n" +
            "            \"vid\": \"0000\",\n" +
            "            \"crpl\": \"0000\",\n" +
            "            \"features\": {\n" +
            "                \"relay\": 0,\n" +
            "                \"proxy\": 0,\n" +
            "                \"friend\": 0,\n" +
            "                \"lowPower\": 0\n" +
            "            },\n" +
            "            \"relayRetransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"networkTransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"netKeys\": [\n" +
            "                {\n" +
            "                    \"index\": 0\n" +
            "                }\n" +
            "            ],\n" +
            "            \"appKeys\": [],\n" +
            "            \"elements\": [],\n" +
            "            \"secureNetworkBeacon\": false,\n" +
            "            \"configComplete\": false,\n" +
            "            \"blacklisted\": false,\n" +
            "            \"defaultTTL\": 0\n" +
            "        },\n" +
            "        {\n" +
            "            \"UUID\": \"00000000000000000000000000000000\",\n" +
            "            \"name\": \"\",\n" +
            "            \"deviceKey\": \"8b7a6f4c1bfd26bf1e93f16e6e359eaa\",\n" +
            "            \"unicastAddress\": \"0004\",\n" +
            "            \"security\": \"\",\n" +
            "            \"cid\": \"0211\",\n" +
            "            \"pid\": \"0001\",\n" +
            "            \"vid\": \"0001\",\n" +
            "            \"crpl\": \"0000\",\n" +
            "            \"features\": {\n" +
            "                \"relay\": 1,\n" +
            "                \"proxy\": 1,\n" +
            "                \"friend\": 1,\n" +
            "                \"lowPower\": 0\n" +
            "            },\n" +
            "            \"relayRetransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"networkTransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"netKeys\": [\n" +
            "                {\n" +
            "                    \"index\": 0\n" +
            "                }\n" +
            "            ],\n" +
            "            \"appKeys\": [],\n" +
            "            \"elements\": [\n" +
            "                {\n" +
            "                    \"name\": \"\",\n" +
            "                    \"index\": 0,\n" +
            "                    \"location\": \"0000\",\n" +
            "                    \"models\": [\n" +
            "                        {\n" +
            "                            \"modelId\": \"0000\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"0002\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"0003\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"fe00\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"fe01\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"fe02\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"ff00\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"ff01\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1000\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1002\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1004\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1300\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1301\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1303\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1304\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"1306\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        },\n" +
            "                        {\n" +
            "                            \"modelId\": \"02110000\",\n" +
            "                            \"subscribe\": [],\n" +
            "                            \"bind\": [],\n" +
            "                            \"publish\": {\n" +
            "                                \"address\": \"0000\",\n" +
            "                                \"index\": 0,\n" +
            "                                \"ttl\": 0,\n" +
            "                                \"period\": 0,\n" +
            "                                \"credentials\": 0,\n" +
            "                                \"retransmit\": {\n" +
            "                                    \"count\": 0,\n" +
            "                                    \"interval\": 0\n" +
            "                                }\n" +
            "                            }\n" +
            "                        }\n" +
            "                    ]\n" +
            "                }\n" +
            "            ],\n" +
            "            \"secureNetworkBeacon\": false,\n" +
            "            \"configComplete\": false,\n" +
            "            \"blacklisted\": false,\n" +
            "            \"defaultTTL\": 0\n" +
            "        },\n" +
            "        {\n" +
            "            \"UUID\": \"00000000000000000000000000000000\",\n" +
            "            \"name\": \"\",\n" +
            "            \"deviceKey\": \"00000000000000000000000000000000\",\n" +
            "            \"unicastAddress\": \"0005\",\n" +
            "            \"security\": \"\",\n" +
            "            \"cid\": \"0000\",\n" +
            "            \"pid\": \"0000\",\n" +
            "            \"vid\": \"0000\",\n" +
            "            \"crpl\": \"0000\",\n" +
            "            \"features\": {\n" +
            "                \"relay\": 0,\n" +
            "                \"proxy\": 0,\n" +
            "                \"friend\": 0,\n" +
            "                \"lowPower\": 0\n" +
            "            },\n" +
            "            \"relayRetransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"networkTransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"netKeys\": [],\n" +
            "            \"appKeys\": [],\n" +
            "            \"elements\": [],\n" +
            "            \"secureNetworkBeacon\": false,\n" +
            "            \"configComplete\": false,\n" +
            "            \"blacklisted\": false,\n" +
            "            \"defaultTTL\": 0\n" +
            "        },\n" +
            "        {\n" +
            "            \"UUID\": \"00000000000000000000000000000000\",\n" +
            "            \"name\": \"\",\n" +
            "            \"deviceKey\": \"00000000000000000000000000000000\",\n" +
            "            \"unicastAddress\": \"0006\",\n" +
            "            \"security\": \"\",\n" +
            "            \"cid\": \"0000\",\n" +
            "            \"pid\": \"0000\",\n" +
            "            \"vid\": \"0000\",\n" +
            "            \"crpl\": \"0000\",\n" +
            "            \"features\": {\n" +
            "                \"relay\": 0,\n" +
            "                \"proxy\": 0,\n" +
            "                \"friend\": 0,\n" +
            "                \"lowPower\": 0\n" +
            "            },\n" +
            "            \"relayRetransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"networkTransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"netKeys\": [],\n" +
            "            \"appKeys\": [],\n" +
            "            \"elements\": [],\n" +
            "            \"secureNetworkBeacon\": false,\n" +
            "            \"configComplete\": false,\n" +
            "            \"blacklisted\": false,\n" +
            "            \"defaultTTL\": 0\n" +
            "        },\n" +
            "        {\n" +
            "            \"UUID\": \"00000000000000000000000000000000\",\n" +
            "            \"name\": \"\",\n" +
            "            \"deviceKey\": \"00000000000000000000000000000000\",\n" +
            "            \"unicastAddress\": \"0007\",\n" +
            "            \"security\": \"\",\n" +
            "            \"cid\": \"0000\",\n" +
            "            \"pid\": \"0000\",\n" +
            "            \"vid\": \"0000\",\n" +
            "            \"crpl\": \"0000\",\n" +
            "            \"features\": {\n" +
            "                \"relay\": 0,\n" +
            "                \"proxy\": 0,\n" +
            "                \"friend\": 0,\n" +
            "                \"lowPower\": 0\n" +
            "            },\n" +
            "            \"relayRetransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"networkTransmit\": {\n" +
            "                \"count\": 0,\n" +
            "                \"interval\": 0\n" +
            "            },\n" +
            "            \"netKeys\": [],\n" +
            "            \"appKeys\": [],\n" +
            "            \"elements\": [],\n" +
            "            \"secureNetworkBeacon\": false,\n" +
            "            \"configComplete\": false,\n" +
            "            \"blacklisted\": false,\n" +
            "            \"defaultTTL\": 0\n" +
            "        }\n" +
            "    ],\n" +
            "    \"groups\": [\n" +
            "        {\n" +
            "            \"name\": \"\",\n" +
            "            \"address\": \"0000\",\n" +
            "            \"parentAddress\": \"0000\"\n" +
            "        }\n" +
            "    ],\n" +
            "    \"scenes\": [\n" +
            "        {\n" +
            "            \"name\": \"\",\n" +
            "            \"number\": 0,\n" +
            "            \"addresses\": []\n" +
            "        }\n" +
            "    ]\n" +
            "}";


    public static void main(String args[]) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putLong(0x1234567890123456L);
//        byteBuffer.;
        System.out.print("bytes: " + Arrays.bytesToHexString(byteBuffer.array(), ""));
        */
/*Gson gson = new Gson();
        MeshStorage instance = gson.fromJson(jsonData, MeshStorage.class);
        String json = gson.toJson(instance);
        System.out.print("json: " + json);*//*

//        testJson();
//        System.out.print("val: " + String.format("%020X",393939393));
//        testNodeInfo();
    }

    private static void testNodeInfo() {
        */
/*byte[] data = {
                (byte) 0x04, (byte) 0x00, (byte) 0x02, (byte) 0xFF, (byte) 0x3F, (byte) 0xD4, (byte) 0xB1, (byte) 0x76, (byte) 0xAC, (byte) 0x1E, (byte) 0x34, (byte) 0x9B, (byte) 0xBA, (byte) 0xDC, (byte) 0x1C, (byte) 0x94, (byte) 0x9E, (byte) 0xB8, (byte) 0xAB, (byte) 0x55, (byte) 0x38, (byte) 0x00, (byte) 0x11, (byte) 0x02, (byte) 0x01, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x07, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x0F, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0xFE, (byte) 0x01, (byte) 0xFE, (byte) 0x02, (byte) 0xFE, (byte) 0x00, (byte) 0xFF, (byte) 0x01, (byte) 0xFF, (byte) 0x00, (byte) 0x10, (byte) 0x02, (byte) 0x10, (byte) 0x04, (byte) 0x10, (byte) 0x00, (byte) 0x13, (byte) 0x01, (byte) 0x13, (byte) 0x03, (byte) 0x13, (byte) 0x04, (byte) 0x13, (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x02, (byte) 0x10, (byte) 0x06, (byte) 0x13, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF
        };
        System.out.println("node info1: " + Arrays.bytesToHexString(data, ":"));
        NodeInfo nodeInfo = NodeInfo.from(data);
        System.out.println("node info: " + nodeInfo.elementCnt + " -- " + nodeInfo.cpsData.elements.get(0).sigModels.get(0) + " -- " + nodeInfo.cpsDataLen);

        byte[] re = nodeInfo.toVCNodeInfo();
        System.out.println("node info2: " + Arrays.bytesToHexString(re, ":"));*//*


        byte[] data = {
                (byte) 0x04, (byte) 0x00, (byte) 0x02, (byte) 0xFF, (byte) 0x3F, (byte) 0xD4, (byte) 0xB1, (byte) 0x76, (byte) 0xAC, (byte) 0x1E, (byte) 0x34, (byte) 0x9B, (byte) 0xBA, (byte) 0xDC, (byte) 0x1C, (byte) 0x94, (byte) 0x9E, (byte) 0xB8, (byte) 0xAB, (byte) 0x55, (byte) 0x38, (byte) 0x00, (byte) 0x11, (byte) 0x02, (byte) 0x01, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x07, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x0F, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0xFE, (byte) 0x01, (byte) 0xFE, (byte) 0x02, (byte) 0xFE, (byte) 0x00, (byte) 0xFF, (byte) 0x01, (byte) 0xFF, (byte) 0x00, (byte) 0x10, (byte) 0x02, (byte) 0x10, (byte) 0x04, (byte) 0x10, (byte) 0x00, (byte) 0x13, (byte) 0x01, (byte) 0x13, (byte) 0x03, (byte) 0x13, (byte) 0x04, (byte) 0x13, (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x02, (byte) 0x10, (byte) 0x06, (byte) 0x13, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF, (byte) 0xFF
        };
        System.out.println("node info1: " + Arrays.bytesToHexString(data, ":"));

        NodeInfo nodeInfoFromData = NodeInfo.from(data);
        byte[] re = nodeInfoFromData.toVCNodeInfo();
        System.out.println("node info2: " + Arrays.bytesToHexString(re, ":"));

        Gson gson = new Gson();
        MeshStorage instance = gson.fromJson(jsonData, MeshStorage.class);
        MeshStorageService dataService = MeshStorageService.getInstance();
        NodeInfo nodeInfo = dataService.convertNodeToNodeInfo(instance.nodes.get(1));
        System.out.println("node info3: " + Arrays.bytesToHexString(nodeInfo.toVCNodeInfo(), ":"));
    }


    // OFFSET = 946684800
    private void calculateTime() throws ParseException {

        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Calendar cal = Calendar.getInstance();
        cal.setTime(sdf.parse("2000-1-1 00:00:00"));
        long time1 = cal.getTimeInMillis();
        MeshLogger.log("2000-1-1 00:00:00 time: " + time1 / 1000);
        cal.setTime(sdf.parse("1970-1-1 00:00:00"));
        long time2 = cal.getTimeInMillis();
        MeshLogger.log("1970-1-1 00:00:00 time: " + time2 / 1000);
        MeshLogger.log("period: " + (time1 - time2) / 1000);

        MeshLogger.log("current time: " + System.currentTimeMillis() / 1000 + "zone: " + cal.get(Calendar.ZONE_OFFSET) / 60 / 60 / 1000);
    }

    private static void testJson() {
        Gson gson = new Gson();
        List<String> names = new ArrayList<>(2);
        String json = gson.toJson(names);
        System.out.print("json: " + json);
    }
}
*/
