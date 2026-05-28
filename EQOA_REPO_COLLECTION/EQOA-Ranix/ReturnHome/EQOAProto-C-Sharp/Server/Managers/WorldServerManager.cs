using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using ReturnHome.Database.SQL;
using ReturnHome.Server.EntityObject.Actors;
using ReturnHome.Server.EntityObject;
using QuadTrees;

using ReturnHome.Server.Network.Managers;
using ReturnHome.Server.EntityObject.Player;

namespace ReturnHome.Server.Managers
{
    public static class WorldServer
    {

        private static Stopwatch gameTimer;
        private static int serverTick = 1000 / 10;

        static WorldServer()
        {

        }

        public static void Initialize()
        {
            //Creates NPC List
            CharacterSQL npcList = new();
            //Calls sql query function that fills list full of NPCs
            List<Actor> myNpcList = npcList.WorldActors();
            //Closing DB connection
            npcList.CloseConnection();
            MapManager.Initialize();
            //Loops through each npc in list and sets their position, adds them to the entity manager, and mapmanager
            Console.WriteLine("Adding NPCs...");
            foreach (Actor myActor in myNpcList)
            {
                EntityManager.AddEntity(myActor);
                MapManager.Add(myActor);

            }
            Console.WriteLine("Done.");

            Console.WriteLine("Loading Default character options");
            CharacterSQL LoadDefaultCharacters = new();
            LoadDefaultCharacters.CollectDefaultCharacters();
            LoadDefaultCharacters.CloseConnection();
            Console.WriteLine("Done...");
            var thread = new Thread(() =>
            {
                UpdateWorld();
            });
            thread.Name = "World Manager";
            thread.Priority = ThreadPriority.AboveNormal;
            thread.Start();
        }

        public async static void UpdateWorld()
        {
            gameTimer = new Stopwatch();

            while (true)
            {
                gameTimer.Restart();

                SessionManager.BeginSessionWork();

                gameTimer.Stop();

                if (gameTimer.ElapsedMilliseconds > serverTick)
                {
                    Console.WriteLine("Server can't keep up");
                }

                await Task.Delay(Math.Max(0, serverTick - (int)gameTimer.ElapsedMilliseconds));
            }
        }
    }
}
