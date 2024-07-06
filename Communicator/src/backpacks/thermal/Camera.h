#include <Adafruit_MLX90640.h>
#include "../../globals/Globals.h"

#ifndef CAMERA_H
#define CAMERA_H
class Camera {
  public:
    Camera ();
    bool captureImage ();
    short getResolutionHeight ();
    short getResolutionWidth ();
    float* getImageData ();
    bool isReady ();

  private:
    Adafruit_MLX90640 mlx;
    float frame[32*24]; // buffer for full frame of temperatures
    void logConsole (String);
    void logSettings ();
    bool ready = false;
};
#endif