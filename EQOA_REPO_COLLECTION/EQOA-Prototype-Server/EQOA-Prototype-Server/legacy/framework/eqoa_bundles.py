'''
Created on May 1, 2016

@author: Ben Turi
'''
from scapy.all import *

import logging
import eqoa_messages
import eqoa_utilities
import eqoa_sessions
import struct


# this is a utility method that allows the creation of a Python 2.7 compatible enumerations
def enum(**enums):
    return type('Enum', (), enums)


###############################################################################
#   
# enumeration to represent the different bundle types
# there is a higher purpose for these that we having figured out
BundleType = enum(NONE=0x99,
                  MESSAGE_BUNDLE_TYPE0=0x00,
                  MSG_ACK_TYPE0=0x03,
                  MESSAGE_BUNDLE_TYPE1=0x10,
                  MSG_ACK_TYPE1=0x13,
                  MESSAGE_BUNDLE_TYPE2=0x20,
                  MSG_ACK_TYPE2=0x23,
                  SES_ACK_MSG_ACK_MSG_BUNDLE_TYPE6=0x63)


#
#
#
class BundleHeader():
    # Null constructor for the BundleHeader class
    def __init__(self):
        self.bundleChannel = 0x00
        self.bundleLength = 0x00
        self.fromMaster = 0
        self.SessionAction = 0x00
        self.SessionID_A = 0x0000
        self.SessionID_B = 0x0000

    ###############################################
    #
    # This method builds a BundleHeader object from known quantities
    #
    def buildBundleHeader(self, bundleChannel, bundleLength, fromMaster, SessionAction, SessionID_A, SessionID_B):
        self.bundleChannel = bundleChannel
        self.bundleLength = bundleLength
        self.fromMaster = fromMaster
        self.SessionAction = SessionAction
        self.SessionID_A = SessionID_A
        self.SessionID_B = SessionID_B

    # This method generates raw bytes from  a Bundle object
    def encodeBundleHeader(self):  # still need to code

        encode_fmt = '<'  # pack as little endian
        encode_fmt += 'B'  # 1 byte for bundle type
        #
        encodedBundle = struct.pack(encode_fmt,
                                    self.bundleType)

        # if the bundle type has a message container, then output them

        return encodedBundleHeader
        #

    def decodeBundleHeader(self, bundleByteArray):
        #
        self.bundleByteArray = bundleByteArray
        decode_fmt = '<'  # unpack as little endian
        decode_fmt += 'H'  # read 2 bytes for bundle class and bundle length
        #
        num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
        s = struct.unpack(decode_fmt, self.bundleByteArray[0:num_bytes_read])  # actually unpacks the messageByteList
        self.bundleByteArray = self.bundleByteArray[num_bytes_read:]  # remove the bytes we read from the message
        #
        self.bundleChannel = (s[0] & 0xF000) >> 12
        self.bundleLength = eqoa_utilities.bunLenDecode(s[0] & 0x0FFF)  # use the modified technique
        #
        print
        print 'BUNDLE CHANNEL         : 0x{:01X}'.format(self.bundleChannel)
        print 'BUNDLE PAYLOAD LENGTH  : {:04d}'.format(self.bundleLength)
        #
        # if bundle channel > 0x80 then read session action and 4 byte session ID
        #
        self.SessionAction = eqoa_sessions.SessionAction.NONE  # NONE -
        #
        if self.bundleChannel > 0x8:  # REMOTE ENDPOINT IS MASTER
            #
            decode_fmt = '<'  # one byte doesn't matter
            decode_fmt += 'B'  # read session option
            decode_fmt += 'I'  # read SessionID coming master
            #
            num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
            s = struct.unpack(decode_fmt,
                              self.bundleByteArray[0:num_bytes_read])  # actually unpacks the messageByteList
            self.bundleByteArray = self.bundleByteArray[num_bytes_read:]  # remove the bytes we read from the message
            self.fromMaster = True
            self.SessionAction = s[0]
            self.SessionID_A = s[1]  #
            self.SessionID_B = 0x000  #
            #
            print 'REMOTE ENDPOINT IS       : MASTER'
            print 'SESSION ACTION BYTE      : 0x{:1X}'.format(self.SessionAction)
            print 'SESSIONID_A from MASTER  : 0x{:08X}'.format(self.SessionID_A)
            print 'SESSIONID_B from MASTER  : 0x{:06X}'.format(self.SessionID_B)
            print

        elif self.bundleChannel <= 0x8:  # REMOTE ENDPOINT IS SLAVE
            #
            decode_fmt = '<'  # one byte doesn't matter
            decode_fmt += 'I'  # read SessionID_A coming from slave
            decode_fmt += 'I'  # read SessionID_B coming from slave
            #
            num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
            s = struct.unpack(decode_fmt,
                              self.bundleByteArray[0:num_bytes_read])  # actually unpacks the messageByteList
            num_bytes_read = num_bytes_read - 1  # I read an extra byte, but don't want to lose it.
            self.bundleByteArray = self.bundleByteArray[num_bytes_read:]  # remove the bytes we read from the message
            self.fromMaster = False
            self.SessionID_A = s[0]  #
            self.SessionID_B = s[1] & 0x0FFF  #
            #
            print 'REMOTE ENDPOINT IS       : SLAVE'
            print 'SESSIONID_A              : 0x{:08X}'.format(self.SessionID_A)
            print 'SESSIONID_B              : 0x{:06X}'.format(self.SessionID_B)
            print

        else:
            print 'I am lost'
            print 'Should not get here'


