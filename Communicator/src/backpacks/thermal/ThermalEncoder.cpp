#include "ThermalEncoder.h"

ThermalEncoder::ThermalEncoder(int _resolutionHeight, int _resolutionWidth, bool _compress) {
  resolutionHeight = _resolutionHeight;
  resolutionWidth = _resolutionWidth;
  compress = _compress;

  // precalculate interpolated round width/heights, as recalculating is super slow
  interpolatedRoundWidth[0] = resolutionWidth;
  interpolatedRoundHeight[0] = resolutionHeight;
  for (int rounds = 1; rounds < 3; rounds++) {
    interpolatedRoundWidth[rounds] = resolutionWidth * pow(2, rounds);  
    interpolatedRoundHeight[rounds] = resolutionHeight * pow(2, rounds);  
  }
}


bool ThermalEncoder::encode (float* image) {
  // Copy the image into the encoded buffer, as uint8_t rounded, for easy transmission
  for (int row = 0; row < resolutionHeight; row++) {
    for (int col = 0; col < resolutionWidth; col++) {
      int position = (row * resolutionWidth) + col;
      encodeBuffer[position] = round(image[position]);
      //Serial.print(encodeBuffer[position]);Serial.print(",");
    }
    //Serial.println("");
  }
  encodedBufferLength = resolutionHeight * resolutionWidth;
  //Serial.println("Encoded Buffer length " + String(encodedBufferLength));
  return true;
}


bool ThermalEncoder::decode (uint8_t* encodedImage) {
  // just copy straight over since we aren't doing compression yet
  for (int row = 0; row < resolutionHeight; row++) {
    for (int col = 0; col < resolutionWidth; col++) {
      int position = (row * resolutionWidth) + col;
      encodeBuffer[position] = encodedImage[position];
    }
  }
  encodedBufferLength = resolutionHeight * resolutionWidth;
  return true;
}

uint8_t* ThermalEncoder::getEncodeDecodeBuffer () {
  return encodeBuffer;
}

int ThermalEncoder::getEncodedBufferLength () {
  return encodedBufferLength;
}

int ThermalEncoder::getInterpolatedResolutionWidth () {
  return interpolatedRoundWidth[THERMAL_INTERPOLATE_LEVEL];
}

int ThermalEncoder::getInterpolatedResolutionHeight () {
  return interpolatedRoundHeight[THERMAL_INTERPOLATE_LEVEL];
}

// identical to uint8 version, just needed different pointer type
// maybe can refactor these down somehow
void ThermalEncoder::primeInterpolationBuffer (const float* image, int projectedRow) {

  // scale the source row index down to match the projected row
  projectedRow /= THERMAL_INTERPOLATE_LEVEL;

  // if the projected row is an even row, we already have half the pixels
  // if it's odd, we have no rows of the pixel

  // -1 means unknown or not yet calculated
  //  first copy in all the data we know
  int width = getInterpolatedWidth(1);
  int height = getInterpolatedHeight(1);
  int intBufferRow = 0;
  for (int currProjRow = projectedRow - 2; currProjRow <= projectedRow + 2; currProjRow++) {
    if (currProjRow % 2 == 0 && currProjRow < height && currProjRow >= 0) {
      projectRow (image + (resolutionWidth * (currProjRow / 2)), interpolationBuffer + (intBufferRow * width), width);
    }
    else {
      for (int currProjCol = 0; currProjCol < width; currProjCol++) {
        setInterpolatedBufferValue(intBufferRow, currProjCol, -1, 1);
      }
    }
    intBufferRow++;
  }
}

// identical to float version, just needed different pointer type
// maybe can refactor these down somehow
void ThermalEncoder::primeInterpolationBuffer (const uint8_t* image, int projectedRow) {
  // scale the source row index down to match the projected row
  projectedRow /= THERMAL_INTERPOLATE_LEVEL;

  // if the projected row is an even row, we already have half the pixels
  // if it's odd, we have no rows of the pixel

  // -1 means unknown or not yet calculated
  //  first copy in all the data we know
  int width = getInterpolatedWidth(1);
  int height = getInterpolatedHeight(1);
  int intBufferRow = 0;
  for (int currProjRow = projectedRow - 2; currProjRow <= projectedRow + 2; currProjRow++) {
    if (currProjRow % 2 == 0 && currProjRow < height && currProjRow >= 0) {
      projectRow (image + (resolutionWidth * (currProjRow / 2)), interpolationBuffer + (intBufferRow * width), width);
    }
    else {
      for (int currProjCol = 0; currProjCol < width; currProjCol++) {
        setInterpolatedBufferValue(intBufferRow, currProjCol, -1, 1);
      }
    }
    intBufferRow++;
  }
}

