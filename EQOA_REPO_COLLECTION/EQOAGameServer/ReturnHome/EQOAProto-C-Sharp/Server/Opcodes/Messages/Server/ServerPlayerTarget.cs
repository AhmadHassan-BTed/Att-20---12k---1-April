using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Server
{
    class ServerPlayerTarget
    {
        public static void PlayerTarget(Session session, byte faceColor, byte conColor, uint target, uint targetCounter)
        {
            Message message = Message.Create(MessageType.ReliableMessage, GameOpcode.TargetInformation);
            BufferWriter writer = new BufferWriter(message.Span);

            writer.Write(message.Opcode);

            writer.Write(faceColor); // 0/1 = red face 2/3 = neutral face 4/5 = blue face //Perform Calculations to check for 
            writer.Write(conColor); // 0 = red con 1 = yellow con 2 = white con 3 = Dark Blue con 4 = Light Blue Con 5 = Green con 6 = Yellowish/white con? 7 = no con at all? But can still target? 14 = faded yellow con? 15 = faded orange con? 60 = yellowish/green con?

            writer.Position = 124;
            writer.Write(target);

            writer.Position = 261;
            writer.Write(targetCounter);
            message.Size = writer.Position;
            session.sessionQueue.Add(message);
        }
    }
}
