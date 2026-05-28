using AuthServer.Utility;
using AuthServer.Server;
using System;

namespace AuthServer.Account
{

    static class EQOAAccountManager
    {

    }

    public class AccountManagement
    {
        public void LoginRequest(EQOAClient Client)
        {
            //Get first 2 integers... not sure on their purpose.
            int Unknown1 = BinaryPrimitiveWrapper.GetBEInt(Client);
            int Unknown2 = BinaryPrimitiveWrapper.GetBEInt(Client);

            //Get username
            //Appears 32 bytes is allocated for username
            Client.Username = Utilities.ReturnString(Client, 32);

            //Console.WriteLine($"Username: {Client.Username}");

            //Get Encrypted Password
            //Appears 32 bytes is allocated for encrypted Password
            Client.EncPassword = Utilities.ReturnString(Client, 32);

            //Console.WriteLine($"Password: {Client.EncPassword}");

            int unknown3 = BinaryPrimitiveWrapper.GetBEInt(Client);

            //Assuming next 60 bytes are saved for other game types? Unsure but works for now.
            string GameID = Utilities.ReturnString(Client, 60);


            //Do some stuff to verify username and password against database

            Client.AccountVerified = true;

            if(Client.AccountVerified == true)
            {
                new GoodLogin(Client);
            }

            else
            {
                //Client failed to authenticate
                new BadAttempt(Client);
            }
        }

        public void CreateAccount(EQOAClient Client)
        {
            int Header = BinaryPrimitiveWrapper.GetBEInt(Client); //Message header
            Client.Username = Utilities.ReturnString(Client, 32); // Username
            Client.EncPassword = Utilities.ReturnString(Client, 32); //Encrypted Password
            //More information to get here possibly... concerns on privacy also?
            //Name, country, zip birthday
            //email may be good to gather

            Console.WriteLine($" Creating account using Username: {Client.Username}");
            Console.WriteLine($"Encrypted Password: {Client.EncPassword}");

            //Check database for username
            //If it exists
            //new BadAttempt(ThisClient);

            //If username is free, start saving to database
            //Should we save connecting ip address and time stamp the login?
            new GoodCreateAccount(Client);
        }

        //This happens once you have logged in already, username should be saved to this Client Session
        public void ChangePassword(EQOAClient Client)
        {
            Console.WriteLine($"{Client.Username} is attempting to change password....");

            int Unknown1 = BinaryPrimitiveWrapper.GetBEInt(Client);
            int Unknown2 = BinaryPrimitiveWrapper.GetBEInt(Client);

            // Need to verify old password was correct against database
            string OldPassword = Utilities.ReturnString(Client, 32); //Encrypted Old Password
            
            //If old Password checks out against database, replace it with new password
            string NewPassword = Utilities.ReturnString(Client, 32); //Encrypted New Password

            Console.WriteLine($"Attempting to change Client Password. Old encrypted Password: {OldPassword}, New Encrypted Password: {NewPassword}");

            //Success
            new GoodChangePassword(Client);

            //If Oldpassword verification fails, send failure
            //new BadAttempt(ThisClient);
        }
    }
}
