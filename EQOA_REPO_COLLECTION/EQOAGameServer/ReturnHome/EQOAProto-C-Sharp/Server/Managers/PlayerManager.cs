using System;
using System.Collections.Generic;
using ReturnHome.Server.EntityObject.Player;

namespace ReturnHome.Server.Managers
{
    public static class PlayerManager
    {
        private static List<Character> playerList = new();

        public static bool AddPlayer(Character character)
        {
            //Add character to our tracking List
            if (playerList.Contains(character))
                //Return false here? Boot in world character and load new one?
                return false;

            playerList.Add(character);
            return true;
        }

        public static bool RemovePlayer(Character character)
        {
            if (!playerList.Contains(character))
                return false;
            playerList.Remove(character);
            return true;
        }

        public static bool QueryForPlayer(string name, out Character c)
        {
            foreach (Character c2 in playerList)
            {
                if (c2.CharName == name)
                {
                    c = c2;
                    return true;
                }
            }
            c = default;
            return false;
        }

        public static List<Character> QueryForAllPlayers()
        {
            return playerList;
        }


        //Stub method to be called on timer to save all characters in world
        public static void SaveCharacterData()
        {
            //List<Entity> charList = (List<Entity>)qtree.GetAllObjects().Cast<Entity>().ToList();
        }

        public static bool QueryForPlayer(uint targetID, out Character player)
        {
            foreach (Character c in playerList)
            {
                if(c.ServerID == targetID)
                {
                    player = c;
                    return true;
                }
            }
            player = default;
            return false;
        }
    }
}
