#include <EEPROM.h>
#include <stdlib.h>


void ordenaDistancias(float vetor[], int n);

/* VARIÁVEIS DE CONTROLE PARA ANALISAR SE ESTÁ DENTRO DA ROTA */

    float *distancias;
    float *dist_pontos_adj;
    float distancia_final;
    float distancia_inicial;

  int estado=-1;
  float temp;
  int controle=0;
  int ok=1;
  int comecou=0;


/* Localização da rota recebida EEPROM */
  float *latitude;
  float *longitude;
  int num_pontos;

/* Variáveis para leitura da memória EEPROM */
   int endereco;
   int cont;
   int limit;
   float dadosRecebido;


/* Variáveis para inserir dados na memória EEPROM */
   float dadosInserir;
   String dados;
   String subString;


/* Coordenadas do GPS: */
    float latitude_gps;
    float longitude_gps;

    float latitude_gps_v[6]= {33.324234, -22.975990000000003, 34.2342344, 56.324234,-22.977430000000002, -22.98156929};
    float longitude_gps_v[6] = {56.3324234, -43.22851, 32.34543534, 32.324234, -43.227120000000006, -43.22649765 };
   

/* COORDENADAS RECEBIDAS COM A ROTA DEFINIDA */

char coordenadas[1000]=""; //LAT e LONG
float latitude_final;
float longitude_final;


/* IMPLEMENTAÇÃO MAIN()
/* ---------------------------------------------------------------------------------------------------------------*/

void setup() {

Serial.begin(9600);
Serial.setTimeout(10);


/* SEPARAÇÃO DAS COORDENADAS E INSERÇÃO NA MEMÓRIA EEPROM */

String texto =  "28,-22.975990000000003,-43.22851,-22.975990000000003,-43.228370000000005,-22.976110000000002,-43.2282,-22.977430000000002,-43.227120000000006,-22.97784,-43.22677,-22.977890000000002,-43.226690000000005,-22.97792,-43.22657,-22.97792,-43.22639,-22.978040000000004,-43.22646,-22.978180000000002,-43.2265,-22.978430000000003,-43.226560000000006,-22.97978,-43.22665000000001,-22.981530000000003,-43.226760000000006,-22.98157,-43.2265,-22.98138,-43.22648,-22.98141,-43.226110000000006,-22.981550000000002,-43.226200000000006,-22.981730000000002,-43.226220000000005,-22.982310000000002,-43.225930000000005,-22.982860000000002,-43.225730000000006,-22.983580000000003,-43.225480000000005,-22.98419,-43.225260000000006,-22.98486,-43.225030000000004,-22.98485,-43.22496,-22.98459,-43.22406,-22.984420000000004,-43.22328,-22.98438,-43.223130000000005,-22.984340000000003,-43.22314";

texto.trim();

/* Converte o objeto String em um array de char para coordenadas */
texto.toCharArray(coordenadas,1000);
  
/* Conta quantos pontos existem no array de char coordenadas */
subString = strtok (coordenadas,",");
cont = subString.toInt();

  
char * pch;
pch = strtok (NULL,",");
  
endereco=0;
while (pch != NULL)
{
    dados =  pch;
    dadosInserir = dados.toFloat();

    EEPROM.put(endereco, dadosInserir);
    endereco+=4;    
    pch = strtok (NULL, ",");
}
  Serial.print("Numero de pontos inseridos na memoria EPPROM:");
  Serial.println(cont/2);
  Serial.println();


num_pontos = cont/2;
latitude = (float *) malloc(num_pontos * sizeof(float));
longitude = (float *) malloc(num_pontos * sizeof(float));
distancias = (float *) malloc(num_pontos * sizeof(float)) ;
dist_pontos_adj = (float *) malloc((num_pontos-1) * sizeof(float));


  
  /* LEITURA DAS COORDENADAS DA MEMÓRIA EEPROM */
limit=0;
endereco=0;
int i,j;
i=j=0;

while(limit < cont)  // ---> Nesse caso usamos 10 pontos, mas substituir por cont ou o número de pontos*2
{
    EEPROM.get(endereco, dadosRecebido);
   /* LATITUDE */
   if(limit % 2 ==0)
     {
       latitude[i] = dadosRecebido;   
       i++;
     }
   /* LONGITUDE */  
   else
   { 
      longitude[j] = dadosRecebido;
      j++; 
   }
   
   endereco+=4;
   limit++;
 }

   latitude_final = latitude[num_pontos-1];
   longitude_final = longitude[num_pontos-1];

  int k=0;
  while (k < num_pontos) 
  {
   Serial.print("Latitude: ");
   Serial.println(latitude[k],8);

   Serial.print("Longitude: ");
   Serial.println(longitude[k],8);
   k++; 
  }

 Serial.println();
  
  /* CALCULO DAS DISTANCIAS DO PONTO DO GPS ATÉ OS PONTOS DA ROTA   --- PASSAR PARA O LOOP ESSE BLOCO*/
  
    Serial.println();
  k=0;
  while(k < (num_pontos-1)) /* Distancia entre pontos adjacentes da rota*/
  {
    temp = pow(latitude[k]-latitude[k+1],2) + pow(longitude[k]-longitude[k+1],2) ;
    dist_pontos_adj[k] = sqrt(temp);

    Serial.print("Distancia de pontos adjacentes:");
    Serial.print(dist_pontos_adj[k],8);
    Serial.println();
    k++;
  }

    Serial.println();
  /* ORDENA O VETOR DE DISTÂNCIAS EM ORDEM CRESCENTE */
  ordenaDistancias(dist_pontos_adj, 4);
  
  Serial.print("Menor distancia encontrada entre GPS e algum ponto da rota encontrado: ");
  Serial.println(distancias[0],8);
  Serial.print("Maior distancia entre pontos:");
  Serial.print(dist_pontos_adj[num_pontos-2],8);
  Serial.println();
  Serial.println("===============================================================================");

}


