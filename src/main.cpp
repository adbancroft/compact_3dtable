#include <Arduino.h>
#define _countof(array) (sizeof(array) / sizeof(array[0]))

// This will move the index around the range, so we do NOT smoothly
// loop from min to max-1. Avoid cache effects
#define LOOP_INDEXER(index, max) (index %2==0 ? index : (max-index)%max)

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

#define TEST_BASELINE 0
#define TEST_ORIGINAL 1
#define TEST_COMPACT 2
#define TEST_CASE TEST_COMPACT

#if TEST_CASE==TEST_COMPACT
#include "new\table3d.h"
//#include "new\table3d.hpp"

table3D<int8_t, 16, 16> fuelTable;
table3D<int8_t, 16, 16> fuelTable2;
table3D<int8_t, 16, 16> ignitionTable;
table3D<int8_t, 16, 16> ignitionTable2;
table3D<int8_t, 16, 16> afrTable;
table3D<int8_t, 8, 8> stagingTable;
table3D<int8_t, 8, 8> boostTable;
table3D<int8_t, 8, 8> vvtTable;
table3D<int8_t, 8, 8> wmiTable;
table3D<int8_t, 6, 6> trim1Table;
table3D<int8_t, 6, 6> trim2Table;
table3D<int8_t, 6, 6> trim3Table;
table3D<int8_t, 6, 6> trim4Table;

template <class _Ty, int8_t _xDim, int8_t _yDim>
void setupTable(table3D<_Ty, _xDim, _yDim> *pTable, int8_t size, const int8_t *pValues, const int16_t *pXAxis, const int16_t *pYAxis)
{
  pTable->setXAxis(pXAxis, _xDim);
  pTable->setYAxis(pYAxis, _yDim);
  pTable->setValues(pValues);  
}

template <class _Ty, int8_t _xDim, int8_t _yDim>
long testHarnessGet3dTableValue(table3D<_Ty, _xDim, _yDim> *pTable, int16_t xValue, int16_t yValue)
{
  return pTable->get3DTableValue(yValue, xValue);
}

#elif TEST_CASE==TEST_ORIGINAL
#include "src\table.h"
#include "src\table.hpp"

struct table3D fuelTable; //16x16 fuel map
struct table3D fuelTable2; //16x16 fuel map
struct table3D ignitionTable; //16x16 ignition map
struct table3D ignitionTable2; //16x16 ignition map
struct table3D afrTable; //16x16 afr target map
struct table3D stagingTable; //8x8 fuel staging table
struct table3D boostTable; //8x8 boost map
struct table3D vvtTable; //8x8 vvt map
struct table3D wmiTable; //8x8 wmi map
struct table3D trim1Table; //6x6 Fuel trim 1 map
struct table3D trim2Table; //6x6 Fuel trim 2 map
struct table3D trim3Table; //6x6 Fuel trim 3 map
struct table3D trim4Table; //6x6 Fuel trim 4 map

void setupTable(table3D *pTable, int8_t size, const int8_t *pValues, const int16_t *pXAxis, const int16_t *pYAxis)
{
  table3D_setSize(pTable, size);

  memcpy(pTable->axisX, pXAxis, size * sizeof(int16_t));
  memcpy(pTable->axisY, pYAxis, size * sizeof(int16_t));

  for (uint8_t loop=0; loop<size; loop++)
  {
    memcpy(pTable->values[loop], &pValues[loop], size * sizeof(pValues[0]));
  }
}

long testHarnessGet3dTableValue(table3D *pTable, int16_t xValue, int16_t yValue)
{
  return get3DTableValue(pTable, yValue, xValue);
}

#else
// Dummy up vars & functions
int fuelTable;
int fuelTable2;
int ignitionTable;
int ignitionTable2;
int afrTable;
int stagingTable;
int boostTable;
int vvtTable;
int wmiTable;
int trim1Table;
int trim2Table;
int trim3Table;
int trim4Table;


void setupTable(int *pTable, int8_t size, const int8_t *pValues, const int16_t *pXAxis, const int16_t *pYAxis)
{
  ; // no op
}

long testHarnessGet3dTableValue(int *pTable, int16_t xValue, int16_t yValue)
{
  return 1L; // no op
}

#endif

