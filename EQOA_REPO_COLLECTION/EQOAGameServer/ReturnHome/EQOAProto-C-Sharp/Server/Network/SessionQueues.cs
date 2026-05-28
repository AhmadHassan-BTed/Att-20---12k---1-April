using ReturnHome.Utilities;
using System;
using System.Collections.Concurrent;

namespace ReturnHome.Server.Network
{
    public class SessionQueue
    {
        private Message _segmentMessage = null;
        private int _segmentPosition = 0;
        private ConcurrentQueue<Message> _outGoingReliableMessageQueue = new();
        private ConcurrentQueue<Message> _outGoingUnreliableMessageQueue = new();
        private ConcurrentDictionary<ushort, Message> _resendMessageQueue = new();
		
		private Session _session;

		public SessionQueue(Session session)
		{
			_session = session;
		}
		
        public void Add(Message message)
        {
            //If it is a reliable message type
            if (message.Messagetype == MessageType.ReliableMessage || message.Messagetype == MessageType.SegmentReliableMessage || message.Messagetype == MessageType.PingMessage)
                _outGoingReliableMessageQueue.Enqueue(message);

            //Unreliable message types
            else
                _outGoingUnreliableMessageQueue.Enqueue(message);
        }

        public bool CheckQueue()
        {
            //If no message exist, just return false
            if (_outGoingReliableMessageQueue.IsEmpty && _outGoingUnreliableMessageQueue.IsEmpty && _resendMessageQueue.IsEmpty && _segmentMessage == null)
                return false;

            //If either of these have items in the queue, return true
            if (!_outGoingReliableMessageQueue.IsEmpty || !_outGoingUnreliableMessageQueue.IsEmpty || _segmentMessage != null)
                return true;

            //If resend queue is not empty, check resend timer.
            if (!_resendMessageQueue.IsEmpty)
            {
                //Should we track how many times a message doesn't end up getting ack'd? Eventually disconnect client?
                foreach (var item in _resendMessageQueue)
                {
                    long stuff = DateTimeOffset.Now.ToUnixTimeMilliseconds() - item.Value.Time;
                    //If message needs to be resent, just add it back to the reliable queue
                    if (stuff > 2000)
                        return true;
                }
            }

            return false;
        }

