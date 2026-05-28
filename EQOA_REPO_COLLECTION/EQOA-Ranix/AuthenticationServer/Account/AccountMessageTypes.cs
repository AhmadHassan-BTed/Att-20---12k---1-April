namespace AuthServer.Account
{

    static public class AccountMessageTypes
    {
        public const uint DISABLED_FEATURE = 0x01;
        public const uint LOGIN_RESPONSE = 0x25;  // FROM SERVER  
        public const uint ACCT_CREATE_RESPONSE = 0x29;  // FROM SERVER
        public const uint CHANGE_PASSWORD_RESPONSE = 0X2B;  // FROM SERVER
        public const uint INVALID_KEY = 0X2D;  // FROM SERVER
        public const uint NO_KEY = 0X2F;  // FROM SERVER
        public const uint REQUEST_TIMED_OUT = 0X31;  // FROM SERVER
        public const uint BILLING_RESPONSE = 0X33;  // FROM SERVER
        public const uint NO_SUB_TO_EQOA = 0X35;  // FROM SERVER
        public const uint GAME_CARD_RESPONSE = 0X37;  // FROM SERVER
        public const uint ACQUIRING_SUB = 0X3F;  // FROM SERVER
        public const uint UPDATE_ACCT = 0X41;  // FROM SERVER
        public const uint FORGOT_PASSWORD_RESPONSE = 0X43;  // FROM SERVER

        public const uint REQUEST_CHANGE_PWD = 0x02;  // FROM CLIENT - sends two messages
        public const uint LOGIN_REQUEST = 0x24;  // FROM CLIENT  
        public const uint SUBMIT_ACCT_CREATE = 0x38;  // FROM CLIENT  

        public const uint CHANGE_PASSWORD = 0X2A;  // FROM CLIENT
        public const uint CONSUME_GAMECARD = 0x2E;  // FROM CLIENT
        public const uint REQUEST_ACCT_KEY = 0x30;  // FROM CLIENT
        public const uint REQUEST_CANCEL_ACCT = 0x32;  // FROM CLIENT
        public const uint REQUEST_GAME_CARD = 0x36;  // FROM CLIENT
        public const uint REQUEST_SUBSCRIPTION = 0x3E;  // FROM CLIENT
        public const uint REQUEST_UPDATE_ACCT = 0x40;  // FROM CLIENT
        public const uint FORGOT_PASSWORD = 0X42;  // FROM CLIENT

    }

    static public class AccountStatus
    {
        public const short INACTIVE = 0x00;
        public const short NORMAL = 0x01;
        public const short REQUESTKEY = 0x02;
        public const short UNKNOWN = 0x03;
        public const short UNKNOWN_ERR = 0x04;
        public const short NORMAL2 = 0x05;
        public const short TRIAL_EXP = 0x06;
        public const short NORMAL_SUSPENDED = 0x07;
        public const short BANNED = 0x08;
        public const short NORMAL_RENTAL = 0x09;
        public const short EXPIRED_RENTAL = 0x0A;
        public const short UNKNOWN_ERR2 = 0x0B;
        public const short NOT_TESTEDC = 0x0C;
        public const short NOT_TESTEDD = 0x0D;
        public const short NOT_TESTEDE = 0x0E;
        public const short NOT_TESTEDF = 0x0F;

    }

    static public class LoginResults
    {
        public const byte NO_USER = 0x00;
        public const byte BAD_PASS = 0x01;
        public const byte NAME_TAKEN = 0x02;
        public const byte ERROR = 0x07;
        public const byte GOOD = 0x08;

    }
}
