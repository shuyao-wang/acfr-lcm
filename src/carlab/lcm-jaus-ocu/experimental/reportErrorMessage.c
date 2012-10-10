/*****************************************************************************
 *  Copyright (c) 2009, OpenJAUS.com
 *  All rights reserved.
 *  
 *  This file is part of OpenJAUS.  OpenJAUS is distributed under the BSD 
 *  license.  See the LICENSE file for details.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of the University of Florida nor the names of its 
 *       contributors may be used to endorse or promote products derived from 
 *       this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/
// File Name: reportErrorMessage.c
//
// Written By: Danny Kent (jaus AT dannykent DOT com), Tom Galluzzo (galluzzo AT gmail DOT com)
//
// Version: 3.3.0b
//
// Date: 09/08/09
//
// Description: This file defines the functionality of a ReportErrorMessage

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jaus/jaus.h>
#include "reportErrorMessage.h"

static const int commandCode = JAUS_REPORT_ERROR;
static const int maxDataSizeBytes = 267;

static JausBoolean headerFromBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static JausBoolean headerToBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static int headerToString(ReportErrorMessage message, char **buf);

static JausBoolean dataFromBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static int dataToBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static void dataInitialize(ReportErrorMessage message);
static void dataDestroy(ReportErrorMessage message);
static unsigned int dataSize(ReportErrorMessage message);

// ************************************************************************************************************** //
//                                    USER CONFIGURED FUNCTIONS
// ************************************************************************************************************** //

// Initializes the message-specific fields
static void dataInitialize(ReportErrorMessage message)
{
	// Set initial values of message fields
    message->version = newJausByte (0);
    message->errorCriticality = newJausByte (0);
    message->errorValid = JAUS_FALSE;
    message->reserved = newJausByte (0);
    message->timestamp = jausTimeCreate ();
    message->errorCode = newJausInteger (0);
}

// Destructs the message-specific fields
static void dataDestroy(ReportErrorMessage message)
{
	// Free message fields
    if (message->timestamp) jausTimeDestroy (message->timestamp);
}

// Return boolean of success
static JausBoolean dataFromBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	int index = 0;
    JausByte tmp_byte;
	
    // Unpack Message Fields from Buffer
    
    // Version
    if (!jausByteFromBuffer (&message->version, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_BYTE_SIZE_BYTES;

    if (!jausByteFromBuffer (&tmp_byte, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_BYTE_SIZE_BYTES;

    if (jausByteIsBitSet (tmp_byte, 0)) jausByteSetBit (&message->errorCriticality, 0);
    if (jausByteIsBitSet (tmp_byte, 1)) jausByteSetBit (&message->errorCriticality, 1);
    message->errorValid = jausByteIsBitSet (tmp_byte, 7);


    if (!jausByteFromBuffer (&message->reserved, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_BYTE_SIZE_BYTES;

    message->timestamp = jausTimeCreate ();
    if (!message->timestamp) return JAUS_FALSE;
    if (!jausTimeStampFromBuffer (message->timestamp, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_TIME_STAMP_SIZE_BYTES;

    if (!jausIntegerFromBuffer (&message->errorCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_INTEGER_SIZE_BYTES;

    index += snprintf (message->errorDescription, 256, "%s", buffer+index);

    return JAUS_TRUE;
}

// Returns number of bytes put into the buffer
static int dataToBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	int index = 0;
    JausByte tmp_byte;

    // Pack Message Fields to Buffer

    // Version
    if (!jausByteToBuffer (message->version, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_BYTE_SIZE_BYTES;

    tmp_byte = message->errorCriticality;
    if (message->errorValid)
        jausByteSetBit (&tmp_byte, 7);
    else
        jausByteClearBit (&tmp_byte, 7);

    if (!jausByteToBuffer (tmp_byte, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_BYTE_SIZE_BYTES;


    if (!jausByteToBuffer (message->reserved, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_BYTE_SIZE_BYTES;

    // Timestamp
    if (!jausTimeStampToBuffer (message->timestamp, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_TIME_STAMP_SIZE_BYTES;

    if (!jausIntegerToBuffer (message->errorCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
    index += JAUS_INTEGER_SIZE_BYTES;

    index += snprintf ((char*)(buffer+index), 256, "%s", message->errorDescription);

	return index;
}

static int dataToString(ReportErrorMessage message, char **buf)
{
  //message already verified 

  //Setup temporary string buffer
  
  //Fill in maximum size of output string
  unsigned int bufSize = 800;
  (*buf) = (char*)malloc(sizeof(char)*bufSize);
  
  strcpy((*buf), "\nVersion: ");
  jausByteToHexString (message->version, (*buf)+strlen(*buf));

  strcpy((*buf), "\nError Criticality: ");
  jausByteToString (message->errorCriticality, (*buf)+strlen(*buf));

  strcpy((*buf), "\nError Valid: ");
  if (message->errorValid)
      strcpy((*buf), "Yes");
  else
      strcpy((*buf), "No");

  strcpy((*buf), "\nReserved: ");
  jausByteToHexString (message->reserved, (*buf)+strlen(*buf));

  strcpy((*buf), "\nTime Stamp: ");
  jausTimeToString (message->timestamp, (*buf)+strlen(*buf), bufSize-strlen(*buf)-1);
  
  strcpy((*buf), "\nError Code: ");
  jausIntegerToString (message->errorCode, (*buf)+strlen(*buf));
  
  strcpy((*buf), "\nError Description: ");
  strcpy((*buf), message->errorDescription);

  return (int)strlen((*buf));
}

// Returns number of bytes put into the buffer
static unsigned int dataSize(ReportErrorMessage message)
{
    // Constant size
    return maxDataSizeBytes;
}

// ************************************************************************************************************** //
//                                    NON-USER CONFIGURED FUNCTIONS
// ************************************************************************************************************** //

ReportErrorMessage reportErrorMessageCreate(void)
{
	ReportErrorMessage message;

	message = (ReportErrorMessage)malloc( sizeof(ReportErrorMessageStruct) );
	if(message == NULL)
	{
		return NULL;
	}
	
	// Initialize Values
	message->properties.priority = JAUS_DEFAULT_PRIORITY;
	message->properties.ackNak = JAUS_ACK_NAK_NOT_REQUIRED;
	message->properties.scFlag = JAUS_NOT_SERVICE_CONNECTION_MESSAGE;
	message->properties.expFlag = JAUS_NOT_EXPERIMENTAL_MESSAGE;
	message->properties.version = JAUS_VERSION_3_3;
	message->properties.reserved = 0;
	message->commandCode = commandCode;
	message->destination = jausAddressCreate();
	message->source = jausAddressCreate();
	message->dataFlag = JAUS_SINGLE_DATA_PACKET;
	message->dataSize = maxDataSizeBytes;
	message->sequenceNumber = 0;
	
	dataInitialize(message);
	message->dataSize = dataSize(message);
	
	return message;	
}

void reportErrorMessageDestroy(ReportErrorMessage message)
{
	dataDestroy(message);
	jausAddressDestroy(message->source);
	jausAddressDestroy(message->destination);
	free(message);
}

JausBoolean reportErrorMessageFromBuffer(ReportErrorMessage message, unsigned char* buffer, unsigned int bufferSizeBytes)
{
	int index = 0;
	
	if(headerFromBuffer(message, buffer+index, bufferSizeBytes-index))
	{
		index += JAUS_HEADER_SIZE_BYTES;
		if(dataFromBuffer(message, buffer+index, bufferSizeBytes-index))
		{
			return JAUS_TRUE;
		}
		else
		{
			return JAUS_FALSE;
		}
	}
	else
	{
		return JAUS_FALSE;
	}
}

JausBoolean reportErrorMessageToBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < reportErrorMessageSize(message))
	{
		return JAUS_FALSE; //improper size	
	}
	else
	{	
		message->dataSize = dataToBuffer(message, buffer+JAUS_HEADER_SIZE_BYTES, bufferSizeBytes - JAUS_HEADER_SIZE_BYTES);
		if(headerToBuffer(message, buffer, bufferSizeBytes))
		{
			return JAUS_TRUE;
		}
		else
		{
			return JAUS_FALSE; // headerToReportErrorBuffer failed
		}
	}
}

ReportErrorMessage reportErrorMessageFromJausMessage(JausMessage jausMessage)
{
	ReportErrorMessage message;
	
	if(jausMessage->commandCode != commandCode)
	{
		return NULL; // Wrong message type
	}
	else
	{
		message = (ReportErrorMessage)malloc( sizeof(ReportErrorMessageStruct) );
		if(message == NULL)
		{
			return NULL;
		}
		
		message->properties.priority = jausMessage->properties.priority;
		message->properties.ackNak = jausMessage->properties.ackNak;
		message->properties.scFlag = jausMessage->properties.scFlag;
		message->properties.expFlag = jausMessage->properties.expFlag;
		message->properties.version = jausMessage->properties.version;
		message->properties.reserved = jausMessage->properties.reserved;
		message->commandCode = jausMessage->commandCode;
		message->destination = jausAddressCreate();
		*message->destination = *jausMessage->destination;
		message->source = jausAddressCreate();
		*message->source = *jausMessage->source;
		message->dataSize = jausMessage->dataSize;
		message->dataFlag = jausMessage->dataFlag;
		message->sequenceNumber = jausMessage->sequenceNumber;
		
		// Unpack jausMessage->data
		if(dataFromBuffer(message, jausMessage->data, jausMessage->dataSize))
		{
			return message;
		}
		else
		{
			return NULL;
		}
	}
}

JausMessage reportErrorMessageToJausMessage(ReportErrorMessage message)
{
	JausMessage jausMessage;
	
	jausMessage = (JausMessage)malloc( sizeof(struct JausMessageStruct) );
	if(jausMessage == NULL)
	{
		return NULL;
	}	
	
	jausMessage->properties.priority = message->properties.priority;
	jausMessage->properties.ackNak = message->properties.ackNak;
	jausMessage->properties.scFlag = message->properties.scFlag;
	jausMessage->properties.expFlag = message->properties.expFlag;
	jausMessage->properties.version = message->properties.version;
	jausMessage->properties.reserved = message->properties.reserved;
	jausMessage->commandCode = message->commandCode;
	jausMessage->destination = jausAddressCreate();
	*jausMessage->destination = *message->destination;
	jausMessage->source = jausAddressCreate();
	*jausMessage->source = *message->source;
	jausMessage->dataSize = dataSize(message);
	jausMessage->dataFlag = message->dataFlag;
	jausMessage->sequenceNumber = message->sequenceNumber;
	
	jausMessage->data = (unsigned char *)malloc(jausMessage->dataSize);
	jausMessage->dataSize = dataToBuffer(message, jausMessage->data, jausMessage->dataSize);
		
	return jausMessage;
}

unsigned int reportErrorMessageSize(ReportErrorMessage message)
{
	return (unsigned int)(dataSize(message) + JAUS_HEADER_SIZE_BYTES);
}

char* reportErrorMessageToString(ReportErrorMessage message)
{
  if(message)
  {
    char* buf1 = NULL;
    char* buf2 = NULL;
    
    int returnVal;
    
    //Print the message header to the string buffer
    returnVal = headerToString(message, &buf1);
    
    //Print the message data fields to the string buffer
    returnVal += dataToString(message, &buf2);
    
    char* buf;
    buf = (char*)malloc(strlen(buf1)+strlen(buf2));
    strcpy(buf, buf1);
    strcat(buf, buf2);
    
    free(buf1);
    free(buf2);
    
    return buf;
  }
  else
  {
    char* buf = "Invalid ReportError Message";
    char* msg = (char*)malloc(strlen(buf)+1);
    strcpy(msg, buf);
    return msg;
  }
}
//********************* PRIVATE HEADER FUNCTIONS **********************//

