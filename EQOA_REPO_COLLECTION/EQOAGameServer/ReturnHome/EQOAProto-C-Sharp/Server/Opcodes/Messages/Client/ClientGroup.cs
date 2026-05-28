using System.Text;
using ReturnHome.Server.EntityObject.Grouping;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Utilities;

namespace ReturnHome.Server.Opcodes.Messages.Client
{
    public static class ClientGroup
    {
        public static void DisbandGroup(Session session, Message ClientPacket) => GroupManager.DisbandGroup(session);

        public static void LeaveGroup(Session session, Message ClientPacket) => GroupManager.RemoveGroupMember(session, GroupActionEnum.LeaveGroup);

        public static void AcceptGroupInvite(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            uint playerID = reader.Read<uint>();

            GroupManager.AcceptInviteToGroup(session, playerID);
        }

        public static void AddCharacterToGroup(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            uint playerID = reader.Read<uint>();
            string name = reader.ReadString(Encoding.UTF8, reader.Read<int>());

            GroupManager.InviteCharacterToGroup(session, playerID, name);
        }

        public static void DeclineGroupInvite(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            uint playerID = reader.Read<uint>();
            byte action = reader.Read<byte>();

            GroupManager.PlayerDeclinedInvite(session, playerID, action);
        }

        public static void BootGroupMember(Session session, Message ClientPacket)
        {
            BufferReader reader = new(ClientPacket.message.Span);

            uint PlayerIDToBoot = reader.Read<uint>();

            GroupManager.RemoveGroupMember(session, GroupActionEnum.GroupLeaderEjectedYou, PlayerIDToBoot);
        }
    }
}
