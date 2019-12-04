#define C4 0x30
#define Cs4 0x31
#define D4 0x32
#define E4 0x34
#define F4 0x35
#define G4 0x37
#define A4a 0x39
#define B4 0x3B

#define C5 0x3C
#define D5 0x3E
#define E5 0x40
#define F5 0x41
#define G5 0x43
#define A5a 0x45
#define As5 0x46
#define B5 0x47
#define C6 0x48
#define Cs6 0x49

#define laser_pin 7
#define num_readings 50

uint16_t threshold_sensor[8] = {0};
uint16_t sensor_data_sum[8] = {0};
uint8_t laser_string[8] = {C5, D5, E5, F5, G5, A5a, B5, C6};
uint8_t sel_pin[2] = {2, 3};//red 2: orange 3
uint8_t analog_pin[2] = {A0, A1};//yellow A1, brown A0
uint16_t sensor_data[8] = {0};
uint16_t threshold[8] = {512};
boolean prev_sensor_data[8] = {0}; //8 not proper
uint16_t sensor_analog_val[8] = {0};
//int prev_sensor_data = {0};
boolean calib = 0;
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  delay(1000);
  init_pins();
  if (!calib)
  {
    calibrate_harp();
  }


  //Serial.begin(115200);
}

void loop() {
  read_sensors();
  /*for (int k = 0 ; k < 8; k++)
    {
    Serial.print(sensor_data[k]);
    Serial.print("  ");
    }*/
  //Serial.println("  ");
  for (int i = 0; i < 8; i++)
  { //Serial.println("b-");
    if (prev_sensor_data[i] == 0 && sensor_data[i] == 1)
    {
      //Serial.println("Note ON");
      midi((0x90), laser_string[i], 127);

    }
    if (prev_sensor_data[i] == 1 && sensor_data[i] == 0)
    { //Serial.println("Note OFF");
      midi((0x80), laser_string[i], 127);
    }
    prev_sensor_data[i] = sensor_data[i];

  }
}

void midi(int cmd, int data1, int data2)
{
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}

void read_sensors()
{
  //if (calib)
    //delay(500);

  for (int i = 0; i < 4; i++)
  {
    //Serial.print("read sensor loop: ");
    //Serial.println(i);
    digitalWrite(sel_pin[0], i & 0x01 );
    digitalWrite(sel_pin[1], (i >> 1) & 0x01); //bitmasking
    //Serial.print((i >> 1) & 0x01);
    //Serial.print(i & 0x01);
    //Serial.println("");
    delay(1);
    sensor_data[i] = analogRead(analog_pin[0]);
    sensor_data[i + 4] = analogRead(analog_pin[1]);
    sensor_analog_val[i] = sensor_data[i];
    sensor_analog_val[i + 4] = sensor_data[i + 4];
    //Serial.print(sensor_data[i]);
    //Serial.print(" ");
    //Serial.print(sensor_data[i+1]);
    //Serial.println("hi ");
    if (calib)
    {
      if (sensor_data[i] < threshold[i])
        sensor_data[i] = 1;
      else
        sensor_data[i] = 0;
      if (sensor_data[i + 4] < threshold[i + 4])
        sensor_data[i + 4] = 1;
      else
        sensor_data[i + 4] = 0;

      if (sensor_data[i] && sensor_data[i + 4])
      {
        sensor_data[i] = 1;
        sensor_data[i+4] = 0;
      }  
    }
  }
  //Serial.println("Read Sensor complete");
  //Serial.println("Sensor Analog values are: ");
  for (int m = 0; m < 8; m++)
  {
    //Serial.print(sensor_analog_val[m]);
    //Serial.print(" ");
  }
  //Serial.println("");
  if (calib)
  {
    //Serial.println("Sensor digital values are: ");
    for (int m = 0; m < 8; m++)
    {
      //Serial.print(sensor_data[m]);
      //Serial.print(" ");
    }
    //Serial.println(" ");
  }

}
void init_pins()
{
  pinMode(laser_pin, OUTPUT);
  for (int i = 0; i < 2; i++)
  {
    pinMode(sel_pin[i], OUTPUT);
    pinMode(analog_pin[i], INPUT);
  }
  //Serial.println("Pins initialised");
}
void calibrate_harp()
{
  uint16_t min_reading[8] = {0};
  uint16_t max_reading[8] = {0};
  for (int i = 0; i < 8; i++)
  {
    min_reading[i] = 1023;
    max_reading[i] = 0;

  }
  //digitalWrite(laser_pin, LOW);
  //Serial.println("Going to read sensor");
  /*for (int j = 0; j < num_readings; j++)
    {
    //Serial.println("calib");
    read_sensors();
    for ( int i = 0; i < 8; i++)
    {
      if (sensor_data[i] < min_reading[i])
        min_reading[i] = sensor_data[i];
    }

    }
    //digitalWrite(laser_pin, HIGH);
    for (int j = 0; j < num_readings; j++)
    {
    read_sensors();
    for ( int i = 0; i < 8; i++)
    {
      if (sensor_data[i] > max_reading[i])
        max_reading[i] = sensor_data[i];
    }

    }*/

  for (int j = 0; j < num_readings; j++)
  {
    read_sensors();
    for (int i = 0; i < 8; i++)
      sensor_data_sum[i] += sensor_data[i];
      //delay(500);
  }

  for (int i = 0; i < 8; i++)
    threshold[i] = ((sensor_data_sum[i] / num_readings)-100);

  //Serial.println("FINAL THRESHOLD VALUES ARE");
  for (int i = 0; i < 8; i++)
  {
    //threshold[i] = (((min_reading[i] + max_reading[i]) / 2) - 100);
    //Serial.print(threshold[i]);
    //Serial.print(threshold[i]);
    //Serial.print("  ");
  }

  calib = 1;
  //Serial.println("");
  //Serial.println("CALIBRATION IS COMPLETE");
  delay(2000);
}
