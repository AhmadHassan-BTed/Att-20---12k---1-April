using System.Threading.Tasks;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientAttack
    {
        public static void ClientProcessAttack(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);
            uint targetID = reader.Read<uint>();

            
            if (EntityManager.QueryForEntity(targetID, out Entity npc))
            {
                npc.TakeDamage((uint)session.MyCharacter.ServerID, 500);
                
                //Blacksmith starts on 0x0008, 0x0018

                if (npc.CurrentHP <= 0)
                {
                    npc.Animation = 0x0e;

                    npc.KillTime = 1;
                }
            }
        }
    }
}
