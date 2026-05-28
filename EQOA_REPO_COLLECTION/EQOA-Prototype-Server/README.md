Decided to open this as the original developer/collaborator with me has been gone for over a year now. There is no license, or guarantee to what is provided here.

# EQOA-Prototype-Server
A very rough, and honestly probably not great, prototype server for the EQOA revival. Using Python3 and Mariadb for the database, it is possible to get through Account Login (Create account, login, etc). Up to World Select , through character select(Which is tied to account creation and characters on account.) In to entering world.

# What is this?
Thats a good question. This would be a rough attempt at a prototype for the EQOA revival ambitions. While it may lack a lot of "features", and the code may not be very pretty (Sorry, I'm no expert), it accomplishes some key functions such as an account login server. A decent starting point for a world server and character select code base. The character select and world select displays some of the key functions that would likely need to be found within the true game server. Again, alot of things are missing, and it may not be very optimized or pretty. Alot of work I have mostly done on my own after a certain point.

## Why now?
This group started with a few people, everyone with a common goal and aspirations, let's bring this game back. Overtime we have gotten busy and has led to us making very minimal progress. 

Ben Turi and I have done a lot of work on this, which may or may not be seen. Ben has become extremely busy in real life, and my time is very limited. Over the last few months I have built onto the report system framework Ben left behind to "create" worlds, reach character select (With characters pulled from a database) and even begin (And I think have "completed") the memory dump process, which should be right at the point of entering the world. 

## Can I help?
What is here is obviously not perfect, and I have ommited some parts to preserve environment IP's and such. If you have questions feel free to ask, if you know python3 and want to help, feel free to fork this. I plan to push updates as I work, probably once a week if I do work on this.

What this is capable of is;
 - Account login server (Mostly login and account creation for time being)
 - World select options (Editable within the Config file in the config folder)
 - Character select (Once an account is created, it is possible to tie character to this accounts)
 - Once a character is selected, the server begins the memory dump.
 
What to do;
 - Clean up alot of the code, add alot of features in current work.
 - Complete entering world. I believe all that needs to be done is generating a co packet for client
 - Making this multiplayer, see other players moving around. Make it work well
   - I believe this will require multiple processes due to python GIL. Thoughts are to use a third party tool for message queues for quicker message passes. This would allow 1 process to process and monitor player locations and push this information to respective clients, quicker then multiprocess python queues
 
 - Clean up multi-player capabilities and make it smooth.
 - Begin developing map meshes for server to monitor characters over.
 - Once meshes are created, begin some NPC trials and AI testing. 
 - Start adding other opcodes for processing, and lastly, combat.

What to do is much more advanced then that, but I made it fairly high level for ease of understanding. I believe that once in world, developing the multiplayer aspect and making other players appear on clients smoothly is a very critical step moving forward, and quite possibly would be one of the most difficult to be accomplished, becoming a critical milestone. Once this is completed, I believe everything else should follow suite much easier.

## How do I run the test server?
 
 ### Client Setup
  - PCSX2 setup can be found here; http://wiki.eqoarevival.com/index.php/Client_Setup
  - Need to find the EQOA frontiers iso on your own, additional may want to get the network adapter iso
  - Have a DNS redirect(Example here; http://wiki.eqoarevival.com/index.php/Server_Setup_Windows#DNS_Server or useful on linux; https://github.com/Trackbool/DerpNSpoof)
  - Lastly, an http serve will be needed which can be located here; http://wiki.eqoarevival.com/index.php/Server_Setup_Windows#HTTP_Server or can use pythons built in http serve
 
 ### Server setup
  - Python3 and required packages (Seen in the requirements.txt)
  - Mariadb (Other types may work but this is the database we have chosen to use)
    - Creating a user within MariaDB with admin privileges (Just easier for testing)
    - If this is setup right and username/password entered into config file, databases are built in to be made if they do not exist
  - Check the config file within the config folder, information will need to be entered there.

Independently start up;
 - EQOA-Prototype-Server/groundWork/startEQOA_v4.py
 - EQOA-Prototype-Server/groundWork/startEQOA_v4_2.py
 - EQOA-Prototype-Server/worldServerManager/worldServerManager.py
 - EQOA-Prototype-Server/loginserver2/LoginServer3.py
 
 "Pre-made" characters can be found and entered into the database utilizing (EQOA-Prototype-Server/groundWork/ProcessCoreIO/characterSelect.py) and uncommenting the last 3 lines. Running that file should input a few characters to the database that should line up with your account you make. Don't forget to comment those lines back out after the data has been entered. :)
