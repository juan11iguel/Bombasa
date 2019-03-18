//Programa para el control de la bomba en bucle abierto

#include <Arduino.h>
#include <TM1637Display.h>


const int Vmax = 20;

// Se asocian los pines con su función
//Motor
const int en = 3;
const int in1 = 7;
const int in2 = 4;

int velMotor = 0;

//Botones modo manual
const int botonUp = 13;
const int botonDown = 12;

int estado_botonDown = 0;
int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;
int ultimo_estadoDown = 0;

//Modo automático
const int Labjack = A0;
float valor;

//Pantalla 7 segmentos 4 dígitos
#define CLK 11
#define DIO 10

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

  pinMode(en, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);


display.setBrightness(0x0f); //Limpia la pantalla
}

void loop() {

  // ELECCIÓN MANUAL - AUTOMÁTICO
  // Inicialmente se toma el estado del interruptor y se ejecuta el modo MANUAL
  // o AUTO en función de su posición
  eleccion();

  //En cualquier momento el cambio del interruptor interrumpe el programa y
  // se vuelve a comprobar el estado para ejecutar el modo correspondiente




}
void eleccion() {
  estado_interruptor = digitalRead(interruptor);

  if (estado_interruptor == HIGH) {
    //Entonces el interruptor está en modo AUTOMÁTICO
    
    //                                                                          Serial.println("AUTOMATICO");
    
    automatico();
  } else {
    //Entonces el interruptor está en modo MANUAL
    manual();
  }
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

    
    //value = map(buttonPushCounter, 0, Vmax, 0, 255);
    //Serial.print("Lo que se le mete al motor: ");
    //Serial.println(value);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    value = map(buttonPushCounter, 0, 25.5, 0, 255);
    Serial.println(value);
    analogWrite(en, value);
}

void automatico() {
  int valor;


  if (analogRead(Labjack) > 0.1) {
    //Se toman las referencias a partir del Labjack
    valor = analogRead(Labjack); //0-5V
    Serial.println("Labjack funsionando papi");
    Serial.println();
    Serial.print("El valor introducido es: ");   
    
    Serial.println(valor);
    
    valor = map(valor, 0, 1024, 0, 255); 

    Serial.print("El valor introducido mapeado es: "); 
    Serial.println(valor);
    
    

    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    //valor = map(valor, 0, Vmax, 0, 255);
    analogWrite(en,valor);


  } else {

    // Serial.println("Estamos en serie bebé");
     //Se toman las entradas del puerto serie
  
  Serial.println("Introduzca un valor de 0 - 1024 (resolución de la salida analog) ");


  while (Serial.available() <= 0) {
  String str = Serial.readStringUntil('\n'); //lectura de la entrada de varios dígitos
  float valor = str.toFloat();
  // valor=Serial.read() - '0'; // lectura de un dígito en ASCII

  if (valor>0 && valor <=20){



    
    int entera = 0;
    int decimal = 0;
    //leemos la opcion enviada
    entera = (int)(valor);
    decimal = 10 * (valor - entera);

    Serial.println(entera);
    Serial.println(decimal);
    
    display.setBrightness(0x0f); 
    display.showNumberDec(entera,false,2,0); 
    display.setBrightness(0x0f); 
    display.showNumberDec(decimal,false,1,3);
    
   

    
    Serial.println();
    Serial.print("El valor introducido es: ");   
    
    Serial.println(valor);

    
      
    
    
    

    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    //valor = map(valor, 0, Vmax, 0, 255);
    analogWrite(en,valor);


  }

  }
  
    



}

}



//Para activar el motor hacia delante

//    digitalWrite(in1, HIGH);
//    digitalWrite(in2, LOW);
//    valor=map(valor, 0,Vmax, 0, 255)
//    AnalogWrite(valor, en)
