#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "index.h"

int index_createfrom(const char *key_file, const char *text_file, Index **idx){
	int qtdLinhas = 0, maiorLinha = 0, tamLinhaAtual = 0;
	char lineKey[17], auxOcorrencia[18];
	int auxOccurrences = 0;
	char ch, *ocorrencia;
	int i, j, chaveInt = 0;
	int hashInt;

	FILE *file;

	for (i = 0; i < M; i++){ // Inicializando tabela hash
		idx[i] = malloc(sizeof(RegPointer));
	}
	
	file = fopen(text_file, "r"); // Lendo arquivo text_file
	if(file == NULL){
		printf("Error: Could not read file!\n");
		return 1;
	}
	
	// Verificando o tamanho do arquivo de texto
	ch = getc(file);
	while (ch != EOF){
		while (ch != '\n' && ch != EOF){
			tamLinhaAtual++;
			ch = getc(file);
		}
		if (tamLinhaAtual > maiorLinha){
			maiorLinha = tamLinhaAtual;
		}
		qtdLinhas++;
		tamLinhaAtual = 0;
		ch = getc(file);
	}
	
	// Gerando uma matriz de string, matriz auxiliar e vetor de ocorrências (linhas)
	char texto[qtdLinhas][maiorLinha+1];
	char auxTexto[qtdLinhas][maiorLinha+1];
	int occurrences[qtdLinhas];
	i = j = 0;
	rewind(file);
	ch = getc(file);
	while (ch != EOF){
		while (ch != '\n' && ch != EOF){
			texto[i][j++] = ch;
			ch = getc(file);
		}
		texto[i][j] = '\0';
		
		// Preparando variaveis para a proxima iteracao
		i++;
		j = 0;
		ch = getc(file);
	}
	
	fclose(file); // Fechando arquivo texto
	
	// Abrindo arquivo de chaves
	file = fopen(key_file, "r");
	if(file == NULL){
		printf("Error: Could not read file!\n");
		return 1;
	}
	ch = getc(file);

	// Extraindo cada palavra chave do key_file
	i = 0;
	while (ch != EOF){ 
		while (ch != '\n' && ch != EOF){
			lineKey[i++] = ch;
			ch = getc(file);
		}
		lineKey[i] = '\0';
		if ( strlen(lineKey) > MAX_LINE_LENGHT ){
			printf("\nPalavra chave excedeu o limite.");
			i = 0;
			chaveInt = 0;
			ch = getc(file);
			continue;
		}
		
		// Mapeando a palavra para um valor int
		for (j = 0; j < i; j++){
			chaveInt += lineKey[j];
		}
		// Obtendo posicao da palavra atual na tabela hash
		hashInt = chaveInt % M;
		printf("\nPalavra chave %s identificada. Posicao %d na tabela hash.", lineKey, hashInt);
	
		// Criando auxTexto para apalavra abaixo
		for ( i = 0 ; i < qtdLinhas ; i++ ){
			strcpy(auxTexto[i], texto[i]);
		}
		
		// Buscando palavra chave no texto (procura a palavra atual em cada linha)
		for (i = 0; i < qtdLinhas; i++){
			ocorrencia = strstr(auxTexto[i], lineKey);
			if ( ocorrencia != NULL ){
				// Achou no fim da linha. Não precisa testar o que vem depois
				if ( ocorrencia[strlen(lineKey)] == '\0' ){
					// É também o começo da linha? (na realidade, é a única palavra da linha)
					if ( *ocorrencia == *auxTexto[i] ){
						occurrences[auxOccurrences++] = i+1;
						//printf("\n %s encontrada na linha %d (unica palavra da linha).\n", lineKey, i+1);
						continue;
					}
					// É a última e tem um espaço antes, ok.
					if ( ocorrencia[-1] == ' ' ) {
						occurrences[auxOccurrences++] = i+1;
						//printf("\n %s encontrada na linha %d (ultima palavra da linha).\n", lineKey, i+1);
						continue;
					}
				}
				else {
					for ( j = 0 ; j < strlen(lineKey)+1 ; j++ ){
						auxOcorrencia[j] = ocorrencia[j];
					}
					auxOcorrencia[j] = '\0';
					/* A palavra encontrada é exatamente a palavra chave?
					Só é se, após a palavra encontrada no texto, houver
					espaço em branco, pontuações e aspas (Bob's car)
					Não se considerou construções como 
					*/
					if ( auxOcorrencia[j-1] == ' ' || auxOcorrencia[j-1] == '.' 
						|| auxOcorrencia[j-1] == '!' || auxOcorrencia[j-1] == '?'
						|| auxOcorrencia[j-1] == ';' || auxOcorrencia[j-1] == ',' 
						|| auxOcorrencia[j-1] == ':' || auxOcorrencia[j-1] == '\'' ) {
					
						// É a primeira palavra da linha?
						if ( *ocorrencia == *auxTexto[i] ){
							occurrences[auxOccurrences++] = i+1;
							//printf("\n %s encontrada na linha %d (comeco de frase).\n", lineKey, i+1);
							continue;
						}
						else {
							// Considerei que só se admite espaço antes da palavra
							if ( ocorrencia[-1] == ' ' ){
								occurrences[auxOccurrences++] = i+1;
								//printf("\n %s encontrada na linha %d (meio de frase).\n", lineKey, i+1);
								continue;
							}
							else {
								// Falhou pelo que vem antes.
								ocorrencia[0] = '-';
								i--;
							}
						}
					} else {
						// Falhou pelo que vem depois
						ocorrencia[0] = '-';
						i--;
					}
				}
			}	
			
		}
		if ( auxOccurrences > 0 ){
			(**idx)[hashInt] = (RegPointer)malloc(sizeof(Registry));
			if ( (**idx)[hashInt] == NULL ){
				return 1;
			}
			(**idx)[hashInt]->key = hashInt;
			(**idx)[hashInt]->num_occurrences = auxOccurrences;
			//printf("\n(**idx)[hashInt]->num_occurrences %d hashInt = %d ", (**idx)[hashInt]->num_occurrences, hashInt);
			(**idx)[hashInt]->occurrences = malloc(qtdLinhas*sizeof(int));
			for (i = 0; i < auxOccurrences; i++){ 
				(**idx)[hashInt]->occurrences[i] = occurrences[i];
			}
			(**idx)[hashInt]->next = NULL;
			
			printf("\nPalavra %s (posicao hash %d) apareceu %d vezes no texto nas linhas : %d", lineKey, (**idx)[hashInt]->key, (**idx)[hashInt]->num_occurrences, (**idx)[hashInt]->occurrences[0]);
			for (i = 1; i < (**idx)[hashInt]->num_occurrences; i++){
				printf(", %d", (**idx)[hashInt]->occurrences[i]);
			}
			printf("\n\n");
		}
		
		// Preparando variaveis para a proxima iteracao
		auxOccurrences = 0;
		strcpy(lineKey,"");
		i = 0;
		chaveInt = hashInt = 0;
		ch = getc(file);
	}
	
	//printf("\n num *idx num_occurrences na funcao %d key=%d posicao zero %d \n",(**idx)[434]->num_occurrences, (**idx)[434]->key, (**idx)[434]->occurrences[0]);
	fclose(file);
	return 0;
}

