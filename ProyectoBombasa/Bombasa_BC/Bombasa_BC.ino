
//  Programa para el control de la bomba en bucle cerrado

//  Inclusión de todas las librerías necesarias
#include <Arduino.h>
#include <TM1637Display.h>
#include <PID_v1.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Definición e inicialización de variables
const int Vmax = 30;

// Se asocian los pines con su función
//  Motor
void MoverMotor(float valor);
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

//  Lectura Caudal
const int caudalimetro = 8;
int nmedidas = 0;   //Almacena el número de lecturas (100 lecturas)
int estadoo = 0;
int tiempoo = 0;
int ultimo_estadoo = 0;
int ultimo_tiempoo = 0;
float frecuencia;   //Variable que almacena la lectura instantánea del voltaje
float F[100];       //Almacena el valor de frecuencia para las nmedidas   
double Fmedia;      //Frecuencia media de las nlecturas

//  Lectura Voltaje
double leerVoltaje();
int tiempo;
int ultimo_tiempo;
int V;            //Variable que almacena la lectura instantánea del voltaje
int nlecturas=1;  //Almacena el número de lecturas (100 lecturas)
int voltaje[100]; //Almacena el valor de V para las nlecturas   
double Vmedio;    //Voltaje medio de las nlecturas

//  Controlador
double salida_V = 0; //Salida del sensor del voltaje enviado al motor
double salida_Q = 0; //Salida del sensor del caudal propulsado
double sc_V;         //Señal de control para el voltaje
double sc_Q;         //Señal de control para el caudal
double referencia_V; //Referecia para el voltaje
double referencia_Q; //Referencia para el caudal
double Setpoint, Input, Output;
//Parámetros del controlador de voltaje. PARALELO
float Kp_V;
float Ki_V;
float Kd_V;
//Parámetros del controlador de caudal.  PARALEO
float Kp_Q;
float Ki_Q;
float Kd_Q;

PID ControlVoltaje(&Input, &Output, &Setpoint,Kp_V,Ki_V,Kd_V, DIRECT);
//PID ControlCaudal(&Input, &Output, &Setpoint,Kp_Q,Ki_Q,Kd_Q, DIRECT);

//  Variables auxiliares que se comparten en distintos subprogramas
int i;
float suma;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Programas y subprogramas

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


  display.clear();                      //Limpia la pantalla
  display.setBrightness(0x0f);
  display.setSegments(SEG_OCHO);
  delay(1);

  if (digitalRead(interruptor == 1)) {  //Para que si el sistema se inicia en modo AUTO
    ultimo_estado = 1;                  //el ultimo_estado lo refleje y represente el 8888.
  }

  ControlVoltaje.SetOutputLimits(0,100);
  //ControlCaudal.SetOutputLimits(0,100);
  
  ControlVoltaje.SetMode(AUTOMATIC);  //Activación de controladores
  //ControlCaudal.SetMode(AUTOMATIC);   //Probablemente debería de ir dentro de elección para ver si activar o no el controlador
}


// Se ejecuta continuamente, se incluyen aquí todas las tareas a realizar
void loop() {

  // ELECCIÓN MANUAL - AUTOMÁTICO
  eleccion();
  
  
  //salida_Q = leerCaudal();          //Medida del caudal
  salida_V = leerVoltaje();           //Medida del voltaje enviado al motor

  //MostrarPantallaOscilante(double salida_Q, salida_V); //Que vaya cambiando la pantalla entre mostrar caudal y voltaje enviado.
  
  Setpoint = referencia_V;
  Input = salida_V;
  
  ControlVoltaje.Compute();           //Se calcula la señal de control de voltaje                     
  //ControlCaudal.Compute();          //Señal de control de caudal

  MoverMotor(Output);                 //Se manda la señal de control calculada al motor

}




//  Procedimiento que lee el estado del interruptor y lleva el programa al modo AUTO o MANUAL
void eleccion() {


  estado_interruptor = digitalRead(interruptor);

  if (estado_interruptor == HIGH) {
    //Entonces el interruptor está en modo AUTOMÁTICO
    if (ultimo_estado != estado_interruptor) {
      display.setSegments(SEG_AUTO);
    }
    automatico();

  } else {
    //Entonces el interruptor está en modo MANUAL
    manual();
    if (ultimo_estado != estado_interruptor) {
      display.setSegments(SEG_MAN);
    }
  }

  ultimo_estado = estado_interruptor;
}




