#include <SD.h>
#include <SPI.h>


const int CS_PIN = 17; // Chip Select conectado al ADS1263
const int DRDY_PIN = 15;
const int LED = 25;
const int SD_CS = 14;

SPISettings ads1256SPI(2500000, MSBFIRST, SPI_MODE1);
SPISettings SD_Memory(25000000, MSBFIRST, SPI_MODE1);


String escrito = "";
long r_max=2147483647;
int lora_status=0;
int bits[26];
float voltaje[30000];
bool DIF1=LOW;
bool DIF2=LOW;
bool AC1=LOW;
bool AC2=LOW;
bool AC3=LOW;
bool AC4=LOW;
bool DC1=LOW;
bool DC2=LOW;
bool FILT1=LOW;
bool FILT3=LOW;
bool modo_cont=LOW;
bool modo_alm=LOW;
bool operar=LOW;
int32_t muestras=50;
long resolucion=0;
float vref=2.5;
byte bytes[4];

String direccion="";

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  SPI.begin();
  if (!SD.begin(SD_CS)) {
    Serial.println("Fallo al inicializar la tarjeta SD.");
    while (1);
  }
  else{
    Serial.println("Tarjeta inicializada");
  }
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  delay(1000);
  ADS_CONFI();
  delay(100); // Dar tiempo al ADS1256 para arrancar
  delayMicroseconds(10);
  byte valor_registro=readRegister(0x01);
  Serial.print("Valor de POWER: 0x");
  Serial.println(valor_registro, HEX);
  valor_registro=readRegister(0x02);
  Serial.print("Valor de INTERFACE: 0x");
  Serial.println(valor_registro, HEX);
  valor_registro=readRegister(0x03);
  Serial.print("Valor de MODE0: 0x");
  Serial.println(valor_registro, HEX);
  valor_registro=readRegister(0x04);
  Serial.print("Valor de MODE1: 0x");
  Serial.println(valor_registro, HEX);
  valor_registro=readRegister(0x05);
  Serial.print("Valor de MODE2: 0x");
  Serial.println(valor_registro, HEX);
  valor_registro=readRegister(0x06);
  Serial.print("Valor de INPMUX: 0x");
  Serial.println(valor_registro, HEX);
  valor_registro=readRegister(0x0F);
  Serial.print("Valor de REFMUX: 0x");
  Serial.println(valor_registro, HEX);
  while (digitalRead(DRDY_PIN) == HIGH);
  long dummy=lectura();
  delay(1000);//tiempo para que se acomode el bastardo
}

void loop() {
  confi_lora();
  while(lora_status==1){
    recepcion();
    if(operar==HIGH){
      if(modo_cont==HIGH){
        if(AC1==HIGH || DIF1==HIGH){
          writeRegister(0x06,0b00001010);
          for(int i=0;i<muestras;i++){
            while (digitalRead(DRDY_PIN) == HIGH);
            voltaje[i] = ((float)lectura() / r_max) * vref;
          }
          if(modo_alm==HIGH){
            escribir_sd_control("AC1.txt");
          }
          else{
            transmi_cont();
          }
        }
        if(AC2==HIGH && DIF1==LOW){
          writeRegister(0x06,0b00011010);
          for(int i=0;i<muestras;i++){
            while (digitalRead(DRDY_PIN) == HIGH);
            voltaje[i] = ((float)lectura() / r_max) * vref;
          }
          if(modo_alm==HIGH){
            escribir_sd_control("AC2.txt");
          }
          else{
            transmi_cont();
          }
        }
        if(AC3==HIGH || DIF2==HIGH){
          writeRegister(0x06,0b00101010);
          for(int i=0;i<muestras;i++){
            while (digitalRead(DRDY_PIN) == HIGH);
            voltaje[i] = ((float)lectura() / r_max) * vref;
          }
          if(modo_alm==HIGH){
            escribir_sd_control("AC3.txt");
          }
          else{
            transmi_cont();
          }
        }
        if(AC4==HIGH && DIF2==LOW){
          writeRegister(0x06,0b00111010);
          for(int i=0;i<muestras;i++){
            while (digitalRead(DRDY_PIN) == HIGH);
            voltaje[i] = ((float)lectura() / r_max) * vref;
          }
          if(modo_alm==HIGH){
            escribir_sd_control("AC4.txt");
          }
          else{
            transmi_cont();
          }
        }
        if(DC1==HIGH){
          writeRegister(0x06,0b01001010);
          for(int i=0;i<muestras;i++){
            while (digitalRead(DRDY_PIN) == HIGH);
            voltaje[i] = ((float)lectura() / r_max) * vref;
          }
          if(modo_alm==HIGH){
            escribir_sd_control("DC1.txt");
          }
          else{
            transmi_cont();
          }
        }
        if(DC2==HIGH){
          writeRegister(0x06,0b01011010);
          for(int i=0;i<muestras;i++){
            while (digitalRead(DRDY_PIN) == HIGH);
            voltaje[i] = ((float)lectura() / r_max) * vref;
          }
          if(modo_alm==HIGH){
            escribir_sd_control("DC2.txt");
          }
          else{
            transmi_cont();
          }
        }
      }
      else{
        if(AC1==HIGH || DIF1==HIGH){
          writeRegister(0x06,0b00001010);
          voltaje[0] = ((float)lectura() / r_max) * vref;
          if(modo_alm==HIGH){
            escribir_sd_simple("AC1.txt");
          }
          else{
            transmi_simple();
          }
        }
        if(AC2==HIGH && DIF1==LOW){
          writeRegister(0x06,0b00011010);
          voltaje[0] = ((float)lectura() / r_max) * vref;
          if(modo_alm==HIGH){
            escribir_sd_simple("AC2.txt");
          }
          else{
            transmi_simple();
          }
        }
        if(AC3==HIGH || DIF2==HIGH){
          writeRegister(0x06,0b00101010);
          voltaje[0] = ((float)lectura() / r_max) * vref;
          if(modo_alm==HIGH){
            escribir_sd_simple("AC3.txt");
          }
          else{
            transmi_simple();
          }
        }
        if(AC4==HIGH && DIF2==LOW){
          writeRegister(0x06,0b00111010);
          voltaje[0] = ((float)lectura() / r_max) * vref;
          if(modo_alm==HIGH){
            escribir_sd_simple("AC4.txt");
          }
          else{
            transmi_simple();
          }
        }
        if(DC1==HIGH){
          writeRegister(0x06,0b01001010);
          voltaje[0] = ((float)lectura() / r_max) * vref;
          if(modo_alm==HIGH){
            escribir_sd_simple("DC1.txt");
          }
          else{
            transmi_simple();
          }
        }
        if(DC2==HIGH){
          writeRegister(0x06,0b01011010);
          voltaje[0] = ((float)lectura() / r_max) * vref;
          if(modo_alm==HIGH){
            escribir_sd_simple("DC2.txt");
          }
          else{
            transmi_simple();
          }
        }
      }
      operar=LOW;
    }
  }
}