int index_get( const Index *idx, const char *key, int **occurrences, int *num_occurrences){
	int i, hashInt = 0, chaveInt = 0, tamanhoChave;
	//RegPointer()
	
	// Posição da palavra chave na tabela hash
	tamanhoChave = strlen(key);
	for (i = 0; i < tamanhoChave; i++){
		chaveInt += key[i];
	}
	hashInt = chaveInt % M;
	
	/*while (aux->next != NULL){
		
		
	}
	*/
	*num_occurrences = (*idx)[hashInt]->num_occurrences;
	for (i = 0; i < *num_occurrences; i++){
		occurrences[i] = &(*idx)[hashInt]->occurrences[i];
	}
	
	return 0;
}

int index_put( Index *idx, const char *key){
	int qtdLinhas = 0, maiorLinha = 0, tamLinhaAtual = 0;
	char auxOcorrencia[18];
	int auxOccurrences = 0;
	char ch, *ocorrencia;
	int i, j, chaveInt = 0;
	int hashInt;
	
	FILE *file;

	file = fopen("text_file", "r"); // Lendo arquivo text_file
	if(file == NULL){
		printf("Error: Could not read file!\n");
		return 1;
	}
	
	// Verificando o tamanho do arquivo de texto
	ch = getc(file);
	while (ch != EOF){
		while (ch != '\n' && ch != EOF){
			tamLinhaAtual++;
			ch = getc(file);
		}
		if (tamLinhaAtual > maiorLinha){
			maiorLinha = tamLinhaAtual;
		}
		qtdLinhas++;
		tamLinhaAtual = 0;
		ch = getc(file);
	}
	
	// Gerando uma matriz de string, matriz auxiliar e vetor de ocorrências (linhas)
	char texto[qtdLinhas][maiorLinha+1];
	char auxTexto[qtdLinhas][maiorLinha+1];
	int occurrences[qtdLinhas];
	i = j = 0;
	rewind(file);
	ch = getc(file);
	while (ch != EOF){
		while (ch != '\n' && ch != EOF){
			texto[i][j++] = ch;
			ch = getc(file);
		}
		texto[i][j] = '\0';
		
		// Preparando variaveis para a proxima iteracao
		i++;
		j = 0;
		ch = getc(file);
	}
	
	fclose(file); // Fechando arquivo texto
	
	// Mapeando a palavra para um valor int
	i = strlen(key);
	for (j = 0; j < i; j++){
		chaveInt += key[j];
	}
	// Obtendo posicao da palavra atual na tabela hash
	hashInt = chaveInt % M;
	
	//Criando auxTexto para a palavra abaixo
	for ( i = 0 ; i < qtdLinhas ; i++ ){
		strcpy(auxTexto[i], texto[i]);
	}
		
	// Buscando palavra chave no texto (procura a palavra atual em cada linha)
	for (i = 0; i < qtdLinhas; i++){
		ocorrencia = strstr(auxTexto[i], key);
		if ( ocorrencia != NULL ){
			//Achou no fim da linha. Não precisa testar o que vem depois
			if ( ocorrencia[strlen(key)] == '\0' ){
				//É também o começo da linha? (na verdade, é a única palavra da linha)
				if ( *ocorrencia == *auxTexto[i] ){
					//printf("\n Inserir %s linha %d na hash (única da linha)", lineKey, i+1);
					occurrences[auxOccurrences++] = i+1;
					continue;
				}
				//É a última e tem um espaço antes, ok.
				if ( ocorrencia[-1] == ' ' ) {
					occurrences[auxOccurrences++] = i+1;
					continue;
				}
			}
			else {
				for ( j = 0 ; j < strlen(key)+1 ; j++ ){
					auxOcorrencia[j] = ocorrencia[j];
				}
				auxOcorrencia[j] = '\0';
				/*A palavra encontrada é exatamente a palavra chave?
				Só é se, após a palavra encontrada no texto, houver
				espaço em branco, pontuações e aspas (Bob's car)
				Não se considerou construções como 
				*/
				if ( auxOcorrencia[j-1] == ' ' || auxOcorrencia[j-1] == '.' 
					|| auxOcorrencia[j-1] == '!' || auxOcorrencia[j-1] == '?'
					|| auxOcorrencia[j-1] == ';' || auxOcorrencia[j-1] == ',' 
					|| auxOcorrencia[j-1] == ':' || auxOcorrencia[j-1] == '\'' ) {
				
					//É a primeira palavra da linha?
					if ( *ocorrencia == *auxTexto[i] ){
						//printf("\n Inserir %s linha %d na hash (começo de frase)", lineKey, i+1);
						occurrences[auxOccurrences++] = i+1;
						continue;
					}
					else {
						//Considerei que só se admite espaço antes da palavra
						if ( ocorrencia[-1] == ' ' ){
							//printf("\n Inserir %s linha %d na hash (meio de frase com espaço antes)", lineKey, i+1);
							occurrences[auxOccurrences++] = i+1;
							continue;
						}
						else {
							//Falhou pelo que vem antes.
							ocorrencia[0] = '-';
							i--;
						}
					}
				} else {
					//Falhou pelo que vem depois
					ocorrencia[0] = '-';
					i--;
				}
			}
		}	
			
	}
	if ( auxOccurrences > 0 ){
		(*idx)[hashInt] = (RegPointer)malloc(sizeof(Registry));
		if ( (*idx)[hashInt] == NULL ){
			return 1;
		}
		(*idx)[hashInt]->key = hashInt;
		(*idx)[hashInt]->num_occurrences = auxOccurrences;
		//printf("\n(**idx)[hashInt]->num_occurrences %d hashInt = %d ", (**idx)[hashInt]->num_occurrences, hashInt);
		(*idx)[hashInt]->occurrences = malloc(qtdLinhas*sizeof(int));
		for (i = 0; i < auxOccurrences; i++){ 
			(*idx)[hashInt]->occurrences[i] = occurrences[i];
		}
		(*idx)[hashInt]->next = NULL;
			
		//printf("\n %d %d %d %d", (**idx)[hashInt]->key, (**idx)[hashInt]->num_occurrences, (**idx)[hashInt]->occurrences[0], (**idx)[hashInt]->occurrences[1]);
	}
		
	// Preparando variaveis para a proxima iteracao
	auxOccurrences = 0;
	i = 0;
	chaveInt = hashInt = 0;
	
	//printf("\n num *idx num_occurrences na funcao %d key=%d posicao zero %d \n",(**idx)[434]->num_occurrences, (**idx)[434]->key, (**idx)[434]->occurrences[0]);
	
	return 0;
}

/*
int index_print(const Index *idx){
	return 0;
}*/

