#include <Wire.h>
#include <MPU6050.h>

#define SAMPLES 100 // Número de leituras para cada posição

MPU6050 mpu;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  mpu.initialize();
  
  // Realiza a calibração do acelerômetro
  calibrateAccelerometer();
}

void loop() {
  // Realiza as leituras das posições
  for (int i = 0; i < SAMPLES; i++) {
    // Leitura do acelerômetro
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
  
    // Imprime as posições X, Y e Z
    Serial.print("Leitura ");
    Serial.print(i+1);
    Serial.print(": X=");
    Serial.print(ax);
    Serial.print(", Y=");
    Serial.print(ay);
    Serial.print(", Z=");
    Serial.println(az);

    delay(10); // Intervalo entre as leituras
  }

  // Calcula o offset e o ganho para cada eixo
  float offset[3];
  float gain[3];

  calculateOffsetAndGain(offset, gain);

  // Imprime os valores de offset e ganho
  Serial.println("Offset e Ganho:");
  for (int i = 0; i < 3; i++) {
    Serial.print("Posição ");
    Serial.print(i);
    Serial.print(": Offset=");
    Serial.print(offset[i]);
    Serial.print(", Ganho=");
    Serial.println(gain[i]);
  }

  // Calcula e imprime o valor de Asaida
  float Aref = 1.0; // Valor de referência
  float Asaida[3];
  for (int i = 0; i < 3; i++) {
    Asaida[i] = (gain[i] * Aref) + offset[i];
    Serial.print("Asaida posição ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(Asaida[i]);
  }

  // Calcula e imprime o valor da média de cada posição
  float positionAverage[3];
  calculatePositionAverage(positionAverage);

  Serial.println("Média de cada posição:");
  for (int i = 0; i < 3; i++) {
    Serial.print("Posição ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(positionAverage[i]);
  }

  // Aguarda 1 segundo antes de iniciar a próxima leitura
  delay(1000);
}

void calibrateAccelerometer() {
  // Realiza 100 leituras para calibrar o acelerômetro
  for (int i = 0; i < 100; i++) {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
  
    delay(10); // Intervalo entre as leituras
  }
}

void calculateOffsetAndGain(float* offset, float* gain) {
  // Realiza as leituras das médias das leituras positivas e negativas
  float avg_pos[3] = {0.0f}; // Vetor para armazenar as médias das leituras positivas
  float avg_neg[3] = {0.0f}; // Vetor para armazenar as médias das leituras negativas

  for (int i = 0; i < SAMPLES; i++) {
    // Leitura do acelerômetro
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    // Soma as leituras positivas e negativas separadamente
    avg_pos[0] += ax > 0 ? ax : 0;
    avg_pos[1] += ay > 0 ? ay : 0;
    avg_pos[2] += az > 0 ? az : 0;
    avg_neg[0] += ax < 0 ? ax : 0;
    avg_neg[1] += ay < 0 ? ay : 0;
    avg_neg[2] += az < 0 ? az : 0;

    delay(10); // Intervalo entre as leituras
  }

  // Calcula as médias das leituras positivas e negativas
  for (int i = 0; i < 3; i++) {
    avg_pos[i] /= SAMPLES;
    avg_neg[i] /= SAMPLES;
  }

  // Calcula o offset e o ganho para cada eixo
  for (int i = 0; i < 3; i++) {
    offset[i] = (avg_pos[i] + avg_neg[i]) / 2;
    gain[i] = (avg_pos[i] - avg_neg[i]) / 2;
  }
}

void calculatePositionAverage(float* positionAverage) {
  // Vetores para armazenar as leituras das posições
  int16_t ax[SAMPLES], ay[SAMPLES], az[SAMPLES];

  // Realiza as leituras das posições
  for (int i = 0; i < SAMPLES; i++) {
    mpu.getAcceleration(&ax[i], &ay[i], &az[i]);
    delay(10); // Intervalo entre as leituras
  }

  // Calcula a média de cada posição
  for (int i = 0; i < 3; i++) {
    float sum = 0.0f;
    for (int j = 0; j < SAMPLES; j++) {
      switch (i) {
        case 0: sum += ax[j]; break; // Posição X
        case 1: sum += ay[j]; break; // Posição Y
        case 2: sum += az[j]; break; // Posição Z
      }
    }
    positionAverage[i] = sum / SAMPLES;
  }
}
