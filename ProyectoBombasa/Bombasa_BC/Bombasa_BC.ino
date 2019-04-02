
//  Programa para el control de la bomba en bucle cerrado

//  Inclusión de todas las librerías necesarias
#include <Arduino.h>
#include <math.h>
#include <TM1637Display.h>
#include <PID_v1.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Definición e inicialización de variables
const int Vmax = 30;
const int QMAX = 60;

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

//  Interruptor MANUAL - AUTOMÁTICO
const int interruptor = 2;
int estado_interruptor = 0;

//  Caudalímetro
double Qmedio = 0;
double leerCaudal();
int Q;            //Variable que almacena la lectura instantánea del voltaje
int nlecturas = 0; //Almacena el número de lecturas (100 lecturas)
int caudal[20]; //Almacena el valor de V para las nlecturas


// Interpreteción del voltaje que da el caudalímetro en caudal
double calibracionCaudalimetro(double x);

// Ajuste polinomial de los parámetros del sistema en base al punto de operación
//ganancia
double ganancia(double x);
//constante de tiempo
double cteTiempo(double x);

void eleccion();

//Parámetros de la planta
double k = 0;
double tau = 0;

//  Controlador
double Input;
double Output;
double Setpoint;

//Parámetros del controlador de caudal.  PARALELO
double Kp_Q;
double Ki_Q;

int ultimo_tiempo = 0;
int tiempo;

PID ControlCaudal(&Input, &Output, &Setpoint, Kp_Q, Ki_Q, 0, DIRECT);

//  Variables auxiliares que se comparten en distintos subprogramas
int i;
float suma;



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

  ControlCaudal.SetOutputLimits(0, Vmax);
  Input = leerCaudal();
  Setpoint = 0;
  //Activación de controladores

}


// Se ejecuta continuamente, se incluyen aquí todas las tareas a realizar
void loop() {

  // ELECCIÓN MANUAL - AUTOMÁTICO
  eleccion();

  //MostrarPantallaOscilante(double salida_Q, salida_V); //Que vaya cambiando la pantalla entre mostrar caudal y voltaje enviado.


  Input = leerCaudal();          //Medida del caudal
  //Serial.println(Input);
  //Control adaptativo

  //Cálculo de los parámetros actuales del sistema
  if (Input > 20) {
    k = ganancia(Input);
    tau = cteTiempo(Input);
  } else {
    k = ganancia(30);
    tau = cteTiempo(30);
  }


  //Cálculo de los parámetros del controlador
  Kp_Q = (double) 1 / (0.5 * k);
  Ki_Q = (double) Kp_Q / tau;

  //Se actualizan los parámetros del controlador en la función
  ControlCaudal.SetTunings(0.08, 0.3, 0);

  //Se calcula la señal de control
  ControlCaudal.Compute();

  tiempo = millis();
  if (tiempo -  ultimo_tiempo > 1000) {  //Representación en pantalla
    //Serial.print("La referencia actual es: ");
    Serial.println(Setpoint);
    //Serial.println("S. Control: ");
    Serial.println(Output);
    //Serial.println("Salida: ");
    Serial.println(Input);


    ultimo_tiempo = tiempo;
  }

  if (Setpoint == 0) {
    MoverMotor(0);
  } else {
    MoverMotor(Output);
  }


}




//  Procedimiento que lee el estado del interruptor y lleva el programa al modo AUTO o MANUAL
void eleccion() {


  estado_interruptor = digitalRead(interruptor);

  if (estado_interruptor == HIGH) {
    //Entonces el interruptor está en modo AUTOMÁTICO
    if (ultimo_estado != estado_interruptor) {
      display.setSegments(SEG_AUTO);
    }
    ControlCaudal.SetMode(AUTOMATIC);
    automatico();

  } else {
    //Entonces el interruptor está en modo MANUAL
    ControlCaudal.SetMode(MANUAL);
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

      //Setpoint = double(buttonPushCounter * (QMAX/20)); //(0 - 20) --> (0 - QMAX)    // MARCA LA REFERENCIA



    }


    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  ultimo_estadoDown = estado_botonDown;

  //buttonPushCounter = map(buttonPushCounter,0,20, 0, QMAX);
  MoverMotorManual(buttonPushCounter);
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

    Setpoint = map(valor, 0, 1023, 0, QMAX);    //(0 - 5) --> (0 - 100)    // MARCA LA REFERENCIA

  } else {

    if (Serial.available() > 0) {
      String str = Serial.readStringUntil('\n'); //lectura de la entrada de varios dígitos
      float valor = str.toFloat();

      if (valor >= 0 && valor <= QMAX) {
        Setpoint = valor;                    //(0 - QMAX)    // MARCA LA REFERENCIA
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

  valor = (float) valor * 255 / Vmax;
  analogWrite(en1, valor);

}


void MoverMotorManual(float valor) {


  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  if (valor == 0) {
    analogWrite(en1, valor);
  } else {
    valor = map(valor, 0, 27.4, 4.4, 22);
    valor = map(valor, 4.4, 25.4, 37.4, 255);
    analogWrite(en1, valor);
  }
}



//  Función para realizar la lectura del caudalímetro contador y asociarla al caudal correspondiente
//Pendiente, asociar valor de frecuencia con el caudal correspondiente
double leerCaudal() {

  Q = analogRead(A5);
  if (nlecturas < 20) {
    caudal[nlecturas] = Q;                    //Se almacenan las lecturas tomadas durante un ms
    nlecturas++;

  } else {                                       //Se han tomado y almacenado las N medidas
    suma = 0;                                   //Cálculo de la media de las medidas tomadas en el intervalo
    for (i = 0; i < nlecturas; i++) {
      suma = suma + caudal[i];
    }

    double Vmedio = (double) suma / nlecturas;
    Vmedio = (double) Vmedio * 5 / 1023; //Se pasa de bits a Voltios
    Qmedio = calibracionCaudalimetro(Vmedio);   //Se pasa de voltios a mL/min

    nlecturas = 0;                              //Se resetea el contador de lecturas realizadas

  }

  if (Qmedio > 20) {
    return Qmedio;
  } else {
    return 0;
  }

}




double calibracionCaudalimetro(double x) {
  //En base a la calibración se obtiene el siguiente polinomio:
  //  curva(x) = p1*x^2 + p2*x + p3
  //con:
  double p1 = -7.361, p2 = 51.48, p3 = 0.2817;

  double caudal = p1 * pow(x, 2) + p2 * x + p3;

  return caudal;
}

double ganancia(double x) {
  //En base al ajuste de la curva punto de operación - ganancia estática se obtiene:
  //y(x) = p1*x^4 + p2*x^3 + p3*x^2 + p4*x + p5
  double p1 = 0.0004088, p2 = -0.06596 , p3 = 3.92 , p4 = -102.4 , p5 = 1010;

  double y = p1 * pow(x, 4) + p2 * pow(x, 3) + p3 * pow(x, 2) + p4 * x + p5;

  return y;
}

double cteTiempo(double x) {
  //En base al ajuste de la curva punto de operación - cte de tiempo se obtiene:
  //y(x) = p1*x^4 + p2*x^3 + p3*x^2 + p4*x + p5
  double p1 = 6.876e-05, p2 = -0.01448 , p3 = 1.06 , p4 = -32.93 , p5 = 378.2;

  double y = p1 * pow(x, 4) + p2 * pow(x, 3) + p3 * pow(x, 2) + p4 * x + p5;

  return y;
}
