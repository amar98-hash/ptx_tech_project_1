#include <SPI.h>
#include "sensordef_80x64.h"



// SENSOR CHARACTERISTICS
struct characteristics {
  uint8_t number_row;    // number ows
  uint8_t number_col;    // number of column
  uint8_t number_blocks; // number of blocks (top + down)
  uint16_t number_pixel;  // number of pixel
};
characteristics sensor = {64, 80, 8, 5120};


// EEPROM DATA
uint8_t mbit_calib, bias_calib, clk_calib, bpa_calib, pu_calib, nrofdefpix, gradscale, vddscgrad, vddscoff, epsilon, arraytype;
int8_t globaloff;
uint8_t mbit_user, bias_user, clk_user, bpa_user, pu_user;
uint32_t id, ptatoff;
float ptatgr_float, ptatoff_float, pixcmin, pixcmax, bw;

uint8_t loop_itr;

uint8_t data_R, data_B, data_G, data_A, gray_scale;
uint32_t data;

// SENSOR DATA
//uint8_t data_bottom_block0[1282];
uint8_t data_top_block0[1282];

//int32_t data_pixel[64][80];
uint8_t statusreg;


uint16_t timer_duration;

int x;int i;


void setup() {

  Serial.begin(115200);

  // start the SPI library:
  SPI.begin();
  //SPI.setClockDivider(21);

  //serial read trigger
  pinMode(6, INPUT);

  x=0;i=0;




  // SET INPUTS/OUTPUS (for DUE set pin 11 - 13 to INPUT)
  pinMode(13, OUTPUT);  // CLK
  pinMode(12, INPUT);  // MISO
  pinMode(11, OUTPUT);  // MOSI
  pinMode(10, OUTPUT); // chip select


  // search sensor via ID
 // Serial.print("search device");
  while (id == 0 || id == 0xFFFFFFFF) {
    id = read_EEPROM_byte(E_ID4) << 24 | read_EEPROM_byte(E_ID3) << 16 | read_EEPROM_byte(E_ID2) << 8 | read_EEPROM_byte(E_ID1);
    //Serial.print(".");
    delay(200);
  }

 // Serial.print("\nread eeprom");

  //Serial.print("\nwake up sensor");

  // to wake up sensor set configuration register to 0x01
  // |  7  |  6  |  5  |  4  |   3   |    2     |   1   |    0   |
  // |    RFU    |   Block   | Start | VDD_MEAS | BLIND | WAKEUP |
  // |  0  |  0  |  0  |  0  |   0   |    0     |   0   |    1   |
  write_sensor_byte(CONFIGURATION_REGISTER, 0x01);

  //Serial.print("\ninitialization");
  write_calibration_settings_to_sensor();
  //Serial.print("\nstart sensor");
  // to start sensor set configuration register to 0x09
  // |  7  |  6  |  5  |  4  |   3   |    2     |   1   |    0   |
  // |    RFU    |   Block   | Start | VDD_MEAS | BLIND | WAKEUP |
  // |  0  |  0  |  0  |  0  |   1   |    0     |   0   |    1   |
  write_sensor_byte(CONFIGURATION_REGISTER, 0x09);



  timer_duration = calc_timer_duration(bw, clk_calib, mbit_calib);
}

/********************************************************************
   Function:        void loop()

   Description:

   Dependencies:
 *******************************************************************/
//

