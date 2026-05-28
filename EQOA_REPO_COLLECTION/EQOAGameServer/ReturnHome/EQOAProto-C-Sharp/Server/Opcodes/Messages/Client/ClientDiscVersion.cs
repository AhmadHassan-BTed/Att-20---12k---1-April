using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    class ClientDiscVersion
    {
        ///Game Disc Version
        public static void DiscVersion(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            ///Gets Gameversion sent by client
            int GameVersion = reader.Read<int>();

            switch (GameVersion)
            {
                ///Game Disc Version
                case GameVersions.EQOA_FRONTIERS:
                    Logger.Info("EQOA Frontiers Selected.");
                    break;

                case GameVersions.EQOA_VANILLA:
                    Logger.Info("EQOA Vanilla Disc, no support");
                    break;

                case GameVersions.UNKNOWN:
                    Logger.Err("UNKNOWN Game Disc");
                    break;

                default:
                    Logger.Err("Unable to identify Game Disc");
                    break;
            }

            ServerDiscVersion.DiscVersion(session, GameVersion);
        }
    }
}
