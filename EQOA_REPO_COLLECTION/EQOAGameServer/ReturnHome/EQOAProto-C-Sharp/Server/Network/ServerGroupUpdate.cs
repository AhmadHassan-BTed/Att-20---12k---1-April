using System;
using System.Collections.Generic;
using ReturnHome.Server.EntityObject.Grouping;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class ServerGroupUpdate
    {
        //Holds receiving clients session info
        private Session _session;

        //Stores our base data to xor against
        private Memory<byte> _baseXOR = new Memory<byte>(new byte[0x27]);

        //This is when the client ack's a specific message, we then set this to that ack'd message # to help generate our xor
        private ushort _baseMessageCounter = 0;

        //Simple message counter
        private ushort _messageCounter = 1;

        //May not be needed
        private byte _objectChannel;

        //Place to hold all of our XOR result's till client acks. We then xor that against the baseXOR to get new base object update, clear list once a message is ack'd by client
        private Dictionary<ushort, Memory<byte>> _currentXORResults = new Dictionary<ushort, Memory<byte>>();

        //Holds Group to share object updates for
        public Group group;

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

                        group = null;

                        DisableChannel();
                        return;
                    }

                    _isActive = true;
                }
            }
        }

        public ServerGroupUpdate(Session session, byte objectChannel)
        {
            _session = session;
            _objectChannel = objectChannel;
        }

        public void AddObject(Group g)
        {
            group = g;
            IsActive = true;
        }

        public void GenerateUpdate()
        {
            if (IsActive)
            {
                //If group becomes null, disable channel?
                if (group == null)
                {
                    IsActive = false;
                    return;
                }

                //See if character and current message has changed
                if (!_baseXOR.Span.SequenceEqual(_session.MyCharacter.GroupUpdate.Span))
                {
                    Memory<byte> temp = new Memory<byte>(new byte[0x27]);
                    CoordinateConversions.Xor_data(temp, _session.MyCharacter.GroupUpdate, _baseXOR, 0x27);
                    _currentXORResults.Add(_messageCounter, temp);
                    _session.sessionQueue.Add(new Message((MessageType)_objectChannel, _messageCounter, _baseMessageCounter == 0 ? (byte)0 : (byte)(_messageCounter - _baseMessageCounter), temp));
                    _messageCounter++;
                }
            }
        }

        public void DisableChannel()
        {
            //0 out group channel information to disable it
            Span<byte> temp2 = _session.MyCharacter.GroupUpdate.Span;
            temp2.Fill(0);

            Memory<byte> temp = new Memory<byte>(new byte[0x27]);
            CoordinateConversions.Xor_data(temp, _session.MyCharacter.GroupUpdate, _baseXOR, 0x27);
            _currentXORResults.Add(_messageCounter, temp); Message message = Message.Create((MessageType)_objectChannel, 0);
            _session.sessionQueue.Add(new Message((MessageType)_objectChannel, _messageCounter, _baseMessageCounter == 0 ? (byte)0 : (byte)(_messageCounter - _baseMessageCounter), temp));
            _messageCounter++;
        }

        public void UpdateBaseXor(ushort msgCounter)
        {
            //Get the message client ack'd
            Memory<byte> tempMemory = _currentXORResults.GetValueOrDefault(msgCounter);

            if (tempMemory.Length == 0)
                return;

            //xor base against this message
            CoordinateConversions.Xor_data(_baseXOR, tempMemory, 0x27);

            //Clear Dictionary
            _currentXORResults.Clear();

            //Ensure this is new base
            _baseMessageCounter = msgCounter;
        }
    }
}
