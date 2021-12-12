//
//  dataTypes.h
//  GL threads
//
//  Created by Jean-Yves Hervé on 2021-12-07
//

#ifndef DATA_TYPES_H
#define DATA_TYPES_H


//	Travel direction data type
//	Note that if you define a variable
//	TravelDirection dir = whatever;
//	you get the opposite directions from dir as (NUM_TRAVEL_DIRECTIONS - dir)

// David comment... what the hell does this mean?

// There are 4 travel directions... lets say N = 0, W = 1, S = 2, E = 3.  
// If dir is 0 (north), (3 - 1) = 2... so that's South still... not opposite.
// I think he means NUM_TRAVEL_DIRECTIONS - 1.

// But it doesn't... here the number of travel directions is 4.  If we did
// 4-1 = 3 that is 3.  Or if we did 4-2 that equals 2 and is not th opposite direction


//	you get left turn from dir as (dir + 1) % NUM_TRAVEL_DIRECTIONS
// Okay this makes more sense

typedef enum Direction {
								NORTH = 0,
								WEST = 1,
								EAST = 3,
								SOUTH = 4,
								NUM_TRAVEL_DIRECTIONS = 4
} Direction;




#endif //	DATA_TYPES_H