int ThermalEncoder::getInterpolatedWidth(int round) {
  return interpolatedRoundWidth[round];
  //return resolutionWidth * pow(2, round);
}

int ThermalEncoder::getInterpolatedHeight(int round) {
  return interpolatedRoundHeight[round];
  //return resolutionHeight * pow(2, round);
}


void ThermalEncoder::reprimeInterpolationBuffer () {
  
  /*Serial.println("Round 1:");
  logInterpolationBuffer(1);
  Serial.println("");*/
  

  // Take the existing buffer and space it out for a second round of interpolation
  // space 
  int round2Width = getInterpolatedWidth(2);

  // Prime using the image from the priming buffer, being careful not to wipe out any needed info

  // row 5 gets info spaced out from row 3
  projectRow(interpolationBuffer + (3 * getInterpolatedWidth(1)), interpolationBuffer + (4 * round2Width), round2Width);

  // row 2 gets center (row 2) spaced out
  for (int colCount = round2Width - 1; colCount >= 0; colCount--) {
    if (colCount % 2 == 0) {
      setInterpolatedBufferValue(2, colCount, getInterpolatedBufferValue(2, colCount / 2, 1), 2);
    }
    else {
      setInterpolatedBufferValue(2, colCount, -1, 2);
    }
  }

  // row 0 gets info spaced out from existing row 1
  projectRow(interpolationBuffer + getInterpolatedWidth(1), interpolationBuffer, round2Width);


  // row 1 & 3 get blanks
  for (int r = 1; r < 4; r += 2) {
    for (int c = 0; c < round2Width; c++) {
      setInterpolatedBufferValue(r, c, -1, 2);
    }
  }

  /*
  Serial.println("Round 2:");
  logInterpolationBuffer(2);
  Serial.println("");
  */
}

// projects an existing row to a new one at double the size, with -1s in the gaps
void ThermalEncoder::projectRow (const float* existingRow, float* newRow, int projectionSize) {
  for (int col = 0; col < projectionSize; col++) {
    if (col % 2 == 0) {
      newRow[col] = existingRow[col/2];
    }
    else {
      newRow[col] = -1;
    }
  }
}

// projects an existing row to a new one at double the size, with -1s in the gaps
void ThermalEncoder::projectRow (const uint8_t* existingRow, float* newRow, int projectionSize) {
  for (int col = 0; col < projectionSize; col++) {
    if (col % 2 == 0) {
      newRow[col] = existingRow[col/2];
    }
    else {
      newRow[col] = -1;
    }
  }
}

const float* ThermalEncoder::getInterpolatedRow (const uint8_t* image, int projectedRow) {
  if (THERMAL_INTERPOLATE_LEVEL == 0) {
    // no interpolation, just convert to floats at the beginning of interpolation buffer
    for (int c = 0; c < resolutionWidth; c++) {
      interpolationBuffer[c] = image[c];
    }
    return interpolationBuffer;
  }

  // Level 1 or higher requires priming the buffer with spaced out image data, leaving gaps to be filled with averages
  primeInterpolationBuffer(image, projectedRow);
  completePrimedInterpolationBuffer(1);

  if (THERMAL_INTERPOLATE_LEVEL > 1) {
    reprimeInterpolationBuffer();
    completePrimedInterpolationBuffer(2);

    // the middle row (of largest possible width) is the one requested
    return interpolationBuffer + (getInterpolatedWidth(2) * 2);
  }
  else {
    // the middle row is the one requested
    return interpolationBuffer + (getInterpolatedWidth(1) * 2);
  }
}

const float* ThermalEncoder::getInterpolatedRow (const float* image, int projectedRow) {
  if (THERMAL_INTERPOLATE_LEVEL == 0) {
    // no interpolation, just point to the original
    return image + (resolutionWidth * projectedRow);
  }

  // Level 1 or higher requires priming the buffer with spaced out image data, leaving gaps to be filled with averages
  primeInterpolationBuffer(image, projectedRow);
  completePrimedInterpolationBuffer(1);

  if (THERMAL_INTERPOLATE_LEVEL > 1) {
    reprimeInterpolationBuffer();
    completePrimedInterpolationBuffer(2);

    // the middle row (of largest possible width) is the one requested
    return interpolationBuffer + (getInterpolatedWidth(2) * 2);
  }
  else {
    // the middle row is the one requested
    return interpolationBuffer + (getInterpolatedWidth(1) * 2);
  }
}

