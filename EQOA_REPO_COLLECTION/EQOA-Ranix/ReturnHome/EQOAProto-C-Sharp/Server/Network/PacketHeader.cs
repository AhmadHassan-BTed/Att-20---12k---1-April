using System;
using System.Collections.Generic;
using System.Linq;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class PacketHeader
    {
        public ushort ClientEndPoint { get; set; }
        public ushort TargetEndPoint { get; set; }
        public uint HeaderData { get; set; }
        public PacketHeaderFlags headerFlags { get; set; }
        public PacketBundleFlags bundleFlags { get; set; }
		public bool NewInstance {get; private set;} = false;
        public bool CRCChecksum { get; private set; } = false;
        public bool RDPReport { get; private set; } = false;
		public bool ProcessMessage { get; private set; } = false;
        public bool CancelSession { get; private set; } = false;
        public bool SessionAck { get; private set; } = false;
        public ushort BundleSize { get; set; }
        public uint InstanceID { get; set; }
        public uint SessionID { get; set; }
        public uint SessionAckID { get; private set; }
        public ushort ClientBundleNumber { get; set; }
        public ushort ClientBundleAck { get; set; }
        public ushort ClientMessageAck { get; set; }

        public Dictionary<byte, ushort> ChannelAcks;

        public void Unpack(ref BufferReader reader, ReadOnlyMemory<byte> buffer)
        {
            ClientEndPoint = reader.Read<ushort>();
            TargetEndPoint = reader.Read<ushort>();

            //Check if it is a transfer packet
            if (!(TargetEndPoint == 0xFFFF))
            {
                //Not Transfer packet, Validate CRC Checksum for packet
                //Should probably do something if it fails here, so we don't waste cycles processing a broken packet
                int temp = reader.Position;
                reader.Position = reader.Length - 4;

                CRCChecksum = reader.Read<uint>() == CRC.calculateCRC(reader.Buffer.Slice(0, reader.Length - 4));

                //Put Position back to original
                reader.Position = temp;
            }

            else
                //Eventually do transfers here some how
                return;

            HeaderData = (uint)reader.Read7BitEncodedUInt64();
            BundleSize = (ushort)(HeaderData & 0x7FF);
            headerFlags = (PacketHeaderFlags)(HeaderData - BundleSize);

            //Verify packet has instance in header
            if (HasHeaderFlag(PacketHeaderFlags.HasInstance))
                InstanceID = reader.Read<uint>();

            if (HasHeaderFlag(PacketHeaderFlags.ResetConnection))
                //SessionID is duplicated with resetconnection header, indicates to drop session
                //What do we do if this... isn't the case?
                //Chalk it up to invalid packet and drop?
                if (InstanceID ==reader.Read<uint>())
                {
                    CancelSession = true;
                    return;
                }

            //if Client is "remote", means it is not "master" anymore and an additional pack value to read which ties into character instanceID
            if (HasHeaderFlag(PacketHeaderFlags.IsRemote))
                SessionID = (uint)reader.Read7BitEncodedInt64();

            else
                SessionID = 0;
				
				
			if (HasHeaderFlag(PacketHeaderFlags.NewInstance))
                NewInstance = true;

            //Else?????

            //Read Bundle Type, needs abit of a work around....
            bundleFlags = (PacketBundleFlags)reader.Read<byte>();

            if (HasBundleFlag(PacketBundleFlags.ProcessAck))
            {
                SessionAckID = reader.Read<uint>();
                SessionAck = true;
            }

            ClientBundleNumber = reader.Read<ushort>();

            if (HasBundleFlag(PacketBundleFlags.RDPReport))
            {
                ClientBundleAck = reader.Read<ushort>();
                ClientMessageAck = reader.Read<ushort>();
                RDPReport = true;
            }

            if (HasBundleFlag(PacketBundleFlags.UnknownSingleByte))
            {
                if(reader.Read<byte>() == 0xFF)
                    _ = reader.Read<byte>();
            }

            if (HasBundleFlag(PacketBundleFlags.ProcessUnreliableAcks))
            {
                byte chk;
                ushort msgNum;
                ChannelAcks = new();
                do
                {
                    chk = reader.Read<byte>();
                    if ((chk >= 0 && chk <= 0x17) || (chk >= 0x40 && chk <= 0x43))
                    {
                        msgNum = reader.Read<ushort>();
                        ChannelAcks.Add(chk, msgNum);
                    }

                    else if (chk == 0xF8)
                        break;

                    else
                        throw new Exception("An Error has occured processing this packet, no more reliable ack's and did not meet the 0xF8 terminator");

                } while (true);

            }

			if (HasBundleFlag(PacketBundleFlags.NewProcessMessages) || HasBundleFlag(PacketBundleFlags.ProcessMessages))
				ProcessMessage = true;
        }

        public bool HasHeaderFlag(PacketHeaderFlags HeaderFlags) { return (HeaderFlags & headerFlags) == HeaderFlags; }

        public bool HasBundleFlag(PacketBundleFlags BundleFlags) { return (BundleFlags & bundleFlags) == BundleFlags; }
    }
}
