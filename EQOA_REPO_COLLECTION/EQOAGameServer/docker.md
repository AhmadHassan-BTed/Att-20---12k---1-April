# Docker Quick Start

Prerequisites:

-   Docker desktop must be installed
-   Port 53, 7000, 10070, 3306 and 9735 must be open for Docker to bind to
-   The `app.config` and `dnsmasq.conf` file must be modified with your local server's ip address.

Run the following (`docker-compose up -d`) in the project's directory to spin up a cluster with the following containers

-   returnhome server
-   authentication server
-   dns service (dnsmasq)
-   http server
-   mariadb database
