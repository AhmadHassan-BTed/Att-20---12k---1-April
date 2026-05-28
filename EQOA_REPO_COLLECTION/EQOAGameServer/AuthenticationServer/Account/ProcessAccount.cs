using System.Configuration;
using System;

using AuthServer.Utility;
using AuthServer.Server;
using AuthServer.Account.Database;
using EQOACryptoLibrary;

namespace AuthServer.Account
{

    static class EQOAAccountManager
    {

    }

    public static class AccountManagement
    {
        private static bool _useDecryption;
        private static byte[] _deskey;
        public static void Initialize()
        {
            _useDecryption = bool.Parse(ConfigurationManager.AppSettings["UseDecryption"]);
            if (_useDecryption)
            {
                string temp = ConfigurationManager.AppSettings["DESkey"];
                byte[] t = new byte[8];
                for (int i = 0; i < (temp.Length / 2); ++i)
                    t[i] = Convert.ToByte(temp.Substring(i * 2, 2), 16);
                _deskey = t;
            }
        }
        public static void LoginRequest(EQOAClient Client)
        {
            //Get first 2 integers... not sure on their purpose.
            int Unknown1 = BinaryPrimitiveWrapper.GetBEInt(Client);
            int Unknown2 = BinaryPrimitiveWrapper.GetBEInt(Client);

            //Get username
            //Appears 32 bytes is allocated for username
            Client.Username = Utilities.ReturnString(Client, 32);
            for (int i = 0; i < Client.Username.Length; i++)
            {
                if (Client.Username[i] != '\0')
                    continue;
                Client.Username = Client.Username.Remove(i, Client.Username.Length - i);
                break;
            }

            //Console.WriteLine($"Username: {Client.Username}");

            //Get Encrypted Password
            //Appears 32 bytes is allocated for encrypted Password
            Client.EncPassword = Client.ClientPacket.Slice(Client.offset, 32);
            Client.offset += 32;

            //Console.WriteLine($"Password: {Client.EncPassword}");

            int unknown3 = BinaryPrimitiveWrapper.GetBEInt(Client);
            Client.MessageType = AccountMessageTypes.LOGIN_RESPONSE;
            //Assuming next 60 bytes are saved for other game types? Unsure but works for now.
            string GameID = Utilities.ReturnString(Client, 60);

            SQLAccount sql = new SQLAccount();
            if (_useDecryption)
            {
                if (sql.VerifyAccount(Client.Username, new CryptoLibrary(CryptoOptions.DES, _deskey).Decrypt(Client.EncPassword), out int _))
                    new GoodLogin(Client);

                else
                    new BadAttempt(Client);
            }

            else
            {
                if (sql.AccountExists(Client.Username, out int _))
                    new GoodLogin(Client);

                else
                    new BadAttempt(Client);
            }
            sql.CloseConnection();
        }

        public static void CreateAccount(EQOAClient Client)
        {
            int Header = BinaryPrimitiveWrapper.GetBEInt(Client); //Message header
            Client.Username = (Utilities.ReturnString(Client, 32)); // Username
            for (int i = 0; i < Client.Username.Length; i++)
            {
                if (Client.Username[i] != '\0')
                    continue;
                Client.Username = Client.Username.Remove(i, Client.Username.Length - i);
                break;
            }

            Client.EncPassword = Client.ClientPacket.Slice(Client.offset, 32);
            Client.offset += 32; //Encrypted Password
            //More information to get here possibly... concerns on privacy also?
            //Name, country, zip birthday
            //Jump to email for now

            Client.offset = 348;
            Client.Email = Utilities.ReturnString(Client, 128);

            for (int i = 0; i < Client.Email.Length; i++)
            {
                if (Client.Email[i] != '\0')
                    continue;
                Client.Email = Client.Email.Remove(i, Client.Email.Length - i);
                break;
            }

            Client.MessageType = AccountMessageTypes.ACCT_CREATE_RESPONSE;
            //Check database for username
            //If it exists
            SQLAccount sql = new SQLAccount();
            if (_useDecryption)
            {
                if (sql.AccountCreation(Client, new CryptoLibrary(CryptoOptions.DES, _deskey).Decrypt(Client.EncPassword)))
                    new GoodCreateAccount(Client);
                else
                    new BadAttempt(Client);
            }

            else
            {
                if (sql.AccountCreation(Client, Client.EncPassword))
                    new GoodCreateAccount(Client);
                else
                    new BadAttempt(Client);
            }
        }

        //This happens once you have logged in already, username should be saved to this Client Session
        public static void ChangePassword(EQOAClient Client)
        {
            Console.WriteLine($"{Client.Username} is attempting to change password....");

            int Unknown1 = BinaryPrimitiveWrapper.GetBEInt(Client);
            int Unknown2 = BinaryPrimitiveWrapper.GetBEInt(Client);

            // Need to verify old password was correct against database
            string OldPassword = Utilities.ReturnString(Client, 32); //Encrypted Old Password

            //If old Password checks out against database, replace it with new password
            string NewPassword = Utilities.ReturnString(Client, 32); //Encrypted New Password

            Client.MessageType = AccountMessageTypes.CHANGE_PASSWORD_RESPONSE;
            Console.WriteLine($"Attempting to change Client Password. Old encrypted Password: {OldPassword}, New Encrypted Password: {NewPassword}");

            //Success
            new GoodChangePassword(Client);

            //If Oldpassword verification fails, send failure
            //new BadAttempt(ThisClient);
        }
    }
}