#
##########################################################################################################
#        
# Represents a bundle of messages
class Bundle():
    # Null constructor for the Bundle object
    def __init__(self):
        self.bundleType = None
        self.messageContainer = None
        self.sessionAck = None
        self.communicationReport = None

    ###############################################
    #
    # This method builds a Bundle object from known quantities
    def buildBundle(self, bundleType):
        self.bundleType = bundleType

    # This method generates raw bytes from  a Bundle object
    def encodeBundle(self):

        encode_fmt = '<'  # pack as little endian
        encode_fmt += 'B'  # 1 byte for bundle type
        #
        encodedBundle = struct.pack(encode_fmt,
                                    self.bundleType)

        # if the bundle type has a message container, then output them
        if (self.bundleType == BundleType.MESSAGE_BUNDLE_TYPE2):

            encode_fmt = '<'  # pack as little endian
            encode_fmt += 'H'  # 2 bytes for bundle number

            # output the bundle number
            encodedBundle += struct.pack(encode_fmt,
                                         self.bundleType)

            # call the encode on the message container
            messageContainerBytes = self.messageContainer.encodeMessageContainer()  # nothing sent here because works on self?
            if (messageContainerBytes == None):
                logging.error("Bundle encoding abandoned due to error during message container encoding")
                return None
            else:
                encode_fmt = '<'  # pack as little endian
                encode_fmt += '{}'.format(len(messageContainerBytes)) + 's'  # lendth of encoded messageContainerBytes
                encodedBundle = struct.pack(encode_fmt, messageContainerBytes)

        else:
            logging.error("Unimplemented bundle encoding for bundle type: " + hex(self.bundleType))
            return None

        return encodedBundle

    # This method extracts a Bundle object from raw bytes
    def decodeBundle(self, bundleByteArray):

        self.bundleByteArray = bundleByteArray
        decode_fmt = '<'  # unpack as little endian
        decode_fmt += 'B'  # 1 byte  - bundle Type
        #
        num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
        s = struct.unpack(decode_fmt, self.bundleByteArray[0:num_bytes_read])  # actually unpacks the messageByteList
        self.bundleByteArray = self.bundleByteArray[
                               num_bytes_read:]  # remove the bytes we read from the message
        #
        self.bundleType = s[0]  # store bundle type

        if (self.bundleType == BundleType.MESSAGE_BUNDLE_TYPE2):
            #
            decode_fmt = '<'  # unpack as little endian
            decode_fmt += 'H'  # 2 byte  - bundle number
            #
            num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
            s = struct.unpack(decode_fmt,
                              self.bundleByteArray[0:num_bytes_read])  # actually unpacks the messageByteList
            self.bundleByteArray = self.bundleByteArray[
                                   num_bytes_read:]  # remove the bytes we read from the message
            #
            self.bundleNumber = s[0]  # store bundle type

            # create message container and decode it
            self.messageContainer = MessageContainer()
            self.bundleByteArray = self.messageContainer.decodeMessageContainer(self.bundleByteArray)
            if (self.bundleByteArray == None):
                logging.error("Abandoning attempt to decode bundle due to error.")
        else:
            logging.error("Unimplemented bundle decoding for bundle type: " + self.bundleType)