static JausBoolean headerFromBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < JAUS_HEADER_SIZE_BYTES)
	{
		return JAUS_FALSE;
	}
	else
	{
		// unpack header
		message->properties.priority = (buffer[0] & 0x0F);
		message->properties.ackNak	 = ((buffer[0] >> 4) & 0x03);
		message->properties.scFlag	 = ((buffer[0] >> 6) & 0x01);
		message->properties.expFlag	 = ((buffer[0] >> 7) & 0x01);
		message->properties.version	 = (buffer[1] & 0x3F);
		message->properties.reserved = ((buffer[1] >> 6) & 0x03);
		
		message->commandCode = buffer[2] + (buffer[3] << 8);
	
		message->destination->instance = buffer[4];
		message->destination->component = buffer[5];
		message->destination->node = buffer[6];
		message->destination->subsystem = buffer[7];
	
		message->source->instance = buffer[8];
		message->source->component = buffer[9];
		message->source->node = buffer[10];
		message->source->subsystem = buffer[11];
		
		message->dataSize = buffer[12] + ((buffer[13] & 0x0F) << 8);

		message->dataFlag = ((buffer[13] >> 4) & 0x0F);

		message->sequenceNumber = buffer[14] + (buffer[15] << 8);
		
		return JAUS_TRUE;
	}
}

