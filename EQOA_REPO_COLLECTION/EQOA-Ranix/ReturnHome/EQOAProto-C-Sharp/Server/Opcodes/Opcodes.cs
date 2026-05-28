namespace ReturnHome.Server.Opcodes
{
    public enum PacketHeader : ushort
    {
        serverTransfer = 0xFFFF,
        NewConnection = 0xFFFE
    }

    public static class UnreliableTypes
    {

        public const byte ClientActorUpdate = 64; //0x40 Client Actor update
    }

    public enum GameOpcode : ushort
    {
        //Server/Client shared opcodes
        DiscVersion = 0x00, //0
        RandomName = 0x12, //18
        RemoveInvItem = 0x3C, //60
        ArrangeItem = 0x3D, //61

        //Server Only Opcodes
        CharacterSelect = 0x2C, ///44
        BestEffortPing = 0x07D0,
        Camera1 = 0x07D1, ///2001
        Camera2 = 0x07F5, ///2037
        GameServers = 0x07B3, ///1971
        DelCharacter = 0x2D, ///45
        NameTaken = 0x2F, ///47
        MemoryDump = 0x0D, //13
        Time = 0x13, //19
        IgnoreList = 0x1005, //4101
        PlayerSpeed = 0xF8, //248
        ClientMessage = 0x0A7A, //2682 - White text message
        DisconnectClient = 0x09B0, //2480
        ColoredChat = 0x0A7B, //2683 - Color text message
        TargetInformation = 0x0761, //Target information for player/Actor
        OptionBox = 0x34, //52
        AddInvItem = 0x3B, //59
        AddQuestLog = 0x007C, //124
        PlayerTunar = 0x0052, //82
        ConfirmBankTunar = 0x1253, //4691
        BankItem = 0x1254, //4692
        DialogueBox = 0x46, //70
        GrantXP = 0x0020, //32
        MerchantBox = 0xB7, //183
        EquipItem = 0x3F,
        UnequipItem = 0x40,
        RemoveBankItem = 0x1250,
        AddBankItem = 0x1251,
        Teleport = 0x07F6,
        UpdateTrainingPoints = 0x001D,
        ClassMasteryServer = 0x1402,
        FactionStuff  = 0xa7a,
        CharacterModifiedDisconnect = 0x07d2,
        LoggedInFromAnotherLocationDisconnect = 0x07A4,
        BadLoginPassword = 0x0729,
        OutmatchedAttackingAgainNotAdvised = 0x77A, //Outmatched Attacking again not advised
        NotSubscribed = 0x072B,
        AddBuddyFailed = 0x03d8,
        BuddyListFull = 0x03d7,
        Something = 0x468, //Guild stuff

        //From client
        ClientSayChat = 0x000B, //11 - Normal say message from client
        ClientShout = 0x0C01, //3073 - Shout message from client
        ChangeChatMode = 0x000E, //14 - Client requests change to "permanant" chat mode
        Authenticate = 0x0904, ///2308
        Authenticate2 = 0x01, ///1
        Interact = 0x04, //4
        Target = 0x0760, //1888
        CreateCharacter = 0x2B, ///43
        SELECTED_CHAR = 0x2A, // 42
        InteractItem = 0x3E,
        BankUI = 0x124D, //4685
        DepositBankTunar = 0x1255, //4693
        MerchantDiag = 0x4C, //76
        DialogueBoxOption = 0x35, //53
        DeleteQuest = 0x7D, //125
        MerchantBuy = 0x4A, //74
        MerchantSell = 0x4B, //75
        EnableChannel = 0x49,
        ClassMastery = 0x1401,
        ClientFaction = 0x0AC4,
        Attack = 0x0F,
        BlacksmithRepair = 0x0061,
    }

    public static class GameVersions
    {

        public const int EQOA_FRONTIERS = 0x25;
        public const int EQOA_VANILLA = 0x12;
        public const int UNKNOWN = 0x00;

    }


    public static class SessionOpcode
    {
        public static readonly sbyte NewSession = 0x21; ///Client requesting new session
        public static readonly sbyte CloseSession = 0x14; ///Client requesting to close session
        public static readonly sbyte ContinueSession = 0x1; ///Client is continuing session

    }

    public static class BundleOpcode
    {
        public const byte ProcessAll = 99; ///Client requesting new session
        public const byte ProcessReport = 35; ///Client requesting to close session
        public const byte ProcessMessages = 32; ///Client is continuing session
        public const byte ProcessMessageAndReport = 19; ///Client is continuing session
        public const byte NewProcessReport = 3; ///Client is continuing session
        public const byte NewProcessMessages = 0; ///Client is continuing session
    }
}
