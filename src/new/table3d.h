#pragma once
#include <Arduino.h>

template <class _Ty, int8_t _xDim, int8_t _yDim>
struct table3D
{
  using axis_type = int16_t;
  
  _Ty values[_yDim][_xDim];
  axis_type axisX[_xDim];
  axis_type axisY[_yDim];

  table3D() = default;
  table3D(const table3D &) = delete;
  
  void setYAxis(const axis_type *pValues, int8_t len)
  {
    memcpy(axisY, pValues, min(_yDim, len)*sizeof(axis_type));
  }
  void setXAxis(const axis_type *pValues, int8_t len)
  {
    memcpy(axisX, pValues, min(_xDim, len)*sizeof(axis_type));
  }
  void setValues(const _Ty *pValues)
  {
    memcpy(values, pValues, _xDim*_yDim*sizeof(_Ty));
  }

  //This function pulls a value from a 3D table given a target for X and Y coordinates.
  //It performs a 2D linear interpolation as descibred in: www.megamanual.com/v22manual/ve_tuner.pdf
  _Ty get3DTableValue(axis_type Y_in, axis_type X_in)
{
    axis_type X = X_in;
    axis_type Y = Y_in;

    int tableResult = 0;
    //Loop through the X axis bins for the min/max pair
    //Note: For the X axis specifically, rather than looping from tableAxisX[0] up to tableAxisX[max], we start at tableAxisX[Max] and go down.
    //      This is because the important tables (fuel and injection) will have the highest RPM at the top of the X axis, so starting there will mean the best case occurs when the RPM is highest (And hence the CPU is needed most)
    axis_type xMinValue = axisX[0];
    axis_type xMaxValue = axisX[_xDim-1];
    int8_t xMin = 0;
    int8_t xMax = 0;

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
    if ( (X <= axisX[lastXMax]) && (X > axisX[lastXMin]) )
    {
      xMaxValue = axisX[lastXMax];
      xMinValue = axisX[lastXMin];
      xMax = lastXMax;
      xMin = lastXMin;
    }
    //2nd check is whether we're in the next RPM bin (To the right)
    else if ( ((lastXMax + 1) < _xDim ) && (X <= axisX[lastXMax +1 ]) && (X > axisX[lastXMin + 1]) ) //First make sure we're not already at the last X bin
    {
      xMax = lastXMax + 1;
      lastXMax = xMax;
      xMin = lastXMin + 1;
      lastXMin = xMin;
      xMaxValue = axisX[lastXMax];
      xMinValue = axisX[lastXMin];
    }
    //3rd check is to look at the previous bin (to the left)
    else if ( (lastXMin > 0 ) && (X <= axisX[lastXMax - 1]) && (X > axisX[lastXMin - 1]) ) //First make sure we're not already at the first X bin
    {
      xMax = lastXMax - 1;
      lastXMax = xMax;
      xMin = lastXMin - 1;
      lastXMin = xMin;
      xMaxValue = axisX[lastXMax];
      xMinValue = axisX[lastXMin];
    }
    else
    //If it's not caught by one of the above scenarios, give up and just run the loop
    {
      for (int8_t x = _xDim-1; x >= 0; x--)
      {
        //Checks the case where the X value is exactly what was requested
        if ( (X == axisX[x]) || (x == 0) )
        {
          xMaxValue = axisX[x];
          xMinValue = axisX[x];
          xMax = x;
          lastXMax = xMax;
          xMin = x;
          lastXMin = xMin;
          break;
        }
        //Normal case
        if ( (X <= axisX[x]) && (X > axisX[x-1]) )
        {
          xMaxValue = axisX[x];
          xMinValue = axisX[x-1];
          xMax = x;
          lastXMax = xMax;
          xMin = x-1;
          lastXMin = xMin;
          break;
        }
      }
    }

    //Loop through the Y axis bins for the min/max pair
    axis_type yMaxValue = axisY[0];
    axis_type yMinValue = axisY[_yDim-1];
    _Ty yMin = 0;
    _Ty yMax = 0;

    //If the requested Y value is greater/small than the maximum/minimum bin, reset Y to be that value
    if(Y > yMaxValue) { Y = yMaxValue; }
    if(Y < yMinValue) { Y = yMinValue; }

    //1st check is whether we're still in the same Y bin as last time
    if ( (Y >= axisY[lastYMax]) && (Y < axisY[lastYMin]) )
    {
      yMaxValue = axisY[lastYMax];
      yMinValue = axisY[lastYMin];
      yMax = lastYMax;
      yMin = lastYMin;
    }
    //2nd check is whether we're in the next MAP/TPS bin (Next one up)
    else if ( (lastYMin > 0 ) && (Y <= axisY[lastYMin - 1 ]) && (Y > axisY[lastYMax - 1]) ) //First make sure we're not already at the top Y bin
    {
      yMax = lastYMax - 1;
      lastYMax = yMax;
      yMin = lastYMin - 1;
      lastYMin = yMin;
      yMaxValue = axisY[lastYMax];
      yMinValue = axisY[lastYMin];
    }
    //3rd check is to look at the previous bin (Next one down)
    else if ( ((lastYMax + 1) < _yDim) && (Y <= axisY[lastYMin + 1]) && (Y > axisY[lastYMax + 1]) ) //First make sure we're not already at the bottom Y bin
    {
      yMax = lastYMax + 1;
      lastYMax = yMax;
      yMin = lastYMin + 1;
      lastYMin = yMin;
      yMaxValue = axisY[lastYMax];
      yMinValue = axisY[lastYMin];
    }
    else
    //If it's not caught by one of the above scenarios, give up and just run the loop
    {

      for (int8_t y = _yDim-1; y >= 0; y--)
      {
        //Checks the case where the Y value is exactly what was requested
        if ( (Y == axisY[y]) || (y==0) )
        {
          yMaxValue = axisY[y];
          yMinValue = axisY[y];
          yMax = y;
          lastYMax = yMax;
          yMin = y;
          lastYMin = yMin;
          break;
        }
        //Normal case
        if ( (Y >= axisY[y]) && (Y < axisY[y-1]) )
        {
          yMaxValue = axisY[y];
          yMinValue = axisY[y-1];
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
    int A = values[yMin][xMin];
    int B = values[yMin][xMax];
    int C = values[yMax][xMin];
    int D = values[yMax][xMax];

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
  
private:
  static const long TABLE_SHIFT_FACTOR = 8;
  static const long TABLE_SHIFT_POWER = (1UL<<TABLE_SHIFT_FACTOR);

  //Store the last X and Y coordinates in the table. This is used to make the next check faster
  int8_t lastXMax, lastXMin;
  int8_t lastYMax, lastYMin;

  //Store the last input and output values, again for caching purposes
  axis_type lastXInput, lastYInput;
  _Ty lastOutput;
  bool cacheIsValid = false; ///< This tracks whether the tables cache should be used. Ordinarily this is true, but is set to false whenever TunerStudio sends a new value for the table
};

