//
//  PharmaCart_Server.h
//  
//
//  Created by Forrest Lee on 2/19/16.
//
//

#ifndef PharmaCart_Server_h
#define PharmaCart_Server_h
#include <stdio.h>


char Map[11][11];


int getX(int beacon_id)
{
    switch(beacon_id)
    {
        case 0:
        case 1:
        case 2:
            return 0;
            break;
        case 3:
        case 7:
            return 5;
            break;
        case 4:
        case 5:
        case 6:
            return 10;
            break;
    }
    return 0;
}
int getY(int beacon_id)
{
    switch(beacon_id)
    {
        case 0:
        case 7:
        case 6:
            return 0;
            break;
        case 1:
        case 5:
            return 5;
            break;
        case 2:
        case 3:
        case 4:
            return 10;
            break;
    }
    return 0;
}

void initializeMap() {
    int i;
    for (i = 0; i < 11; i++)
    {
        int j;
        for (j = 0; j < 11; j++)
        {
            Map[i][j] = '-';
        }
    }
    
    
    for (i = 0; i < 8; i++)
    {
        char number[3];
        sprintf(number, "%d", i);
        Map[10 - getY(i)][getX(i)] = number[0];
    }
    
}

//will need to be updated for multiple robots
void updateMap(int beacon_id) {
    initializeMap();
    
    Map[10 - getY(beacon_id)][getX(beacon_id)] = 'P';
}


char * clear_string(char str[], int strlen)
{
    int i;
    for (i = 0; i < strlen; i++)
        str[i] = '\0';
    return str;
}


struct bot_info
{
    int vacant;
    char ip[15];
    int last_location_x;
    int last_location_y;
    int inventory_a;
    int inventory_b;
};

struct command_str
{
    char txt[256];
};


#endif /* PharmaCart_Server_h */
