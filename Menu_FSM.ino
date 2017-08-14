#include <LCD.h>
#include <RTC.h> //Libreria del Módulo RTC (Real Time Clock)
#define I2C_ADDR 0x27
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
//Definicion de estados==================
#define S_HOME 0
#define S_SET_LED1 1
#define S_SET_LED2 2
#define S_SET_BOTH 3
#define S_SET_BLINK 3
//=======================================
LiquidCrystal_I2C lcd(I2C_ADDR, 2, 1, 0, 4, 5, 6, 7);
uint8_t estado = S_HOME; //Estado Actual
//Definicion de Menus====================
#define BTN_MENU 0
#define BTN_EXIT 1
#define BTN_UP 2
#define BTN_DOWN 3
#define BZZ 8
#define VERDE 6
#define ROJO 7
//=======================================
//rtc.setDateTime(2017, 8, 1, 5, 50, 0 ); // Year, Month, Day, Hour24H, Minutes and Seconds
int ContadorLED1 = 0, ContadorLED2 = 0, ContadorBoth = 0;
uint8_t h = 0, m = 0, s = 0;
uint8_t button[4] = {
  2,
  3,
  4,
  5
};
String dia, mes, fecha, hora;
RTC rtc(DST_OFF); // OR DST_OFF
unsigned long tiempo = 0, ultimoCambio = 0, tiempoCambio = 0, tiempoNecesario = 60000;
uint8_t button_state[4];
uint8_t flancoSubida(int btn)//Detección del flanco de subida
{
  uint8_t valor_nuevo = (digitalRead(button[btn]));
  uint8_t result = button_state[btn] != valor_nuevo && valor_nuevo == HIGH;
  button_state[btn] = valor_nuevo;
  return result;
};

void message_init()
{
  lcd.setCursor(2,0);
  lcd.print("Controladora");
  lcd.setCursor(0,1);
  lcd.print("Programable V1.0");
}

void message_middle()
{
  lcd.setCursor(1,0);
  lcd.print("Arduino F.S.M.");
  lcd.setCursor(0,1);
  lcd.print("Por Jesus Corona");
}

void message_end()
{
  lcd.setCursor(5,0);
  lcd.print("Agosto");
  lcd.setCursor(6,1);
  lcd.print("2017");
}

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(VERDE, OUTPUT);
  pinMode(ROJO, OUTPUT);
  digitalWrite(13, LOW);
  pinMode(button[BTN_MENU], INPUT_PULLUP);
  pinMode(button[BTN_EXIT], INPUT_PULLUP);
  pinMode(button[BTN_UP], INPUT_PULLUP);
  pinMode(button[BTN_DOWN], INPUT_PULLUP);
  button_state[0] = HIGH;
  button_state[1] = HIGH;
  button_state[2] = HIGH;
  button_state[3] = HIGH;
  lcd.setBacklightPin(3, POSITIVE);
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  lcd.clear();
  message_init();
  delay(3000);
  lcd.clear();
  message_middle();
  delay(3000);
  lcd.clear();
  message_end();
  delay(3000);
  lcd.clear();
  

}
/** Inicio de funciones de dibujado de mensajes **/
void printHome()
{
  //lcd.clear();
  Data d = rtc.getData();
  switch (d.dayWeek)
  {
    case 1:
      dia = "Lunes";
      break;
    case 2:
      dia = "Martes";
      break;
    case 3:
      dia = "Miercoles ";
      break;
    case 4:
      dia = "Jueves";
      break;
    case 5:
      dia = "Viernes";
      break;
    case 6:
      dia = "Sabado";
      break;
    case 7:
      dia = "Domingo";
      break;
  }
  switch (d.month)
  {
    case 1:
      mes = "Enero";
      break;
    case 2:
      mes = "Febrero";
      break;
    case 3:
      mes = "Marzo";
      break;
    case 4:
      mes = "Abril";
      break;
    case 5:
      mes = "Mayo";
      break;
    case 6:
      mes = "Junio";
      break;
    case 7:
      mes = "Julio";
      break;
    case 8:
      mes = "Agosto";
      break;
    case 9:
      mes = "Septiembre";
      break;
    case 10:
      mes = "Octubre";
      break;
    case 11:
      mes = "Noviembre";
      break;
    case 12:
      mes = "Diciembre";
      break;
  }
  if (dia == "Lunes")
  {
    lcd.setCursor(1, 0);
    lcd.print(dia);
  }
  else if (dia == "Miercoles ")
  {
    lcd.setCursor(0, 0);
    lcd.print(dia);
    lcd.print(d.toString(" d/m"));
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print(dia);
  }
  lcd.print(d.toString(" d/m/y"));
  lcd.setCursor(1, 1);
  lcd.print(d.toString("H:i:s "));
  lcd.print(rtc.getTemperature());

}
void printLED1()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU] Set LED1");
  printStatus();
}
void printLED2()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU] Set LED2");
  printStatus();
}
void printBoth()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU] Set Ambos");
  printStatus();
}

void printBlink()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU] Set Blink");
  printTiempo();
}

void printTiempo()
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Tiempo: ");
  //lcd.print(tn, DEC);
}

