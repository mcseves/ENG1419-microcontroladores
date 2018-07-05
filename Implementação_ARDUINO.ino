#include <EEPROM.h>

void ordenaDistancias(float vetor[], int n);

/* COEFICIENTES DAS RETAS TRAÇADAS POR INTERPOLAÇÃO */
  float a[250];
  float b[250];
  float c[250];


/* VARIÁVEIS DE CONTROLE PARA ANALISAR SE ESTÁ DENTRO DA ROTA */
  float distancias[10]; //ATUALIZADO A CADA 1 MIN
  float raiz;
  float temp;
  int controle=0;


/* Localização da rota recebida EEPROM --- 2,4Kbytes  */
  float  latitude[250]; 
  float  longitude[250];


/* Variáveis para leitura da memória EEPROM */
   int endereco;
   int cont;
   int limit;
   float dadosRecebido;


/* Variáveis para inserir dados na memória EEPROM */
   float dadosInserir;
   String dados;


/* Coordenadas do GPS: */
   float latitude_gps[5] =  {-43.43565675, 23.56765678, 24.34567823, -94.34215678, 10.45675478}; 
   float longitude_gps[5] =  {43.43775675, 28.55765678, 56.34567723, 94.34215678, 10.45675478}; 



/* EQUIVALENTE a distancia de 730m | PUC-SHOPPING DA GÁVEA */
const float D_TOLERENCIA =  0.00701;  

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
  while (k < 4) 
  {
   Serial.print("Latitude*:");
   Serial.println(latitude[k],8);

   Serial.print("Longitude*:");
   Serial.println(longitude[k],8);
   k++; 
  }

 Serial.println();

  /* CALCULO DAS RETAS POR INTERPOLAÇÃO:
  /* No plano cartesiano o eixo x está representado pela latitude e o eixo y pela longitude  */
  k=0;
  while(k < 4) /* Para 5 pontos preciso de 4 retas---> k=[0,3]*/
  {
    /* 
     *  Coeficientes das RETAS:
     *  Sejam dois pontos no plano (x,y) = (latitude,longitude), temos:
     * A = y1 - y2
     * B = x2 - x1
     * C = x1*y2 - x2*y1
     */
    a[k] = longitude[k] - longitude[k+1];
    b[k] = latitude[k+1] - latitude[k];
    c[k] = ((latitude[k])*(longitude[k+1])) - ((latitude[k+1])*(longitude[k]));


    /* CALCULA AS DISTÂNCIAS ENTRE O PONTO DE LOCALIZAÇÃO GPS E AS RETAS PRÉ-DEFINIDAS */ 
    temp = pow(a[k],2) + pow(b[k],2);
    raiz = sqrt(temp);
    distancias[k] = abs(( (a[k] * latitude_gps[k]) + (b[k] * longitude_gps[k]) + c[k])) / raiz  ;
    
    Serial.println("-----> Coeficientes:");
    Serial.print("A=");
    Serial.print(a[k],8);
    Serial.print(" B=");
    Serial.print(b[k],8);
    Serial.print(" C=");
    Serial.print(c[k],8);
    Serial.println();
    Serial.println();
    Serial.print("Distancia minima entre a localizacao GPS e alguma reta ");
    Serial.print(k+1);
    Serial.print(": ");
    Serial.print(distancias[k],8);
    Serial.println();
    Serial.println();
    k++;
  }

    
  /* ORDENA O VETOR DE DISTÂNCIAS EM ORDEM CRESCENTE */
  ordenaDistancias(distancias, 4);  // número de pontos-1
  Serial.print("Menor distancia encontrada: ");
  Serial.println(distancias[0],8);
  Serial.print("TOLERAVEL:");
  Serial.print(D_TOLERENCIA,8);
  Serial.println();
  Serial.println("===============================================================================");


  /*  PASSAR PARA O LOOP COM INTERVALOS DE 1MIN -- CALCULO DAS DISTANCIAS, VERIFICAÇÃO E AÇÃO */
  /*SE A MENOR DISTANCIA MEDIDA ESTA ACIMA DA TOLERANCIA, ENTÃO ESTA SUPOSTAMENTE ESTA FORA DO TRAJETO */

  /*
  if( D_TOLERENCIA < distancias[0] )
  { 
     controle++;
     if(controle > 0 && controle < 5)
     {
        Serial.println("Voce esta fora da rota definida!!!");
        Serial.println("Volte imediatamente para rota.");
     }
     else
     {
        Serial.println("DESTRUICAO");
     }
  }
  */

}

void loop() {
    if( D_TOLERENCIA < distancias[0] ){ /*SE A MENOR DISTANCIA MEDIDA ESTA ACIMA DA TOLERANCIA, ENTÃO ESTA SUPOSTAMENTE ESTA FORA DO TRAJETO */
     controle++;
     if(controle > 0 && controle < 5)
     {
        Serial.println("Voce esta fora da rota definida!!!");
        Serial.println("Volte imediatamente para rota.");
     }
     else
     {
        Serial.println("DESTRUICAO");
     }

  }

  delay(5000);

}

void ordenaDistancias(float vetor[], int n){
    int k, j;
    float aux;

    for (k = 1; k < n; k++) {
        printf("\n[%d] ", k);

        for (j = 0; j < n - 1; j++) {
            printf("%d, ", j);

            if (vetor[j] > vetor[j + 1]) {
                aux          = vetor[j];
                vetor[j]     = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}
