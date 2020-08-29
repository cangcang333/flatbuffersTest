#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include "monster_generated.h"

using namespace MyGame::Sample;
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define SERV_PORT 9876

int main() {
    std::cout << "Hello, World!" << std::endl;

    flatbuffers::FlatBufferBuilder builder(1024);

    auto weapon_one_name = builder.CreateString("Sword");
    short weapon_one_damage = 3;

    auto weapon_two_name = builder.CreateString("Axe");
    short weapon_two_damage = 5;

    auto sword = MyGame::Sample::CreateWeapon(builder, weapon_one_name, weapon_one_damage);
    auto axe = MyGame::Sample::CreateWeapon(builder, weapon_two_name, weapon_two_damage);

    // Serialize a name for our monster, called "Orc".
    auto name = builder.CreateString("Orc");

    // Create a `vector` representing the inventory of the Orc. Each number
    // could correspond to an item that can be claimed after he is slain.
    unsigned char treasure[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto inventory = builder.CreateVector(treasure, 10);

    // Place the weapon into a `std::vector`, then convert that into a FlatBuffer `vector`.
    std::vector<flatbuffers::Offset<Weapon>> weapon_vector;
    weapon_vector.push_back(sword);
    weapon_vector.push_back(axe);
    auto weapons = builder.CreateVector(weapon_vector);

    Vec3 points[] = { Vec3(1.0f, 2.0f, 3.0f), Vec3(4.0f, 5.0f, 6.0f)};
    auto path = builder.CreateVectorOfStructs(points, 2);

    // Create the position struct
    auto position = Vec3(1.0f, 2.0f, 3.0f);

    // set his hit points to 300 and his mana to 150.
    int hp = 23;
    int mana = 888;

    // Finally, create the monster using the `CreateMonster` helper function
    // to set all fields.
    flatbuffers::Offset<Monster> orc = MyGame::Sample::CreateMonster(builder, &position, mana, hp, name, inventory,
                                             Color_Red, weapons, Equipment_Weapon,
                                             axe.Union(), path);

    // ***** must call the Finish() deliberately *****

    // Call `Finish()` to instruct the builder that monster is complete.
    // Note: Regardless of how you created the `orc`, you still need to call
    // `Finish()` on the `FlatBufferBuilder`.

    //builder.Finish(orc);
    // You could also call `FinishMonsterBuffer(build, orc);`
    FinishMonsterBuffer(builder, orc);

    // ***** This must be called after `Finish()`. *****
    // The buffer is now ready to be stored somewhere
    uint8_t *buf = builder.GetBufferPointer();




    // Returns the size of the buffer that `GetBufferPointer()` points to
    int size = builder.GetSize();
    std::cout << "builder get size is " << size << std::endl;



    // store 'orc' to file
    std::ofstream of;
    of.open("orc2.bin", std::ios::binary | std::ios::out);
    of.write((char *) buf, size);
    of.close();


    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        handle_error("socket");
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(SERV_PORT);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
    {
        handle_error("connect");
    }
    std::cout << "connect to server successfully\n";
    std::cout << "sizeof(orc) is " << sizeof(orc) << std::endl;

    // Using the buf and size, not the `(uint8_t *) &orc` directly, that is wrong
    int byte_sent = send(sockfd, buf, size, 0);
    std::cout << byte_sent << " bytes written to socket\n";


    return 0;
}
