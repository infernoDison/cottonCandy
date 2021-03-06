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

#ifndef HEADER_FORWARD_ENGINE
#define HEADER_FORWARD_ENGINE

#include "DeviceDriver.h"
#include "MessageProcessor.h"
#include "Utilities.h"

/*-------------States of a Node------------*/
#define INIT 0
#define SEARCH 1
#define SELECT 2
#define CREATE 3
#define JOINED 4

/* Setting the first bit of the address to 1 indicates a gateways */
#define GATEWAY_ADDRESS_MASK 0x80

/* The default time for discovery is 10 seconds */
#define DISCOVERY_TIMEOUT 10000

/* The default timeout value for receiving a message is 1 seconds */
#define RECEIVE_TIMEOUT 1000

/* The RSSI threshold for choosing a parent node */
#define RSSI_THRESHOLD -100

/* The maximum number of children a node can have */
#define MAX_NUM_CHILDREN 5

/* The default time interval for checking if the parent is alive */
#define DEFAULT_CHECK_ALIVE_INTERVAL 30000

/* The default time interval for checking if the parent is alive */
#define CHECK_ALIVE_TIMEOUT 10000

/* The minimum backoff time when the node reply back */
#define MIN_BACKOFF_TIME 100

/** The maximum backoff time when tranmitting a JoinACK message
 * 
 * Unlike the backoff time for sending NodeReply and forwarding GatewayReq, this is a static value
 * since the chance of collisions depends on the number of nearby connected nodes, which can not be
 * easily inferred. Also, setting the backoff time too long (DISCOVERY_TIMEOUT) for JoinACK can 
 * results in a consistent discovery timeout.
*/
#define MAX_JOIN_ACK_BACKOFF_TIME 3000

/** The maximum backoff time for one child node to send NodeReply or forward GatewayReq 
* 
Parent node (including gateway) uses this value and multiply it with the number of child nodes
* to inform the child nodes of the maximum backoff time they have to wait.
*/
#define MAX_BACKOFF_TIME_FOR_ONE_CHILD 3000

/** Default time for waiting for the next GatewayRequest is a day (24 hours = 86,400,000 milliseconds).
 * If the user do not specify the GatewayReq time during setup,  the node will wait forever for
 * the next GatewayRequest. If connection is broken before the next GatewayRequest comes in,
 * the node will not self-heal until a full day is passed
*/
#define DEFAULT_NEXT_GATEWAY_REQ_TIME 86400E3

/** Gateway request may arrive later than expected due to transmission and processing delays.
 * The node marks a missing gateway request if none is received for 
 *         (NEXT_GATEWAY_REQ_TIME_TOLERANCE_FACTOR * Advertised next request time interval)
*/
#define NEXT_GATEWAY_REQ_TIME_TOLERANCE_FACTOR 1.2

struct ParentInfo{
    unsigned long lastAliveTime;
    byte hopsToGateway;
    
    byte parentAddr[2];
    int Rssi;  
    bool requireChecking;
};

struct ChildNode{
    byte nodeAddr[2];

    ChildNode* next;
};

class ForwardEngine{

public:
    /**
     * Copy constructor
     */
    ForwardEngine(const ForwardEngine &node);

    /**
     * Destructor
     */
    ~ForwardEngine();

    /**
     * Constructor. Requires driver and assigned addr
     * TODO: Add the function pointer for callback
     */
    ForwardEngine(byte* addr, DeviceDriver* driver);

    /**
     * Try to join an existing network by finding a parent. Return true if successfully joined an 
     * existing network
     */
    bool join();

    /**
     * Node exits an existing network
     */
    void disconnect();

    /**
     * This is the core loop where the node operates sending and receiving after it  joined the network.
     * Returns true if the node is started successfully.
     * 
     * Note: This method can be used without calling join() prior. In this case, the node assumes that
     * there is no existing network and it is the first node in the new network. 
     */
    bool run();


    //Setter for the node address
    void setAddr(byte* addr);

    /**
     * Getter for the node address
     */
    byte* getMyAddr();

    /**
     * Getter for the parent address
     */
    byte* getParentAddr();

    void setGatewayReqTime(unsigned long gatewayReqTime);

    unsigned long getGatewayReqTime();

    void onReceiveRequest(void(*callback)(byte**, byte*));
    void onReceiveResponse(void(*callback)(byte*, byte, byte*));


private:
    /**
     * Node address
     */
    byte myAddr[2];

    /**
     * DeviceDriver driver;
     */ 
    DeviceDriver* myDriver;

    /**
     * A record of current parent
     */
    ParentInfo myParent;

    /**
     * Hops to the gateway
     */ 
    byte hopsToGateway; 

    /**
     * Current State
     */
    char state;

    /**
     * Number of direct children currently connected to
     */ 
    uint8_t numChildren;

    /**
     * A linked list of children nodes
     */
    ChildNode* childrenList;

    unsigned long checkAliveInterval = 300000;

    /**
     * Time interval for gateway to request data from nodes
     */
    unsigned long gatewayReqTime = DEFAULT_NEXT_GATEWAY_REQ_TIME; 

    /**
     * The last time that gateway sends out a request
     */
    unsigned long lastReqTime;

    /**
     * Sequence Number used to identify each Gateway Request
     */ 
    uint8_t seqNum = 0;

    /**
     * Maximum time value for the random backoff time before transmitting
     * gateway requests and node replies.
     * 
     * This value can be dynamically changed by the parent node based on its
     * number of child nodes. The parent node informs the child node the new
     * max backoff time in the Gateway Request message. 
     */
    unsigned long maxBackoffTime = MAX_BACKOFF_TIME_FOR_ONE_CHILD;

    /**
     * callback function pointer when Node receives Gateway Requests
     * arguments are to pass back msg and num of bytes
     */ 
    void (*onRecvRequest)(byte**, byte*);

    /**
     * callback function pointer when Gateway receives responses from Nodes
     * argument is msg, num of bytes and sender address
     */ 
    void (*onRecvResponse)(byte*, byte, byte*);


};

#endif
