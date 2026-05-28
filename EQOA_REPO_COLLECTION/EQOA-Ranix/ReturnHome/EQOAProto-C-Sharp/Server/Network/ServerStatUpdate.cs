// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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


        public ServerStatUpdate(Session session, byte objectChannel)
        {
            _session = session;
            _objectChannel = objectChannel;
        }

        public void GenerateUpdate()
        {
            //See if character and current message has changed
            if (!CompareObjects(_baseXOR, _session.MyCharacter.StatUpdate))
            {
                Memory<byte> temp = new Memory<byte>(new byte[0xEC]);
                CoordinateConversions.Xor_data(temp, _session.MyCharacter.StatUpdate, _baseXOR, 0xEC);
                _currentXORResults.Add(MessageCounter, temp);
                _session.sessionQueue.Add(new Message((MessageType)_objectChannel, MessageCounter, _baseMessageCounter == 0 ? (byte)0 : (byte)(MessageCounter - _baseMessageCounter), temp));
                MessageCounter++;
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
