using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Opcodes.Messages.Server;

namespace ReturnHome.Server.EntityObject.Grouping
{
    public class Group
    {
        //Keep it basic for now, eventually need to control pet's here I think? Though it's not a real group with just a pet..
        public uint GroupID { private set; get; }
        private List<Entity> charList = new();

        public int Size => charList.Count;
        public List<Entity> GroupList => charList;

        public Group(uint id, Entity char1, Entity char2)
        {
            GroupID = id;
            charList.Add(char1);
            charList.Add(char2);

            foreach (Character c in charList)
            {
                c.characterSession.rdpCommIn.connectionData.serverGroupUpdate.AddObject(this);
                c.GroupID = GroupID;
            }

            UpdatePlayerList();
        }

        public bool AddMember(Entity e)
        {
            if (charList.Count < 4)
            {
                charList.Add(e);
                ((Character)e).characterSession.rdpCommIn.connectionData.serverGroupUpdate.AddObject(this);
                ((Character)e).GroupID = GroupID;
                UpdatePlayerList();

                return true;
            }
            return false;
        }

        public void Disband()
        {
            //Loop over member's and remove association's to the group, then send out the opcode to disband
            foreach (Character c in charList)
            {
                c.GroupID = 0;
                c.characterSession.rdpCommIn.connectionData.serverGroupUpdate.IsActive = false;
                ServerGroup.DisbandGroup(c.characterSession);
            }
        }

        private void UpdatePlayerList()
        {
            foreach (Character c in charList)
                ServerGroup.CreateGroup(c.characterSession, this);
        }

        public bool RemoveMember(uint MemberToRemove, GroupActionEnum e)
        {
            for (int i = 0; i < charList.Count; i++)
            {
                //Remove member
                if (MemberToRemove == charList[i].ObjectID)
                {
                    Character c = (Character)charList[i];
                    DisableGroupForCharacter(c);
                    ServerGroup.RemoveGroupMember(c.characterSession, e);
                    charList.RemoveAt(i);
                    UpdatePlayerList();
                    return true;
                }
            }
            return false;
        }

        private void DisableGroupForCharacter(Character c)
        {
            c.GroupID = 0;
            c.characterSession.rdpCommIn.connectionData.serverGroupUpdate.IsActive = false;
        }

        public void  DistributeUpdates()
        {
            int counter;
            //Iterate over each character for the group
            for(int i = 0; i < charList.Count; i++)
            {
                counter = 0;
                Span<byte> temp = charList[i].GroupUpdate.Span;
                for(int j = 0; j < charList.Count; j++)
                {
                    //Don't want to write ourselve's to our data
                    if(i == j)
                        continue;

                    uint id = charList[j].ObjectID;
                    MemoryMarshal.Write(temp[counter..], ref id);
                    counter += 4;

                    if(Vector3.Distance(charList[i].Position, charList[j].Position) < 100f)
                    {
                        temp[counter++] = 1;
                        temp[counter++] = (byte)((255 * charList[i].CurrentHP) / charList[i].HPMax);
                    }

                    else
                    {
                        temp[counter++] = 0;
                        temp[counter++] = 0xFF;
                    }

                    temp[counter++] = 4;
                    ushort x = (ushort)charList[i].x;
                    ushort y = (ushort)charList[i].y;
                    ushort z = (ushort)charList[i].z;

                    MemoryMarshal.Write(temp[7..], ref x);
                    counter += 2;
                    MemoryMarshal.Write(temp[9..], ref y);
                    counter += 2;
                    MemoryMarshal.Write(temp[11..], ref z);
                    counter += 2;
                }

                //Fill rest of span with 0's
                temp[counter..].Fill(0);
            }
        }
    }
}