void confi_lora(){
  while(lora_status==0){
      while (Serial1.available()) {
        char c = Serial1.read(); // Leer desde Serial1 (conectado al RYLR998)
        if (c == '\n') {
          int primeracoma = escrito.indexOf(',');
          int igual = escrito.indexOf('=');
          direccion=escrito.substring(igual+1,primeracoma);
          escrito = ""; // Limpiar para la siguiente entrada
          lora_status=1;
          Serial.println("Acaba de recibir señal");
        } else if (c != '\r') {
          escrito += c; // Evitar el retorno de carro
        }
      }
  }
  Serial.println("LoRa inicializado");
  
}

void recepcion(){ 
  while (Serial1.available()) {
      char c = Serial1.read(); // Leer desde Serial1 (conectado al RYLR998)
      if (c == '\n') {
        int primeracoma = escrito.indexOf(',');
        int segundacoma = escrito.indexOf(',', primeracoma + 1);
        int terceracoma = escrito.indexOf(',', segundacoma + 1);
        int igual = escrito.indexOf('=');
        String direccion_mensaje=escrito.substring(igual+1,primeracoma);
        if (direccion_mensaje==direccion){
          String extraido=escrito.substring(segundacoma+1,terceracoma);
          if(extraido=="x"){
            operar=HIGH;
            Serial.println("operando");
          }
          else{
            int32_t numero=extraido.toInt();
            for (int i = 0; i < 25; i++) {
            bits[i] = (numero >> i) & 1;
            }
            decodificacion();
            Serial.println("configurado");
          }
        }
        escrito = "";
      }else if (c != '\r') {
        escrito += c; // Evitar el retorno de carro
      }
  }
}

void decodificacion(){
  int bytemode2=(bits[6] << 6) | (bits[5] << 5) | (bits[4] << 4) | (bits[3] << 3) | (bits[2] << 2) | (bits[1] << 1) | bits[0];
  writeRegister(0x05, bytemode2);
  int valor_pga=(bits[6] << 2) | (bits[5] << 1) | bits[4];
  switch (valor_pga){
    case 0:
      vref=2.5;
      break;
    case 1:
      vref=2.5/2;
      break;
    case 2:
      vref=2.5/4;
      break;
    case 3:
      vref=2.5/8;
      break;
    case 4:
      vref=2.5/16;
      break;
    case 5:
      vref=2.5/32;
      break;
  }
  DIF1=bits[7];
  DIF2=bits[8];
  AC1=bits[9];
  AC2=bits[10];
  AC3=bits[11];
  AC4=bits[12];
  DC1=bits[13];
  DC2=bits[14];
  FILT1=bits[15];
  FILT3=bits[16];
  modo_cont=bits[17];
  modo_alm=bits[21];
  if(bits[17]){
    int byte_muestras=(bits[20] << 2) | (bits[19] << 1) | bits[18];
    switch (byte_muestras){
      case 0:
        muestras=50;
        break;
      case 1:
        muestras=250;
        break;
      case 2:
        muestras=1000;
        break;
      case 3:
        muestras=2000;
        break;
      case 4:
        muestras=5000;
        break;
      case 5:
        muestras=10000;
        break;
      case 6:
        muestras=15000;
        break;
      case 7:
        muestras=30000;
        break;
      
    }
  }
  delay(1);

}

