#include "math.h"

class MOVG {
public:
  MOVG(int windowSize, float outlierThreshold) {
    WINDOW_SIZE = windowSize;
    OUTLIER_THRESHOLD = outlierThreshold;
    values = new float[WINDOW_SIZE];
    currentIndex = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
      values[i] = 0.0;
    }
  }

  ~MOVG() {
    delete[] values;
  }

  float computeMOVG(float newValue) {
    addValue(newValue);
    return computeMOVG();
  }

  // computeMOVG method without parameters
  float computeMOVG() {
    float sum = 0.0;
    int count = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
      sum += values[i];
      count++;
    }
    float average = sum / count;
    float sumSquaredDiff = 0.0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
      sumSquaredDiff += pow((values[i] - average), 2);
    }
    float standardDeviation = sqrt(sumSquaredDiff / count);
    for (int i = 0; i < WINDOW_SIZE; i++) {
      if (abs(values[i] - average) > OUTLIER_THRESHOLD * standardDeviation) {
        sum -= values[i];
        count--;
      }
    }
    if (count > 0) {
      average = sum / count;
    }
    return average;
  }
  
  // Add a value to the buffer without computing the moving average
  void addValue(float newValue) {
    values[currentIndex] = newValue;
    currentIndex = (currentIndex + 1) % WINDOW_SIZE;
  }

private:
  int WINDOW_SIZE;
  float OUTLIER_THRESHOLD;
  float* values;
  int currentIndex;
};
