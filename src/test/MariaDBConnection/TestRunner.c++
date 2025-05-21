/*******************************************************
* Created by Cryos on 4/13/25.
* Copyright 2025 Stilt Fox® LLC
*
* See LICENSE on root project directory for terms
* of use.
********************************************************/
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <Stilt_Fox/Scribe/File.h++>

using namespace std;
using namespace nlohmann;
using namespace StiltFox::Scribe;

/***********************************************************************************************************************
 * Because the tests for MariaDB require some additional setup to run, this main function exists to provide that extra
 * support. By setting environment variables you can change how this program will link to the data.
 *
 * note: you must create a docker container. this container will not be created automatically. To help prevent system
 * command injection, this container name has been hardcoded to stilt_fox_maraiadb_test_container.
 *
 * environment variables:
 * STILT_FOX_MARIADB_TESTS_HOSTNAME: this sets the hostname for the database. It defaults to localhost.
 * STILT_FOX_MARIADB_TESTS_PORT: this sets the port number on the host that we're connecting to. It defaults to 3306.
 * STILT_FOX_MARIADB_TESTS_USER: this is the username to login as. It defaults to root.
 * STILT_FOX_MARIADB_TESTS_PASSWORD: this is the password for the root user. It defaults to password.
 * STILT_FOX_MARIADB_TESTS_CONTAINER_MANAGER: this sets the container manager command to use. Defaults to podman.
 * STILT_FOX_MARIADB_TESTS_RUN_CONTAINER: any value other than True will be interpreted as false. This setting will
 *                                        determine weather or not a container is spun up to connect to. defaults to
 *                                        false.
 **********************************************************************************************************************/
int main(int argc, char **argv)
{
    string containerManager = getenv("STILT_FOX_MARIADB_TESTS_CONTAINER_MANAGER") == "docker" ? "docker" : "podman";
    bool runContainer = getenv("STILT_FOX_MARIADB_TESTS_RUN_CONTAINER");

    if (runContainer)
    {
        system((containerManager + " start stilt_fox_maraiadb_test_container").c_str());
        sleep(3);
    }

    testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();

    if (runContainer) system((containerManager + " stop stilt_fox_maraiadb_test_container").c_str());

    return result;
}