/*    
    Copyright 2020, Network Research Lab at the University of Toronto.

    This file is part of CottonCandy.

    CottonCandy is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CottonCandy is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CottonCandy.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef HEADER_MESSAGE_PROCESSOR
#define HEADER_MESSAGE_PROCESSOR

#define MESSAGE_JOIN              1
#define MESSAGE_JOIN_ACK          2
#define MESSAGE_JOIN_CFM          3
#define MESSAGE_CHECK_ALIVE       4
#define MESSAGE_REPLY_ALIVE       5
#define MESSAGE_GATEWAY_REQ       6
#define MESSAGE_NODE_REPLY        7

#define MSG_LEN_GENERIC           5
#define MSG_LEN_JOIN              5
#define MSG_LEN_JOIN_ACK          6
#define MSG_LEN_JOIN_CFM          6
#define MSG_LEN_CHECK_ALIVE       6
#define MSG_LEN_REPLY_ALIVE       5
#define MSG_LEN_GATEWAY_REQ       14
#define MSG_LEN_HEADER_NODE_REPLY 7

#define MAX_LEN_DATA_NODE_REPLY 64

#include "DeviceDriver.h"

union LongConverter{
    unsigned long l;
    byte b[4];
};

/* We will use polymorphism here */

class GenericMessage
{

public:
    byte type;
    byte srcAddr[2];
    byte destAddr[2];
    
    /**
     * For every message receveid, there will be an RSSI value associated
     */
    int rssi;

    GenericMessage(byte type, byte* srcAddr, byte* destAddr);
    // return number of bytes sent
    virtual int send(DeviceDriver* driver, byte* destAddr);
    void copyTypeAndAddr(byte* msg);

    virtual ~GenericMessage();
};

/*--------------------Join Beacon-------------------*/
class Join: public GenericMessage
{
public:
    Join(byte* srcAddr, byte* destAddr);
};

/*--------------------JoinACK Message-------------------*/
class JoinAck: public GenericMessage
{
public:
    byte hopsToGateway;
    JoinAck(byte* srcAddr, byte* destAddr, byte hopsToGateway);
    int send(DeviceDriver* driver, byte* destAddr);
};

/*--------------------JoinCFM Message-------------------*/
class JoinCFM: public GenericMessage
{
public:
    byte depth;

    JoinCFM(byte* srcAddr, byte* destAddr, byte depth);
    int send(DeviceDriver* driver, byte* destAddr);
};

/*--------------------CheckAlive Message-------------------*/
class CheckAlive: public GenericMessage
{
public:
    byte depth;

    CheckAlive(byte* srcAddr, byte* destAddr, byte depth);
    int send(DeviceDriver* driver, byte* destAddr);
};

/*--------------------ReplyAlive Message-------------------*/
class ReplyAlive: public GenericMessage
{
public:
    ReplyAlive(byte* srcAddr, byte* destAddr);
};


/*--------------------GatewayRequest Message-------------------*/
class GatewayRequest: public GenericMessage
{
public:
    byte seqNum;
    unsigned long nextReqTime;
    unsigned long childBackoffTime;

    GatewayRequest(byte* srcAddr, byte* destAddr, byte seqNum, unsigned long nextReqTime, unsigned long childBackoffTime);
    int send(DeviceDriver* driver, byte* destAddr);
};

/*--------------------NodeReply Message-------------------*/
class NodeReply: public GenericMessage
{
public:
    byte seqNum;
    byte dataLength;
    byte* data; // maximum length 64 bytes

    NodeReply(byte* srcAddr, byte* destAddr, byte seqNum, 
                byte dataLength, byte* data);
    ~NodeReply();
    int send(DeviceDriver* driver, byte* destAddr);
};

/*
 * Reads from device buffer, constructs a message and returns a pointer to it.
 * The timeout value will be used for terminating the receiving in the following
 * 2 scenarios:
 * 
 *      1. No valid message has been received
 *      2. Valid messages are received but are truncated due to collisions or
 *         other wireless signal corruption. In this case, the program can
 *         block to read the remaining data. For messages with variable length 
 *         (i.e. NodeReply). They can be received but the "length" field can be 
 *         corrupted. Thus, the program might block to read an arbitarily long 
 *         byte array, which causes the program to hang.
 * 
 * Note that the timeout value does not limit the program run-time. The actual run
 * time might exceed 1 second.
 * 
 * !! Caller needs to free the memory after using the returned pointer
 */
GenericMessage* receiveMessage(DeviceDriver* driver, unsigned long timeout);


/*
 * Read certain bytes from device buffer
 * !! Caller needs to free the memory after using the returned pointer
 */
byte* readMsgFromBuff(DeviceDriver* driver, uint8_t msgLen, unsigned long timeout);

#endif