void printStatus()
{

  int s_rojo = digitalRead(ROJO);
  int s_verde = digitalRead(VERDE);
  if ((s_rojo == HIGH) || (s_verde == HIGH))
  {
    lcd.setCursor(0, 1);
    lcd.print("Encendido");
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Apagado");
  }
}/** Fin de funciones de dibujado de mensajes **/
void conteoHoras()
{
  Data d = rtc.getData();
  int conteo = 0;
  int hora12 = d.hour;
  int hora = d.hour24h, minuto = d.minutes, segundo = d.seconds;
  if ((minuto == 0) && (segundo == 0))//Configuracion de Alarma
  {
    for (conteo = 0; conteo < hora12 ; conteo++)
    {
      for (int j = 0; j < 4; j++)
      {
        tone(BZZ, 2000, 60);
        delay(60);
        noTone(BZZ);
        delay(60);
      }
      delay(500);
    }
  }
}
void loop()
{
  conteoHoras(); //establece el numero de ciclos de pitidos segun la hora actual a través del buzzer
  tiempo = millis();
  tiempoCambio = tiempo - ultimoCambio;
  if (tiempoCambio >= tiempoNecesario) //Configuracion del Tiempo de Encendido del Backlight
  {
    lcd.setBacklight(LOW);
    ultimoCambio = tiempo;
    lcd.clear();
  }
  switch (estado)
  {
    case S_HOME:
      printHome();
      if (flancoSubida(BTN_MENU))
      {
        lcd.clear();
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        estado = S_SET_LED1;
        printLED1();

      }
      break;                        //fin home
    case S_SET_LED1:                 //inicio rojo
      {
        if (flancoSubida(BTN_MENU))
        {
          tone(BZZ, 2000, 60);
          lcd.setBacklight(HIGH);
          ultimoCambio = tiempo;
          estado = S_SET_LED2;
          printLED2();
          delay(120);
          break;
        }
        if (flancoSubida(BTN_EXIT))
        {
          tone(BZZ, 2000, 60);
          lcd.clear();
          estado = S_HOME; //(Evento de salida) repetir en cada estado.
          lcd.setBacklight(HIGH);
          ultimoCambio = tiempo;
          printHome();
          break;
        }
      }
      if (flancoSubida(BTN_UP))
      {
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        ContadorLED1++;
        delay(60);
      }
      switch (ContadorLED1)
      {
        case 1:
          if (digitalRead(ROJO) == LOW)
          {
            digitalWrite(ROJO, HIGH);
            lcd.clear();
            printLED1();
            printStatus();
          }
          break;
        case 2:
          if (digitalRead(ROJO) == HIGH)
          {
            digitalWrite(ROJO, LOW);
            lcd.clear();
            printLED1();
            printStatus();
            ContadorLED1 = 0;
          }
          break;
      }

      break;//Fin rojo
    case S_SET_LED2:                                               //inicio verde
      if (flancoSubida(BTN_MENU))
      {
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        estado = S_SET_BOTH;
        printBoth();
        break;
      }
      if (flancoSubida(BTN_EXIT))
      {
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        lcd.clear();
        estado = S_HOME; //(Evento de salida) repetir en cada estado.
        printHome();
        break;
      }
      if (flancoSubida(BTN_UP))
      {
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        ContadorLED2++;
        delay(120);
      }

      switch (ContadorLED2)
      {
        case 1:
          if (digitalRead(VERDE) == LOW)
          {
            digitalWrite(VERDE, HIGH);
            lcd.clear();
            printLED2();
            printStatus();
          }
          break;
        case 2:
          if (digitalRead(VERDE) == HIGH)
          {
            digitalWrite(VERDE, LOW);
            lcd.clear();
            printLED2();
            printStatus();
            ContadorLED2 = 0;
          }
          break;;
      }
      break;                                                                      //fin verde
    case S_SET_BOTH:                                                              //inicio azul
      if (flancoSubida(BTN_MENU))
      {
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        lcd.clear();
        estado = S_SET_LED1;
        printLED1();
        break;
      }
      if (flancoSubida(BTN_EXIT))
      {
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        lcd.clear();
        estado = S_HOME; //(Evento de salida) repetir en cada estado.
        printHome();
        break;
      }
      if (flancoSubida(BTN_UP))
      {
        tone(BZZ, 2000, 60);
        lcd.setBacklight(HIGH);
        ultimoCambio = tiempo;
        ContadorBoth++;
        delay(120);
      }
      switch (ContadorBoth)
      {
        case 1:
          if ((digitalRead(ROJO) == LOW) && (digitalRead(VERDE) == LOW))
          {
            digitalWrite(VERDE, HIGH);
            digitalWrite(ROJO, HIGH);
            lcd.clear();
            printBoth();
            printStatus();
          }
          break;
        case 2:
          if ((digitalRead(ROJO) == HIGH) && (digitalRead(VERDE) == HIGH))
          {
            digitalWrite(VERDE, LOW);
            digitalWrite(ROJO, LOW);
            lcd.clear();
            printBoth();
            printStatus();
            ContadorBoth = 0;

          }
          break;
      }
  }
}