        public void WriteMessages(ref BufferWriter writer, SegmentBodyFlags segmentBodyFlags)
        {
            //Resend queue is not needed here since we loop it back into the reliable queue on the check
            while (writer.Position + 4 < RdpCommOut.maxSize)
            {

                //Check for Segment messages needing to be sent sequentially
                if(_segmentMessage != null)
                {
                    //Write next message as a segment
                    if (_segmentMessage.Size - _segmentPosition > 0x0484 && writer.Position + 1166 < RdpCommOut.maxSize)
                    {
                        //Make sure to increment Index here
                        Message tempMessage = new(_segmentMessage.Messagetype, _segmentMessage.message.Slice(_segmentPosition, 0x0484), _session.rdpCommIn.connectionData.lastSentMessageSequence++);

                        writer.Write((byte)tempMessage.Messagetype);
                        writer.WriteSize(tempMessage.Size);
                        writer.Write(tempMessage.Sequence);
                        writer.Write(tempMessage.Span[0..tempMessage.Size]);
                        _segmentPosition += tempMessage.Size;

                        //Reset Timestamp, this works well in our favour of reliables needing to be resent here
                        tempMessage.updateTime();

                        //Place segment into resend queue
                        _resendMessageQueue.TryAdd(tempMessage.Sequence, tempMessage);
                        segmentBodyFlags |= SegmentBodyFlags.rdpMessage;
                        break;
                    }

                    //Final message of segment, use FB type
                    else if (_segmentMessage.Size - _segmentPosition < 0x0484 && writer.Position + _segmentMessage.Size - _segmentPosition < RdpCommOut.maxSize)
                    {
                        //Make sure to increment Index here
                        Message tempMessage = new(MessageType.ReliableMessage, _segmentMessage.message.Slice(_segmentPosition, _segmentMessage.Size - _segmentPosition), _session.rdpCommIn.connectionData.lastSentMessageSequence++);

                        writer.Write((byte)tempMessage.Messagetype);
                        writer.WriteSize(tempMessage.Size);
                        writer.Write(tempMessage.Sequence);
                        writer.Write(tempMessage.Span[0..tempMessage.Size]);

                        //Reset Timestamp, this works well in our favour of reliables needing to be resent here
                        tempMessage.updateTime();

                        //Place segment into resend queue
                        _resendMessageQueue.TryAdd(tempMessage.Sequence, tempMessage);
                        segmentBodyFlags|= SegmentBodyFlags.rdpMessage;

                        //Release original message
                        Message.Return(_segmentMessage);
                        _segmentMessage = null;
                        _segmentPosition = 0;
                        continue;
                    }

                    else
                        //break?
                        break;
                }

                //Check for Resend
                //Should we track how many times a message doesn't end up getting ack'd? Eventually disconnect client?
                long time = DateTimeOffset.Now.ToUnixTimeMilliseconds();
                foreach (var item in _resendMessageQueue)
                {
                    //If message needs to be resent, just add it back to the reliable queue
                    if ((time - item.Value.Time) > 2000)
                    {
                        if(writer.Position + item.Value.Size + 4 < RdpCommOut.maxSize)
                        {
                            writer.Write((byte)item.Value.Messagetype);
                            writer.WriteSize(item.Value.Size);
                            writer.Write(item.Value.Sequence);
                            writer.Write(item.Value.Span[0..item.Value.Size]);
                            item.Value.updateTime();
                        }
                    }
                }

                //Process reliable messages
                if (_outGoingReliableMessageQueue.TryPeek(out Message temp2))
                {
                    if ((writer.Position + 4 + temp2.Size + temp2.HeaderSize()) < RdpCommOut.maxSize || temp2.Messagetype == MessageType.SegmentReliableMessage)
                    {
                        if (_outGoingReliableMessageQueue.TryDequeue(out Message reliableMessage))
                        {
                            if (reliableMessage.Messagetype == MessageType.ReliableMessage || reliableMessage.Messagetype == MessageType.PingMessage || (reliableMessage.Messagetype == MessageType.SegmentReliableMessage && reliableMessage.Size < 0x0484))
                            {
                                reliableMessage.AddSequence(_session.rdpCommIn.connectionData.lastSentMessageSequence++);
                                if(reliableMessage.Messagetype == MessageType.SegmentReliableMessage)
                                    writer.Write((byte)MessageType.ReliableMessage);
                                else
                                    writer.Write((byte)reliableMessage.Messagetype);
                                writer.WriteSize(reliableMessage.Size);
                                writer.Write(reliableMessage.Sequence);
                                writer.Write(reliableMessage.Span[0..reliableMessage.Size]);

                                //Reset Timestamp, this works well in our favour of reliables needing to be resent here
                                reliableMessage.updateTime();

                                //Place into resend queue
                                _resendMessageQueue.TryAdd(reliableMessage.Sequence, reliableMessage);
                                segmentBodyFlags |= SegmentBodyFlags.rdpMessage;

                                //continue processing
                                continue;
                            }

                            //Should be a segment message, process a little differently
                            else
                            {
                                //Make sure to increment Index here
                                Message tempMessage = new(reliableMessage.Messagetype, reliableMessage.message.Slice(_segmentPosition, 0x0484), _session.rdpCommIn.connectionData.lastSentMessageSequence++);

                                writer.Write(tempMessage.Messagetype);
                                writer.WriteSize(tempMessage.Size);
                                writer.Write(tempMessage.Sequence);
                                writer.Write(tempMessage.Span[0..tempMessage.Size]);
                                _segmentPosition += tempMessage.Size;

                                //Reset Timestamp, this works well in our favour of reliables needing to be resent here
                                tempMessage.updateTime();

                                //Place segment into resend queue
                                _resendMessageQueue.TryAdd(tempMessage.Sequence, tempMessage);
                                _segmentMessage = reliableMessage;
                                segmentBodyFlags |= SegmentBodyFlags.rdpMessage;
                                break;
                            }
                        }
                    }
                }

                //process unreliable messages
                if (_outGoingUnreliableMessageQueue.TryPeek(out Message temp))
                {
                    if (temp.Messagetype == MessageType.UnreliableMessage)
                    {
                        if ((writer.Position + 4 + temp.Size + temp.HeaderSize()) < RdpCommOut.maxSize)
                        {
                            if (_outGoingUnreliableMessageQueue.TryDequeue(out Message message))
                            {
                                //Place message into outgoing message
                                writer.Write((byte)message.Messagetype);
                                writer.WriteSize(message.Size);
                                writer.Write(message.Span[0..temp.Size]);
                                continue;
                            }
                        }
                    }

                    else
                    {
                        if ((writer.Position + 4 + temp.Size + temp.HeaderSize()) < RdpCommOut.maxSize)
                        {
                            if (_outGoingUnreliableMessageQueue.TryDequeue(out Message message))
                            {
                                //Place message into outgoing message
                                writer.Write((byte)message.Messagetype);
                                writer.WriteSize(message.Size);
                                writer.Write(message.Sequence);
                                writer.Write(message.XORByte);
                                Compression.runLengthEncode(ref writer, message.Span);
                                continue;
                            }
                        }

                    }
                }

                //Shouldn't hit this unless there is no messages at all
                break;
            }
        }

        //This method will check against 2 connection variables, if the hardack is less then the soft ack
        //It will cycle over the dictionary and try to remove the resend messages, and increment the counter of the hard ack in the process.
        public void RemoveReliables()
        {
            while(_session.rdpCommIn.connectionData.clientLastReceivedMessage > _session.rdpCommIn.connectionData.clientLastReceivedMessageFinal)
            {
                if(_session.sessionQueue._resendMessageQueue.TryRemove(++_session.rdpCommIn.connectionData.clientLastReceivedMessageFinal, out Message message))
                    //TODO: Successfully removed message, should these get placed into a backup dictionary?
                    //Not 100% on proof yet, but pretty sure client can backstep and request a message #
                    continue;

                else
                {
                    //Back step message ack
                    _session.rdpCommIn.connectionData.clientLastReceivedMessageFinal--;

                    //Log this, would mean the message wasn't in our resend queue
                    Logger.Err($"Message #{_session.rdpCommIn.connectionData.clientLastReceivedMessageFinal}");
                    break;
                    //Should session get dropped at this point? Something went wrong here.
                }
            }
        }
    }
}
