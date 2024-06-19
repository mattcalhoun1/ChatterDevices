#include "Camera.h"

Camera::Camera() {
  logConsole("Initialize MLX90640 Thermal Camera");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    logConsole("MLX90640 not found!");
    while (1) delay(10);
  }
  
  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
  mlx.setResolution(MLX90640_ADC_18BIT);
  //mlx.setRefreshRate(MLX90640_2_HZ);
  mlx.setRefreshRate(MLX90640_4_HZ);
  logConsole("Adafruit MLX90640 Thermal is Configured");

  logSettings();
}


bool Camera::captureImage () {
  //logConsole("Capturing thermal image");
  if (mlx.getFrame(frame) != 0) {
    logConsole("Image capture failed");
    return false;
  }

  //logConsole("Image captured");
  return true;
}

short Camera::getResolutionHeight () {
  return 24;
}

short Camera::getResolutionWidth () {
  return 32;
}

float* Camera::getImageData () {
  return frame;
}

void Camera::logSettings () {
  logConsole("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    logConsole("Chess");
  } else {
    logConsole("Interleave");    
  }

  logConsole("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: logConsole("16 bit"); break;
    case MLX90640_ADC_17BIT: logConsole("17 bit"); break;
    case MLX90640_ADC_18BIT: logConsole("18 bit"); break;
    case MLX90640_ADC_19BIT: logConsole("19 bit"); break;
  }

  logConsole("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: logConsole("0.5 Hz"); break;
    case MLX90640_1_HZ: logConsole("1 Hz"); break; 
    case MLX90640_2_HZ: logConsole("2 Hz"); break;
    case MLX90640_4_HZ: logConsole("4 Hz"); break;
    case MLX90640_8_HZ: logConsole("8 Hz"); break;
    case MLX90640_16_HZ: logConsole("16 Hz"); break;
    case MLX90640_32_HZ: logConsole("32 Hz"); break;
    case MLX90640_64_HZ: logConsole("64 Hz"); break;
  }
}

void Camera::logConsole (String msg) {
  if (LOG_ENABLED) {
    Serial.println(msg);
  }
}