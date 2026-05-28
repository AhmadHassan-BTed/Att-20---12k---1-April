using ReturnHome.Server.Opcodes.Messages.Server;

namespace ReturnHome.Server.EntityObject.Grouping
{
    public struct GroupInvite
    {
        public Entity GroupLeader;
        public Entity Invitee;

        public GroupInvite(Entity primary, Entity secondary)
        {
            GroupLeader = primary;
            Invitee = secondary;
        }
    }
}
