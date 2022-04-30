/**
 * @author Hamza RAHAL
 * 
 * @dirigé par Walid ABDAOUI
 * 
 * 30th April 2022
 * 
 * projet rtos
 * 
 **/

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

struct valeurCapteurs {
  int analogique;
  int numerique;
  double tempsEnMillisecondes;
};

void tache1( void *pvParameters);
void tache2( void *pvParameters);
void tache3( void *pvParameters);
void tache4( void *pvParameters);
void tache5( void *pvParameters);

QueueHandle_t analogQueue;
QueueHandle_t resultQueue;
QueueHandle_t structureQueue;
QueueHandle_t structureQueueMIN;

SemaphoreHandle_t mutex;

void setup() {
  
  Serial.begin(9600);
  
  xTaskCreate(tache1, "tache1", 1000, NULL, 4, NULL );
  
  xTaskCreate(tache2, "tache2", 1000, NULL, 3, NULL );
  
  xTaskCreate(tache3, "tache3", 1000, NULL, 2, NULL );
  
  xTaskCreate(tache4, "tache4", 1000, NULL, 1, NULL );
  
  xTaskCreate(tache5, "tache5", 1000, NULL, 0, NULL );

  
  mutex = xSemaphoreCreateBinary();
  

  analogQueue     =   xQueueCreate(1, sizeof(int));
  
  resultQueue     =   xQueueCreate(1, sizeof(int));
  
  structureQueue  =   xQueueCreate(2, sizeof(struct valeurCapteurs));
  
  structureQueueMIN  =   xQueueCreate(2, sizeof(struct valeurCapteurs));
  
  vTaskStartScheduler();
  
}

void loop() {

}

void tache1( void *pvParameters){

  pinMode(A0, INPUT);

  while(1){

    int analogValue = analogRead(A0);
    Serial.println(analogValue);


    xQueueSendToBack(analogQueue, &analogValue, portMAX_DELAY);

    vTaskDelay(100);

  }

}


void tache2( void *pvParameters){

  int val3;
  int val4;
  int res = 0;
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  while(1){

    val3  = digitalRead(3);
    val4  = digitalRead(4);
    res   = val3 + val4;

    xQueueSendToBack(resultQueue, &res, portMAX_DELAY);
    vTaskDelay(100);

  }

}


void tache3( void *pvParameters){

  struct valeurCapteurs data;
  int num,ana;

  while(1){

    if (xQueueReceive(analogQueue, &(ana), portMAX_DELAY) == pdPASS) {
      
      xQueueReceive(resultQueue, &(num), portMAX_DELAY);

      
      data.tempsEnMillisecondes = millis();
      data.analogique = ana;
      data.numerique = num;
      xQueueSendToBack(structureQueue, &data, portMAX_DELAY);
    
    }

    vTaskDelay(100);
  }
}


void tache4( void *pvParameters){

  struct valeurCapteurs data;

  while(1){

    if(xQueueReceive(structureQueue, &(data), portMAX_DELAY) == pdPASS);
      

      //xSemaphoreTake(mutex, portMAX_DELAY);

      Serial.println("_________recus______");
      Serial.print("resultante : ");
      Serial.println(data.numerique);
      Serial.print("analogique : ");
      Serial.println(data.analogique);
      Serial.print("temps : ");
      Serial.println(data.tempsEnMillisecondes);

      xQueueSendToBack(structureQueueMIN, &data, portMAX_DELAY);
      xSemaphoreGive(mutex);
      
     

      vTaskDelay(100);
  }

  

}

void tache5(void *pvParameters){
  
  struct valeurCapteurs data;
  double min;

  while(1){

    if (xQueueReceive(structureQueueMIN, &(data), 0) == pdPASS) {
      
      min = data.tempsEnMillisecondes / 60000;
      
      xSemaphoreTake(mutex, portMAX_DELAY);

      Serial.println("_________recus______");
      Serial.print("resultante : ");
      Serial.println(data.numerique);
      Serial.print("analogique : ");
      Serial.println(data.analogique);
      Serial.print("temps mn : ");
      Serial.println(min);
      xSemaphoreGive(mutex);
        
    
      vTaskDelay(100);
    }
  }
} 
