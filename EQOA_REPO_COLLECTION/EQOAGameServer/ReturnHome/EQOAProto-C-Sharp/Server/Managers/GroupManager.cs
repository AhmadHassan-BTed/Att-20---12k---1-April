using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Grouping;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Chat;
using ReturnHome.Server.Opcodes.Messages.Server;

namespace ReturnHome.Server.Managers
{
    public static class GroupManager
    {
        private static ConcurrentDictionary<uint, Group> _groupDict = new();
        //Probably not needed but... was an idea to track invites
        private static List<GroupInvite> GroupInviteList = new();

        public static Group QueryForGroup(uint i) => _groupDict[i];
        public static void InviteCharacterToGroup(Session s, uint id, string name)
        {
            GroupActionEnum action = GroupActionEnum.IsStillLoading;
            string InviteeName = "";

            //Grab the invited character
            if (PlayerManager.QueryForPlayer(name, out Character c))
            {
                //is invited character in a group?
                if (c.GroupID != 0)
                    action = GroupActionEnum.AlreadyInAGroup;

                //Does a group for this character exist?
                if (_groupDict.ContainsKey(s.MyCharacter.GroupID))
                {
                    //Check if group is full
                    if (_groupDict[s.MyCharacter.GroupID].Size >= 4)
                        ChatMessage.GenerateClientSpecificChat(s, "Your group is full.");

                    else
                    {
                        GroupInviteList.Add(new GroupInvite(s.MyCharacter, c));
                        //Send invite to said character
                        ServerCharacterGroupInvite.GroupInvite(s, c);
                    }
                    return;
                }

                else
                {
                    //TODO: Add code to detect and track ignoring player's, add catch here to utilize the proper opcode
                    if (true)
                    {
                        GroupInviteList.Add(new GroupInvite(s.MyCharacter, c));
                        //Send invite to said character
                        ServerCharacterGroupInvite.GroupInvite(s, c);
                        return;
                    }

                    else
                        action = GroupActionEnum.IsIgnoringYou;
                }

                InviteeName = c.CharName;
            }

            //Player isn't online? Or something...
            else
                action = GroupActionEnum.RecipientNotFound;

            ServerGroup.PlayerDeclinedInvite(s, action, InviteeName);
        }

        //Need to make sure group isn't full
        public static void AcceptInviteToGroup(Session session, uint InviterID)
        {
            if(EntityManager.QueryForEntity(InviterID, out Entity temp))
            {
                Character GroupOwner = (Character)temp;

                //if inviting player doesn't own a group, create it
                if (GroupOwner.GroupID == 0)
                {
                    //Create the group
                    _groupDict.TryAdd(GroupOwner.ObjectID, new Group(GroupOwner.ObjectID, GroupOwner, session.MyCharacter));
                    ServerGroup.PlayerAcceptedInvite(GroupOwner.characterSession, session.MyCharacter);
                }

                //Add the member to the group
                else
                {
                    //Make sure group isn't full first
                    if (_groupDict[GroupOwner.ObjectID].Size < 4)
                    {
                        _groupDict[GroupOwner.ObjectID].AddMember(session.MyCharacter);
                        ServerGroup.PlayerAcceptedInvite(GroupOwner.characterSession, session.MyCharacter);
                    }

                    else
                        ServerGroup.RemoveGroupMember(session, GroupActionEnum.GroupIsFull);
                }
                return;
            }

            ServerGroup.PlayerDeclinedInvite(session, GroupActionEnum.RecipientNotFound);
        }

        public static void PlayerDeclinedInvite(Session session, uint InviterID, byte action)
        {
            foreach (GroupInvite invite in GroupInviteList)
            {
                if (invite.GroupLeader.ObjectID == InviterID && invite.Invitee == session.MyCharacter)
                {
                    ServerGroup.PlayerDeclinedInvite(((Character)invite.GroupLeader).characterSession, GroupActionEnum.DeclinedInvitation, invite.Invitee.CharName);
                    GroupInviteList.Remove(invite);
                    break;
                }
            }
        }

        public static void RemoveGroupMember(Session session, GroupActionEnum g, uint GroupMemberToBoot = 0)
        {
            //Use character's group ID to attempt finding the group
            if (_groupDict.TryGetValue(session.MyCharacter.GroupID, out Group temp))
            {
                //If the request was to boot a player, verify the groupleader requested this, if not ignore?
                if (g == GroupActionEnum.GroupLeaderEjectedYou && session.MyCharacter == temp.GroupList[0])
                    temp.RemoveMember(GroupMemberToBoot, g);

                if(g == GroupActionEnum.LeaveGroup)
                    temp.RemoveMember(session.MyCharacter.ObjectID, g);
            }
        }

        public static void DisbandGroup(Session session)
        {
            //If this works, then requesting player is leader, and able to disband
            if (_groupDict.TryRemove(session.MyCharacter.ObjectID, out Group group))
            {
                group.Disband();
            }

            //Character isn't the leader? => Send an error message? Client natively *should* catch this
        }

        public static void DenyInviteToGroup()
        {

        }

        public static void DistributeGroupUpdates()
        {
            foreach(KeyValuePair<uint, Group> pair in _groupDict)
            {
                pair.Value.DistributeUpdates();
            }
        }
    }
}
