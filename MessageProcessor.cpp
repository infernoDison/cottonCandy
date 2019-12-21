#include "MessageProcessor.h"
#include <string.h>


GenericMessage::GenericMessage(unsigned char type, address srcAddr)
{
  this->type = type;
  this->srcAddr = srcAddr;
}

//Note:Little Endian reverse the order of bytes

void GenericMessage::copyTypeAndAddr(char* msg)
{
  msg[0] = this->type;
  //reverse the bytes
  msg[1] = ((unsigned char *)&(this->srcAddr))[1];
  msg[2] = ((unsigned char *)&(this->srcAddr))[0];
}

int GenericMessage::send(DeviceDriver* driver, address destAddr)
{
  char msg[3]; 
  copyTypeAndAddr(msg);

  //TODO: send out using device driver
}

/*--------------------Join Beacon-------------------*/
Join::Join(address srcAddr) : GenericMessage(MESSAGE_JOIN, srcAddr){

}


/*--------------------JoinACK Message-------------------*/
JoinAck::JoinAck(address srcAddr) : GenericMessage(MESSAGE_JOIN_ACK, srcAddr){

}

/*--------------------JoinCFM Message-------------------*/
JoinCFM::JoinCFM(address srcAddr, unsigned char depth) : GenericMessage(MESSAGE_JOIN_CFM, srcAddr)
{
  this->depth = depth;
}

int JoinCFM::send(DeviceDriver* driver, address destAddr)
{
  char msg[4];
  copyTypeAndAddr(msg);

  msg[3] = this->depth;

  //TODO: send out using device driver
}

/*--------------------CheckAlive Message-------------------*/
CheckAlive::CheckAlive(address srcAddr, unsigned char depth) : GenericMessage(MESSAGE_CHECK_ALIVE, srcAddr)
{
  this->depth = depth;
}

int CheckAlive::send(DeviceDriver* driver, address destAddr)
{
  char msg[4];
  copyTypeAndAddr(msg);

  msg[3] = this->depth;

  //TODO: send out using device driver
}


/*--------------------ReplyAlive Message-------------------*/
ReplyAlive::ReplyAlive(address srcAddr) : GenericMessage(MESSAGE_REPLY_ALIVE, srcAddr){
  
}

/*--------------------GatewayRequest Message-------------------*/
GatewayRequest::GatewayRequest(address srcAddr, unsigned char seqNum): GenericMessage(MESSAGE_GATEWAY_REQ, srcAddr)
{
  this->seqNum = seqNum;
}

int GatewayRequest::send(DeviceDriver* driver, address destAddr)
{
  char msg[4];
  copyTypeAndAddr(msg);

  msg[3] = this->seqNum;

  //TODO: send out using device driver
}

/*--------------------NodeReply Message-------------------*/
NodeReply::NodeReply(address srcAddr, unsigned char numOfNodes, unsigned char seqNum, 
                unsigned char dataLength, char data[8]) : GenericMessage(MESSAGE_NODE_REPLY, srcAddr)
{
  this->type = type;
  this->srcAddr = srcAddr;
  this->numOfNodes = numOfNodes;
  this->seqNum = seqNum;
  this->dataLength = dataLength;
  strcpy(this->data, data);
}

int NodeReply::send(DeviceDriver* driver, address destAddr)
{
  char msg[14];
  copyTypeAndAddr(msg);

  msg[3] = this->numOfNodes;
  msg[4] = this->seqNum;
  msg[5] = this->dataLength;

  for(int i = 0; i < 8; i++)
  {
    msg[6 + i] = data[i];
  }

  //TODO: send out using device driver
}


