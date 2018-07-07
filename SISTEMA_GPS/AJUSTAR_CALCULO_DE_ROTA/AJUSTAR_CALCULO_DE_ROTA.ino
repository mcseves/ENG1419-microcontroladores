#include <EEPROM.h>

void ordenaDistancias(float vetor[], int n);


/* VARIÁVEIS DE CONTROLE PARA ANALISAR SE ESTÁ DENTRO DA ROTA */
  float distancias[5]; //ATUALIZADO A CADA 1 MIN
  float dist_pontos_adj[5];

  float temp;
  int controle=0;
  int ok=1;


/* Localização da rota recebida EEPROM --- 2,4Kbytes  */
  float  latitude[10]; 
  float  longitude[10];


/* Variáveis para leitura da memória EEPROM */
   int endereco;
   int cont;
   int limit;
   float dadosRecebido;


/* Variáveis para inserir dados na memória EEPROM */
   float dadosInserir;
   String dados;


/* Coordenadas do GPS: */
   float latitude_gps =  -60.43565675;
   float longitude_gps =  70.43775675;


/* COORDENADAS RECEBIDAS COM A ROTA DEFINIDA */
char coordenadas[400] = "-33.34234325, 31.34343243, -32.00000000, 35.32373341, 20.34336325, 70.33643243, 30.34234325, 36.34343243, 50.34634325, 75.34243243"; //LAT e LONG


/* IMPLEMENTAÇÃO MAIN()
/* ---------------------------------------------------------------------------------------------------------------*/

void setup() {

Serial.begin(9600);

/* SEPARAÇÃO DAS COORDENADAS E INSERÇÃO NA MEMÓRIA EEPROM */
/*
  char * pch;
  pch = strtok (coordenadas,",");

  cont=0;
  endereco=0;
  while (pch != NULL)
  {
    cont++;
    dados =  pch;
    dadosInserir = dados.toFloat();
    //Serial.println(dadosInserir,8);

    EEPROM.put(endereco, dadosInserir);
    endereco+=4;
    
    pch = strtok (NULL, ",");
  }
  Serial.print("Numero de pontos inseridos na memoria EPPROM:");
  Serial.println(cont/2);
  Serial.println();
*/
  
  /* LEITURA DAS COORDENADAS DA MEMÓRIA EEPROM */
limit=0;
endereco=0;
int i,j;
i=j=0;

while(limit < 10)  // ---> Nesse caso usamos 10 pontos, mas substituir por cont ou o número de pontos*2
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

 int k=0;
  while (k < 5) 
  {
   Serial.print("Latitude: ");
   Serial.println(latitude[k],8);

   Serial.print("Longitude: ");
   Serial.println(longitude[k],8);
   k++; 
  }

 Serial.println();
  
  /* CALCULO DAS DISTANCIAS DO PONTO DO GPS ATÉ OS PONTOS DA ROTA   --- PASSAR PARA O LOOP ESSE BLOCO*/
  k=0;
  while(k < 5) /* Nº total de pontos */
  {
    temp = pow((latitude[k]-latitude_gps),2) + pow(longitude[k]-longitude_gps,2) ;
    distancias[k] = sqrt(temp);

    Serial.print("Distancia da localizacao do GPS ate os pontos da rota:");
    Serial.print(distancias[k],8);
    Serial.println();
    k++;
  }

    Serial.println();
  k=0;
  while(k < 4) /* Distancia entre pontos adjacentes da rota*/
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
  ordenaDistancias(distancias, 5);  
  ordenaDistancias(dist_pontos_adj, 4);

  
  Serial.print("Menor distancia encontrada entre GPS e algum ponto da rota encontrado: ");
  Serial.println(distancias[0],8);
  Serial.print("Maior distancia entre pontos:");
  Serial.print(dist_pontos_adj[3],8);
  Serial.println();
  Serial.println("===============================================================================");


}

void loop() {
    if( dist_pontos_adj[3] < distancias[0] && ok == 1 ) /*SE A MENOR DISTANCIA MEDIDA ESTA ACIMA DA TOLERANCIA, ENTÃO ESTA SUPOSTAMENTE ESTA FORA DO TRAJETO */
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
          Serial.println("Voce voltou para rota!!!\nTudo Certo :");
          controle=0;
       }
     }      
     else
     { 
       // Serial.println("ACENDE UM LED!!!!!!!!!!!!!!!!");
     }
  }
  delay(5000);

}

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
