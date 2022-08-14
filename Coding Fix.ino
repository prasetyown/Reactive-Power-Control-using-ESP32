#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_adc_cal.h"

//Define Relays Pin to ESP32/////////////////////////////////////////////////////////////////
//Define Relays Pin to ESP32/////////////////////////////////////////////////////////////////
#define RelayActivate 27 //DI1 (pin Digital A1)
#define RelayAkhir 25 //AI1 (pin Analog A25)
#define Led1 4 //LED Activate

String activate, qmode, qref, vref;
char relay_activate[50], relay_qref[50];

// Declaring Pins and Constants/////////////////////////////////////////////////////////////
// Declaring Pins and Constants/////////////////////////////////////////////////////////////
float pin_activate;
float pin_Q_mode;
float volt_q;
float volt_v;
float pin_Q_inst;
float pin_Q_ref = 0;
float new_pin_Q_ref;
float q_max = 200000;
float constrained_Q_inst;
float pin_V_t;
float Switch_Awal;
float Switch_Tengah;
float Switch_Akhir;
float Out_PI_Q;
float Q_Sum_Akhir;
float Kali_Q;
float result_q;
float dac_result_q;
float Out_Qref;

// Subscribed Topics/////////////////////////////////////////////////////////////////////////
// Subscribed Topics/////////////////////////////////////////////////////////////////////////
#define sub1 "olly1/relay1" //Activate
#define sub2 "olly1/relay2" //Q Ref


// Update network information////////////////////////////////////////////////////////////
// Update network information////////////////////////////////////////////////////////////
const char* ssid = "Olly XR";
const char* password = "tanyadia";
const char* mqtt_server = "broker.hivemq.com"; // Local IP address of Raspberry Pi

const char* username = "ollyesp";
const char* pass = "esp123esp";


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

//Setup Function////////////////////////////////////////////////////////////////////////////////////////
//Setup Function////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  pinMode(RelayActivate, OUTPUT);

  pinMode(Led1,OUTPUT);

  Serial.begin(9600);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