void loop() {

  //Serial.println("a");

  read_data();  
//Serial.println(i);
//else
  //Serial.println("Pull Pin 6 high");  
  exit(1);

}
void read_data(){
  // --- BLOCK 0 with PTAT ---

  // change block in configuration register (to block 0)
  // |  7  |  6  |  5  |  4  |   3   |    2     |   1   |    0   |
  // |    RFU    |   Block   | Start | VDD_MEAS | BLIND | WAKEUP |
  // |  0  |  0  |  0  |  0  |   1   |    0     |   0   |    1   |
  write_sensor_byte(CONFIGURATION_REGISTER, 0x09 );


  // wait for end of conversion bit (~27ms)
  delayMicroseconds(timer_duration); // poll when 90% done
  statusreg = read_sensor_register( STATUS_REGISTER);
  while (bitRead(statusreg, 0) == 0) {
    statusreg = read_sensor_register( STATUS_REGISTER);
  }
  // get data of top half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(TOP_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor

  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data=data_top_block0[2 * n + 2 + m * 160] << 8 | data_top_block0[2 * n + 3 + m * 160];
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");

    }
    Serial.println();
  }



  // get data of bottom half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(BOTTOM_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor
  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data=data_top_block0[2 * n + 2 + m * 160] << 8 | data_top_block0[2 * n + 3 + m * 160];
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");
      

    }
    Serial.println();
  }
  

  // --- BLOCK 1 with PTAT --- 

  // change block in configuration register (to block 1)
  //it for end of conversion bit (~27ms)
  delayMicroseconds(timer_duration); // poll when 90% done
  statusreg = read_sensor_register( STATUS_REGISTER);
  while (bitRead(statusreg, 0) == 0) {
    statusreg = read_sensor_register( STATUS_REGISTER);
  }
  // get data of top half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(TOP_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor

  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data=data_top_block0[2 * n + 2 + m * 160] << 8 | data_top_block0[2 * n + 3 + m * 160];
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");

    }
    Serial.println();
  }
  
  // get data of bottom half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(BOTTOM_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor

  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");

    }
    Serial.println();
  }

  // --- BLOCK 2 with PTAT ---

  // change block in configuration register (to block 2)
  // |  7  |  6  |  5  |  4  |   3   |    2     |   1   |    0   |
  // |    RFU    |   Block   | Start | VDD_MEAS | BLIND | WAKEUP |
  // |  0  |  0  |  1  |  0  |   1   |    0     |   0   |    1   |
  write_sensor_byte(CONFIGURATION_REGISTER, 0x29 );

  // wait for end of conversion bit (~27ms)
  delayMicroseconds(timer_duration); // poll when 90% done
  statusreg = read_sensor_register( STATUS_REGISTER);
  while (bitRead(statusreg, 0) == 0) {
    statusreg = read_sensor_register( STATUS_REGISTER);
  }
  // get data of top half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(TOP_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor

  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");
    }
    Serial.println();
  }


  // get data of bottom half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(BOTTOM_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor

  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");
    }
    Serial.println();
  }
  


  // --- BLOCK 3 with PTAT ---

  // change block in configuration register (to block 3)
  // |  7  |  6  |  5  |  4  |   3   |    2     |   1   |    0   |
  // |    RFU    |   Block   | Start | VDD_MEAS | BLIND | WAKEUP |
  // |  0  |  0  |  1  |  1  |   1   |    0     |   0   |    1   |
  write_sensor_byte(CONFIGURATION_REGISTER, 0x39 );

  // wait for end of conversion bit (~27ms)
  delayMicroseconds(timer_duration); // poll when 90% done
  statusreg = read_sensor_register( STATUS_REGISTER);
  while (bitRead(statusreg, 0) == 0) {
    statusreg = read_sensor_register( STATUS_REGISTER);
  }
  // get data of top half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(TOP_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor
   for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");
    }
    Serial.println();
  }


  // get data of bottom half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(BOTTOM_HALF);  // read command
  SPI.transfer(data_top_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor

  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < sensor.number_col; n++) {
      // --- PIXEL DATA TOP HALF --
      // block 0
      data_R=0xFF& data;

      data_B=0xFF00&data;
      data_G=0xFF0000&data;
      data_A=0xFF000000&data;

      gray_scale=(data_R+data_B+data_G)/3.0;

      Serial.print(gray_scale);
      Serial.print(" ");
    }
    Serial.println();
  }

  //saving data
  delay(10);




}



/********************************************************************
   Function:      calc_timer_duration(float bw, uint8_t clk, uint8_t mbit)

   Description:   calculate the duration of the timer which reads the sensor blocks

   Dependencies:  band width (bw)
                  clock (clk)
                  adc resolution (mbit)
 *******************************************************************/