static JausBoolean headerToBuffer(ReportErrorMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	JausUnsignedShort *propertiesPtr = (JausUnsignedShort*)&message->properties;
	
	if(bufferSizeBytes < JAUS_HEADER_SIZE_BYTES)
	{
		return JAUS_FALSE;
	}
	else
	{	
		buffer[0] = (unsigned char)(*propertiesPtr & 0xFF);
		buffer[1] = (unsigned char)((*propertiesPtr & 0xFF00) >> 8);

		buffer[2] = (unsigned char)(message->commandCode & 0xFF);
		buffer[3] = (unsigned char)((message->commandCode & 0xFF00) >> 8);

		buffer[4] = (unsigned char)(message->destination->instance & 0xFF);
		buffer[5] = (unsigned char)(message->destination->component & 0xFF);
		buffer[6] = (unsigned char)(message->destination->node & 0xFF);
		buffer[7] = (unsigned char)(message->destination->subsystem & 0xFF);

		buffer[8] = (unsigned char)(message->source->instance & 0xFF);
		buffer[9] = (unsigned char)(message->source->component & 0xFF);
		buffer[10] = (unsigned char)(message->source->node & 0xFF);
		buffer[11] = (unsigned char)(message->source->subsystem & 0xFF);
		
		buffer[12] = (unsigned char)(message->dataSize & 0xFF);
		buffer[13] = (unsigned char)((message->dataFlag & 0xFF) << 4) | (unsigned char)((message->dataSize & 0x0F00) >> 8);

		buffer[14] = (unsigned char)(message->sequenceNumber & 0xFF);
		buffer[15] = (unsigned char)((message->sequenceNumber & 0xFF00) >> 8);
		
		return JAUS_TRUE;
	}
}

