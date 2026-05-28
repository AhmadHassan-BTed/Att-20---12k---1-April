using System;
using System.Timers;

using ReturnHome.Server.Managers;
using ReturnHome.Server.Network.Managers;
using ReturnHome.Server.Opcodes.Messages.Client;

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

            //Initialize Password stuff if 
            ClientAuthenticate.Initialize();
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
            
           

            string stuff = Console.ReadLine();



        }
    }
}