//Loop Function///////////////////////////////////////////////////////////////////////////////////////////
//Loop Function///////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  if (!client.connected()){
    reconnect();
  }
  client.loop();   
    
  //Main Function//////////////////////////////////////////////////////////////////////////////////////////////

  if (pin_activate == 1){

    //Q CONTROL////////////////////////////////////////////////////////////////////////////////////////
    Serial.print("Out pin_Q_ref =");
    Serial.println(pin_Q_ref);
    
    Serial.print("Out new_pin_Q_ref =");
    Serial.println(new_pin_Q_ref);

    if (new_pin_Q_ref >= 0){
      if (new_pin_Q_ref > pin_Q_ref){
        for ( pin_Q_ref ; pin_Q_ref < new_pin_Q_ref; pin_Q_ref = pin_Q_ref + 1000 ){
          Out_PI_Q = 0.0025*pin_Q_ref;
          Q_Sum_Akhir = 1 + Out_PI_Q;
          Kali_Q = (Q_Sum_Akhir * BacaTerminalVoltage())+100000;
    
          Serial.print("Out Out_PI_Q =");
          Serial.println(Out_PI_Q);
        
          Serial.print("Out Kali_Q =");
          Serial.println(Kali_Q);
    
          dac_result_q = (Kali_Q/q_max)*255;
    
          Serial.print("Out result_q =");
          Serial.println(result_q); 
         
          Out_Qref = dac_result_q;

          dacWrite(RelayAkhir,Out_Qref);
    
          Serial.print("Out Qref Akhir =");
          Serial.println(Out_Qref);

          char temp_q[20];
          dtostrf(BacaDayaReaktif(), 3, 3, temp_q);
          client.publish("esp32/qinst", temp_q);

          char temp_v[20];
          dtostrf(BacaTerminalVoltage(), 3, 3, temp_v);
          client.publish("esp32/vt", temp_v);
        }
      }
      else if (new_pin_Q_ref < pin_Q_ref){
        for ( pin_Q_ref ; pin_Q_ref > new_pin_Q_ref; pin_Q_ref = pin_Q_ref - 1000 ){
          Out_PI_Q = 0.0025*pin_Q_ref;
          Q_Sum_Akhir = 1 + Out_PI_Q;
          Kali_Q = (Q_Sum_Akhir * BacaTerminalVoltage())+100000;
    
          Serial.print("Out Out_PI_Q =");
          Serial.println(Out_PI_Q);
        
          Serial.print("Out Kali_Q =");
          Serial.println(Kali_Q);
    
          dac_result_q = (Kali_Q/q_max)*255;
    
          Serial.print("Out result_q =");
          Serial.println(result_q);    
         
          Out_Qref = dac_result_q;

          dacWrite(RelayAkhir,Out_Qref);
    
          Serial.print("Out Qref Akhir =");
          Serial.println(Out_Qref);

          char temp_q[20];
          dtostrf(BacaDayaReaktif(), 3, 3, temp_q);
          client.publish("esp32/qinst", temp_q);

          char temp_v[20];
          dtostrf(BacaTerminalVoltage(), 3, 3, temp_v);
          client.publish("esp32/vt", temp_v);
        }
      }
      else if(new_pin_Q_ref == pin_Q_ref){
        Out_PI_Q = 0.0025*pin_Q_ref;
        Q_Sum_Akhir = 1 + Out_PI_Q;
        Kali_Q = (Q_Sum_Akhir * BacaTerminalVoltage())+100000;
    
        Serial.print("Out Out_PI_Q =");
        Serial.println(Out_PI_Q);
        
        Serial.print("Out Kali_Q =");
        Serial.println(Kali_Q);
    
        dac_result_q = (Kali_Q/q_max)*255;
    
        Serial.print("Out result_q =");
        Serial.println(result_q);    
         
        Out_Qref = dac_result_q;

        char temp_q[20];
        dtostrf(BacaDayaReaktif(), 3, 3, temp_q);
        client.publish("esp32/qinst", temp_q);

        char temp_v[20];
        dtostrf(BacaTerminalVoltage(), 3, 3, temp_v);
        client.publish("esp32/vt", temp_v);

        dacWrite(RelayAkhir,Out_Qref);
      }
      Serial.print("Out Out_PI_Q =");
      Serial.println(Out_PI_Q);
        
      Serial.print("Out Kali_Q =");
      Serial.println(Kali_Q);
      
      Serial.print("Out Qref Akhir =");
      Serial.println(Out_Qref);     
    }

    
    else if (new_pin_Q_ref < 0){
      if (new_pin_Q_ref > pin_Q_ref){
        for ( pin_Q_ref ; pin_Q_ref < new_pin_Q_ref; pin_Q_ref = pin_Q_ref + 1000 ){
          Out_PI_Q = 0.0025*pin_Q_ref;
          Q_Sum_Akhir = 1 + Out_PI_Q;
          Kali_Q = (Q_Sum_Akhir * BacaTerminalVoltage())+100000;
    
          Serial.print("Out Out_PI_Q =");
          Serial.println(Out_PI_Q);
        
          Serial.print("Out Kali_Q =");
          Serial.println(Kali_Q);
    
          dac_result_q = (Kali_Q/q_max)*255;
    
          Serial.print("Out result_q =");
          Serial.println(result_q);
          
          Out_Qref = dac_result_q;

          dacWrite(RelayAkhir,Out_Qref);
    
          Serial.print("Out Qref Akhir =");
          Serial.println(Out_Qref);

          char temp_q[20];
          dtostrf(BacaDayaReaktif(), 3, 3, temp_q);
          client.publish("esp32/qinst", temp_q);

          char temp_v[20];
          dtostrf(BacaTerminalVoltage(), 3, 3, temp_v);
          client.publish("esp32/vt", temp_v);
        }
      }
      else if (new_pin_Q_ref < pin_Q_ref){
        for ( pin_Q_ref ; pin_Q_ref > new_pin_Q_ref; pin_Q_ref = pin_Q_ref - 1000 ){
          Out_PI_Q = 0.0025*pin_Q_ref;
          Q_Sum_Akhir = 1 + Out_PI_Q;
          Kali_Q = (Q_Sum_Akhir * BacaTerminalVoltage())+100000;
    
          Serial.print("Out Out_PI_Q =");
          Serial.println(Out_PI_Q);
        
          Serial.print("Out Kali_Q =");
          Serial.println(Kali_Q);
    
          dac_result_q = (Kali_Q/q_max)*255;
    
          Serial.print("Out result_q =");
          Serial.println(result_q);    
         
          Out_Qref = dac_result_q;

          dacWrite(RelayAkhir,Out_Qref);
    
          Serial.print("Out Qref Akhir =");
          Serial.println(Out_Qref);

          char temp_q[20];
          dtostrf(BacaDayaReaktif(), 3, 3, temp_q);
          client.publish("esp32/qinst", temp_q);

          char temp_v[20];
          dtostrf(BacaTerminalVoltage(), 3, 3, temp_v);
          client.publish("esp32/vt", temp_v);
        }
      }
      else if(new_pin_Q_ref == pin_Q_ref){
        Out_PI_Q = 0.0025*pin_Q_ref;
        Q_Sum_Akhir = 1 + Out_PI_Q;
        Kali_Q = (Q_Sum_Akhir * BacaTerminalVoltage())+100000;
    
        Serial.print("Out Out_PI_Q =");
        Serial.println(Out_PI_Q);
        
        Serial.print("Out Kali_Q =");
        Serial.println(Kali_Q);
    
        dac_result_q = (Kali_Q/q_max)*255;
    
        Serial.print("Out result_q =");
        Serial.println(result_q);  
         
        Out_Qref = dac_result_q;

        char temp_q[20];
        dtostrf(BacaDayaReaktif(), 3, 3, temp_q);
        client.publish("esp32/qinst", temp_q);

        char temp_v[20];
        dtostrf(BacaTerminalVoltage(), 3, 3, temp_v);
        client.publish("esp32/vt", temp_v);

        dacWrite(RelayAkhir,Out_Qref);
      }      
      Serial.print("Out Out_PI_Q =");
      Serial.println(Out_PI_Q);
        
      Serial.print("Out Kali_Q =");
      Serial.println(Kali_Q);
      
      Serial.print("Out Qref Akhir =");
      Serial.println(Out_Qref);
      
    }
  }
  else if (pin_activate == 0){
    Switch_Tengah = 0;
    dacWrite(RelayAkhir,Switch_Tengah);
  }
}


