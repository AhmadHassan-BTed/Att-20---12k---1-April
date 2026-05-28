# EQOAGameServer

The /current/ overall eqoa game server, not so complex, complex.

# Authentication Server

EQOA Account server.
Utilizing TCP server in C#, able to process and interact with a few requests of the client.

-   Primarily logging in (Authentication of username/password).
-   Creating an account.
-   Changing password.
-   All other requests should be met with, "This option is disabled in order to bring you content faster. If you feel this is an error,please contact the development team for further assistance."

This is super basic at this time and just automatically processes and accepts these requests.
We have a database utilizing mariadb at this time, to be shared soon.

Functions needing to be added:

~~- Some SQL Choice (raw? orm?)~~

~~- Verifying username/password with sql.~~

~~- Authenticating old password and replacing it with new password if correct.~~

~~- Receiving create account requests, checking for username availability and writing the information to the database.~~

These have been done on a seperate version, as this interfaces with the encryption for the game, it is not currently released publicly. The server still allows for a quick login on any information at this time, though.

# ReturnHome

UDP server implementing the packet framework to handle packet requests.
This includes:

-   World Select Screen
-   Character Select Screen (Character creation and deletion)
-   Memory Dump
-   running around in the world

#_New Changes_

-   Able to now see other players running around, very rudimentary state. Little choppy but it's something to start with
-   Ingame chat, local say along with a (currently) global shout.
-   Ingame "admin" system via chat just for adjusting some character states, eventually to be used with NPC testing.
-   World fully populated with NPCs.
-   Save characters upon logging off/disconnecting.
-   Other small, minor things.

#TO-DO List

-   Scripting for NPC pathing, interactions,etc
-   Combat
-   Various player op codes for interacting. Spells, etc.
-   Begin work with Recast Tool
-   Incorporate the stat update type
-   Implement Tells/Replies
-   Look into grouping mechanisms and the group unreliable type

# Docker Quick Start

For rolling up a server quickly, locally.

Prerequisites:

-   Linux, Mac, Windows OS
-   Docker desktop must be installed
-   Port 53, 7000, 10070 and 9735 must be open for Docker to bind to
-   The `app.config` and `dnsmasq.conf` file must be modified with your local server's ip address.

Steps:
Run the following command (`docker-compose up -d`) in the project's directory to spin up a cluster in detached mode with the following containers

-   returnhome server
-   authentication server
-   dns service (dnsmasq)
-   http server
-   mariadb database

You can omit any of the following services above by commenting out their corresponding service stanza in the `docker-compose.yml` file.

# Docker Troubleshooting

If you are running into any issues with your Docker setup, and want to start from scratch, here are some instructions for doing so, and some common commands for managing the compose cluster.

-   Shutting down your Docker Compose cluster : `docker-compose down`
-   Rebuilding your local images (if a file change isn't being picked up) : `docker-compose build`
-   Restarting a specific service in Docker Compose : `docker-compose restart <service name>`
-   See the running status of all of your services : `docker-compose ps`
-   Viewing all container logs: `docker-compose logs`
-   Viewing process specific logs and following output: `docker-compose logs -f <service-name>` 
-   Kill a single service : Collect the ID using `docker ps`, run `docker kill <id>`
-   Removing all local images from your machine (fresh start, make sure you have composed down) : `docker system prune -af`
-   Remove all docker volumes (good idea if you are having issues with a data volume) : `docker volume prune -f`

# Set up

Client setup can be found here: http://wiki.eqoarevival.com/index.php/Client_Setup

For a windows environment, a DNS redirect can be found here: http://wiki.eqoarevival.com/index.php/Server_Setup_Windows#DNS_Server

For a windows environment, A HTTP Server can be found here: http://wiki.eqoarevival.com/index.php/Server_Setup_Windows#HTTP_Server

For Linux environments, A HTTP Server can be used through python's httpserve. Apache can also be used via a more traditional web server setup and seems to be more stable.

Downloading and using Visual studio's, you need to clone the repo. Example of how to do this should be found here: https://docs.microsoft.com/en-us/visualstudio/get-started/tutorial-open-project-from-repo?view=vs-2019
Upon cloning the repo, you should find a config file here -> EQOAGameServer\EQOA_Proto_C-sharp\EQOAProto-C-Sharp\app.config
Modify the config file to have the appropriate IP's for your machine hosting the game server and the database.

Once completed, opening two seperate instances of the repo in Visual Studio's, select and run both solution files from the drop down box.

-   Database to be added with a "trial" account
