
#include <SPI.h>
#include <mcp_can.h>
#include <EEPROM.h>

const int SPI_CS_PIN = 10;//pin 9 en modulo 1 relay Atmega328p
const int interrupcion = 9;//pin 5 en modulo 1 relay Atmega328p
const int LED=2;//pin 8 en modulo 1 relay Atmega328p

const int Relay_1=A0;//digital
const int Relay_2=A2;//digital

const int Acs712_1=A1;//solo ADC
const int Acs712_2=A3;//solo ADC

unsigned char canId;
unsigned char ID_Local=0x03;
unsigned char ID_Master=0x02;

unsigned char MsgUpOk[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char MsgUpEEprom[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char MsgLeido[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char MsgAcs712[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char  reg_control=0x00;
unsigned char  reg_config=0x00;
unsigned char  reg_5=0x00;
unsigned char  reg_4=0x00;
unsigned char  reg_3=0x00;
unsigned char  reg_2=0x00;
unsigned char  reg_1=0x00;
unsigned char  reg_0=0x00;
unsigned char len = 0;
unsigned char buf[8];

int acs_1,acs_2;



MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(9600);

  // EEPROM.begin(0x1024);

    EEPROM.write(0x00,ID_Local);// escribe en la dir 0x00 el id del dispositivo LOCAL
    EEPROM.write(0x01,ID_Master);// escribe en la dir 0x01 el id del MASTER controlador


   // ID_Local= EEPROM.read(0x00);    // almaceno el Id del receptor
   // ID_Master= EEPROM.read(0x01);

    MsgUpEEprom[0]=ID_Local;
    MsgUpEEprom[1]=ID_Master;
    
    pinMode(LED,OUTPUT);
   digitalWrite(LED,true);
    pinMode(Relay_1,OUTPUT);
    digitalWrite(Relay_1,true);
    pinMode(Relay_2,OUTPUT);
    digitalWrite(Relay_2,true);
    pinMode(Acs712_1,INPUT);
    pinMode(Acs712_2,INPUT);
// PRUEBA INICIO RELAYS //
    digitalWrite(Relay_1,false);
    digitalWrite(LED,false);
    delay(200);
    digitalWrite(Relay_1,true);
    digitalWrite(LED,true);
    delay(200);
    digitalWrite(Relay_2,false);
    digitalWrite(LED,false);
    delay(200);
    digitalWrite(Relay_2,true);
    digitalWrite(LED,true);
       
    pinMode(interrupcion ,INPUT);

   // ID_Local=EEPROM.read(0x00);
  //  ID_Master=EEPROM.read(0x01);
  
START_INIT:

    if(CAN_OK == CAN.begin(CAN_250KBPS,MCP_16MHz))                 
        {
          CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
          Led_CanUpOK();
         }
    else
        {
        Led_CanFail();
          goto START_INIT;
          }
 //interrupts();                 // Autoriza las interrupciones

   
}

void loop()
{   


      
    if(!digitalRead(interrupcion))            // check if data coming
    
    {   

////////////////////////////////////    
      Led_Blink(1);
      
      // noInterrupts();               // Suspende las interrupciones
          CAN.readMsgBuf(&len, buf);    // leo el mensaje recibido
          canId = CAN.getCanId();       // almaceno el Id del emisor
         // ID_Local= EEPROM.read(0x00);    // leo el Id del receptor de la EEPROM
        //  ID_Master= EEPROM.read(0x01);
          int ID=int(ID_Local);
        
          MsgLeido[7]=buf[7];MsgLeido[6]=buf[6];MsgLeido[5]=buf[5];MsgLeido[4]=buf[4];
          MsgLeido[3]=buf[3];MsgLeido[2]=buf[2];MsgLeido[1]=buf[1];MsgLeido[0]=buf[0];

        if( canId==0x00){// MAster  broadcast 0xFF para que todos los ID LOCALEs publiquen su info
          //  Led_mensaje_recibido_blink();
          
////////////////////////////////////    
 
         Led_Blink(2);
             if(MsgLeido[0]==0xFF){
         
////////////////////////////////////    
 
         Led_Blink(3);      
            if(MsgLeido[7]==ID_Local){
 
////////////////////////////////////    
         Led_Blink(4);
              EEPROM.write(0x00, MsgLeido[6]);// escribe en la dir 0x00 el id del dispositivo LOCAL
              EEPROM.write(0x01, MsgLeido[5]);// escribe en la dir 0x01 el id del MASTER controlador
              ID_Local= EEPROM.read(0x00);    // almaceno el Id del receptor
              ID_Master= EEPROM.read(0x01);
              MsgUpEEprom[0]=ID_Local;
              MsgUpEEprom[1]=ID_Master;
              CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
        //      Led_grabacion_3();}
            }
           }
       
       if( canId==0xFF){// MAster  broadcast 0xFF para que todos los ID LOCALEs publiquen su info
        //   Led_mensaje_recibido_blink();
       Led_Blink(5);
           CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
             
        
      }
       else{        
            if( canId == ID_Master){  // su el Master coinside con el emisor
             // Led_mensaje_recibido_blink();
              
////////////////////////////////////    
         Led_Blink(6);
    
              if(0x00==MsgLeido[0]){    //si el control es 00 solo envia su info el local
                 CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
                  
////////////////////////////////////    
         Led_Blink(7);
    
                 }
              if(MsgLeido[0]==0x01){  
              
                if(MsgLeido[1]==0x00){
                  digitalWrite(Relay_1,true);
                  digitalWrite(Relay_2,true);}
                if(MsgLeido[1]==0x01){
                  digitalWrite(Relay_1,false);
                  digitalWrite(Relay_2,true);}
                if(MsgLeido[1]==0x10){
                  digitalWrite(Relay_1,true);
                  digitalWrite(Relay_2,false);}
                if(MsgLeido[1]==0x11){
                  digitalWrite(Relay_1,false);
                  digitalWrite(Relay_2,false); }
             //   CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
              
                }
            }

           if(MsgLeido[0]==0xFF){
             
////////////////////////////////////    
         Led_Blink(4);
              if(MsgLeido[7]==ID_Local){
              EEPROM.write(0x00, MsgLeido[6]);// escribe en la dir 0x00 el id del dispositivo LOCAL
              EEPROM.write(0x01, MsgLeido[5]);// escribe en la dir 0x01 el id del MASTER controlador
              ID_Local= EEPROM.read(0x00);    // almaceno el Id del receptor
              ID_Master= EEPROM.read(0x01);
              MsgUpEEprom[0]=ID_Local;
              MsgUpEEprom[1]=ID_Master;
              CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
              //Led_grabacion_3();}
            }
     
       }
      //  interrupts();                 // Autoriza las interrupciones
      }
        
    }


 void Led_grabacion_3(){
          digitalWrite(LED,false);
          delay(200);
          digitalWrite(LED,true);
          delay(200);
          digitalWrite(LED,false);
          delay(200);
          digitalWrite(LED,true);
          delay(200);
          digitalWrite(LED,false);
          delay(1000);
          digitalWrite(LED,true);
 
  }

 void Led_mensaje_recibido_blink(){
          digitalWrite(LED,false);
          delay(200);
          digitalWrite(LED,true);  
          delay(200);
  }
 void Led_Testigo(){
          digitalWrite(LED,false);
          delay(100);
          digitalWrite(LED,true);  
          delay(100);
          digitalWrite(LED,false);
          delay(100);
          digitalWrite(LED,true);  
          delay(100);
          digitalWrite(LED,false);
          delay(100);
          digitalWrite(LED,true);  
          delay(100);
  }

 void Led_0xFF_CanID(){
                digitalWrite(LED,false);
                delay(500);
                digitalWrite(LED,true);
                delay(500);
                digitalWrite(LED,false);
                delay(500);
                digitalWrite(LED,true);
                delay(500);
                digitalWrite(LED,false);
                delay(500);
                digitalWrite(LED,true);
                delay(500);
                digitalWrite(LED,false);
                delay(500);
                digitalWrite(LED,true);
                delay(500);
             
 }

 void Led_CanUpOK(){

           digitalWrite(LED,false);
           delay(200);
           digitalWrite(LED,true);
           delay(200);
           digitalWrite(LED,false);
           delay(200);
           digitalWrite(LED,true);
       
 }



 void Led_CanFail(){

           digitalWrite(LED,false);
           delay(200);
           digitalWrite(LED,true);
           delay(200);
           digitalWrite(LED,false);
           delay(200);
           digitalWrite(LED,true);
           delay(200);
           digitalWrite(LED,false);
           delay(1000);
           digitalWrite(LED,true);
           delay(1000);
         
  }

void Consumo_ACS712() {
 
  acs_1=analogRead(Acs712_1);
  MsgAcs712[0]= acs_1;
  acs_2=analogRead(Acs712_2);
  MsgAcs712[1]= acs_2;
  CAN.sendMsgBuf(ID_Local,0,8,MsgAcs712);
 
 }

//////////////////////////////////// prueba test

/*

 void Led_Blink(int c){

    for(int i=0;i=c;i++){
           digitalWrite(LED,false);
           delay(500);
           digitalWrite(LED,true);
           delay(2000);
    }
  }

*/
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/