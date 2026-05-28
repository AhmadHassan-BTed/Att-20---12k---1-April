using System;
using System.Text;
using EQOACryptoLibrary;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;
using AuthServer.Account.Database;
using System.Configuration;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientAuthenticate
    {
        private static byte[] _aeskey;
        private static bool _useDecryption;
        public static void Initialize()
        {
            _useDecryption = bool.Parse(ConfigurationManager.AppSettings["UseDecryption"]);
            if (_useDecryption)
            {
                string temp = ConfigurationManager.AppSettings["AESkey"];
                byte[] t = new byte[32];
                for (int i = 0; i < (temp.Length / 2); ++i)
                    t[i] = Convert.ToByte(temp.Substring(i * 2, 2), 16);
                _aeskey = t;
            }

        }
        ///Authentication check
        public static void Authenticate(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            Logger.Info("Processing Authentication");
            ///Opcode option? just skip for now
            reader.Position = 5;

            ///Game Code Length
            int GameCodeLength = reader.Read<int>();

            ///the actual gamecode
            string GameCode = reader.ReadString(Encoding.UTF8, GameCodeLength);

            if (GameCode == "EQOA")
            {
                ///Authenticate
                Logger.Info("Received EQOA Game Code, continuing...");

                ///Account name Length
                int AccountNameLength = reader.Read<int>();

                ///the actual gamecode
                string AccountName = reader.ReadString(Encoding.UTF8, AccountNameLength);

                Logger.Info($"Received Account Name: {AccountName}");

                ///Username ends with 01, no known use, skip for now
                reader.Position += 1;

                ReadOnlyMemory<byte> Password = reader.ReadArray<byte>(16);
                reader.Position += 16;

                SQLAccount sql = new SQLAccount();

                if (_useDecryption)
                {
                    if(sql.VerifyAccount(AccountName, new CryptoLibrary(CryptoOptions.AES, _aeskey).Decrypt(Password), out int accountID))
                        session.AccountID = accountID;

                    else
                        //Verifications failed, drop session?
                        session.DropSession();
                }

                else
                {
                    if (sql.AccountExists(AccountName, out int accountID))
                        session.AccountID = accountID;

                    else
                        //Verifications failed, drop session?
                        session.DropSession();
                }

                sql.CloseConnection();
            }

            else
            {
                ///If not EQOA.... drop?
                Logger.Err("Did not receive EQOA Game Code, not continuing...");
                ///Should we attempt to disconnect the session here?
            }
        }
    }
}
