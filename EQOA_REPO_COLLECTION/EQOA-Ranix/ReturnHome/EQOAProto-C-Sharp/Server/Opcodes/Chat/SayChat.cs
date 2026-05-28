// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Text;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Chat
{
    public static class SayChat
    {

        public static float Radius = 200.0f;

        //TODO: Consider reworking player query to send the say message?
        ///This method queries for nearby objects to player, then iterates over list, looking if any are players and distributing the message
        public static void ProcessSay(Session MySession, string chatMessage)
        {
            chatMessage = $"{MySession.MyCharacter.CharName}: " + chatMessage;

            //Query for nearby objects
            List<Entity> entityList = MapManager.QueryObjects(MySession.MyCharacter, Radius);

            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.ClientMessage);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.WriteString(Encoding.Unicode, chatMessage);

            message.Size = writer.Position;
            //Loop over entity list, if any are players, distribute the message
            foreach (Entity e in entityList)
            {
                if (e.isPlayer)
                {
                    ((Character)e).characterSession.sessionQueue.Add(message);
                }
            }
        }
    }
}