#
##########################################################################################################
#             
#  A Bundle Component Class that is used to hold Messages
class MessageContainer():
    def __init__(self):
        self.messageList = []

        # This method builds a Message Container Bundle Component object  from known values

    # messageList  - list of messages contained in the Container
    def buildMessageContainer(self, messageList):
        self.messageList = messageList;

        # This method extracts a Bundle object from raw bytes

    def decodeMessageContainer(self, messagesByteArray):
        # iterate over the messages
        #
        self.messagesByteArray = messagesByteArray
        while (len(self.messagesByteArray) > 0):
            #
            decode_fmt = '<'  # unpack as little endian
            decode_fmt += 'H'  # read two bytes assuming it might be long message
            #
            num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
            s = struct.unpack(decode_fmt,
                              self.messagesByteArray[0:num_bytes_read])  # actually unpacks the messageByteList
            #
            testMessageFormat = s[0]

            if testMessageFormat > 0xFF00:  # Then we have a long message (two byte message format)
                #
                decode_fmt = '<'  # unpack as little endian
                decode_fmt += 'H'  # messageFormat for long message
                decode_fmt += 'H'  # messageLength for long message
                #
                num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
                s = struct.unpack(hdr_fmt1, self.messagesByteArray[0:num_bytes_read])  #
                self.messagesByteArray = self.messagesByteArray[
                                         num_bytes_read:]  # remove the bytes we read from the message
                #
                messageFormat = s[0]
                messageLength = s[1]

                if (messageFormat == 0xFFFB):
                    #
                    # decode the actual message itself
                    # grab the sub list that contains the message's bytes
                    decodedMessage = self.decodeMessage(self.messagesByteArray[0:messageLength])
                    if (decodedMessage == None):
                        logging.error("Bundle decoding halted due to message decoding error")
                        return False;
                    self.messageList.append(decodedMessage)
                    self.messagesByteArray[messageLength:]
                    #
                elif (messageFormat == 0xFFFC):  # decode long system message
                    #
                    # decode the actual message itself
                    # grab the sub list that contains the message's bytes
                    decodedMessage = self.decodeMessage(self.messagesByteArray[0:messageLength])
                    if (decodedMessage == None):
                        logging.error("Bundle decoding halted due to message decoding error")
                        return False;
                    self.messageList.append(decodedMessage)
                    self.messagesByteArray[messageLength:]
                    #
                elif (messageFormat == 0xFFFA):  # decode continued message
                    #
                    # Eventually this will need to be able to add message to last message to form large message
                    #
                    # decode the actual message itself
                    # grab the sub list that contains the message's bytes
                    decodedMessage = self.decodeMessage(self.messagesByteArray[0:messageLength])
                    if (decodedMessage == None):
                        logging.error("Bundle decoding halted due to message decoding error")
                        return False;
                    self.messageList.append(decodedMessage)
                    self.messagesByteArray[messageLength:]
                    #
                else:
                    logging.error("Unknown message format encountered while decoding bundle type " +
                                  hex(self.bundleType) + " with the message format 0x" + hex(messageFormat))
                    return None
            #
            else:  # we have a short message
                #
                decode_fmt = '<'  # unpack as little endian
                decode_fmt += 'B'  # messageFormat for short message
                decode_fmt += 'B'  # messageLength for short message
                decode_fmt += 'H'  # messageLength for short message
                #

                num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
                s = struct.unpack(decode_fmt, self.messagesByteArray[0:num_bytes_read])  #
                self.messagesByteArray = self.messagesByteArray[
                                         num_bytes_read:]  # remove the bytes we read from the message
                #
                messageFormat = s[0]
                messageLength = s[1]
                messageNumber = s[2]  # need to pass back up to session for sure.
                #
                if (messageFormat == 0xFB):  # decode standard message
                    #
                    decodedMessage = self.decodeMessage(self.messagesByteArray[0:messageLength])
                    self.messagesByteArray = self.messagesByteArray[messageLength:]
                    #
                    if (decodedMessage == None):
                        logging.error("Bundle decoding halted due to message decoding error")
                        return None;
                    self.messageList.append(decodedMessage)  # still appends bytearry messages to the container

                elif (
                    messageFormat == 0xFC):  # decode system message - might need to do something different. Same as 0xFB now
                    #
                    decodedMessage = self.decodeMessage(self.messagesByteArray[0:messageLength])
                    self.messagesByteArray = self.messagesByteArray[messageLength:]
                    #
                    if (decodedMessage == None):
                        logging.error("Bundle decoding halted due to message decoding error")
                        return None;
                    self.messageList.append(decodedMessage)  # still appends bytearry messages to the container

                else:
                    logging.error("Unknown message format encountered while decoding bundle type " +
                                  hex(self.bundleType) + " with the message format 0x" + hex(messageFormat))
                    return None

                    # we've run out of bytes to process
        return self.messagesByteArray

    # This method accepts the raw byte list for a message and uses the op code to instantiate the correct
    # Message object and then calls the decodeMessage API on that object, returning the Message object
    # initialized with information from the messageByteArray.
    def decodeMessage(self, messageByteArray):
        # Read the op code without removing it from the message since the decodeMessage API expects it to be
        # in the byte list
        #
        decode_fmt = '<H'  # unpack as little endian / two byte short
        num_bytes_read = struct.calcsize(decode_fmt)  # calculate how many bytes will be read
        s = struct.unpack(decode_fmt, messageByteArray[0:num_bytes_read])  # actually unpacks the messageByteArray
        messageOpCode = s[0]
        if (messageOpCode == 0x0000):
            message = eqoa_messages.GameVersionMessage()
        elif (messageOpCode == 0x0904):
            message = eqoa_messages.LoginMessage()
        elif (messageOpCode == 0x07B3):
            message = eqoa_messages.ServerListingMessage()
        else:
            logging.error("Unable to decode Message data due to unknown Op Code: " + hex(messageOpCode))
            return None
        message.decodeMessage(messageByteArray)
        return message

    # This method generates raw bytes from a Message Container Bundle Component object
    def encodeMessageContainer(self):

        encodedMessageContainer = []

        # iterate over the message list
        for message in self.messageList:

            # standard message (0xFB)
            if (message.messageType == 0xFB):

                # write the single byte message type
                encodedMessageContainer.append(0xFB)

                # encode the message
                messageByteList = message.encodeMessage()
                if (messageByteList == None):
                    logging.error("MessageContainer abandoned due to error encoding message")
                    return None

                # encode the message length
                encodedMessageContainer.append(len(messageByteList))

                # output a message number (fake it for now)
                messageNumber = 0
                encodedMessageContainer.append(messageNumber & 0xFF)
                encodedMessageContainer.append((messageNumber >> 8) & 0xFF)

                # add the encoded message's bytes
                encodedMessageContainer.extend(messageByteList)

            elif (message.messageType == 0xFFFC):

                # write the single byte message type
                encodedMessageContainer.append(message.messageType & 0xFF)
                encodedMessageContainer.append((message.messageType >> 8) & 0xFF)

                # encode the message
                messageByteList = message.encodeMessage()
                if (messageByteList == None):
                    logging.error("MessageContainer abandoned due to error encoding message")
                    return None

                # output the two byte message length
                encodedMessageContainer.append(len(messageByteList) & 0xFF)
                encodedMessageContainer.append((len(messageByteList) >> 8) & 0xFF)

                # add the encoded message's bytes
                encodedMessageContainer.extend(messageByteList)

            else:
                logging.error("Unimplemented message encoding for message type " + message.messageType)
                return None

        return encodedMessageContainer