int16_t xAxis[16] = { 700, 900, 1300, 1800, 2400, 2600, 3000, 3500, 4000, 4500, 5000, 5400, 5800, 6300, 7000, 7500 };
int16_t yAxis[16] = { 100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25 };

template <class _Ty>
long testTable(_Ty *pTable, int8_t xDim, int8_t yDim)
{
  long sum = 0;

  for (int16_t loop = 0; loop<100; loop++)
  {
    for (int8_t loopX = 0; loopX<xDim; loopX++)
    {
      for (int8_t loopY = 0; loopY<yDim; loopY++)
      {       
        sum = sum + testHarnessGet3dTableValue(pTable, yAxis[LOOP_INDEXER(loopY, 16)],  xAxis[LOOP_INDEXER(loopX, 16)]);
      }
    }
  }

  return sum;
}

void setup() {
  Serial.begin(9600);

  int8_t values[16][16] = {
    { 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 },
    { 29, 30, 30, 32, 33, 34, 35, 35, 36, 36, 36, 35, 35, 35, 34, 34 },
    { 30, 30, 32, 34, 36, 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38 },
    { 30, 32, 34, 37, 39, 40, 41, 41, 41, 41, 41, 41, 41, 40, 40, 40 },
    { 31, 34, 37, 40, 42, 42, 43, 44, 44, 44, 44, 44, 43, 43, 42, 42 },
    { 33, 37, 40, 43, 45, 46, 46, 47, 47, 47, 47, 46, 46, 46, 46, 46 },
    { 36, 40, 43, 46, 47, 48, 49, 50, 50, 50, 49, 49, 49, 48, 48, 47 },
    { 39, 44, 47, 49, 50, 51, 52, 52, 52, 52, 52, 52, 51, 51, 50, 50 },
    { 42, 47, 50, 52, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 53 },
    { 46, 50, 53, 54, 55, 56, 57, 58, 58, 58, 58, 57, 57, 56, 56, 55 },
    { 50, 54, 55, 58, 58, 59, 60, 61, 61, 61, 60, 60, 60, 59, 58, 58 },
    { 54, 57, 58, 61, 62, 62, 62, 62, 63, 62, 62, 62, 62, 62, 62, 61 },
    { 57, 60, 62, 62, 64, 65, 66, 66, 66, 66, 66, 65, 65, 64, 64, 63 },
    { 60, 62, 64, 66, 66, 67, 68, 69, 69, 69, 69, 68, 68, 67, 66, 66 },
    { 63, 66, 67, 69, 69, 70, 70, 71, 71, 71, 70, 70, 70, 69, 69, 69 },
    { 66, 69, 70, 70, 72, 73, 74, 74, 74, 74, 74, 74, 73, 72, 71, 71 },  
  };

  setupTable(&fuelTable, 16, &values[0][0], xAxis, yAxis);
  setupTable(&fuelTable2, 16, &values[0][0], xAxis, yAxis);
  setupTable(&ignitionTable, 16, &values[0][0], xAxis, yAxis);
  setupTable(&ignitionTable2, 16, &values[0][0], xAxis, yAxis);
  setupTable(&afrTable, 16, &values[0][0], xAxis, yAxis);
  setupTable(&stagingTable, 8, &values[0][0], xAxis, yAxis);
  setupTable(&boostTable, 8, &values[0][0], xAxis, yAxis);
  setupTable(&vvtTable, 8, &values[0][0], xAxis, yAxis);
  setupTable(&wmiTable, 8, &values[0][0], xAxis, yAxis);
  setupTable(&trim1Table, 6, &values[0][0], xAxis, yAxis);
  setupTable(&trim2Table, 6, &values[0][0], xAxis, yAxis);
  setupTable(&trim3Table, 6, &values[0][0], xAxis, yAxis);
  setupTable(&trim4Table, 6, &values[0][0], xAxis, yAxis);

  unsigned long StartTime = millis();

  int32_t sum = 0;
  sum += testTable(&fuelTable, 16, 16);
  sum += testTable(&stagingTable, 8, 8);
  sum += testTable(&trim1Table, 6, 6);

  unsigned long CurrentTime = millis();	
  unsigned long ElapsedTime = CurrentTime - StartTime;
  Serial.println(sum);
  Serial.println(ElapsedTime);
  Serial.println(freeRam()); 
}

void loop() {
}