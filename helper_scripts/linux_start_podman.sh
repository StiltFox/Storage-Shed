#!/bin/bash

# this script is designed to startup podman on a systemd linux box
# and alias the socket so that it works with standard docker commands
# from jetbrains products.

#systemctl start podman
#chmod -R 755 /run/podman
#ln -s /run/podman/podman.sock /run/docker.sock

docker run -p 127.0.0.1:3306:3306 --env MARIADB_ALLOW_EMPTY_ROOT_PASSWORD=true --name maraidb_container --pull missing mariadb:latest 