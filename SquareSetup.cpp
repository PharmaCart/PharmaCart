//
//  SquareSetup.cpp
//  Total
//
//  Created by David Mathew on 4/5/16.
//  Copyright © 2016 David Mathew. All rights reserved.
//

#include <stdio.h>
#include "SquareSetup.h"

void setDistance(beacon &b)
{
    int distance[8] = {0,1,2,3,4,3,2,1};
    for (int i = 0; i < 8; i++)
    {
        b.distanceToOthers[(b.beaconID +i) % 8] = distance[i];
    }
}


void squareSetup(beacon &b0, beacon &b1, beacon &b2, beacon &b3, beacon &b4, beacon &b5, beacon &b6, beacon &b7, parmaCart &cart) {
    
    cart.allBeacons[0] = &b0;
    cart.allBeacons[1] = &b1;
    cart.allBeacons[2] = &b2;
    cart.allBeacons[3] = &b3;
    cart.allBeacons[4] = &b4;
    cart.allBeacons[5] = &b5;
    cart.allBeacons[6] = &b6;
    cart.allBeacons[7] = &b7;
    
    //beacon ID's
    b0.beaconID = 0; b1.beaconID = 1; b2.beaconID = 2; b3.beaconID = 3; b4.beaconID = 4; b5.beaconID = 5; b6.beaconID =6; b7.beaconID = 7;
    
    //setting up map as a 8 beacon square
    b0.x = b0.y = 0;
    b1.x = 0, b1.y = 5;
    b2.x = 0, b2.y = 10;
    b3.x = 5, b3.y = 10;
    b4.x = 10, b4.y = 10;
    b5.x = 10, b5.y = 5;
    b6.x = 10, b6.y = 0;
    b7.x = 5, b7.y = 0;
    
    //setting beacon0 pointers/distance to others
    b0.northBeacon = &b1;
    b0.eastBeacon = &b7;
    b0.westBeacon = b0.southBeacon = &b0;
    setDistance(b0);
    
    //setting beacon1 pointer/distance to others
    b1.southBeacon = &b0;
    b1.northBeacon = &b2;
    b1.westBeacon = b1.eastBeacon = &b1;
    setDistance(b1);
    
    //beacon 2
    b2.southBeacon = &b1;
    b2.eastBeacon = &b3;
    b2.northBeacon = b2.westBeacon = &b2;
    setDistance(b2);
    
    //beacon 3
    b3.westBeacon = &b2;
    b3.eastBeacon = &b4;
    b3.northBeacon = b3.southBeacon = &b3;
    setDistance(b3);
    
    //beacon 4
    b4.westBeacon = &b3;
    b4.southBeacon = &b5;
    b4.northBeacon = b4.eastBeacon = &b4;
    setDistance(b4);
    
    //beacon 5
    b5.northBeacon = &b4;
    b5.southBeacon = &b6;
    b5.westBeacon = b5.eastBeacon = &b5;
    setDistance(b5);
    
    //beacon 6
    b6.northBeacon = &b5;
    b6.westBeacon = &b7;
    b6.southBeacon = b6.eastBeacon = &b6;
    setDistance(b6);
    
    //beacon 7
    b7.eastBeacon = &b6;
    b7.westBeacon = &b0;
    b7.northBeacon = b7.southBeacon = &b7;
    setDistance(b7);
    
    //Initialize Cart's Map
    cart.initializeMap();
    
    //have the cart start at beacon 0 aka the charging station
    cart.x = cart.y = 0;
    cart.currentBeacon = &b3;
    cart.updateMap();
}

