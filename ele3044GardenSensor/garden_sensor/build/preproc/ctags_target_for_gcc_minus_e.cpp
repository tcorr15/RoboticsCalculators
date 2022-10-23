# 1 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp"
# 2 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2
# 3 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2
# 4 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2
# 5 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2
# 6 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2
# 7 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2
# 8 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2
VEML7700 myVEML;

// Code for the ESP32
// #include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)

// Code for the ESP8266
# 15 "c:\\Users\\Mark\\Desktop\\ele3044t2\\garden_sensor\\src\\main.cpp" 2




//Define values related to the moisture sensor





//DHT sensor variables


DHT_Unified dht(12, DHT11);
sensor_t sensor;

//Define whether each sensor is connected





// WiFi
const char* ssid = "BTHub6-SX5R"; // Your personal network SSID
const char* wifi_password = "xJV7FVJnei3g"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.1.65"; // IP of the MQTT broker
const char* message_topic = "home/message/test"; // Message location
const char* mqtt_username = "username"; // MQTT username
const char* mqtt_password = "test123"; // MQTT password
const char* clientID = "testNode"; // MQTT client ID
char c_buffer[8], f_buffer[8];
float temp = 0.0;
float moisture = 0.0;
float uv = 0.0;
float humid = 0.0;
float lux;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient);


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


int16_t get_temperature() {
  Wire.beginTransmission(0x48); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAA); // read temperature command
  Wire.endTransmission(false); // send repeated start condition
  Wire.requestFrom(0x48, 2); // request 2 bytes from DS1621 and release I2C bus at end of reading
  uint8_t t_msb = Wire.read(); // read temperature MSB register
  uint8_t t_lsb = Wire.read(); // read temperature LSB register

  // calculate full temperature (raw value)
  int16_t raw_t = (int8_t)t_msb << 1 | t_lsb >> 7;
  // convert raw temperature value to tenths °
  raw_t = raw_t * 10/2;
  return raw_t/10;
}

float get_humidity(){
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    //Return error
    return -1;
  }
  else {

    return event.relative_humidity;
  }
}

//Map function with decimal arithmetic
double fmap(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float get_moisture(){
  int moistureADC = analogRead(A0); //Read from ADC
  int constrainedADC = ((moistureADC)<(375)?(375):((moistureADC)>(661)?(661):(moistureADC))); //Constrain ADC to expected max/min
  float dryPercent = (float)fmap(constrainedADC, 375, 661, 0, 100); //Map ADC range to percentage
  float moisturePercent = 100 - dryPercent; //Convert from dryness % to moisture %

  //Ensure percentage is within expected range (update with error detection e.g., negative numbers etc.)
  if(moisturePercent > 100){
    moisturePercent = 100;
  }
  else if(moisturePercent < 0){
    moisturePercent = 0;
  }
  return moisturePercent;
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin(); // join i2c bus
  dht.begin(); //Initialise DHT sensor
  // initialize DS1621 sensor
  Wire.beginTransmission(0x48); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAC); // send configuration register address (Access Config)
  Wire.write(0); // perform continuous conversion
  Wire.beginTransmission(0x48); // send repeated start condition
  Wire.write(0xEE); // send start temperature conversion command
  Wire.endTransmission();
  Serial.begin(9600);
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);
  // temp = get_temperature();
  temp = rand() % 100;
  //uv = rand() % 100;

  //Get humidity
  if(true /*Define whether or not the humidity sensor is being used*/){
    humid = get_humidity();
  }
  else{
    humid = rand() % 100;
  }

  //Get ALS reading in lux
  if(true /*Define whether or not the ambient light sensor is being used*/){
    lux = myVEML.getAmbient();
  }
  else{
    lux = rand() % 100;
  }

  //Get soil moisture value
  if(true /*Define whether or not the moisture sensor is being used*/){
    moisture = get_moisture();
  }
  else{
    moisture = rand() % 100;
  }


  Serial.print("Message/" + String(12345) + "/" + String((float)temp) + "/" + String((float)moisture) + "/" + String((float)lux) + "/" + String((float)humid));

  // MQTT can only transmit strings
  String hs="Message/" + String(12345) + "/"+ String((float)temp) + "/" + String((float)moisture) + "/" + String((float)lux)+ "/" + String((float)humid);

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(message_topic, String(hs).c_str())) {
    Serial.println("Message sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(message_topic, String(hs).c_str());
  }
  client.disconnect(); // disconnect from the MQTT broker
  Serial.println("client disconnected successfully");
  delay(1000*60); // print new values every 1 Minute
}
