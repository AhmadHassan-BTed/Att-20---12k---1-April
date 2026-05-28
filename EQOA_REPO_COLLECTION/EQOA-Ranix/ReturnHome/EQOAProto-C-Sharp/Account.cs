
namespace ReturnHome.AccountAction
{
    class AccountActions
    {
        /// Verify Account Password/username and return account # here if true.
        public static int VerifyPassword(string AccountName, string Password)
        {
            //Standard account #1 for now, should be base #?
            //Once in a more production type environment, change to 0 for default incase of failure
            int AccountNumber = 2;
            //string PasswordHash = ""; 

            return AccountNumber;
        }
    }
}