//Callback Function////////////////////////////////////////////////////////////////////////////////////
//Callback Function////////////////////////////////////////////////////////////////////////////////////
void callback(char* topic, byte* payload, unsigned int length){
  
  String messageTemp;
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (strstr(topic, sub1)) //Activate/////////////////////////////////////
  {
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
      messageTemp += (char)payload[i];
    }
    Serial.println();
    // Turn on switch when Node Red button is turned on
    if ((char)payload[0] == '1')
    {
      pin_activate = messageTemp.toFloat();
      digitalWrite(RelayActivate, HIGH);
      digitalWrite(Led1, HIGH);  
      activate = "Activated";
      activate.toCharArray(relay_activate, 20);      
    }
    // Turn off switch when Node Red button is turned off
    else
    {
      pin_activate = messageTemp.toFloat();
      digitalWrite(RelayActivate, LOW);
      digitalWrite(Led1, LOW);
      activate = "Not Activated";
      activate.toCharArray(relay_activate, 20);
    }
    //Publish to Node-Red
    client.publish("olly1/terima1", relay_activate);
  }

  else if ( strstr(topic, sub2)) //Q Reference/////////////////////////////
  {
    for (int i = 0; i < length; i++){
      Serial.print((char)payload[i]);
      messageTemp += (char)payload[i];
    }
    Serial.println();
    float f1=messageTemp.toFloat();
    new_pin_Q_ref = f1;
  }
}

