#include <Arduino.h>

<<<<<<< HEAD:src/new/table3d.hpp
//The shift amount used for the 3D table calculations
#define TABLE_SHIFT_FACTOR  8
#define TABLE_SHIFT_POWER   (1UL<<TABLE_SHIFT_FACTOR)

  //This function pulls a value from a 3D table given a target for X and Y coordinates.
  //It performs a 2D linear interpolation as descibred in: www.megamanual.com/v22manual/ve_tuner.pdf
  int8_t get3DTableValue(int16_t Y_in, int16_t X_in, const int8_t *values, int8_t yDim, int8_t xDim, int16_t *ppAxisX, int16_t *pAsisY)
{
    int16_t X = X_in;
    int16_t Y = Y_in;
=======
#include "table3d.h"


//This function pulls a value from a 3D table given a target for X and Y coordinates.
//It performs a 2D linear interpolation as descibred in: www.megamanual.com/v22manual/ve_tuner.pdf
int get3DTableValue(struct table3D *fromTable, int Y_in, int X_in)
  {
    int X = X_in;
    int Y = Y_in;
>>>>>>> 840ba6c (Back to basics):src/new/table.hpp

    int tableResult = 0;
    //Loop through the X axis bins for the min/max pair
    //Note: For the X axis specifically, rather than looping from tablepAxisX[0] up to tablepAxisX[max], we start at tablepAxisX[Max] and go down.
    //      This is because the important tables (fuel and injection) will have the highest RPM at the top of the X axis, so starting there will mean the best case occurs when the RPM is highest (And hence the CPU is needed most)
<<<<<<< HEAD:src/new/table3d.hpp
    int16_t xMinValue = pAxisX[0];
    int16_t xMaxValue = pAxisX[xDim-1];
    int8_t xMin = 0;
    int8_t xMax = 0;
=======
    const int16_t* pXAxis = fromTable->getXAxis();
    int xMinValue = pXAxis[0];
    int xMaxValue = pXAxis[fromTable->axisSize-1];
    byte xMin = 0;
    byte xMax = 0;
>>>>>>> 840ba6c (Back to basics):src/new/table.hpp

    //If the requested X value is greater/small than the maximum/minimum bin, reset X to be that value
    if(X > xMaxValue) { X = xMaxValue; }
    if(X < xMinValue) { X = xMinValue; }

    //0th check is whether the same X and Y values are being sent as last time. If they are, this not only prevents a lookup of the axis, but prevents the interpolation calcs being performed
    if( (X_in == lastXInput) && (Y_in == lastYInput) && (cacheIsValid == true))
    {
      return lastOutput;
    }

    //Commence the lookups on the X and Y axis

    //1st check is whether we're still in the same X bin as last time
<<<<<<< HEAD:src/new/table3d.hpp
    if ( (X <= pAxisX[lastXMax]) && (X > pAxisX[lastXMin]) )
    {
      xMaxValue = pAxisX[lastXMax];
      xMinValue = pAxisX[lastXMin];
      xMax = lastXMax;
      xMin = lastXMin;
    }
    //2nd check is whether we're in the next RPM bin (To the right)
    else if ( ((lastXMax + 1) < xDim ) && (X <= pAxisX[lastXMax +1 ]) && (X > pAxisX[lastXMin + 1]) ) //First make sure we're not already at the last X bin
    {
      xMax = lastXMax + 1;
      lastXMax = xMax;
      xMin = lastXMin + 1;
      lastXMin = xMin;
      xMaxValue = pAxisX[lastXMax];
      xMinValue = pAxisX[lastXMin];
    }
    //3rd check is to look at the previous bin (to the left)
    else if ( (lastXMin > 0 ) && (X <= pAxisX[lastXMax - 1]) && (X > pAxisX[lastXMin - 1]) ) //First make sure we're not already at the first X bin
    {
      xMax = lastXMax - 1;
      lastXMax = xMax;
      xMin = lastXMin - 1;
      lastXMin = xMin;
      xMaxValue = pAxisX[lastXMax];
      xMinValue = pAxisX[lastXMin];
=======
    if ( (X <= pXAxis[fromTable->lastXMax]) && (X > pXAxis[fromTable->lastXMin]) )
    {
      xMaxValue = pXAxis[fromTable->lastXMax];
      xMinValue = pXAxis[fromTable->lastXMin];
      xMax = fromTable->lastXMax;
      xMin = fromTable->lastXMin;
    }
    //2nd check is whether we're in the next RPM bin (To the right)
    else if ( ((fromTable->lastXMax + 1) < fromTable->axisSize ) && (X <= pXAxis[fromTable->lastXMax +1 ]) && (X > pXAxis[fromTable->lastXMin + 1]) ) //First make sure we're not already at the last X bin
    {
      xMax = fromTable->lastXMax + 1;
      fromTable->lastXMax = xMax;
      xMin = fromTable->lastXMin + 1;
      fromTable->lastXMin = xMin;
      xMaxValue = pXAxis[fromTable->lastXMax];
      xMinValue = pXAxis[fromTable->lastXMin];
    }
    //3rd check is to look at the previous bin (to the left)
    else if ( (fromTable->lastXMin > 0 ) && (X <= pXAxis[fromTable->lastXMax - 1]) && (X > pXAxis[fromTable->lastXMin - 1]) ) //First make sure we're not already at the first X bin
    {
      xMax = fromTable->lastXMax - 1;
      fromTable->lastXMax = xMax;
      xMin = fromTable->lastXMin - 1;
      fromTable->lastXMin = xMin;
      xMaxValue = pXAxis[fromTable->lastXMax];
      xMinValue = pXAxis[fromTable->lastXMin];
>>>>>>> 840ba6c (Back to basics):src/new/table.hpp
    }
    else
    //If it's not caught by one of the above scenarios, give up and just run the loop
    {
      for (int8_t x = xDim-1; x >= 0; x--)
      {
        //Checks the case where the X value is exactly what was requested
        if ( (X == pXAxis[x]) || (x == 0) )
        {
          xMaxValue = pXAxis[x];
          xMinValue = pXAxis[x];
          xMax = x;
          lastXMax = xMax;
          xMin = x;
          lastXMin = xMin;
          break;
        }
        //Normal case
        if ( (X <= pXAxis[x]) && (X > pXAxis[x-1]) )
        {
          xMaxValue = pXAxis[x];
          xMinValue = pXAxis[x-1];
          xMax = x;
          lastXMax = xMax;
          xMin = x-1;
          lastXMin = xMin;
          break;
        }
      }
    }

    //Loop through the Y axis bins for the min/max pair
<<<<<<< HEAD:src/new/table3d.hpp
    int16_t yMaxValue = pAxisY[0];
    int16_t yMinValue = pAxisY[yDim-1];
    int8_t yMin = 0;
    int8_t yMax = 0;
=======
    const int16_t* pYAxis = fromTable->getYAxis();
    int yMaxValue = pYAxis[0];
    int yMinValue = pYAxis[fromTable->axisSize-1];
    byte yMin = 0;
    byte yMax = 0;
>>>>>>> 840ba6c (Back to basics):src/new/table.hpp

    //If the requested Y value is greater/small than the maximum/minimum bin, reset Y to be that value
    if(Y > yMaxValue) { Y = yMaxValue; }
    if(Y < yMinValue) { Y = yMinValue; }

    //1st check is whether we're still in the same Y bin as last time
<<<<<<< HEAD:src/new/table3d.hpp
    if ( (Y >= pAxisY[lastYMax]) && (Y < pAxisY[lastYMin]) )
    {
      yMaxValue = pAxisY[lastYMax];
      yMinValue = pAxisY[lastYMin];
      yMax = lastYMax;
      yMin = lastYMin;
    }
    //2nd check is whether we're in the next MAP/TPS bin (Next one up)
    else if ( (lastYMin > 0 ) && (Y <= pAxisY[lastYMin - 1 ]) && (Y > pAxisY[lastYMax - 1]) ) //First make sure we're not already at the top Y bin
    {
      yMax = lastYMax - 1;
      lastYMax = yMax;
      yMin = lastYMin - 1;
      lastYMin = yMin;
      yMaxValue = pAxisY[lastYMax];
      yMinValue = pAxisY[lastYMin];
    }
    //3rd check is to look at the previous bin (Next one down)
    else if ( ((lastYMax + 1) < yDim) && (Y <= pAxisY[lastYMin + 1]) && (Y > pAxisY[lastYMax + 1]) ) //First make sure we're not already at the bottom Y bin
    {
      yMax = lastYMax + 1;
      lastYMax = yMax;
      yMin = lastYMin + 1;
      lastYMin = yMin;
      yMaxValue = pAxisY[lastYMax];
      yMinValue = pAxisY[lastYMin];
=======
    if ( (Y >= pYAxis[fromTable->lastYMax]) && (Y < pYAxis[fromTable->lastYMin]) )
    {
      yMaxValue = pYAxis[fromTable->lastYMax];
      yMinValue = pYAxis[fromTable->lastYMin];
      yMax = fromTable->lastYMax;
      yMin = fromTable->lastYMin;
    }
    //2nd check is whether we're in the next MAP/TPS bin (Next one up)
    else if ( (fromTable->lastYMin > 0 ) && (Y <= pYAxis[fromTable->lastYMin - 1 ]) && (Y > pYAxis[fromTable->lastYMax - 1]) ) //First make sure we're not already at the top Y bin
    {
      yMax = fromTable->lastYMax - 1;
      fromTable->lastYMax = yMax;
      yMin = fromTable->lastYMin - 1;
      fromTable->lastYMin = yMin;
      yMaxValue = pYAxis[fromTable->lastYMax];
      yMinValue = pYAxis[fromTable->lastYMin];
    }
    //3rd check is to look at the previous bin (Next one down)
    else if ( ((fromTable->lastYMax + 1) < fromTable->axisSize) && (Y <= pYAxis[fromTable->lastYMin + 1]) && (Y > pYAxis[fromTable->lastYMax + 1]) ) //First make sure we're not already at the bottom Y bin
    {
      yMax = fromTable->lastYMax + 1;
      fromTable->lastYMax = yMax;
      yMin = fromTable->lastYMin + 1;
      fromTable->lastYMin = yMin;
      yMaxValue = pYAxis[fromTable->lastYMax];
      yMinValue = pYAxis[fromTable->lastYMin];
>>>>>>> 840ba6c (Back to basics):src/new/table.hpp
    }
    else
    //If it's not caught by one of the above scenarios, give up and just run the loop
    {

      for (int8_t y = yDim-1; y >= 0; y--)
      {
        //Checks the case where the Y value is exactly what was requested
        if ( (Y == pYAxis[y]) || (y==0) )
        {
          yMaxValue = pYAxis[y];
          yMinValue = pYAxis[y];
          yMax = y;
          lastYMax = yMax;
          yMin = y;
          lastYMin = yMin;
          break;
        }
        //Normal case
        if ( (Y >= pYAxis[y]) && (Y < pYAxis[y-1]) )
        {
          yMaxValue = pYAxis[y];
          yMinValue = pYAxis[y-1];
          yMax = y;
          lastYMax = yMax;
          yMin = y-1;
          lastYMin = yMin;
          break;
        }
      }
    }


    /*
    At this point we have the 4 corners of the map where the interpolated value will fall in
    Eg: (yMin,xMin)  (yMin,xMax)

        (yMax,xMin)  (yMax,xMax)

    In the following calculation the table values are referred to by the following variables:
              A          B

              C          D

    */
<<<<<<< HEAD:src/new/table3d.hpp
    int A = values[yMin][xMin];
    int B = values[yMin][xMax];
    int C = values[yMax][xMin];
    int D = values[yMax][xMax];
=======
    const int8_t* pValues = fromTable->getValues() ;
    int A = pValues[yMin * fromTable->axisSize + xMin];
    int B = pValues[yMin * fromTable->axisSize + xMax];
    int C = pValues[yMax * fromTable->axisSize + xMin];
    int D = pValues[yMax * fromTable->axisSize + xMax];
>>>>>>> 840ba6c (Back to basics):src/new/table.hpp

    //Check that all values aren't just the same (This regularly happens with things like the fuel trim maps)
    if( (A == B) && (A == C) && (A == D) ) { tableResult = A; }
    else
    {
      //Create some normalised position values
      //These are essentially percentages (between 0 and 1) of where the desired value falls between the nearest bins on each axis


      //Initial check incase the values were hit straight on

      unsigned long p = (long)X - xMinValue;
      if (xMaxValue == xMinValue) { p = (p << TABLE_SHIFT_FACTOR); }  //This only occurs if the requested X value was equal to one of the X axis bins
      else { p = ( (p << TABLE_SHIFT_FACTOR) / (xMaxValue - xMinValue) ); } //This is the standard case

      unsigned long q;
      if (yMaxValue == yMinValue)
      {
        q = (long)Y - yMinValue;
        q = (q << TABLE_SHIFT_FACTOR);
      }
      //Standard case
      else
      {
        q = long(Y) - yMaxValue;
        q = TABLE_SHIFT_POWER - ( (q << TABLE_SHIFT_FACTOR) / (yMinValue - yMaxValue) );
      }

      uint32_t m = ((TABLE_SHIFT_POWER-p) * (TABLE_SHIFT_POWER-q)) >> TABLE_SHIFT_FACTOR;
      uint32_t n = (p * (TABLE_SHIFT_POWER-q)) >> TABLE_SHIFT_FACTOR;
      uint32_t o = ((TABLE_SHIFT_POWER-p) * q) >> TABLE_SHIFT_FACTOR;
      uint32_t r = (p * q) >> TABLE_SHIFT_FACTOR;
      tableResult = ( (A * m) + (B * n) + (C * o) + (D * r) ) >> TABLE_SHIFT_FACTOR;
    }

    //Update the tables cache data
    lastXInput = X_in;
    lastYInput = Y_in;
    lastOutput = tableResult;
    cacheIsValid = true;

    return tableResult;
  }
  