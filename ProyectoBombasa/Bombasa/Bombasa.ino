//Programa para el control de la bomba en bucle abierto

#include <Arduino.h>
#include <TM1637Display.h>


const int Vmax = 20;

// Se asocian los pines con su función
//Motor
const int en1 = 3;
const int in1 = 7;
const int in2 = 4;

//Elección
int ultimo_estado = 0;

//Modo manual
const int botonUp = 13;
const int botonDown = 12;
int estado_botonDown = 0;
int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;
int ultimo_estadoDown = 0;

//Modo automático
const int Labjack = A0;


//Pantalla 7 segmentos 4 dígitos
#define CLK 11
#define DIO 10

const uint8_t SEG_MAN[] = {
  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C,                    // M
  SEG_F | SEG_A | SEG_B | SEG_C | SEG_E,                    // 
  SEG_C | SEG_E | SEG_G | SEG_F | SEG_A | SEG_B,            // A
  SEG_F | SEG_A | SEG_E | SEG_F | SEG_B | SEG_C,            // N
};

const uint8_t SEG_AUTO[] = {
  SEG_C | SEG_E | SEG_G | SEG_F | SEG_A | SEG_B,            // A
  SEG_E | SEG_F | SEG_D | SEG_B | SEG_C,                    // U
  SEG_A | SEG_B | SEG_C,                                    // T
  SEG_A | SEG_E | SEG_F | SEG_B | SEG_C | SEG_D,            // O
};

const uint8_t SEG_OCHO[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
};

TM1637Display display(CLK, DIO);

//Interruptor MANUAL - AUTOMÁTICO
const int interruptor = 2;
int estado_interruptor = 0;

void setup() {
  // Asignación de entradas y salidas
  Serial.begin(9600);
  pinMode(interruptor, INPUT);
  pinMode(botonUp, INPUT);
  pinMode(botonDown, INPUT);

  pinMode(en1, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);


  display.clear(); //Limpia la pantalla
  display.setBrightness(0x0f); 
  display.setSegments(SEG_OCHO);
  delay(1);
}

void loop() {

  // ELECCIÓN MANUAL - AUTOMÁTICO
  // Inicialmente se toma el estado del interruptor y se ejecuta el modo MANUAL
  // o AUTO en función de su posición
  eleccion();
}





void eleccion() {

  
  estado_interruptor = digitalRead(interruptor);

  if (estado_interruptor == HIGH) {
    //Entonces el interruptor está en modo AUTOMÁTICO
    if(ultimo_estado != estado_interruptor){
      display.setSegments(SEG_AUTO);
    }                                                                       Serial.println("AUTOMATICO");
    
    automatico();
    
  } else {
    //Entonces el interruptor está en modo MANUAL
    manual();
    if(ultimo_estado != estado_interruptor){
      display.setSegments(SEG_MAN);
    }                          
  }
  
  ultimo_estado = estado_interruptor;
}





void manual() {
  int value = 0;
  
    // read the pushbutton input pin:
  buttonState = digitalRead(botonUp);
  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH && buttonPushCounter < Vmax) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      
      display.setBrightness(0x0f); 
      display.showNumberDec(buttonPushCounter,false);
      
    } 
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;


  estado_botonDown = digitalRead(botonDown);

  // compare the buttonState to its previous state
  if (estado_botonDown != ultimo_estadoDown) {
    // if the state has changed, increment the counter
    if (estado_botonDown == HIGH && buttonPushCounter > 0) {
      buttonPushCounter--;
      Serial.println("off");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      display.setBrightness(0x0f);
      display.showNumberDec(buttonPushCounter,false); 
    } 
    delay(50);    
  }
  // save the current state as the last state, for next time through the loop
  ultimo_estadoDown = estado_botonDown;

    
    MoverMotor(buttonPushCounter);
}





void automatico() {
  float valor = 0;
  int value = 0;
  int lastValue = 0;

  
  
  value = analogRead(Labjack);
  if (analogRead(Labjack) > 0.1 && value != lastValue) {
    //Se toman las referencias a partir del Labjack
    valor = analogRead(Labjack); //0-5V
    Serial.println("Labjack funsionando papi");
    Serial.println();
    Serial.print("El valor introducido es: ");   
    Serial.println(valor);
    
    MoverMotor(valor);

  } else {
  
    if (Serial.available() > 0) {
      Serial.println("Introduzca un valor de 0 - 1024 (resolución de la salida analog) ");
      String str = Serial.readStringUntil('\n'); //lectura de la entrada de varios dígitos
      float valor = str.toFloat();

      if (valor>0 && valor <=20){

        //Mover_motor
        MostrarPantalla(valor);
        MoverMotor(valor);


    
        Serial.println();
        Serial.print("El valor introducido es: ");   
    
        Serial.println(valor);
      }
    }
  }

}



void MostrarPantalla(float valor){

  int entera = 0;
  int decimal = 0;

  //Ajuste de número real para separarlo en dos enteros de parte real e imaginaria
  entera = (int)(valor);
  decimal = 100 * (valor - entera); 

  //Representación en pantalla del número real
    display.clear(); //Limpia la pantalla
    display.setBrightness(0x0f); 
    display.showNumberDecEx(entera,(0x80 >> 1),false,2,0); 
    display.showNumberDecEx(decimal,(0x80 >> 1),false,2,2); 
}


void MoverMotor(float valor){

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  valor = map(valor, 0, Vmax, 0, 255);
  analogWrite(en1,valor);
  
}
