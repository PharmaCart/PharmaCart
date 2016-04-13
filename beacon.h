//
//  beacon.h
//  Total
//
//  Created by David Mathew on 4/5/16.
//  Copyright © 2016 David Mathew. All rights reserved.
//

#ifndef beacon_h
#define beacon_h

typedef class beacon beacon;

class beacon {
    
public:
    
    int x,y; //location
    int beaconID; //identifier
    
    //each beacon has knowledge of adjacent beacons
    beacon *westBeacon;
    beacon *eastBeacon;
    beacon *northBeacon;
    beacon *southBeacon;
    
    //each beacon knows its distance to all others
    int distanceToOthers[13];
};



#endif /* beacon_h */
