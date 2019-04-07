
int data = 0;
double i = 0, j = 0;
double x = 0;
double tiempo;
double ultimoTiempo = 0;
double ultimoTiempoo = 0;
char entrada;

void setup() {
  Serial.begin(9600);
}

void loop() {


//Generación de una entrada
  tiempo = millis();
  if (tiempo - ultimoTiempoo > 100) {
    j = j + 3;
    i = i + 1;
    x = x + 2;

    ultimoTiempoo = tiempo;

  }

  if (i == 50) {
    j = 0;
    i = 0;
    x = 0;
  }

//Comunicación serie
  if (Serial.available() > 0) {
    entrada = Serial.read();
    if (entrada == 'g') {
      Serial.println(i);
      Serial.println(j);
      Serial.println(x);
    }


  }






}
