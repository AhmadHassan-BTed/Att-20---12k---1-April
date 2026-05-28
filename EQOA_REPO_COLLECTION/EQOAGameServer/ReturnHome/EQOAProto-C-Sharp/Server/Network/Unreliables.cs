using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using System.Buffers.Binary;

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
        public static void ProcessUnreliables(Session Mysession, Message message)
        {
            if (message.Messagetype == MessageType.ClientUpdate)
                ProcessUnreliableClientUpdate(Mysession, message);
        }

        //Uncompress and process update
        private static void ProcessUnreliableClientUpdate(Session Mysession, Message message)
        {
            //This needs to happen via an opcode... I forget which but last unknown logging in
            Mysession.characterInWorld = true;

            //If xorbyte is 0, we need to take this as the new base message, if it is not 0, need to xor base to update
            if (message.XORByte != 0)
            {
                if (message.XORByte >= 0x20)
                {
                    Logger.Err($"{Mysession.AccountID}: Received an XOR byte > 32, dropping as an error");
                    return;
                }

                if (Mysession.rdpCommIn.connectionData.client.GetBaseClientArray((ushort)(message.Sequence - message.XORByte), out Memory<byte> temp))
                    CoordinateConversions.Xor_data(message.message, temp, message.Size);
                Mysession.rdpCommIn.connectionData.client.seqnum_remove_thru((ushort)(message.Sequence - message.XORByte));
            }

            //Means XOR should be 0?
            else
                Mysession.rdpCommIn.connectionData.client.seqnum_remove_thru((ushort)(message.Sequence - 0x20));

            Mysession.rdpCommIn.connectionData.client.SeqNum = message.Sequence;

            //Tells us we need to tell client we ack this message
            Mysession.segmentBodyFlags |= SegmentBodyFlags.clientUpdateAck;

            BufferReader reader = new(message.message.Span);
            Mysession.MyCharacter.World = (World)reader.Read<byte>();

            float x = CoordinateConversions.ConvertXZToFloat(reader.ReadUint24());
            float y = CoordinateConversions.ConvertYToFloat(reader.ReadUint24());
            float z = CoordinateConversions.ConvertXZToFloat(reader.ReadUint24());

            //TODO: Figure out how these value's translate into the values for client updates to show proper movement distribution
            ushort Velx = (ushort)(reader.Read<ushort>() ^ 0x80);
            ushort Vely = reader.Read<ushort>();
            ushort Velz = (ushort)(reader.Read<ushort>() ^ 0x80);
            
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

            //Add Base array for client update object, then update character object
            Mysession.rdpCommIn.connectionData.client.AddBaseClientArray(message.message, message.Sequence);
            Mysession.MyCharacter.UpdatePosition(x, y, z);
            Mysession.MyCharacter.Animation = Animation;
            Mysession.MyCharacter.UpdateFacing(Facing, Turning);
            Mysession.MyCharacter.UpdateVelocity(Velx, Vely, Velz);
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
                //This is just a shim for the player intro.
                if (Mysession.MyCharacter.GetPlayerFlags(Mysession, "NewPlayerIntro") == "0")
                {
                    Mysession.MyCharacter.MyDialogue.npcName = "NewPlayerIntro";
                    EventManager.GetNPCDialogue(GameOpcode.DialogueBox, Mysession);
                }
            }

            else
                MapManager.UpdatePosition(Mysession.MyCharacter);
        }
    }
}
