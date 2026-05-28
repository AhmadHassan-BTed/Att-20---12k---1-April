using AuthServer.Server;
using AuthServer.Utility;
using System;
using System.Text;

namespace AuthServer.Account
{
    public class BaseBuildMessage
    {
        public void PreparePacket(EQOAClient Client)
        {

        }

        public void SendMessage(EQOAClient ThisClient)
        {
            //Add Packet Length to beginning of List
            BitConverter.GetBytes(ByteSwaps.SwapBytes((uint)ThisClient.resOffset)).CopyTo(ThisClient.ResponsePacket[0..4]);

            AsynchronousSocketListener.ProcessSend(ThisClient);

            ThisClient.ClearMessages();

        }
    }

    public class GoodLogin : BaseBuildMessage
    {
        public GoodLogin(EQOAClient ThisClient)
        {
            ThisClient.ResponsePacket = new byte[168];

            BitConverter.GetBytes(ByteSwaps.SwapBytes(AccountMessageTypes.LOGIN_RESPONSE)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //LoginResponse
            ThisClient.resOffset += 4;

            new byte[72].CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 72)]);
            ThisClient.resOffset += 72;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(ThisClient.AccountID)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //AccountID 4 bytes
            ThisClient.resOffset += 4;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(ThisClient.Result)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 2)]); //Result 2 bytes
            ThisClient.resOffset += 2;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(ThisClient.AccountStatus)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 2)]); //acct status 2 bytes
            ThisClient.resOffset += 2;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(ThisClient.Subtime)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //subtime 4 bytes
            ThisClient.resOffset += 4;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(ThisClient.Partime)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //partime 4 bytes
            ThisClient.resOffset += 4;

            new byte[64].CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 64)]);
            ThisClient.resOffset += 64;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(ThisClient.Subfeatures)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //AccountID 4 bytes
            ThisClient.resOffset += 4;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(ThisClient.Gamefeatures)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //AccountID 4 bytes
            ThisClient.resOffset += 4;

            SendMessage(ThisClient);
        }
    }

    public class BadAttempt : BaseBuildMessage
    {
        public BadAttempt(EQOAClient ThisClient)
        {
            string Response;
            switch (ThisClient.MessageType)
            {
                case AccountMessageTypes.LOGIN_RESPONSE:
                    Response = "Username or Password was incorrect.";
                    break;

                case AccountMessageTypes.SUBMIT_ACCT_CREATE:
                    Response = "An error occured or username is taken. Please try again and if unsuccessful, try another username. Thank you.";
                    break;

                case AccountMessageTypes.CHANGE_PASSWORD:
                    Response = "An error occured. Please try again. If this persists try a new password.";
                    break;

                case AccountMessageTypes.DISABLED_FEATURE:
                    Response = "This option is disabled in order to bring you content faster. If you feel this is an error, please contact the developer team for further assistance.";
                    break;

                default:
                    Response = "You've managed to do something wrong! Please let the developer team know you received this message.";
                    break;

            }
            ThisClient.ResponsePacket = new byte[276];

            //Response Code
            BitConverter.GetBytes(ByteSwaps.SwapBytes(AccountMessageTypes.ACCT_CREATE_RESPONSE)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //LoginResponse
            ThisClient.resOffset += 16;

            //Add Our message
            Encoding.ASCII.GetBytes(Response).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + Response.Length)]);
            ThisClient.resOffset += 256;
            //Process Last bit of packet and send
            SendMessage(ThisClient);
        }
    }

    public class GoodChangePassword : BaseBuildMessage
    {
        public GoodChangePassword(EQOAClient ThisClient)
        {
            ThisClient.ResponsePacket = new byte[272];

            BitConverter.GetBytes(ByteSwaps.SwapBytes(AccountMessageTypes.CHANGE_PASSWORD_RESPONSE)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //Good Password Response
            ThisClient.resOffset += 8;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(1)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //1 is success I believe? 0 is probably failure? Been awhile
            ThisClient.resOffset += 260;

            SendMessage(ThisClient);
        }
    }

    public class GoodCreateAccount : BaseBuildMessage
    {
        public GoodCreateAccount(EQOAClient ThisClient)
        {
            ThisClient.ResponsePacket = new byte[276];

            BitConverter.GetBytes(ByteSwaps.SwapBytes(AccountMessageTypes.ACCT_CREATE_RESPONSE)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); //Good Password Response
            ThisClient.resOffset += 8;

            BitConverter.GetBytes(ByteSwaps.SwapBytes(1)).CopyTo(ThisClient.ResponsePacket[ThisClient.resOffset..(ThisClient.resOffset + 4)]); ; //1 is success I believe? 0 is probably failure? Been awhile
            ThisClient.resOffset += 264;

            SendMessage(ThisClient);
        }
    }
}
