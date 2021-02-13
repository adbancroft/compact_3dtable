/*
This file is used for everything related to maps/tables including their definition, functions etc
*/
#ifndef TABLE_H
#define TABLE_H

#define TABLE_RPM_MULTIPLIER  100
#define TABLE_LOAD_MULTIPLIER 2

//The shift amount used for the 3D table calculations
#define TABLE_SHIFT_FACTOR  8
#define TABLE_SHIFT_POWER   (1UL<<TABLE_SHIFT_FACTOR)

//Define the total table memory sizes. Used for adding up the static heap size
#define TABLE3D_SIZE_16  (16 * 16 + 32 + 32 + 32) //2 bytes for each value on the axis + allocation for array pointers
#define TABLE3D_SIZE_12  (12 * 12 + 24 + 24 + 24) //2 bytes for each value on the axis + allocation for array pointers
#define TABLE3D_SIZE_8   (8 * 8 + 16 + 16 + 16) //2 bytes for each value on the axis + allocation for array pointers
#define TABLE3D_SIZE_6   (6 * 6 + 12 + 12 + 12) //2 bytes for each value on the axis + allocation for array pointers
#define TABLE3D_SIZE_4   (4 * 4 + 8 + 8 + 8) //2 bytes for each value on the axis + allocation for array pointers

//Define the table sizes
#define TABLE_FUEL1_SIZE    16;
#define TABLE_FUEL2_SIZE    16;
#define TABLE_IGN1_SIZE     16;
#define TABLE_IGN2_SIZE     16;
#define TABLE_AFR_SIZE      16;
#define TABLE_STAGING_SIZE  8;
#define TABLE_BOOST_SIZE    8;
#define TABLE_VVT1_SIZE     8;
#define TABLE_WMI_SIZE      8;
#define TABLE_TRIM1_SIZE    6;
#define TABLE_TRIM2_SIZE    6;
#define TABLE_TRIM3_SIZE    6;
#define TABLE_TRIM4_SIZE    6;

/*
*********** WARNING! ***********
YOU MUST UPDATE THE TABLE COUNTS IN THE LINE BELOW WHENEVER A NEW TABLE IS ADDED!
*/
#define TABLE_HEAP_SIZE     (5 * TABLE3D_SIZE_16) + (4 * TABLE3D_SIZE_8) + (4 * TABLE3D_SIZE_6)+1

static uint8_t _3DTable_heap[TABLE_HEAP_SIZE];
static uint16_t _heap_pointer = 0;

struct table3D {

  //All tables must be the same size for simplicity

  byte xSize;
  byte ySize;

  byte **values;
  int16_t *axisX;
  int16_t *axisY;

  //Store the last X and Y coordinates in the table. This is used to make the next check faster
  byte lastXMax, lastXMin;
  byte lastYMax, lastYMin;

  //Store the last input and output values, again for caching purposes
  int16_t lastXInput, lastYInput;
  byte lastOutput; //This will need changing if we ever have 16-bit table values
  bool cacheIsValid; ///< This tracks whether the tables cache should be used. Ordinarily this is true, but is set to false whenever TunerStudio sends a new value for the table
};

//void table3D_setSize(struct table3D *targetTable, byte);
void table3D_setSize(struct table3D *targetTable, byte);

/*
3D Tables have an origin (0,0) in the top left hand corner. Vertical axis is expressed first.
Eg: 2x2 table
-----
|2 7|
|1 4|
-----

(0,1) = 7
(0,0) = 2
(1,0) = 1

*/
int get3DTableValue(struct table3D *fromTable, int, int);

#endif // TABLE_H
