#include "Globals.h"
#include <Arduino.h>

#ifndef THERMALENCODER_H
#define THERMALENCODER_H

class ThermalEncoder {
  public:
    ThermalEncoder(int _resolutionHeight, int _resolutionWidth, bool _compress);
    bool encode (float* image);
    bool decode (uint8_t* encodedImage);
    uint8_t* getEncodeDecodeBuffer ();
    int getEncodedBufferLength ();
    const float* getInterpolatedRow (const float* image, int projectedRow);
    const float* getInterpolatedRow (const uint8_t* image, int projectedRow);
    int getInterpolatedResolutionWidth ();
    int getInterpolatedResolutionHeight ();

  private:
    int resolutionHeight;
    int resolutionWidth;
    bool compress;
    float interpolationBuffer[THERMAL_INTERPOLATE_BUFFER_SIZE];
    uint8_t encodeBuffer[THERMAL_ENCODE_BUFFER_SIZE];
    int interpolatedBufferLength = 0;
    int encodedBufferLength = 0;

    void primeInterpolationBuffer (const float* image, int projectedRow);
    void primeInterpolationBuffer (const uint8_t* image, int projectedRow);
    void reprimeInterpolationBuffer ();
    void setInterpolatedBufferValue (int row, int col, float value, int interpolationRound);
    float getInterpolatedBufferValue (int row, int col, int interpolationRound);
    float getIntBufferPerimiterAverageSquare (int row, int col, int interpolationRound);
    float getIntBufferPerimiterAverageDiamond (int row, int col, int interpolationRound);
    void logInterpolationBuffer (int interpolationRound);
    void completePrimedInterpolationBuffer (int interpolationRound);
    void projectRow (const float* existingRow, float* newRow, int projectionSize);
    void projectRow (const uint8_t* existingRow, float* newRow, int projectionSize);

    int getInterpolatedWidth(int round);
    int getInterpolatedHeight(int round);

    int interpolatedRoundWidth[3];
    int interpolatedRoundHeight[3];
};

#endif