########################################################
#                  
#  A Bundle Component Class that is used for a Communication Reports
class ComReport():
    def __init__(self):
        self.thisBundleBeingSent = -1
        self.lastBundleReceived = -1
        self.lastMessageReceived = -1

    ###############################################
    #
    # This method builds a Communication Report Bundle Component object
    def buildComReport(self, thisBundleBeingSent, lastBundleReceived, lastMessageReceived):
        self.thisBundleBeingSent = thisBundleBeingSent
        self.lastBundleReceived = lastBundleReceived
        self.lastMessageReceived = lastMessageReceived

        ###############################################

    #
    # This method extracts a Communication Report Bundle Component object from rawbytes
    def decodeComReport(self, CommReportByteArray):
        #
        decode_fmt = '<'  # unpack as little endian
        decode_fmt += 'H'  # two bytes - This bundle number being sent
        decode_fmt += 'H'  # two bytes - Last bundle number received
        decode_fmt += 'H'  # two bytes - Last message received
        #
        s = struct.unpack(decode_fmt, CommReportByteArray)  #
        self.thisBundleBeingSent = s[0]
        self.lastBundleReceived = s[1]
        self.lastMessageReceived = s[2]

        ###############################################

    #
    # This method generates raw bytes from a Communication Report Bundle Component object
    def encodeComReport(self):
        #
        encode_fmt = '<'  # unpack as little endian
        encode_fmt += 'H'  # two bytes - This bundle number being sent
        encode_fmt += 'H'  # two bytes - Last bundle number received
        encode_fmt += 'H'  # two bytes - Last message received
        #
        encodedComReport = struct.pack(encode_fmt,
                                       self.thisBundleBeingSent,
                                       self.lastBundleReceived,
                                       self.lastMessageReceived)  #
        #
        return encodedComReport;

    #  A Bundle Component Class that is used for a Acknowleding Sessions


class SessionACK():
    def __init__(self):
        self.sessionACKd = 0

        ###############################################

    #
    # This method encodes a Communication Report Bundle Component object
    def buildSessionACK(self, sessionID):
        self.sessionID = sessionID
        pass

        ###############################################

    #
    # This method extracts a Session ACK Bundle Component object from raw bytes
    def decodeSessionACK(self, payload):
        #
        # extract these values from the payload
        #
        # self.sessionACKd = 0
        pass
        ###############################################

    #
    # This method creates raw bytes from a Session ACK Bundle Component object
    def encodeSessionACK(self, a_SessionACK):
        #
        pass

    #

###############################################################################
#
