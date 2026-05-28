using System;
using System.Collections.Generic;
using ReturnHome.Utilities;
using ReturnHome.Server.EntityObject;

namespace ReturnHome.Server.Network
{
    public class ServerObjectUpdate
    {
        //Holds receiving clients session info
        private Session _session;

        //Stores our base data to xor against
        private Memory<byte> _baseXOR = new Memory<byte>(new byte[0xC9]);

        //This is when the client ack's a specific message, we then set this to that ack'd message # to help generate our xor
        private ushort _baseMessageCounter = 0;
        public ushort BaseMessageCounter
        {
            get { return _baseMessageCounter; }
            set
            {
                if (value > _baseMessageCounter)
                    _baseMessageCounter = value;

                else
                    Console.WriteLine($"Error setting base message counter. Setting {value} Expected: {_baseMessageCounter}");
            }
        }

        //Simple message counter
        private ushort _messageCounter = 1;
        public ushort MessageCounter
        {
            get { return _messageCounter; }
            set
            {
                if (value > _messageCounter)
                    _messageCounter = value;

                else
                    Console.WriteLine($"Error setting message counter. Setting: {value} Expected: {_messageCounter}");
            }
        }
        //May not be needed
        private byte _objectChannel;

        //Place to hold all of our XOR result's till client acks. We then xor that against the baseXOR to get new base object update, clear list once a message is ack'd by client
        private Dictionary<ushort, Memory<byte>> _currentXORResults = new Dictionary<ushort, Memory<byte>>();

        //Holds character to share object updates for
        public Entity entity;

        //should be needed for transferring continents, probably can get away on just doing character changes? Could also be an easy way to flip character in and out of a channel on the client
        private bool _isActive = false;
        public bool IsActive
        {
            get { return _isActive; }
            set
            {
                if (value == _isActive)
                    return;

                else
                {
                    if (value == false)
                    {
                        _isActive = false;

                        entity = null;

                        DeactivateChannel();
                        return;
                    }

                    _isActive = true;
                }
            }
        }

        public ServerObjectUpdate(Session session, byte objectChannel)
        {
            _session = session;
            _objectChannel = objectChannel;
        }

        public void AddObject(Entity e)
        {
            entity = e;
            IsActive = true;
        }

        public void GenerateUpdate()
        {
            if (IsActive)
            {
                //If entity becomes null, disable channel?
                if (entity == null)
                {
                    IsActive = false;
                    return;
                }

                //See if character and current message has changed
                if (!CompareObjects(_baseXOR.Slice(1, 0xC8), entity.ObjectUpdate))
                {
                    Memory<byte> temp = new Memory<byte>(new byte[0xC9]);
                    temp.Span[0] = _isActive & (_baseXOR.Span[0] == 0) ? (byte)1 : (byte)0;
                    CoordinateConversions.Xor_data(temp.Slice(1, 0xC8), entity.ObjectUpdate, _baseXOR.Slice(1, 0xC8), 0xC8);
                    _currentXORResults.Add(MessageCounter, temp);
                    _session.sessionQueue.Add(new Message((MessageType)_objectChannel, MessageCounter, _baseMessageCounter == 0 ? (byte)0 : (byte)(MessageCounter - _baseMessageCounter), temp));
                    MessageCounter++;
                }
            }
        }

        private void DeactivateChannel()
        {
            //No need to verify if entity is null or not, disabling channel anyway
            Memory<byte> temp = new Memory<byte>(new byte[0xC9]);

            //Since we are deactivating the channel, all we need to do is modify the first byte
            temp.Span[0] = (_baseXOR.Span[0] == 1) ? (byte)1 : (byte)0;

            _currentXORResults.Add(MessageCounter, temp);
            _session.sessionQueue.Add(new Message((MessageType)_objectChannel, MessageCounter, _baseMessageCounter == 0 ? (byte)0 : (byte)(MessageCounter - _baseMessageCounter), temp));
            MessageCounter++;
        }

        public void UpdateBaseXor(ushort msgCounter)
        {
            //Get the message client ack'd
            Memory<byte> tempMemory = _currentXORResults.GetValueOrDefault(msgCounter);

            if (tempMemory.Length == 0)
                return;

            //xor base against this message
            CoordinateConversions.Xor_data(_baseXOR, tempMemory, 0xC9);

            //Clear Dictionary
            _currentXORResults.Clear();

            //Ensure this is new base
            BaseMessageCounter = msgCounter;
        }

        private static bool CompareObjects(Memory<byte> first, Memory<byte> second)
        {
            if (first.Length != second.Length)
                return false;

            Span<byte> firstTemp = first.Span;
            Span<byte> secondTemp = second.Span;

            for (int i = 0; i < first.Length; i++)
            {
                if (firstTemp[i] == secondTemp[i])
                    continue;
                else
                    return false;
            }
            return true;
        }
    }
}
