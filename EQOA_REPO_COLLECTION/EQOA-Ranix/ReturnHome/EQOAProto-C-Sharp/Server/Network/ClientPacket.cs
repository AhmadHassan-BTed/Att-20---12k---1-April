using System;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Network
{
    public class ClientPacket : Packet
    {
        public bool ProcessPacket(ReadOnlyMemory<byte> buffer)
        {
            BufferReader reader = new BufferReader(buffer.Span);

            //Track memory offset as packet is processed
            try
            {
                Header.Unpack(ref reader, buffer);

                //Need a way to identify an additional bundle from client and to process this

                //If crc checksum is false, crc failed...
                //Verify packet is not a transfer, transfers dont have crc
                //Just drop the packet
                if (!(Header.TargetEndPoint == 0xFFFF))
                {
                    //If packet indicates to cancel session, do it, and stop reading.
                    //One off issue... Server select -> Character select, if ip/endpoint is the same, client will bundle old session disconnect
                    //in same packet as the new connection, difficult to process this in current setup. Easy to ignore and let client resend
                    if (Header.CancelSession)
                        return true;

                    //If CRC fails and packet isn't canceling the session
                    if (!Header.CRCChecksum)
                        return false;

                    //If the buffer size is equal to bytes read + 4 (CRC)
                    //just return true as packets been fully broke down
                    if (reader.Length == reader.Position + 4)
                        //Packet should just be an ack or session cancel
                        return true;

                    //Read messages, if this fails... drop the packet
                    if (!ReadMessages(reader, buffer))
                        return false;
                }

                return true;
            }

            catch
            {
                //Log exception

                return false;
            }
        }

        private bool ReadMessages(BufferReader reader, ReadOnlyMemory<byte> buffer)
        {
            //If message type is present, break out messages
            if (Header.ProcessMessage)
            {
                //Subtract 4 from buffer length to account for removing CRC
                while (reader.Position < (reader.Length - 4))
                {
                    try
                    {
                        var message = new ClientPacketMessage();
                        if (!message.Unpack(ref reader, buffer))
                            return false;
                        if (message.Header.messageType == (byte)MessageType.ClientUpdate)
                            clientUpdate = message;
                        else
                            Messages.TryAdd(message.Header.MessageNumber, message);
                    }

                    catch (Exception)
                    {
                        // corrupt packet? Maybe figure out some additional logging here
                        return false;
                    }
                }
            }

            return true;
        }

        //Nothing needs to be released... for now.
        public void ReleaseBuffer()
        {

        }
    }
}
