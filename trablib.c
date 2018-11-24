#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "trablib.h"

//Função que aloca todos os valores de cada path em uma matriz
void ler_path(FILE *path, float **mat,int taml,int tamc){
    int j=0,i=0;
	while (!feof(path)){
         for (i = 0; i < taml; i++) {
            for (j = 0; j < tamc; j++) {
            fscanf(path,"%f,",&mat[i][j]);
            }
        }
    }
}

//Função tam_coluna calcula o numero de colunas da matriz q irá armazenar os valores do arquivo .csv
int tam_coluna(char *l1){
  int i=0,tam=1;
  for ( i = 0; i < strlen(l1); i++) {
     if (l1[i]==','){
      tam = tam + 1;
     }
  }
  return tam;
}

//Função tam_linha calcula o tamanho de linhas da matriz
int tam_linha(FILE *path){
  int taml=0;
  char aux;
  while (!feof(path)){
       fscanf(path,"%c",&aux);
       if (aux == '\n') {
         taml = taml + 1;
        }
  }
  return taml;
}

//Desaloca dinamicamente uma matriz do tipo float
void libera_matriz(int l,float **mat){
    int i=0;
    for ( i = 0; i < l; i++) {
       free(mat[i]);
    }
    free (mat);
}

//Desolaca uma matriz do tipo int
void libera_matriz_int(int l,int **mat){
    int i=0;
    for ( i = 0; i < l; i++) {
       free(mat[i]);
    }
    free (mat);
}

//Calcula o tamanho de uma linha do arquivo.csv
int tam_str(FILE *path){
   char aux;
   int tam=0;
   do{
        fscanf(path,"%c",&aux);
        tam++;
   }while(aux!='\n');
   return tam;
}

//Calcula a distancia euclidiana
void dist_euclidiana (float **mat_te,float **mat_tr,float **dist,int taml_te,int tamc,int taml_tr ){
     int i=0,j=0,k=0;
     for ( k = 0; k < taml_te; ++k)
     {
        for (i = 0; i < taml_tr; ++i)
        {
           for (j = 0; j < tamc; ++j)
           {
              dist[k][i] = dist[k][i] + pow((mat_tr[i][j]-mat_te[k][j]),2);
           }
           dist[k][i] = sqrt(dist[k][i]);
      
        }
    }
}

//Calcula a distancia de minkosky
void dist_minkosky (float **mat_te,float **mat_tr,float **dist,int taml_te,int tamc,int taml_tr,float r ){
     int i=0,j=0,k=0;
     for ( k = 0; k < taml_te; ++k)
     {
        for (i = 0; i < taml_tr; ++i)
        {
           for (j = 0; j < tamc; ++j)
           {
              dist[k][i] = dist[k][i] + pow(sqrt(pow((mat_tr[i][j]-mat_te[k][j]),2)),r);
           }
           dist[k][i] = pow(dist[k][i],1/r);
        }
    }
}

//Calcula a distancia de chebyshev
void dist_chebyshev(float **mat_te, float **mat_tr,float **dist,int taml_te,int tamc,int taml_tr){
  float aux[tamc];
  int i=0,k=0,j=0;
  for (k = 0; k < taml_te; ++k)
  {
     for (i = 0; i < taml_tr; ++i)
     {  
        dist[k][i]=0;
        for (j = 0; j < tamc; ++j)
        {
           aux[j] = sqrt(pow(mat_tr[i][j]-mat_te[k][j],2));
      //Essa expressão(sqrt(pow(p[i]-q[i],2))) = |p[i]-q[i]|
           if (dist[k][i] < aux[j])
           {
             dist[k][i] = aux[j];
           }
        }
     }
  }
}

//Duplica uma matriz. Tal função é necessária para o calculo dos knn, onde a matriz dist vai ser duplicada
void duplica_mat(float **dist, float **du_dist,int taml_te,int taml_tr){
  int i=0,j=0;
  for (i = 0; i < taml_te; ++i)
  {
    for (j = 0; j < taml_tr; ++j)
    {
      du_dist[i][j] = dist[i][j];
    }
  }
}

//Essa função realiza a ordenação de um vetor usaando o algoritmo de insertion sort
void insertionSort(float *vet, int tam )
{
   int i, j;
   float aux;
 
   for( j=1; j < tam; j++ ) 
   {
      aux = vet[j];
      i = j-1;
      
      while(i >= 0 && vet[i] > aux)
      {
        vet[i+1] = vet[i];
        i--;
      } 
              
      vet[i+1] = aux;
   }
}

