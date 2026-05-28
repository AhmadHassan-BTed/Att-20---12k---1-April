using System;
using AuthServer.Utility;
using AuthServer.Account;
using System.Net;
using System.Collections.Generic;
using System.Buffers;
using System.Net.Sockets;

namespace AuthServer.Server
{
    public class EQOAClientTracker
    {
        //Store connected client sessions here
        public List<EQOAClient> EQOAClientList = new List<EQOAClient>();

        //Add new connecting clients here
        public void AddEQOAClient(EQOAClient NewClient) { EQOAClientList.Add(NewClient); }

        //Remove Client from connected list
        public void RemoveEQOAClient(EQOAClient DisconnectedClient) { EQOAClientList.Remove(DisconnectedClient); }

        //Gets total connected clients
        public uint ConnectedEQOAClientCount() { return (uint)EQOAClientList.Count; }
    }

    public class EQOAClient : AccountManagement
    {
        private ReadOnlySequence<byte> _buffer;
        public ReadOnlyMemory<byte> ClientPacket;
        public Memory<byte> ResponsePacket;
        public int offset = 0;
        public int resOffset = 4;
        public IPEndPoint ClientIPEndPoint;
        //Message Data
        public uint MessageLength;
        public uint MessageType;

        //Username
        public string Username;

        //Encryoted Password
        public string EncPassword;

        //Socket
        public Socket Handler;

        public bool AccountVerified = false;

        //Client Information
        //Standard this for now... In the future would draw results from Database
        public uint AccountID = 1;
        public ushort Result = 0;
        public ushort AccountStatus = 1;
        public uint Subtime = 2592000;
        public uint Partime = 2592000;
        public uint Subfeatures = 4;
        public uint Gamefeatures = 3;

        public EQOAClient() { }

        public EQOAClient(IPEndPoint ThisIPEndPoint, Socket handler)
        {
            //Our socket
            Handler = handler;

            //This Clients IPAddress
            ClientIPEndPoint = ThisIPEndPoint;

            Console.WriteLine($"Created new client: {ClientIPEndPoint}");
        }

        public void ClearMessages()
        {
            ClientPacket = null;
            ResponsePacket = null;
            offset = 0;
            resOffset = 4;
        }

        public void CheckClientRequest(byte[] buffer)
        {
            ClientPacket = buffer;
            if (buffer.Length == 0)
                return;

            int MessageLength = BinaryPrimitiveWrapper.GetBEInt(this);
            uint MessageType = (uint)BinaryPrimitiveWrapper.GetBEInt(this);
            //Console.WriteLine($"Received Message with Length {MessageLength} and Message Type {MessageType}");

            switch (MessageType)
            {
                case AccountMessageTypes.LOGIN_REQUEST:
                    //Console.WriteLine("Processing Character Request...");
                    LoginRequest(this);
                    break;

                case AccountMessageTypes.SUBMIT_ACCT_CREATE:
                    //Console.WriteLine("Attempting to create an account...");
                    CreateAccount(this);
                    break;

                case AccountMessageTypes.CHANGE_PASSWORD:
                    //Console.WriteLine("Attempting to change client Password...");
                    ChangePassword(this);
                    break;

                case AccountMessageTypes.CONSUME_GAMECARD:
                case AccountMessageTypes.REQUEST_ACCT_KEY:
                case AccountMessageTypes.REQUEST_CANCEL_ACCT:
                case AccountMessageTypes.REQUEST_GAME_CARD:
                case AccountMessageTypes.REQUEST_SUBSCRIPTION:
                case AccountMessageTypes.REQUEST_UPDATE_ACCT:
                case AccountMessageTypes.FORGOT_PASSWORD:
                    //Let user know this is not supported
                    //Console.WriteLine("Received uncoded opcode");
                    this.MessageType = AccountMessageTypes.DISABLED_FEATURE;
                    new BadAttempt(this);
                    break;

                default:
                    break;
            }
        }
    }
}

