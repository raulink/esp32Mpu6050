// Basic demo for accelerometer readings from Adafruit MPU6050


#include <WiFi.h>

#include <PubSubClient.h>
#include <Wire.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char* ssid ="Raulink";
const char* password = "c0nd0m1n10.";

//Añadir MQTT broker 
const char* mqtt_server= "192.168.0.14:1883";


WiFiClient espCLient;
PubSubClient client(espCLient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledPin = 2;

Adafruit_MPU6050 mpu;

void setup(void) {
  Serial.begin(115200);
  /* while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  } */

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  setup_wifi();
  

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);
}


void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Conectando a:");
  Serial.println(ssid);

  WiFi.begin();
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");    
  }
  Serial.println("");
  Serial.print("Wifi conectado ");
  Serial.println("IP:");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message,unsigned int length){
  Serial.print("MEnsaje recibido topico:");
  Serial.print(topic);
  Serial.print(".Mensaje :");
  String messageTemp;

  for (int i=0;i<length;i++){
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];    
  }
  Serial.println();

  if(String(topic)=="esp32/output"){
    Serial.print("Cambiar salida a ");
    if (messageTemp =="on"){
      Serial.print("on");
      digitalWrite(ledPin,HIGH);      
    }
    else if(messageTemp =="off"){
      Serial.println("off");
      digitalWrite(ledPin,LOW);
    }
  }
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Intentando MQTT conexion...");

    if(client.connect("ESP32Cliente")){
      Serial.println("conectado");
      client.subscribe("esp32/output");      
    }
    else {
      Serial.print("Falla, rc=");
      Serial.print(client.state());
      Serial.println("Intentando en 5 s.");
      delay(5000);
    }
  }
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  if (!client.connected()){
    reconnect();
  }
  Serial.print("Ini: "+millis());  
  client.loop();
  Serial.println("Fin: "+millis());  
  
  mpu.getEvent(&a, &g, &temp);
  /* Print out the values */
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.print(", ");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.println("");

  char ax[8];
  char ay[8];
  char az[8];
  char temperature[8];
  dtostrf(a.acceleration.x,4,2,ax);
  dtostrf(a.acceleration.y,4,2,ay);
  dtostrf(a.acceleration.z,4,2,az);
  dtostrf(a.temperature,4,2,temperature);
  
  client.publish("esp32/ax",ax);
  client.publish("esp32/ay",ay);
  client.publish("esp32/az",az);
  client.publish("exp32/temp",temperature);

  delay(2);
}