//Dado um valor k essa função calcula os k-primeiros numeros de uma matriz, utilizando para isso o insertionsort
void k_proximos(float **du_dist,int k,float **kp,int taml_te,int taml_tr){
  int i,j;
  for (i = 0; i < taml_te; ++i)
  {
    kp[i] = realloc(kp[i],k*sizeof(float));
    insertionSort(du_dist[i],taml_tr);
  }
  for (i = 0; i < taml_te; ++i)
  {
    for (j = 0; j < k; ++j)
    {
      kp[i][j] = du_dist[i][j];
      //PAREI AQUI printf("%.2f ",kp[i][j]);
    }
  }
  
}

//Apos o calculo do k-proximos, essa função busca a posição de cada distancia no arquivo iris_treino;
//Para com essa posição poder descobrir o rotulo;
void busca(float *dist,float *kp,int *pos,int taml_tr,int k){
  int i,j;
  for (i = 0; i < k; ++i)
  {
  	for (j = 0; j < taml_tr; ++j)
  	{
          //printf("%.2f %.2f \n",dist[j],kp[i]);
  		if (dist[j] == kp[i])
  		{
  			pos[i] = j;//+ 1;
  			//printf("%d ",pos[i] );
  			dist[j] = -1;
  			break;

  		}
  	}
  }
}

//Função que dá o rotulo de cada distancia dos k-proximos
void rotulo(float **mat_tr,float **dist,float **kp,int **pos,int taml_te,int taml_tr,int tamc,int k){
	int i,j,aux=0;
	for (int i = 0; i < taml_te; ++i)
	{
		busca(dist[i],kp[i],pos[i],taml_tr,k);
	}
    //printf("%d\n", taml_tr);
	for (j = 0; j < taml_te; ++j)
	{
		//printf("\n");
		for (i = 0; i < k; ++i)
		{  
            aux = pos[j][i];
            //if(aux == 0) {printf("%d %d %d\n",aux, j, i);}
			pos[j][i] = mat_tr[aux][tamc-1];
			//printf("%d ",pos[j][i] );
		}
	}
}

//Função que calcula quantos rotulos possui o arquivo(caso iris sao 3)
int quant_rotulo(float **mat_tr,int taml_tr,int tamc){
	int aux = 0;
	for (int i = 0; i < taml_tr; ++i)
	{
	   if (mat_tr[i][tamc-1] > aux)
	   	{
	   		aux = mat_tr[i][tamc-1];
	   	}	
	}
	return aux;
}

//Função que da o rotulo final de cada teste;ALTEREI AQUI(ROT DE FLOAT PARA INT)
int* rotulo_final(int **pos, int *rot,int num_rot,int taml_te, int k){
	FILE *f2;
    f2 = fopen ("saida.txt", "a+");
    int aux[taml_te][num_rot],help=0;
	for (int i = 0; i < taml_te; ++i)
	{
		for (int j = 0; j < num_rot; ++j)
		{
			aux[i][j]=0;
		}
	}
	for (int i = 0; i < num_rot ; ++i)
	{
		for (int j = 0; j < taml_te; ++j)
		{
			for (int d = 0; d < k; ++d)
			{
				if (pos[j][d] == i+1 )
				{
					aux[j][i] = aux[j][i] + 1;
				}
			}
		}
	}
    for (int i = 0; i < taml_te; ++i)
    {
      for ( int j = 0; j < num_rot; ++j)
      {
      	if (aux[i][j] > help)
      	{
      		help = j + 1;
      	}
      	if (j == num_rot-1)
      	{
      		rot[i] = help - 1;
      		fprintf(f2,"%d\n",rot[i] );
      		help = 0;
      	}
      }
    }
    fclose (f2);
    return rot;

}

/*
a config knn recebe os ponteiros onde serão guardadas as informações:
 da distância k (k)
 do tipo da distancia (d)
 e se for de Minchowsky, o valor de r (r)
aloca dinamicamente as variáveis e printa num arquivo saída todas as configurações
*/

void printmat (float **mat, int nl, int nc){
    int i=0,j=0;
    for (i=0;i<nl;i++){
        for (j=0;j<nc;j++){
        printf ("%f ",mat[i][j]);
        }
        printf ("\n");
    }
    printf ("\n");
}

void printmat_int (int **mat, int nl, int nc){
    int i=0,j=0;
    for (i=0;i<nl;i++){
        for (j=0;j<nc;j++){
        printf ("%d ",mat[i][j]);
        }
        printf ("\n");
    }
    printf ("\n");
}

void configknn (int k[], char d[], float r[], FILE *f1, int lconf){
    for (int i=0;i<lconf-3;i++){
        fscanf (f1, "%i, %c", &k[i], &d[i]);
        if (d[i]=='M'){
            fscanf (f1,", %f", &r[i]);
        }
    }
}

