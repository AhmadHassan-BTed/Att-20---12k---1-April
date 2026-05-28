// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Chat
{
    public static class ShoutChat
    {

        public static float Radius = 500.0f;

        /*TODO: This method queries for nearby objects to player, then iterates over list, looking if any are players and distributing the message
         Allow color to be passed through?*/
        //This method is treated as global shat for now
        public static void ProcessShout(Session session, string sharedMessage)
        {

            sharedMessage = $"{session.MyCharacter.CharName} shouts: " + sharedMessage;

            /*
            List<Entity> entityList = MapManager.QueryNearbyObjects(MySession.Character, Radius);
            */

            List<Character> entityList = PlayerManager.QueryForAllPlayers();

            //Add Colored message Opcode to list
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.ColoredChat);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.WriteString(Encoding.Unicode, sharedMessage);

            //This needs to be dynamic eventually, allow the color to inject a color for the text? Or maybe needs an overload for this to be for specific players to receive
            writer.Write(new byte[] { 0x3F, 0xFF, 0x3F, 0x00});

            message.Size = writer.Position;
            foreach (Character e in entityList)
            {
                //Good to leave as is for now... will just adapt over if shout is changed to more of a local chat instead of global
                e.characterSession.sessionQueue.Add(message);
            }
        }

        public static void ProcessShout(Session MySession, PacketMessage ClientPacket)
        {
            //First byte seems to always be 2? can skip it for now
            int messageLength = BinaryPrimitives.ReadInt32LittleEndian(ClientPacket.Data.Span[1..]);
            string message = Encoding.Unicode.GetString(ClientPacket.Data.Span[5..(5 + messageLength * 2)]);

            //Maybe need some checks here?
            ProcessShout(MySession, message);

        }
    }
}
