  /*
      CÓDIGO PARA O PROJETO FINAL DA DISCIPLINA PROG DE MICROCONTROLADORES
      PROJETO: RASTREADOR GPS COM AUTODESTRUIÇÃO
      AUTORES: BRUNO SANTOS, LORRAM NASCIMENTO, WILLIAM SIMÕES E MARIA CAROLINA MARINHO
      ÚLTIMA ATUALIZAÇÃO: 09/07/2018
   */
  
  #include <DFRobot_sim808.h>
  #include <SoftwareSerial.h>
  
  #define PHONE_NUMBER "5521993073410"  //Mobile phone number to send SMS, change if need
  #define MESSAGE  "AUTODESTRUIÇÃO INICIADA"   
  #define PIN_TX  10   // TX do Arduino -> RX GPS
  #define PIN_RX  11   // RX do Arduino -> TX GPS
  #define MESSAGE_LENGTH 160  

//################### INICIALIZAÇÃO DAS FUNÇÕES ##################
  void get_GPS(void);
  void send_GPSdata(void);  // Realiza a conexão com o servidor e posta as coordenadas
  void disconect_TCP(void); // Finaliza a conexão TCP
  void read_SMS(void);
  void AUTODESTRUIÇÃO(void);
  
//################## INICIALIZAÇÃO DAS VARIÁVEIS #################  
  float latitude = 0.0;
  float longitude = 0.0;
  char message[MESSAGE_LENGTH];
  int messageIndex = 0;
  char phone[16];
  char datetime[24];
  int gnd = A0;
  int buzzer = 5;
  int i; 

  SoftwareSerial mySerial(PIN_TX,PIN_RX);
  DFRobot_SIM808 sim808(&mySerial);
  //DFRobot_SIM808 sim808(&Serial);

  char http_cmd[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\r\n\r\n";  //  
  char buffer[512];
  
//######################## SETUP #################################
  void setup() {
   //******* Initialize Serial Comunication ********
    mySerial.begin(9600);
    Serial.begin(9600);
    
  //******** Initialize Buzzer *********************
    pinMode(gnd, OUTPUT);
    digitalWrite(gnd, LOW);
    pinMode(buzzer, OUTPUT);
  
   //******** Initialize sim808 module *************
    while(!sim808.init()) {
        Serial.print("Sim808 init error\r\n");
        delay(1000);
    }  
    Serial.println("Sim808 init success.");
    
    if(sim808.attachGPS()){
       Serial.println("Open the GPS power success");
       //Serial.println("Start to send message ...");    
    }
    else{
      Serial.println("Open the GPS power failure");
    }
    
    //*********** Attempt DHCP *******************
    while(!sim808.join(F("cmnet"))) {
      Serial.println("Sim808 join network error");
      delay(2000);
     }
  
    //************ Successful DHCP ****************
    Serial.print("IP Address is ");
    Serial.println(sim808.getIPAddress());

    //*********** Establish a TCP connection ************
    if(!sim808.connect(TCP,"mbed.org", 80)) {  //TCP, servidor, porta - TROCAR
      Serial.println("Connect error");
    }
    else{
      Serial.println("Connect mbed.org success"); // renomear para servidor do Raspberry
    } 
   }
   
//######################## LOOP ##################################  
  void loop() {
    get_GPS();
    send_GPSdata();
    //....cálculo e verificação da rota aqui 
  }

//################### CORPO DAS FUNÇÕES ##########################
  void get_GPS(){
     //************** Get GPS data *******************
    if (sim808.getGPS()) {
      Serial.print(sim808.GPSdata.year); 
      Serial.print("/");
      Serial.print(sim808.GPSdata.month);
      Serial.print("/");
      Serial.print(sim808.GPSdata.day);
      Serial.print(" ");
      Serial.print(sim808.GPSdata.hour);
      Serial.print(":");
      Serial.print(sim808.GPSdata.minute);
      Serial.print(":");
      Serial.print(sim808.GPSdata.second);
      Serial.print(":");
      Serial.println(sim808.GPSdata.centisecond);
      
      latitude = sim808.GPSdata.lat;
      longitude = sim808.GPSdata.lon;
      Serial.print("latitude :");
      Serial.println(latitude,4);
      Serial.print("longitude :");
      Serial.println(longitude,4);
      
      Serial.print("speed_kph :");
      Serial.println(sim808.GPSdata.speed_kph);
      Serial.print("heading :");
      Serial.println(sim808.GPSdata.heading);
      //Serial.print("altitude :");
      //Serial.println(sim808.GPSdata.altitude);
      Serial.println();
     
      //************* Turn off the GPS power ************
      sim808.detachGPS();  //Desativa o GPS e sai do if para nao ficar mandando varias msgs
      //delay(3000);
    } 
  }

  void send_GPSdata(){
      //*********** Send a GET request *****************
    Serial.println("waiting to fetch...");
    sim808.send(http_cmd, sizeof(http_cmd)-1);
    while (true) {
        int ret = sim808.recv(buffer, sizeof(buffer)-1);
        if (ret <= 0){
            Serial.println("fetch over...");
            break; 
        }
        buffer[ret] = '\0';
        Serial.print("Recv: ");
        Serial.print(ret);
        Serial.print(" bytes: ");
        Serial.println(buffer);
        break;
    }
      // postar os dados de coordenada no servidor (var global latitude e longitude)
  }

  void disconect_TCP(){
    //************* Close TCP or UDP connections **********
    sim808.close();
    //*** Disconnect wireless connection, Close Moving Scene *******
    sim808.disconnect();
  }
  
  void read_SMS(){
    //*********** Detecting unread SMS ************************
   messageIndex = sim808.isSMSunread();
   Serial.print("messageIndex: ");
   Serial.println(messageIndex);

   //*********** At least, there is one UNREAD SMS ***********
   if (messageIndex > 0) { 
      sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
                 
      //***********In order not to full SIM Memory, is better to delete it**********
      sim808.deleteSMS(messageIndex);
      Serial.print("From number: ");
      Serial.println(phone);  
      Serial.print("Datetime: ");
      Serial.println(datetime);        
      Serial.print("Recieved Message: ");
      Serial.println(message);    
   }
  }

  void AUTODESTRUIÇÃO(){ 
    sim808.sendSMS(PHONE_NUMBER,MESSAGE);  //Envia a mensagem avisando que a Autodestruição foi inicada
   
    for(i=90;i>1;i-=5){
      tone(buzzer, 1400.0,500 );
      delay(i*10);
    }
    tone(buzzer, 1400.0, 1000);
    delay(1000);
  }

