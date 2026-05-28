// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Buffers.Binary;
using System.Text;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Chat
{
    public static class ChatMessage
    {

        public static void ProcessClientChat(Session MySession, PacketMessage ClientPacket)
        {
            int messageLength = BinaryPrimitives.ReadInt32LittleEndian(ClientPacket.Data.Span[0..]);
            string message = Encoding.Unicode.GetString(ClientPacket.Data.Span[4..(4 + messageLength * 2)]);

            //Admin command of sorts, possibly temporary
            if (message[0] == '!')
            {
                ProcessCommands(MySession, message);
            }

            else if( MySession.MyCharacter.chatMode == 0)
            {
                SayChat.ProcessSay(MySession, message);
            }

            else if (MySession.MyCharacter.chatMode == 1)
            {
                //Group?
            }

            else if (MySession.MyCharacter.chatMode == 2)
            {
                //Guild?
            }

            else if (MySession.MyCharacter.chatMode == 3)
            {
                ShoutChat.ProcessShout(MySession, message);
            }
        }

        public static void GenerateClientSpecificChat(Session session, string chatMessage)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.ClientMessage);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.WriteString(Encoding.Unicode, chatMessage);

            message.Size = writer.Position;

            //Send Message
            session.sessionQueue.Add(message);
        }

        /// <summary>
        /// This method processes client Commands
        /// </summary>
        public static void ProcessCommands(Session MySession, string message)
        {
            if (message[0..2] == "!c")
            {
                MySession.CoordinateUpdate();
            }

            //Add a check here to verify account has admin privileges?
            if (message[0..2] == "!t")
            {
                if (ObjectAdminChecks.ProcessChanges(MySession, message.Split(' ')))
                {
                    message = "Completed processing request";
                    GenerateClientSpecificChat(MySession, message);
                }

                else
                {
                    message = "Error Occured";
                    GenerateClientSpecificChat(MySession, message);
                }
            }

            if(message[0..2] == "!a")
            {
                MySession.MyCharacter.ArrangeItem(0, 1);
            }

            if (message[0..2] == "!d")
            {
                MySession.MyCharacter.DestroyItem(0, 1);
            }

            if (message[0..2] == "!oid")
            {
                MySession.TargetUpdate();
            }

            if (message[0..2] == "!o")
            {
                MySession.unkOpcode ^= true;
                if (MySession.unkOpcode)
                {
                    message = "Unknown opcode display is now on.";
                }

                else
                {
                    message = "Unknown opcode display is now off.";
                }

                GenerateClientSpecificChat(MySession, message);
            }

            //Should this require admin? I don' think thats important till live
            if (message[0..2] == "!s")
            {
                float speed;
                try
                {
                    speed = float.Parse(message.Substring(3, message.Length - 3));
                    ServerPlayerSpeed.PlayerSpeed(MySession, speed);
                }

                catch
                {
                    message = "Not a valid value for speed, please enter XX.X or XX. Suggest speed being less then 50";
                    GenerateClientSpecificChat(MySession, message);
                }
            }
        }

        public static void ProcessShoutChat(Session MySession, PacketMessage ClientPacket)
        {
            //Would Process shout chat, and query the quad tree for the range to distribute the chat too
        }

        public static void ProcessTells(Session MySession, PacketMessage ClientPacket)
        {
            //Would take client message designated for a specific player, sending them a message if they are online
        }

        public static void ProcessGuildChat(Session MySession, PacketMessage ClientPacket)
        {
            //Processes client chat bound for their guild
        }

        public static void ProcessGroupChat(Session MySession, PacketMessage ClientPacket)
        {
            //Processes client messages bound for group chat
        }

        public static void DistributeSpecificMessageAndColor(Session session, string chatMessage, byte[] color)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.ColoredChat);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);
            writer.WriteString(Encoding.Unicode, chatMessage);
            writer.Write(color);
            message.Size = writer.Position;

            //Send Message
            session.sessionQueue.Add(message);
        }
    }
}