void treatstr (char s[]){
    char *b;
    if ((b=strchr(s,'\n')) != NULL){
        *b = '\0';
    }
}

void readpaths (char dtreino[], char dteste[], char saida[], FILE *f1){
    FILE *f2;
    fgets (dtreino,50000,f1);
    fgets (dteste,50000,f1);
    fgets (saida,50000,f1);

    treatstr (dtreino);
    treatstr (dteste);
    treatstr (saida);
}

void openfile (char name[]){
    FILE *f1;
    f1 = fopen (name,"r");
    if (f1==NULL){
        printf ("Erro na abertura do arquivo!\n");
    }
    fclose(f1);
}

float** aloca (char arq[],int v[2]){
    FILE *f;
    int tstr,tamc,taml;
    float **mat;
    char *l1;
    
    //Calculando o tamanho da primeira linha do iris_teste (para depois calcular o numero de colunas)
	f = fopen(arq,"r");
    tstr = tam_str(f);
    l1 = (char *)malloc(tstr*sizeof(char));
	fgets(l1,tstr,f);
    
    //Calculando o numero de colunas e o numero de linhas do iris_teste
    tamc = tam_coluna(l1);
    taml = tam_linha(f);
    //printf("%s %d\n", arq, taml);
    
    //Criando uma matriz com todos os valores do iris_teste
    mat = (float**)malloc(taml*sizeof(float*));
    for (int i = 0; i < taml; ++i)
    {
    	mat[i]=(float*)malloc(tamc*sizeof(float));
    }
    rewind (f);
    ler_path(f,mat,taml,tamc);

    v[0]=taml;
    v[1]=tamc;

    fclose (f);
    free (l1);
    return mat;
}

float** cria_mat (int l, int c){
    float **mat = {0};
	
    mat = (float**)malloc(l*sizeof(float*));
    
    for (int i = 0; i < l; ++i)
    {
    	mat[i]=(float*)malloc(c*sizeof(float));
        
    }

    return mat;
}

int** cria_mat_int (int l, int c){
    int **mat = {0};
	
    mat = (int**)malloc(l*sizeof(int*));
    
    for (int i = 0; i < l; ++i)
    {
    	mat[i]=(int*)malloc(c*sizeof(int));
        
    }

    return mat;
}

int* rotula (int taml_te, int taml_tr, int k, int tamc, float **mat_tr, float **dist, float **kp,int*rot){
    int **pos,num_rot;
    //int *rot;

    //Criação do vetor rot que conterá o rotulo de cada teste
    //rot = (int *)malloc(taml_te*sizeof(int));

    pos = (int**)malloc(taml_te*sizeof(int*));
    for (int i = 0; i < taml_te; ++i)
    {   
        rot[i] = 0;
        pos[i]=(int*)malloc(k*sizeof(int));
        for(int j=0;j < k; j++){
            pos[i][j] = 0;
        }
        
    }
    
    //Calculo do numero de rotulos dado(no caso do iris 3 e do vowels 5)
    num_rot = quant_rotulo(mat_tr,taml_tr,tamc);
    
    //Calculo do rotulo de cada distancia mais proxima;
    rotulo(mat_tr,dist,kp,pos,taml_te,taml_tr,tamc,k);

    //Calculo de qual rotulo mais aparece, para assim rotular o teste
    rot = rotulo_final(pos,rot,num_rot,taml_te,k);

    libera_matriz_int(taml_te,pos);
    return rot;
    //free (rot);
}

int* calc_dists (float **mat_te, float **mat_tr, float **dist, float **du_dist, float **kp, int taml_te,int tamc,int taml_tr, int lconf, int k, char d, float r,int* rot){
        for (int i=0;i < taml_te;i++){

            for (int j=0; j < taml_tr;j++){
                dist[i][j] = 0;
            }
        }
        switch (d){
            case 'M':
                dist_minkosky (mat_te,mat_tr,dist,taml_te,(tamc-1),taml_tr,r);
                break;
            case 'E':
                dist_euclidiana (mat_te,mat_tr,dist,taml_te,(tamc-1),taml_tr);
                break;
            case 'C':
                dist_chebyshev (mat_te,mat_tr,dist,taml_te,(tamc -1),taml_tr);
                break;
                }
        duplica_mat(dist,du_dist,taml_te,taml_tr);
        k_proximos(du_dist,k,kp,taml_te,taml_tr);
        rot = rotula (taml_te,taml_tr,k,tamc,mat_tr,dist,kp,rot);
        return rot;
}