static int headerToString(ReportErrorMessage message, char **buf)
{
  //message existance already verified 

  //Setup temporary string buffer
  
  unsigned int bufSize = 500;
  (*buf) = (char*)malloc(sizeof(char)*bufSize);
  
  strcpy((*buf), jausCommandCodeString(message->commandCode) );
  strcat((*buf), " (0x");
  sprintf((*buf)+strlen(*buf), "%04X", message->commandCode);

  strcat((*buf), ")\nReserved: ");
  jausUnsignedShortToString(message->properties.reserved, (*buf)+strlen(*buf));

  strcat((*buf), "\nVersion: ");
  switch(message->properties.version)
  {
    case 0:
      strcat((*buf), "2.0 and 2.1 compatible");
      break;
    case 1:
      strcat((*buf), "3.0 through 3.1 compatible");
      break;
    case 2:
      strcat((*buf), "3.2 and 3.3 compatible");
      break;
    default:
      strcat((*buf), "Reserved for Future: ");
      jausUnsignedShortToString(message->properties.version, (*buf)+strlen(*buf));
      break;
  }

  strcat((*buf), "\nExp. Flag: ");
  if(message->properties.expFlag == 0)
    strcat((*buf), "Not Experimental");
  else 
    strcat((*buf), "Experimental");
  
  strcat((*buf), "\nSC Flag: ");
  if(message->properties.scFlag == 1)
    strcat((*buf), "Service Connection");
  else
    strcat((*buf), "Not Service Connection");
  
  strcat((*buf), "\nACK/NAK: ");
  switch(message->properties.ackNak)
  {
  case 0:
    strcat((*buf), "None");
    break;
  case 1:
    strcat((*buf), "Request ack/nak");
    break;
  case 2:
    strcat((*buf), "nak response");
    break;
  case 3:
    strcat((*buf), "ack response");
    break;
  default:
    break;
  }
  
  strcat((*buf), "\nPriority: ");
  if(message->properties.priority < 12)
  {
    strcat((*buf), "Normal Priority ");
    jausUnsignedShortToString(message->properties.priority, (*buf)+strlen(*buf));
  }
  else
  {
    strcat((*buf), "Safety Critical Priority ");
    jausUnsignedShortToString(message->properties.priority, (*buf)+strlen(*buf));
  }
  
  strcat((*buf), "\nSource: ");
  jausAddressToString(message->source, (*buf)+strlen(*buf));
  
  strcat((*buf), "\nDestination: ");
  jausAddressToString(message->destination, (*buf)+strlen(*buf));
  
  strcat((*buf), "\nData Size: ");
  jausUnsignedIntegerToString(message->dataSize, (*buf)+strlen(*buf));
  
  strcat((*buf), "\nData Flag: ");
  jausUnsignedIntegerToString(message->dataFlag, (*buf)+strlen(*buf));
  switch(message->dataFlag)
  {
    case 0:
      strcat((*buf), " Only data packet in single-packet stream");
      break;
    case 1:
      strcat((*buf), " First data packet in muti-packet stream");
      break;
    case 2:
      strcat((*buf), " Normal data packet");
      break;
    case 4:
      strcat((*buf), " Retransmitted data packet");
      break;
    case 8:
      strcat((*buf), " Last data packet in stream");
      break;
    default:
      strcat((*buf), " Unrecognized data flag code");
      break;
  }
  
  strcat((*buf), "\nSequence Number: ");
  jausUnsignedShortToString(message->sequenceNumber, (*buf)+strlen(*buf));
  
  return strlen((*buf));
  

}
