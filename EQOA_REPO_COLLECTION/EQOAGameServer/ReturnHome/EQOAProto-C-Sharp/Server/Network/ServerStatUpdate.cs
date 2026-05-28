using System;
using System.Collections.Generic;
using System.Linq;
using ReturnHome.Server.EntityObject;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class ServerStatUpdate
    {
        //Holds receiving clients session info
        private Session _session;

        //Stores our base data to xor against
        private Memory<byte> _baseXOR = new Memory<byte>(new byte[0xEC]);

        //This is when the client ack's a specific message, we then set this to that ack'd message # to help generate our xor
        private ushort _baseMessageCounter = 0;

        //Simple message counter
        private ushort _messageCounter = 1;

        //May not be needed
        private byte _objectChannel;

        //Place to hold all of our XOR result's till client acks. We then xor that against the baseXOR to get new base object update, clear list once a message is ack'd by client
        private Dictionary<ushort, Memory<byte>> _currentXORResults = new Dictionary<ushort, Memory<byte>>();

        //Holds character to share object updates for
        public Entity entity;


        public ServerStatUpdate(Session session, byte objectChannel)
        {
            _session = session;
            _objectChannel = objectChannel;
        }

        public void GenerateUpdate()
        {
            //See if character and current message has changed
            if (!_baseXOR.Span.SequenceEqual(_session.MyCharacter.StatUpdate.Span))
            {
                Memory<byte> temp = new Memory<byte>(new byte[0xEC]);
                CoordinateConversions.Xor_data(temp, _session.MyCharacter.StatUpdate, _baseXOR, 0xEC);
                _currentXORResults.Add(_messageCounter, temp);
                _session.sessionQueue.Add(new Message((MessageType)_objectChannel, _messageCounter, _baseMessageCounter == 0 ? (byte)0 : (byte)(_messageCounter - _baseMessageCounter), temp));
                _messageCounter++;
            }
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
            _baseMessageCounter = msgCounter;
        }
    }
}
