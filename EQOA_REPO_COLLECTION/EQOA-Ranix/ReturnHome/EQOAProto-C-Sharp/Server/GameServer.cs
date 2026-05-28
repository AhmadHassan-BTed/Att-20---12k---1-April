using System;
using System.Timers;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network.Managers;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Opcodes;

namespace ReturnHome.Server
{
    /*
    Keep this in main.cs
    Will also start a thread for packet processing
    */
    public class GameServer
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("Starting Server...");
			//Load config for ServerListManager and start it
			ServerListManager.ReadConfig(); 
			Timer t = new(5000);
            t.AutoReset = true;
            t.Elapsed += new ElapsedEventHandler((_, _) => ServerListManager.DistributeServerList());
            t.Start();

            //Load GameManager
            Console.WriteLine("Starting World...");
            WorldServer.Initialize();

            Console.WriteLine("Starting UDP Server...");
            // Start SocketManager
			SocketManager.Initialize();
            Console.WriteLine("Server started.");
            /*EventManager eManager = new EventManager();
            Dialogue newDiag = new Dialogue("Aeric Sparelli", 0, 0);
            Character newCharacter = new Character();
            newCharacter.MyDialogue = newDiag;
            Dialogue thisDiag = eManager.GetNPCDialogue(GameOpcode.DialogueBox,newCharacter);
            foreach(string dialogue in thisDiag.diagOptions)
            {
                Console.WriteLine(dialogue);
            }*/
           

            string stuff = Console.ReadLine();



        }
    }
}