word calc_timer_duration(float bw, uint8_t clk, uint8_t mbit) {
  float Fclk_float = 12000000 / 63 * clk + 1000000;    // calc clk in Hz
  float a, b, c;
  uint16_t calculated_timer_duration;
  a = 1 / NORM_BW;
  b = 32 * (pow(2, mbit & 0b00001111) + 4) / Fclk_float;
  c = b / a;
  c = c / bw;
  c = SAFETY_FAC * c;

  calculated_timer_duration = c * 1000000; // c in s | timer_duration in µs

  return calculated_timer_duration;
}







/********************************************************************
   Function:        void read_pixel_data()

   Description:     read 2 complete pictures (first with ptat, second with vdd) and electrical Offset

   Dependencies:
 *******************************************************************/
/*void read_pixel_data() {


  // --- BLOCK 0 with PTAT ---

  // change block in configuration register (to block 0)
  // |  7  |  6  |  5  |  4  |   3   |    2     |   1   |    0   |
  // |    RFU    |   Block   | Start | VDD_MEAS | BLIND | WAKEUP |
  // |  0  |  0  |  0  |  0  |   1   |    0     |   0   |    1   |
  write_sensor_byte(CONFIGURATION_REGISTER, 0x09 );


  // wait for end of conversion bit (~27ms)
  delayMicroseconds(timer_duration); // poll when 90% done
  statusreg = read_sensor_register( STATUS_REGISTER);
  while (bitRead(statusreg, 0) == 0) {
    statusreg = read_sensor_register( STATUS_REGISTER);
  }
  // get data of bottom half:
  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(BOTTOM_HALF);  // read command
  SPI.transfer(data_bottom_block0, 1282); // receive data
  digitalWrite(10, LOW); // set HIGH, back to sensor

}

*/



/********************************************************************
   Function:        void write_calibration_settings_to_sensor()

   Description:     write calibration data (from eeprom) to trim registers (sensor)

   Dependencies:
 *******************************************************************/
void write_calibration_settings_to_sensor() {

  write_sensor_byte(TRIM_REGISTER1, mbit_calib);
  delay(5);
  write_sensor_byte(TRIM_REGISTER2, bias_calib);
  delay(5);
  write_sensor_byte(TRIM_REGISTER3, bias_calib);
  delay(5);
  write_sensor_byte(TRIM_REGISTER4, clk_calib);
  delay(5);
  write_sensor_byte(TRIM_REGISTER5, bpa_calib);
  delay(5);
  write_sensor_byte(TRIM_REGISTER6, bpa_calib);
  delay(5);
  write_sensor_byte(TRIM_REGISTER7, pu_calib);
}



/********************************************************************
   Function:        void read_EEPROM_byte(unsigned int eeaddress )

   Description:     read eeprom register

   Dependencies:    register address (address)
 *******************************************************************/
byte read_EEPROM_byte(unsigned int address ) {
  byte rdata = 0xFF;
  digitalWrite(10, LOW);  // set LOW to communicate with eeprom
  SPI.transfer(0b00000011); // READ command eeprom
  SPI.transfer16(address);  // register address eeprom
  rdata = SPI.transfer(0xFF); // end of message
  digitalWrite(10, HIGH); // set HIGH, back to sensor

  return rdata;
}




/********************************************************************
   Function:        void write_sensor_register( uint16_t addr)

   Description:     write to sensor register

   Dependencies:    register address (addr),
                    number of bytes (n)
 *******************************************************************/
byte write_sensor_byte( uint8_t addr, uint8_t input) {

  digitalWrite(10, HIGH);  // set HIGH to communicate with Sensor
  SPI.transfer(addr);  // register address eeprom
  SPI.transfer(input);
  SPI.transfer(0xFF);// end of message
  digitalWrite(10, LOW); // set LOW, back to eeprom
}




/********************************************************************
   Function:        void read_sensor_register( uint16_t addr)

   Description:     read sensor register

   Dependencies:    register address (addr),
 *******************************************************************/
byte read_sensor_register( uint8_t addr) {
  byte rdata;

  digitalWrite(10, HIGH);  // set HIGH to communicate with sensor
  SPI.transfer(addr);  // command
  rdata = SPI.transfer(0xFF);// end of message
  digitalWrite(10, LOW); // set Low, back to eeprom

  return rdata;

}