// Subprograma que contiene el modo MANUAL
void manual() {
  
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
      display.showNumberDec(buttonPushCounter, false);

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
      display.showNumberDec(buttonPushCounter, false);

      referencia_V = buttonPushCounter * 5; //(0 - 20) --> (0 - 100)    // MARCA LA REFERENCIA
    }
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  ultimo_estadoDown = estado_botonDown;
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

    referencia_V = map(valor, 0,1023, 0,100);      //(0 - 5) --> (0 - 100)    // MARCA LA REFERENCIA

  } else {

    if (Serial.available() > 0) {
      Serial.println("Introduzca un valor de 0 - 1024 (resolución de la salida analog) ");
      String str = Serial.readStringUntil('\n'); //lectura de la entrada de varios dígitos
      float valor = str.toFloat();

      if (valor >= 0 && valor <= 100) {
          referencia_V = valor;                    //(0 - 100)    // MARCA LA REFERENCIA
      }
    }
  }

}



//  Subprograma para la representación en pantalla de números reales con hasta dos decimales
void MostrarPantalla(float valor) {

  int entera = 0;
  int decimal = 0;

  //Ajuste de número real para separarlo en dos enteros de parte real e imaginaria
  entera = (int)(valor);
  decimal = round(100 * (valor - entera));

  //Representación en pantalla del número real
  display.clear(); //Limpia la pantalla
  display.setBrightness(0x0f);
  display.showNumberDecEx(entera, (0x80 >> 1), false, 2, 0);
  display.showNumberDecEx(decimal, (0x80 >> 1), false, 2, 2);
}


  //  Subprograma para el movimiento del motor en modo automático, por el puerto serie


void MoverMotor(float valor) {

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  valor = map(valor, 0, 100, 0, 255);
  analogWrite(en1, valor);

}



//  Función para realizar la lectura del caudalímetro contador y asociarla al caudal correspondiente
//Pendiente, asociar valor de frecuencia con el caudal correspondiente
double leerCaudal() {
  
  tiempoo = millis();

  estadoo = digitalRead(caudalimetro);
  if (estadoo == HIGH && estadoo != ultimo_estadoo) { //Cada vez que completa una vuelta se cumple la condición
                                                      //Lectura instantánea del caudal
    frecuencia = 1/(1000*(tiempoo - ultimo_tiempoo)); //Se calcula la frecuencia de rotación en Hz
  }
  
  if(nmedidas <= 100){                                //Media de las últimas N medidas
    F[nmedidas] = frecuencia;
    nmedidas++;
  }else{
    suma = 0;
    for(i=0; i<nmedidas; i++){ 
      suma=suma+F[i];
    }
    Fmedia = suma/nmedidas;
    nmedidas = 0;
  }

  if (tiempo - ultimo_tiempo > 1000) {  //Representación en pantalla
    Serial.print("Caudal: ");
    Serial.println(Fmedia);
  }

  ultimo_estadoo = estadoo;
  ultimo_tiempoo = tiempoo;

  return Fmedia;
}



//  Subprograma que lee el voltaje enviado el motor, a través del divisor de tensión para adaptarla
//  al rango de entrada del puerto analógico del arduino
double leerVoltaje() {
  
  tiempo = millis();
  V = analogRead(A5);
  if(nlecturas <= 100){
    if ( V != 0 && V != 1023) {
      voltaje[nlecturas] = V;       //Se almacenan las lecturas tomadas durante un ms
      nlecturas++;   
    }
  }else{                            //Se han tomado y almacenado las N medidas
    suma=0;                         //Cálculo de la media de las medidas tomadas en el intervalo
    for(i=0; i<nlecturas; i++){ 
      suma=suma+voltaje[i];
    }
    Vmedio = suma/nlecturas;
    Vmedio = Vmedio * (Vmax/1023);  //Se pasa de bits a Voltios, Vmax cambia en función del valor máximo
    nlecturas = 0;                  //Se resetea el contador de lecturas realizadas
  }
  
  if (tiempo - ultimo_tiempo > 1000) {  //Representación en pantalla
    Serial.print("Voltaje enviado al motor: ");
    Serial.println(Vmedio);
  }
   
    ultimo_tiempo = tiempo;

    return Vmedio;
}