void ThermalEncoder::completePrimedInterpolationBuffer (int interpolationRound) {
  // first pass on primed buffer,
  // for each row that is fully empty (indicated by -1 in col zero, based on how priming was implemented)
  int intBufferRow = 0;
  int intBufferCol = 1;

  for (int currProjRow = 0; currProjRow < 5; currProjRow++) {
    if (getInterpolatedBufferValue(intBufferRow, 0, interpolationRound) == -1) {
      // start with second pixel, where we have the most surrounding information
      for (int currProjCol = 1; currProjCol < getInterpolatedWidth(interpolationRound); currProjCol+=2) {
        // get the avrage of surrounding pixels (excluding -1s) at the 4 corners (like a square)
        setInterpolatedBufferValue(intBufferRow, intBufferCol, getIntBufferPerimiterAverageSquare(intBufferRow, intBufferCol, interpolationRound), interpolationRound);
        intBufferCol+=2;
      }
    }
    intBufferCol = 1;
    intBufferRow++;
  }

  // If only level 1 interpolation, we only need to calculate the middle row
  // Otherwise, we need to calculate rows 2,3,4, as they'll be used for next round
  intBufferRow = 1;
  int finalPassStart = THERMAL_INTERPOLATE_LEVEL > 1 ? 0 : 2;
  int finalPassEnd = THERMAL_INTERPOLATE_LEVEL > 1 ? 4 : 2;
  for (int finalPassRow = finalPassStart; finalPassRow <= finalPassEnd; finalPassRow++) {
    for (int currProjCol = 0; currProjCol < getInterpolatedWidth(interpolationRound); currProjCol++) {
      if (getInterpolatedBufferValue(finalPassRow, currProjCol, interpolationRound) == -1) {
        setInterpolatedBufferValue(finalPassRow, currProjCol, getIntBufferPerimiterAverageDiamond(finalPassRow, currProjCol, interpolationRound), interpolationRound);
      }
    }
  }
}

void ThermalEncoder::logInterpolationBuffer (int interpolationRound) {
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < getInterpolatedWidth(interpolationRound); c++) {
      Serial.print(String(getInterpolatedBufferValue(r, c, interpolationRound)));Serial.print(",");
    }
    Serial.println("");
  }
}

float ThermalEncoder::getIntBufferPerimiterAverageSquare (int row, int col, int interpolationRound) {
  int numVals = 0;
  float total = 0;
  for (int r = row-1; r <= row+1; r++) {
    for (int c = col-1; c <= col+1; c++) {
      if (r != row && c != col) {
        float val = getInterpolatedBufferValue(r, c, interpolationRound);
        if (val != -1) {
          total += val;
          numVals ++;
        }
      }
    }
  }

  if (numVals > 0) {
    return total / numVals;
  }
  return -1;
}

float ThermalEncoder::getIntBufferPerimiterAverageDiamond (int row, int col, int interpolationRound) {
  int numVals = 0;
  float total = 0;
  for (int r = row-1; r <= row+1; r++) {
    for (int c = col-1; c <= col+1; c++) {
      if (r == row || c == col) {
        float val = getInterpolatedBufferValue(r, c, interpolationRound);
        if (val != -1) {
          total += val;
          numVals ++;
        }
      }
    }
  }



  if (numVals > 0) {
    return total / numVals;
  }
  return -1;
}

void ThermalEncoder::setInterpolatedBufferValue (int row, int col, float value, int interpolationRound) {
  interpolationBuffer[(row * getInterpolatedWidth(interpolationRound)) + col] = value;
}

float ThermalEncoder::getInterpolatedBufferValue (int row, int col, int interpolationRound) {
  // if the requested value is out of bounds, return -1 so it doesn't get counted
  if(row < 0 || col < 0 || row >= 5 || col >= getInterpolatedWidth(interpolationRound)) {
    return -1;
  }
  return interpolationBuffer[(row * getInterpolatedWidth(interpolationRound)) + col];
}