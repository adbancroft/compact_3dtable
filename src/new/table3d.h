/*
This file is used for everything related to maps/tables including their definition, functions etc
*/
#ifndef TABLE_H
#define TABLE_H
#include <Arduino.h>

#define TABLE_RPM_MULTIPLIER  100
#define TABLE_LOAD_MULTIPLIER 2

//The shift amount used for the 3D table calculations
#define TABLE_SHIFT_FACTOR  8
#define TABLE_SHIFT_POWER   (1UL<<TABLE_SHIFT_FACTOR)

/*
The 2D table can contain either 8-bit (int8_t) or 16-bit (int) values
The valueSize variable should be set to either 8 or 16 to indicate this BEFORE the table is used
*/
struct table2D {
  //Used 5414 RAM with original version
  int8_t valueSize;
  int8_t axisSize;
  int8_t xSize;

  void *values;
  void *axisX;

  //int16_t *values16;
  //int16_t *axisX16;

  //Store the last X and Y coordinates in the table. This is used to make the next check faster
  int16_t lastXMax;
  int16_t lastXMin;

  //Store the last input and output for caching
  int16_t lastInput;
  int16_t lastOutput;
  int8_t cacheTime; //Tracks when the last cache value was set so it can expire after x seconds. A timeout is required to pickup when a tuning value is changed, otherwise the old cached value will continue to be returned as the X value isn't changing. 
};

int16_t table2D_getAxisValue(struct table2D*, int8_t);
int16_t table2D_getRawValue(struct table2D*, int8_t);

template <class _Ty>
struct coord2D {
  _Ty x = 0;
  _Ty y = 0;
};

struct table3D {

  table3D(int8_t size) : axisSize(size) {}
  
  int8_t axisSize;

  //Store the last X and Y coordinates in the table. This is used to make the next check faster
  byte lastXMax, lastXMin;
  byte lastYMax, lastYMin;

  //Store the last input and output values, again for caching purposes
  int16_t lastXInput, lastYInput;
  byte lastOutput; //This will need changing if we ever have 16-bit table values
  bool cacheIsValid; ///< This tracks whether the tables cache should be used. Ordinarily this is true, but is set to false whenever TunerStudio sends a new value for the table

  // These will be completely inlined.
  inline int16_t valuesSizeInBytes() const { return sq(axisSize)*sizeof(int8_t); }
  inline int16_t axisSizeInBytes() const { return axisSize*sizeof(int16_t); }

  inline int16_t* getXAxis() const { return (int16_t*)((int8_t*)this+sizeof(table3D)+valuesSizeInBytes()); }
  inline int16_t* getYAxis() const { return (int16_t*)((int8_t*)this+sizeof(table3D)+valuesSizeInBytes()+axisSizeInBytes()); }
  inline int8_t* getValues() const { return (int8_t*)this+sizeof(table3D); }

  // Derived table3D_impl will place data here
  // int8_t values[size][size]
  // int16_t _axisX[size];
  // int16_t _axisY[size];
};

// PR#520 - modified slightly
template <int8_t _Size>
struct table3D_impl: public table3D
{
public:
  table3D_impl() : table3D(_Size)
  {
  }

private:
  int8_t _values[_Size*_Size];
  int16_t _axisX[_Size];
  int16_t _axisY[_Size];
};

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