//Q_INST ANALOG READ////////////////////////////////////////////////////////////////////////////////////
float BacaDayaReaktif(){    
  pin_Q_inst = ReadVoltage(32);
  
  if (pin_Q_inst < 184) { //184 batas bawah bit linear esp
    volt_q = 0.14;
  }
  else if (pin_Q_inst > 3302) { //3302 batas atas bit linear esp
    volt_q = 2.5;
  }
  else {
    volt_q = (((180 + (pin_Q_inst)) * 0.000806) + 0.000806); //0.000806 representasi tegangan tiap bit  3,3/4095
  }
  float batasbawah_q = -100; //Adjustable from excel formula
  float tegangan_typhoon_q = 0.064123116; //Adjustable from excel formula // REPRESENTASI NILAI TYPHOON PER BIT
  float scaling_constant_q = 1000; //Adjustable from schematic constant (Q_inst_ESP probe)
  float Q_inst = scaling_constant_q * ((((volt_q - 0.14) / 0.000757) * tegangan_typhoon_q) + batasbawah_q); // adjustable from excel 0,14 = OFFSET SCADA // adjustable from excel 0,000757 = REPRESENTASI TEGANGAN TIAP BIT ESP //
  
  //    Serial.print("Out Q_inst =");
  //    Serial.println(Q_inst);
  
  float bagiQinst = Q_inst/1000;
  float bagiQref = new_pin_Q_ref/1000;
  
  //    Serial.print("Out bagiQinst =");
  //    Serial.println(bagiQinst);
  
  //    Serial.print("Out bagiQref =");
  //    Serial.println(bagiQref);
  
  if ( bagiQinst >= bagiQref - 5 && bagiQinst <= bagiQref + 5){
    constrained_Q_inst = new_pin_Q_ref;  
  }
  else{
    constrained_Q_inst = Q_inst;
  }
  
  Serial.print("Out constrained_Q_inst =");
  Serial.println(Q_inst);

  return Q_inst;
}

//V_T ANALOG READ/////////////////////////////////////////////////////////////////////////////////////////
float BacaTerminalVoltage(){
  pin_V_t = ReadVoltage(33);
  
  if (pin_V_t < 184) { //184 batas bawah bit linear esp
    volt_v = 0.14;
  }
  else if (pin_V_t > 3302) { //3302 batas atas bit linear esp
    volt_v = 2.5;
  }
  else {
    volt_v = (((180 + (pin_V_t)) * 0.000806) + 0.000806); //0.000806 representasi tegangan tiap bit  3,3/4095
  }
  float batasbawah_v = 0; //Adjustable from excel formula
  float tegangan_typhoon_v = 0.1347; //Adjustable from excel formula // REPRESENTASI NILAI TYPHOON PER BIT
  float scaling_constant_v = 1; //Adjustable from schematic constant (V_t_ESP probe)
  float V_t = scaling_constant_v * ((((volt_v - 0.14) / 0.000757) * tegangan_typhoon_v) + batasbawah_v); // adjustable from excel 0,14 = OFFSET SCADA // adjustable from excel 0,000757 = REPRESENTASI TEGANGAN TIAP BIT ESP //
  
  Serial.print("Out V_t =");
  Serial.println(V_t);

  return V_t;
}  
    
// ESP Calibration//////////////////////////////////////////////////////////////////////////////
// ESP Calibration//////////////////////////////////////////////////////////////////////////////
float ReadVoltage(byte pinTyphoon){
  float calibration  = 1.04; // Adjust for ultimate accuracy when input is measured using an accurate DVM, if reading too high then use e.g. 0.99, too low use 1.01
  float vref = 1100;
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12  , 1100, &adc_chars);
  vref = adc_chars.vref; // Obtain the device ADC reference voltage
  return analogRead(pinTyphoon) * calibration * (1100 / vref); // ESP by design reference voltage in mV
}


// Connecting to WiFi Router//////////////////////////////////////////////////////////////////////////////
// Connecting to WiFi Router//////////////////////////////////////////////////////////////////////////////
void setup_wifi(){
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Not Connected");
    delay(1000);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Connecting to MQTT broker
// Connecting to MQTT broker
void reconnect(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str() , username, pass)){
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(sub1);
      client.subscribe(sub2);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