int w=0;
void loop() {


    Serial.println("==========================================================");
    if(w<6)
    {
      latitude_gps = latitude_gps_v[w];
      longitude_gps = longitude_gps_v[w];      
      Serial.print("Latitude_corrente: ");
      Serial.println(latitude_gps);
      Serial.print("Longitude_corrente: ");
      Serial.println(longitude_gps);
    }

    Serial.println();
    


  int k=0;
  while(k < num_pontos) /* Nº total de pontos */
  {
    temp = pow((latitude[k]-latitude_gps),2) + pow(longitude[k]-longitude_gps,2) ;
    distancias[k] = sqrt(temp);

    //Serial.print("Distancia da localizacao do GPS ate os pontos da rota:");
    //Serial.print(distancias[k],8);
    k++;
  }
  ordenaDistancias(distancias, 5);  

    

    temp = pow(latitude_gps-latitude[0],2) + pow(longitude_gps-longitude[0],2);
    distancia_inicial = sqrt(temp);
    /* COMEÇOU A ROTA */
    if(distancia_inicial < 0.00026 && comecou == 0)
    {
       Serial.println("Voce comecou a rota!!!");
       estado=1;
       comecou=1;
    }
    else
    {
     if(comecou == 0)
     {
      Serial.println("Va para o inicio da rota!!!");
     }
      
    




  if(estado == 1)
  {  
     /* ENCERROU A ROTA */
    temp = pow(latitude_gps-latitude_final,2) + pow(longitude_gps-longitude_final,2);
    distancia_final = sqrt(temp);
    if(distancia_final < 0.00026)
    { 
      estado=2;
    }   

   
    
    /*SE A MENOR DISTANCIA MEDIDA ESTA ACIMA DA TOLERANCIA, ENTÃO ESTA SUPOSTAMENTE ESTA FORA DO TRAJETO */
    if( dist_pontos_adj[num_pontos-2] < distancias[0] && ok == 1 && estado!=2 )
    {
       controle++;
       if(controle > 0 && controle < 5)
       {
        Serial.println("Voce esta fora da rota definida!!!");
        Serial.println("Volte imediatamente para rota.");
        }
        else
       {
        Serial.println("MODO DESTRUICAO ATIVADO!!!");
        ok=0;
       }
    }
      else 
    { 
      if(ok == 1)
      {
       if(controle == 0)
         {
          Serial.println("OK");
         }
       else
       {
          Serial.println("Voce voltou para rota!!!\nTudo Certo :)");
          controle=0;
       }
     }      
     else
     {         
              estado=3;
              // Serial.println("ACENDE UM LED!!!!!!!!!!!!!!!!");
              free(latitude);
              free(longitude);
     }
  }
 }
 if(estado == 2)
 {
  Serial.println("Voce chegou no final da rota!!!!"); 
  free(latitude);
  free(longitude);
 }

 if(estado == 3)
 {
  
  Serial.println("ACENDE UM LED!!!!!!!!!!!!!!!!");
  free(latitude);
  free(longitude);
  
 }
  delay(5000);
  w++;
}}

void ordenaDistancias(float vetor[], int n){
    int k, j;
    float aux;

    for (k = 1; k < n; k++) {
        for (j = 0; j < n - 1; j++) {
            if (vetor[j] > vetor[j + 1]) {
                aux          = vetor[j];
                vetor[j]     = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}
