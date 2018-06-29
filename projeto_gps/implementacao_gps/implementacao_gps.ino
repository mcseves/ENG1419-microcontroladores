#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial serial1(10, 11); // RX, TX ---> portas digitais do Arduino: | porta 10 conectada na TX do módulo |  porta 11 conectada na RX do módulo. 
TinyGPS gps1;                   // interpetração dos dados recebidos padrão NMEA


void setup() {
   
   serial1.begin(9600);   // conexão serial com o módulo GPS
   Serial.begin(9600);    //conexão serial com computador

   Serial.println("Carregando GPS...");
}

void loop() {
  bool recebido = false;    // iniciando com false
  static unsigned long delayPrint;

  while (serial1.available()) {                   // Verifica se temos uma informação disponível via serial pelo módulo GPS.
     char cIn = serial1.read();                   // Caso tenha informação disponível, a informação é passada caractere a cactere para a variável cIn.
     recebido = gps1.encode(cIn || recebido);     // Logo esses caractere são passados para o objeto gps1 que vai montando a informação por completo.
                                                  // Aqui faz com que o arduino espere até receber o primeiro sinal váido do GPS, para então mostrar as coordenadas.
  }

  //Caso os dados foram carregados corretamente para o objeto gps1, então o recebido será true e entrará no if e será atualizado a cada 1 segundo.
  if ((recebido) && ((millis() - delayPrint) > 1000 ) {

     delayPrint = millis();
     Serial.println("----------------------------------------");
     
     //Latitude e Longitude
     long latitude, longitude;
     unsigned long idadeInfo;
     
     gps1.get_position(&latitude, &longitude, &idadeInfo);   
     // irar passar para as variáveis os valores da  latitude e longitude e a idade da informação o tempo que a informação foi gerada pelo satélite até chegar no GPS(em ms).

     // No IF verifica se as informações são válidas, para então poder printar.
     // Pode ser que o sinal é de apenas poucos satélites gerando dados incosistentes, então esse IF seleciona apenas os que devo considerar realmente.
     if (latitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Latitude: ");
        Serial.println(float(latitude) / 100000, 6);
     }

     if (longitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Longitude: ");
        Serial.println(float(longitude) / 100000, 6);
     }

     if (idadeInfo != TinyGPS::GPS_INVALID_AGE) {
        Serial.print("Idade da Informacao (ms): ");
        Serial.println(idadeInfo);
     }


     /* Manda um link do google maps com a localização passada.
      * String urlMapa = "Local Identificado: https://maps.google.com/maps/?&z=10&q=";
         urlMapa += String(flat,6);
         urlMapa += ",";
         urlMapa += String(flon,6);
         Serial.println(urlMapa);
      * 
      * 
      */

     //Dia e Hora: Retorna todos os dados de tempo da informação gerada.
     //A data e o horário é recebida pelo GPS com fuso horario de greenwich(GMT: 0). No caso do Brasil temos GMT(-3), então temos que subtrair 3 da data recebida.
     int ano;
     byte mes, dia, hora, minuto, segundo, centesimo;
     gps1.crack_datetime(&ano, &mes, &dia, &hora, &minuto, &segundo, &centesimo, &idadeInfo);

     Serial.print("Data (GMT): ");
     Serial.print(dia);
     Serial.print("/");
     Serial.print(mes);
     Serial.print("/");
     Serial.println(ano);

     Serial.print("Horario (GMT): ");
     Serial.print(hora);
     Serial.print(":");
     Serial.print(minuto);
     Serial.print(":");
     Serial.print(segundo);
     Serial.print(":");
     Serial.println(centesimo);


     //altitude
     float altitudeGPS;
     altitudeGPS = gps1.f_altitude();

     if ((altitudeGPS != TinyGPS::GPS_INVALID_ALTITUDE) && (altitudeGPS != 1000000)) {
        Serial.print("Altitude (cm): ");
        Serial.println(altitudeGPS);
     }


     //velocidade
     float velocidade;
     //velocidade = gps1.speed();        //nós
     velocidade = gps1.f_speed_kmph();   //km/h
     //velocidade = gps1.f_speed_mph();  //milha/h
     //velocidade = gps1.f_speed_mps();  //milha/segundo

     Serial.print("Velocidade (km/h): ");
     Serial.println(velocidade, 2);  //Conversão de Nós para Km/h



     //sentito (em centesima de graus)
     unsigned long sentido;
     sentido = gps1.course();

     Serial.print("Sentido (grau): ");
     Serial.println(float(sentido) / 100, 2);


     //satelites e precisão
     unsigned short satelites;
     unsigned long precisao;
     satelites = gps1.satellites();
     precisao =  gps1.hdop();

     if (satelites != TinyGPS::GPS_INVALID_SATELLITES) {
        Serial.print("Satelites: ");
        Serial.println(satelites);
     }

     if (precisao != TinyGPS::GPS_INVALID_HDOP) {
        Serial.print("Precisao (centesimos de segundo): ");
        Serial.println(precisao);
     }


     //float distancia_entre;
     //distancia_entre = gps1.distance_between(lat1, long1, lat2, long2);

     //float sentido_para;
     //sentido_para = gps1.course_to(lat1, long1, lat2, long2);
  }
}
