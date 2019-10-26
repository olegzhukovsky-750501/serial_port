#ifndef PACKET_H
#define PACKET_H

struct Packet
{
    char flag = 0b00001110;
    unsigned char destination_adr;
    unsigned char source_adr;
    char data[7];
    char fcs;
};

#endif // PACKET_H