void ADS_CONFI(){
  comando(0x06); //RESET
  delay(10);
  comando(0x16);//System offset calibration
  delay(10);
  comando(0x17);//System gain calibration
  delay(10);
  comando(0x19);//System self calibration
  delay(10);
  //Configura registro POWER
  writeRegister(0x01,0b00010001);//REALIZAR REINICIO MANUAL, AINCOM 0V, REFERENCIA INTERNA HABILITADA
  //Configura registro INTERFACE
  writeRegister(0x02,0b00000000);//NO BYTE STATUS, NO BYTE CHECKSUM
  //Configura registro MODE0
  writeRegister(0x03,0b00000000);//NO CHOP, NO TIEMPO DELAY
  //Configura MODE1
  writeRegister(0x04,0b00000000);//FILTRO SINC1, USANDO ADC1
  //Configura registro MODE2
  writeRegister(0x05,0b00000000);//PGA HABILITADO CON 1, 20SPS
  //Configura registro INPMUX
  writeRegister(0x06,0b10011010);//AIN9-AINCOM PARA CALIBRACION INICIAL
  //Configura registro REFMUX
  writeRegister(0x0F,0b00000000);//Voltaje de referencia interno +-2.5v
  comando(0x16);//System offset calibration
  delay(10);
}

void comando(byte cmd){
  digitalWrite(CS_PIN, LOW);
  SPI.beginTransaction(ads1256SPI);
  SPI.transfer(cmd);
  SPI.endTransaction();
  digitalWrite(CS_PIN, HIGH);
  delayMicroseconds(10);
}

void writeRegister(byte reg, byte value) {
  digitalWrite(CS_PIN, LOW);
  SPI.beginTransaction(ads1256SPI);
  SPI.transfer(0x40 | reg); // Comando WREG + dirección
  SPI.transfer(0x00);       // 0x00 significa solo modificar un registro
  SPI.transfer(value);      // valor en bits de la configuracion del registro

  delayMicroseconds(0.1);     // Tiempo de espera recomendado
  SPI.endTransaction();
  digitalWrite(CS_PIN, HIGH);
}

byte readRegister(byte reg) {
  byte value;

  digitalWrite(CS_PIN, LOW);
  SPI.beginTransaction(ads1256SPI);
  SPI.transfer(0x20 | reg); // Comando RREG + dirección
  SPI.transfer(0x00);       // Leeremos 1 solo registro → 1 - 1 = 0

  delayMicroseconds(0.1);     // Tiempo de espera recomendado

  value = SPI.transfer(0xFF); // Dummy write para recibir el dato
  SPI.endTransaction();
  digitalWrite(CS_PIN, HIGH);

  return value;
}

long lectura(){
  long value=0;
  digitalWrite(CS_PIN, LOW);  // Activar el chip select
  SPI.transfer(0x13);
  byte byte1 = SPI.transfer(0x00);  // Leer el primer byte (MSB)
  byte byte2 = SPI.transfer(0x00);  // Leer el segundo byte
  byte byte3 = SPI.transfer(0x00);  // Leer el tercer byte
  byte byte4 = SPI.transfer(0x00);  // Leer el cuartor byte (LSB)
  
  digitalWrite(CS_PIN, HIGH);
  value = (long)byte1 << 24 |(long)byte2 << 16 | (long)byte3 << 8 | byte4;
  return value;
}

void escribir_sd_control(const char* nombre) {
  // Configura SPI con SPISettings: velocidad, bit order, y modo SPI
  SPI.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));

  File archivo = SD.open(nombre, FILE_WRITE);
  if (archivo) {
    archivo.print("V=[");
    for(int i=0;i<muestras;i++){
      float imprime=voltaje[i];
      archivo.print(voltaje[i],4);
      if(i==(muestras-1)){
        archivo.println("]");
      }
      else{
        archivo.print(",");
      }
    }     
  }
}

void escribir_sd_simple(const char* nombre) {
  SPI.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));

  File archivo = SD.open(nombre, FILE_WRITE);
  if (archivo) {
    archivo.print("V=[");
    archivo.print(voltaje[0],4);
    archivo.println("]");
  }
}

void transmi_cont(){
  delay(10);
  
  for(int i=0;i<muestras;i++){
    delay(60);
    String imprime=String(voltaje[i]);
    int tam=imprime.length();
    Serial1.println("AT+SEND="+direccion+","+String(tam)+","+imprime);
  }
}

void transmi_simple(){
  delay(1);
  String imprime=String(voltaje[0]);
  int tam=imprime.length();
  Serial1.println("AT+SEND="+direccion+","+String(tam)+","+imprime);
}