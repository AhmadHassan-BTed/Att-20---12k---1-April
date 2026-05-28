using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;

using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.EntityObject.Player;
using System.Runtime.CompilerServices;
using ReturnHome.Server.Managers;
using ReturnHome.Server.EntityObject;

namespace ReturnHome.Server.Network
{
    public static class ProcessUnreliable
    {
        public static void ProcessUnreliables(Session Mysession, PacketMessage message)
        {
            if (message.Header.messageType == UnreliableTypes.ClientActorUpdate)
            {
                ProcessUnreliableClientUpdate(Mysession, message);
            }
        }

        //Uncompress and process update
        private static void ProcessUnreliableClientUpdate(Session Mysession, PacketMessage message)
        {
            Mysession.characterInWorld = true;

            //If xorByte > 0, make sure it isn't an outdated update.
            //If message# - xorbyte < last ack'd, drop it
            if (message.Header.XorByte != 0)
            {
                //If this fails, client did not get ack so drop message and resend ack
                if (Mysession.rdpCommIn.connectionData.client.BaseXorMessage > (message.Header.MessageNumber - message.Header.XorByte))
                {
                    //ensure client got ack by resending it
                    Mysession.PacketBodyFlags.clientUpdateAck = true;
                    return;
                }
            }

            //Change Memory<byte to be initialized with 0x29 size in ClientObjectUpdate
            Memory<byte> MyPacket = MemoryMarshal.AsMemory(message.Data);

            //If xorbyte is 0, we need to take this as the new base message, if it is not 0, need to xor base to update
            if (message.Header.XorByte != 0)
                CoordinateConversions.Xor_data(MyPacket, Mysession.rdpCommIn.connectionData.client.GetBaseClientArray(), message.Header.Size);

            Mysession.rdpCommIn.connectionData.client.BaseXorMessage = message.Header.MessageNumber;
            BufferReader reader = new(MyPacket.Span);

            Mysession.MyCharacter.World = (World)reader.Read<byte>();

            float x = CoordinateConversions.ConvertXZToFloat(reader.ReadUint24());
            float y = CoordinateConversions.ConvertYToFloat(reader.ReadUint24());
            float z = CoordinateConversions.ConvertXZToFloat(reader.ReadUint24());

            float Velx = 15.3f * 2 * reader.Read<ushort>() / 0xffff - 15.3f;
            float Vely = 15.3f * 2 * reader.Read<ushort>() / 0xffff - 15.3f;
            float Velz = 15.3f * 2 * reader.Read<ushort>() / 0xffff - 15.3f;

            //Skip 6 bytes...
            reader.Position += 6;
            byte Facing = reader.Read<byte>();
            //offset++;

            byte Turning = 0;//ClientPacket[offset++];

            //Skip 12 bytes...
            reader.Position += 12;
            byte Animation = reader.Read<byte>();

            reader.Position++;

            uint Target = reader.Read<uint>();

            //Update Base array for client update object, then update character object
            Mysession.rdpCommIn.connectionData.client.UpdateBaseClientArray(MyPacket);
            Mysession.MyCharacter.UpdatePosition(x, y, z);
            Mysession.MyCharacter.Animation = Animation;
            Mysession.MyCharacter.UpdateFacing(Facing, Turning);
            Mysession.MyCharacter.UpdateVelocity(Velx, 0, Velz);
            //Mysession.MyCharacter.Target = Target;
            Mysession.objectUpdate = true;

            //Would likely need some checks here eventually? Shouldn't blindly trust client
            //First 4029 means we are ingame
            if (!Mysession.inGame)
            {
                PlayerManager.AddPlayer(Mysession.MyCharacter);
                EntityManager.AddEntity(Mysession.MyCharacter);
                MapManager.Add(Mysession.MyCharacter);
                
                
                Mysession.inGame = true;
            }

            else
                MapManager.UpdatePosition(Mysession.MyCharacter);

            //Tells us we need to tell client we ack this message
            Mysession.PacketBodyFlags.clientUpdateAck = true;
        }
    }
}
