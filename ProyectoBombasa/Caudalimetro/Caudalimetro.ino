
//  Programa para el control de la bomba en bucle cerrado

//  Inclusión de todas las librerías necesarias
#include <Arduino.h>
#include <TM1637Display.h>
#include <PID_v1.h>
/////////////////////////////////////////////

const int Vmax = 20;

// Se asocian los pines con su función
//  Motor
const int en1 = 3;      // PWM  Salida (0 - 255) bits -> (0 - 5) V
const int in1 = 7;      // Entrada lógica del driver para controlar el sentido de giro del motor
const int in2 = 4;      // Entrada lógica del driver para controlar el sentido de giro del motor

//  Elección
int ultimo_estado = 0;

//  Modo manual
const int botonUp = 13;     // Boton Subir (el de al lado de la pantalla)
const int botonDown = 12;   // Boton Bajar (el de al lado de la pantalla)
int estado_botonDown = 0;   // Estado actual del botón bajar
int buttonState = 0;        // Estado actual del botón subir
int lastButtonState = 0;    // Estado anterior del botón subir
int ultimo_estadoDown = 0;  // Estado anterior del botón bajar
int buttonPushCounter = 0;  // Contador de número de pulsaciones (subir suma, bajar resta)

//  Modo automático
const int Labjack = A4;     // Entrada del Labjack para controlar el motor (0-1023) bits -> (0 - 5) V


//Pantalla 7 segmentos 4 dígitos/////////////////////////////////
/////////////////////////////////////////////////////////////////
#define CLK 11                                              /////
#define DIO 10                                              /////
                                                            /////
const uint8_t SEG_MAN[] = {                                 /////
  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C,                    // M
  SEG_F | SEG_A | SEG_B | SEG_C | SEG_E,                    // 
  SEG_C | SEG_E | SEG_G | SEG_F | SEG_A | SEG_B,            // A
  SEG_F | SEG_A | SEG_E | SEG_F | SEG_B | SEG_C,            // N
};                                                          /////
                                                            /////
const uint8_t SEG_AUTO[] = {                                /////
  SEG_C | SEG_E | SEG_G | SEG_F | SEG_A | SEG_B,            // A
  SEG_E | SEG_F | SEG_D | SEG_B | SEG_C,                    // U
  SEG_A | SEG_B | SEG_C,                                    // T
  SEG_A | SEG_E | SEG_F | SEG_B | SEG_C | SEG_D,            // O
};                                                          /////
                                                            /////
const uint8_t SEG_OCHO[] = {                                /////
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
};                                                          /////
                                                            /////
TM1637Display display(CLK, DIO);                            /////
/////////////////////////////////////////////////////////////////

//  Interruptor MANUAL - AUTOMÁTICO
const int interruptor = 2;
int estado_interruptor = 0;


//  Caudalímetro Contador
int caudalimetro = 8;

//  Variables para la función leer caudal
int contador = 0;
int ultimo_estadoo = 0;
int estado;
int tiempo;
int ultimo_tiempo = 0;
float f;
int conteo = 0;
int variacion;


//  Lectura Voltaje
int lectura_Voltaje = A5;
float V;


//  Sólo se ejecuta una vez, establece la configuración inicial
void setup() {
  // Asignación de entradas y salidas
  Serial.begin(9600);
  pinMode(interruptor, INPUT);
  pinMode(botonUp, INPUT);
  pinMode(botonDown, INPUT);
  pinMode(caudalimetro, INPUT);

  pinMode(en1, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);


  display.clear(); //Limpia la pantalla
  display.setBrightness(0x0f); 
  display.setSegments(SEG_OCHO);
  delay(1);
  
  if(digitalRead(interruptor == 1)){    //Para que si el sistema se inicia en modo AUTO
     ultimo_estado = 1;                 //el ultimo_estado lo refleje y represente el 8888.
  }
}


// Se ejecuta continuamente, se incluyen aquí todas las tareas a realizar
void loop() {

  // ELECCIÓN MANUAL - AUTOMÁTICO
  eleccion();
  //f=leerCaudal();
  
  leerVoltaje();
  
}




//  Procedimiento que lee el estado del interruptor y lleva el programa al modo AUTO o MANUAL
void eleccion() {

  
  estado_interruptor = digitalRead(interruptor);

  if (estado_interruptor == HIGH) {
    //Entonces el interruptor está en modo AUTOMÁTICO
    if(ultimo_estado != estado_interruptor){
      display.setSegments(SEG_AUTO);
    }                                                             
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




// Subprograma que contiene el modo MANUAL
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




//  Subprograma que contiene el modo automático
void automatico() {
  float valor = 0;  // valor de salida Analógica para motor1. Inicializar
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

      if (valor>=0 && valor <=100){

        //Mover_motor
        MostrarPantalla(valor);
        MoverMotorSerial(valor);


    
        Serial.println();
        Serial.print("El valor introducido es: ");   
    
        Serial.println(valor);
      }
    }
  }

}



//  Subprograma para la representación en pantalla de números reales con hasta dos decimales
void MostrarPantalla(float valor){

  int entera = 0;
  int decimal = 0;

  //Ajuste de número real para separarlo en dos enteros de parte real e imaginaria
  entera = (int)(valor);
  decimal = round(100 * (valor - entera)); 

  //Representación en pantalla del número real
    display.clear(); //Limpia la pantalla
    display.setBrightness(0x0f); 
    display.showNumberDecEx(entera,(0x80 >> 1),false,2,0); 
    display.showNumberDecEx(decimal,(0x80 >> 1),false,2,2); 
}


//  Subprograma para el movimiento del motor en modo manual
void MoverMotor(float valor){

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  if(valor == 0){
    analogWrite(en1,valor);
  }else{
    valor = map(valor, 0, 27.4, 4.4, 22);
    valor = map(valor, 4.4, 25.4, 37.4, 255);
    analogWrite(en1,valor);
  }

  
//  Subprograma para el movimiento del motor en modo automático, por el puerto serie
}

void MoverMotorSerial(float valor){

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

    valor = map(valor, 0, 100, 0, 28);
    valor = map(valor, 0, 28, 0, 255);
    analogWrite(en1,valor);
  
  
}


//  Función para realizar la lectura del caudalímetro contador y asociarla al caudal correspondiente
float leerCaudal(){
  float frecuencia;
  

  tiempo=millis();

  estado=digitalRead(caudalimetro);

  
  if(estado == HIGH && estado != ultimo_estadoo){
    Serial.println("Hola");
    
    variacion = tiempo - ultimo_tiempo;
    frecuencia = 1/variacion;

    Serial.print("freq:");
    Serial.println(frecuencia);
    Serial.print("estado: ");
    Serial.println(estado);
    Serial.print("contador: ");
    Serial.println(contador);
    contador++;

  }
  ultimo_estadoo = estado;
  ultimo_tiempo = tiempo;
  
  
  return frecuencia;
  
}

//  Subprograma que lee el voltaje enviado el motor, a través del divisor de tensión para adaptarla
//  al rango de entrada del puerto analógico del arduino

void leerVoltaje(){
int voltaje[5];

  tiempo=millis();

  if(tiempo - ultimo_tiempo > 10){

      V=analogRead(A5);

      if( V != 0 && V != 1023){
       
      }

  if(tiempo - ultimo_tiempo > 1000){
    
  }
      
      if( V != 0 && V != 1023){
        Serial.print("Voltaje enviado al motor: ");
        Serial.println(V);
      
      }
      
//(V <800 || V> 850)&&
      ultimo_tiempo = tiempo;
  }

}
