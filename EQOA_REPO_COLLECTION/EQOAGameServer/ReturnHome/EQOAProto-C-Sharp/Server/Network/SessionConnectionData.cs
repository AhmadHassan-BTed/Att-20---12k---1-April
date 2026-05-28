using System;
using System.Collections.Generic;
using ReturnHome.Server.EntityObject;

namespace ReturnHome.Server.Network
{
    public class SessionConnectionData
    {
        private Session _session;
        private ushort _lastReceivedMessageSequence { get; set; } = 0;
        public ushort lastReceivedPacketSequence { get; set; }
        public ushort lastSentMessageSequence { get; set; }
        public ushort lastSentPacketSequence { get; set; }
        public ushort clientLastReceivedMessage { get; set; }
        public ushort clientLastReceivedMessageFinal { get; set; }
        //public ConcurrentStack<PacketMessage> ClientUpdateStack;

        //This property will trigger the rdpreport bool when it increments
        public ushort lastReceivedMessageSequence
        {
            get { return _lastReceivedMessageSequence; }
            set
            {
                if (_lastReceivedMessageSequence <= value)
                {
                    _lastReceivedMessageSequence = value;
                    _session.segmentBodyFlags |= SegmentBodyFlags.guaranted_ack;
                }
            }
        }
        public ClientObjectUpdate client { get; set; }

        //Create an array of all 24 object updates
        //Need a way to ensure no more then 24 objects can be allocated, array may be best?
        public Memory<ServerObjectUpdate> serverObjects;
        public ServerStatUpdate clientStatUpdate;
        public ServerGroupUpdate serverGroupUpdate;
        /*
         * Need to be able to do a full cycle reset for server objects.
         * Whenever a player moves/teleports to a new map/world, all of the c9 channels need to be reset
         * or it causes issues for the client as they remain in the old world or something
         * Should be as simple as a for/foreach loop in a reset method calling the ServerObjects Reset method
         * First theory is that this reset method should reside within the World Property for entity, 
         * checking if entity is a player first then calling this method to reset all channels if world changes
         */
        public SessionConnectionData(Session session)
        {
            _session = session;
            lastReceivedPacketSequence = 0;

            //Client last soft ack of message's received.
            clientLastReceivedMessage = 0;

            //Server hard ack of client messages received. Server will compare this against client,
            //and if client ack is higher, server will cycle through resend list and remove.
            clientLastReceivedMessageFinal = 0;

            lastSentMessageSequence = 1;
            lastSentPacketSequence = 1;

            //Create the object to track incoming client updates
            client = new();
            clientStatUpdate = new(_session, (byte)MessageType.StatUpdate);
            serverGroupUpdate = new(_session, (byte)MessageType.GroupUpdate);
            serverObjects = new Memory<ServerObjectUpdate>(new ServerObjectUpdate[0x17]);

            //TODO: Prioritize channels for group member's and pet's, 3 group member's and a pet max possible
            Span<ServerObjectUpdate> tempSpan = serverObjects.Span;

            for (int i = 0; i < tempSpan.Length; i++)
                tempSpan[i] = new(_session, (byte)i);
        }

        //Need to define a way to prioritize group members, and eventually pet's for c9 applications
        public void AddChannelObjects(List<Entity> charList)
        {
            Span<ServerObjectUpdate> temp = serverObjects.Span;
            charList.Remove(_session.MyCharacter);
            if (charList.Count == 0)
            {
                for (int i = 1; i < serverObjects.Length; i++)
                {
                    temp[i].IsActive = false;
                }
                return;
            }

            charList = charList.GetRange(0, charList.Count > 22 ? 22 : charList.Count);

            //Iterate over List from QuadTree against Channels
            for (int i = 1; i < serverObjects.Length; i++)
            {
                //Character is already in a channel
                if (charList.Contains(temp[i].entity) & temp[i].IsActive)
                {
                    charList.Remove(temp[i].entity);
                    continue;
                }

                else
                    //Calls the disable method for the channel
                    temp[i].IsActive = false;
            }

            if (charList.Count == 0)
                return;

            for (int i = 1; i < serverObjects.Length; i++)
            {
                if (!temp[i].IsActive)
                {
                    temp[i].AddObject(charList[0]);
                    charList.RemoveAt(0);
                    if (charList.Count == 0)
                        return;

                }
                continue;
            }
        }

        public void DisableChannels()
        {
            Span<ServerObjectUpdate> temp = serverObjects.Span;

            for (int i = 0; i < serverObjects.Length; i++)
            {
                temp[i].IsActive = false;
            }
        }
    }
}
