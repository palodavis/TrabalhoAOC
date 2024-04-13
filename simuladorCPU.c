#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//variaveis globais
unsigned char memoria[154], ir, ro0, ro1, ro2, e, l, g;
unsigned int reg[16], mbr, mar, imm, pc = 0;
unsigned char *pt; //Ponteiro de leitura do arquivo .txt

/**
 * conjunto de instrucoes da CPU
 * e, l, g registragores internos: flags
 * pc endereco da proxima instrucao
 * mar endereco de memoria a lida ou escrita na memoria
 * imm operando imediato da instrucao
 * ir opcode da instrucao a ser executada
 * mbr contido a palavra a ser armazenada na memoria
 * ro0 endereco do primeiro operando registrador da instrucao
 * ro1 endereco do segundo operando registrador da instrucao
 * ro2 endereco do terceiro operando registrador da instrucao
 * reg[ro0], reg[ro1] , reg[ro2] registradores de proposito geral
*/
void buscaDecodificaExecuta() {
	mar = pc;
	mbr = memoria[mar++] << 8;
	mbr = (mbr | memoria[mar++]) << 8;
	mbr = (mbr | memoria[mar++]) << 8;
	mbr = mbr | memoria[mar++];
	//opcode com os 5 bits mais significativos
	ir = (mbr >> 27);

	if (ir == 0) { // hlt
        printf("Programa encerrado pelo comando 'hlt' no arquivo de instruções.\n");
		exit(0);//CPU nao faz nada, hlt, finalizar programa
	} else if (ir == 1) { //nop
		pc += 4;//apenas incrementar o PC
	} else if (ir >= 2 && ir <= 4){
		ro0 = ((mbr & 0x07800000) >> 23);
		ro1 = (mbr &  0x00780000) >> 19;

		if (ir == 2) { //not
		    reg[ro0] = !reg[ro0];// negar o registrador regX;
			pc += 4;
		} else if (ir == 3) { //movr
			reg[ro0] = reg[ro1];// movimentar regX p/ regY
			pc += 4;
		}
		else if (ir == 4) { //cmp
			if (reg[ro0] == reg[ro1]){//comparar palavra no regX c/ palavra no regY
				e = 0x01;
			} else {
				e = 0x00;
			}

			if (reg[ro0] < reg[ro1]){
				l = 0x01;
			} else {
				l = 0x00;
			}

			if (reg[ro0] > reg[ro1]){
				g = 0x01;
			} else {
				g = 0x00;
			}

            pc += 4;
		}
	} else if ( ir == 5) { // LDBO
		ro0 = ((mbr & 0x07800000) >> 23);
		ro1 = (mbr &  0x00780000) >> 19;
        mar = (mbr & 0x007fffff) + reg[ro1]; // Calcula o endereco de memoria baseado em M[Z] + reg[ro1]
        mbr = memoria[mar++] << 8; // Carrega o valor da memoria no endereco calculado para o registrador ro0
		mbr = (mbr | memoria[mar++]) << 8;
		mbr = (mbr | memoria[mar++]) << 8;
		mbr = mbr | memoria[mar++];
        reg[ro0] = mbr;

        pc += 4;
	} else if ( ir == 6) { // STBO
		ro0 = ((mbr & 0x07800000) >> 23);
		ro1 = (mbr &  0x00780000) >> 19;
        mar = (mbr & 0x007fffff) + reg[ro1]; // Calcula o endereco de memoria baseado em M[Z] + reg[ro1]
        mbr = reg[ro0]; // Armazena o valor do registrador ro0 na memoria no endereco calculado
        memoria[mar++] = (mbr >> 24) & 0xff;
        memoria[mar++] = (mbr >> 16) & 0xff;
        memoria[mar++] = (mbr >> 8) & 0xff;
        memoria[mar++] = mbr & 0xff;

        pc += 4;
	} else if ( ir >= 7 & ir <= 13) { //operacoes logicas e aritmeticas
		ro0 = (mbr &  0x07800000) >> 23;
		ro1 = (mbr &  0x00780000) >> 19;
		ro2 = (mbr &  0x00078000) >> 15;
		if (ir == 7){ //add
			reg[ro0] = reg[ro1] + reg[ro2];
			pc+=4;
		}
		else if (ir == 8){ //sub
			reg[ro0] = reg[ro1] - reg[ro2];
			pc+=4;
		}
		else if (ir == 9){ //mul
			reg[ro0] = reg[ro1] * reg[ro2];
			pc+=4;
		}
		else if (ir == 10){ //div
				reg[ro0] = reg[ro1] / reg[ro2];
				pc+=4;
		}
		else if (ir == 11){ //and
			reg[ro0] = reg[ro1] & reg[ro2];
			pc+=4;
		}
		else if (ir == 12){ //or
			reg[ro0] = reg[ro1] | reg[ro2];
			pc+=4;
		}
		else if (ir == 13){ //xor
			reg[ro0] = reg[ro1] ^ reg[ro2];
			pc+=4;
		}
	} else if (ir >= 14 && ir <= 15) {
		ro0 = (mbr &  0x07800000) >> 23;
		ro1 = (mbr &  0x780000) >> 19;
		mar = (mbr & 0x007fffff); /**
									*  mbr:   1111 1111 1111 1111 1111 1111 1111 1111
									*  & mar  0000 0000 0111 1111 1111 1111 1111 1111
									*    =    0000 0000 0111 1111 1111 1111 1111 1111
								  */
		if (ir == 14) { //ld
			mbr = memoria[mar++];
			mbr = (mbr << 8) | memoria[mar++];
			mbr = (mbr << 8) | memoria[mar++];
			mbr = (mbr << 8) | memoria[mar++];
			reg[ro0] = mbr;
			pc += 4;
		} else if (ir == 15) { //st
			mbr = reg[ro0];
			memoria[mar++] = (mbr >> 24) & 0xff;
        	memoria[mar++] = (mbr >> 16) & 0xff;
        	memoria[mar++] = (mbr >> 8) & 0xff;
        	memoria[mar++] = mbr & 0xff;
			pc += 4;
		}
	} else if (ir == 16) { // movil
	  ro0 = (mbr & 0x07800000) >> 23; // obtem o registrador X
	    imm = (mbr & 0x0000FFFF); // obtem os 16 bits menos significativos do imediato
	    reg[ro0] = imm; // logo após atribui o valor de imm ao reg X
		pc += 4;
	} else if (ir == 17) { // movih
	    ro0 = (mbr & 0x07800000) >> 23; // obtem o registrador X
	    imm = (mbr & 0x0000FFFF); // obtem os 16 bits menos significativos do imediato
	    reg[ro0] = (reg[ro0] & 0x0000FFFF) | (imm << 16); //move os 16 bits menos significativos para parte superior regX
		pc += 4;
	} else if (ir >= 18 && ir <= 23) {
		ro0 = (mbr &  0x07800000) >> 23;
		imm = (mbr & 0x007fffff); /**
									*  mbr:   1111 1111 1111 1111 1111 1111 1111 1111
									*  & imm  0000 0000 0111 1111 1111 1111 1111 1111
									*    =    0000 0000 0111 1111 1111 1111 1111 1111
							   	  */
		if (ir == 18) { //addi
			reg[ro0] = (reg[ro0] + imm);
			pc += 4;
		} else if (ir == 19) { //subi
			reg[ro0] = (reg[ro0] - imm);
			pc += 4;
		} else if (ir == 20) { //muli
			reg[ro0] = (reg[ro0] * imm);
			pc += 4;
		} else if (ir == 21) { //divi
			reg[ro0] = (reg[ro0] / imm);
			pc += 4;
		} else if (ir == 22) { //lsh
			reg[ro0] = (reg[ro0] << imm);
			pc += 4;
		} else if (ir == 23) { //rsh
			reg[ro0] = (reg[ro0] >> imm);
			pc += 4;
		}
	} else if (ir >= 24 && ir <= 30){ // instrucoes de jump
		mar = (mbr & 0x007fffff);

		if (ir == 24) { //je
			if (e == 0x01) {
				pc = mar;
			} else {
                pc += 4;
            }
		} else if (ir == 25) { //jne
			if (e == 0x00) {
				pc = mar;
			} else {
                pc += 4;
            }
		} else if (ir == 26) { //jl
 			if (l == 0x01) {
				pc = mar;
			} else {
                pc += 4;
             }
		} else if (ir == 27) { //jle
			if (e == 0x01 || l == 0x01) {
				pc = mar;
			}else{
                pc += 4;
            }
		} else if (ir == 28) { //jg
			if (g == 0x01) {
				pc = mar;
			} else {
                pc += 4;
            }
		} else if (ir == 29) { //jge
			if (e == 0x01 || g == 0x01) {
				pc = mar;
			} else {
                pc += 4;
            }
		} else if (ir == 30) { //jmp
			pc = mar;
		}
	}
}

/**
 * Aplica mascara e desloca bits a direita para pegar de 8 em 8 bits todos os bits
 * da variavel 32 bits 'valor' e atribuir em 4 posicoes na memoria RAM a partir da posicao
 * definida em 'posInicial'
 * @param posInicial posicao inicial da memoria que sera preenchida ela e as 3 posteriores
 * @param valor valor inteiro sem sinal 32 bits para ser armazenado na memoria (vetor de char 8 bits)
 */
void guardarValorParaMemoria(unsigned int posInicial, unsigned int valor){
    memoria[posInicial++] = (valor & 0xff000000) >> 24;//pega de 8 em 8 os bits da variavel valor (32 bits) e desloca para o inicio para ser convertido em char e armazenado em 4 posicoes
    memoria[posInicial++] = (valor & 0x00ff0000) >> 16;
    memoria[posInicial++] = (valor & 0x0000ff00) >> 8;
    memoria[posInicial]   = (valor & 0x000000ff);
}

/**
 * Preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits e completa com zero os demais
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaPrimeiroFormato(unsigned int opcode, unsigned int posicao){
    opcode = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    guardarValorParaMemoria(posicao, opcode);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits e um registrador nos proximos 4 bits
 * completando com 0 a esquerda o restante
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser utilizado na instrucao
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaSegundoFormato(unsigned int opcode, unsigned int rX, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representarao na variavel valor os 5 bits do opcode + 4 bits do rX
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, um registrador nos proximos 4 bits
 * e outro registrador nos proximos 4 bits
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param rY registrador Y para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaTerceiroFormato(unsigned int opcode, unsigned int rX, unsigned int rY, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = (rY << 19) | valor;//desloca o valor de rY para os 13 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rY
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, primeiro registrador nos proximos 4 bits,
 * segundo registrador nos proximos 4 bits e uma posicao da memoria nos proximos 19 bits
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param rY registrador Y para ser anexado na palavra
 * @param posMem posicao da memoria para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaQuartoFormato(unsigned int opcode, unsigned int rX, unsigned int rY, unsigned int posMem, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = (rY << 19) | valor;//desloca o valor de rY para os 13 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rY
    valor = posMem | valor;//desloca o valor de rY para os 13 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX e deixa os outros 14 bits para representar a posi��o da memoria na palavra
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, primeiro registrador nos proximos 4 bits,
 * segundo registrador nos proximos 4 bits e terceiro registrador nos proximos 4 bits, completando o restante com zeros
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param rY registrador Y para ser anexado na palavra
 * @param rZ registrador Z para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaQuintoFormato(unsigned int opcode, unsigned int rX, unsigned int rY,  unsigned int rZ, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = (rY << 19) | valor;//desloca o valor de rY para os 13 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rY
    valor = (rZ << 15) | valor;//desloca o valor de rY para os 17 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rX + 4 bits do rY + 4 bits rZ
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, primeiro registrador nos proximos 4 bits,
 * e a posicao de memoria nos ultimos 23 bits
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param posMem posicao da memoria para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaSextoFormato(unsigned int opcode, unsigned int rX, unsigned int posMem, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = posMem | valor;//desloca o valor de rX para os 9 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX e deixa os outros 23 bits para registrar a posi��o da memoria na palavra
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, preenche 4 bits com zero
 * e guarda a posicao de memoria nos ultimos 23 bits
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param posMem posicao da memoria para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaSetimoFormato(unsigned int opcode, unsigned int posMem, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (0 << 23) | valor;//desloca 23 bits a direita preenchendo com 0 os 4 bits subsequentes
    valor = posMem | valor;//desloca o valor de rX para os 9 bits mais significativos que representar�o na variavel valor os 5 bits do opcode + 4 bits do rX e deixa os outros 23 bits para registrar a posi��o da memoria na palavra
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

void processarInstrucao(int mem){
    unsigned char *ponteiro, reg0, instrucao[16];
    unsigned int count = 0, reg1, imediatoMem;

    ponteiro = strtok(pt," ,");//divide a palavra de instrucao em pedacos divididos por , ou espaco

    while(ponteiro){
        if(count == 0){
            strcpy(instrucao, ponteiro);
        }
        if (count== 1){
            reg0 = (int)strtol(ponteiro, NULL, 16);
        }
        if(count == 2){
            reg1 = (int)strtol(ponteiro,NULL,16);
        }
        if(count == 3){
            imediatoMem = (int)strtol(ponteiro,NULL,16);
        }
        ponteiro = strtok(NULL," ,r");
        count++;
    }

    if(strstr(instrucao, "nop") != NULL) {
        preencherMemoriaPrimeiroFormato(1, mem);
    }
    else if(strcmp(instrucao, "not") == 0) {
        preencherMemoriaSegundoFormato(2, reg0, mem);
    } else if(strcmp(instrucao, "movr") == 0){
        preencherMemoriaTerceiroFormato(3, reg0, reg1, mem);
    } else if(strcmp(instrucao, "cmp") == 0){
        preencherMemoriaTerceiroFormato(4, reg0, reg1, mem);
    } else if(strcmp(instrucao, "ldbo") == 0){
        preencherMemoriaQuartoFormato(5, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "stbo") == 0){
        preencherMemoriaQuartoFormato(6, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "add") == 0){
        preencherMemoriaQuintoFormato(7, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "sub") == 0){
        preencherMemoriaQuintoFormato(8, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "mul") == 0){
        preencherMemoriaQuintoFormato(9, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "div") == 0){
        preencherMemoriaQuintoFormato(10, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "and") == 0){
        preencherMemoriaQuintoFormato(11, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "or") == 0){
        preencherMemoriaQuintoFormato(12, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "xor") == 0){
        preencherMemoriaQuintoFormato(13, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "ld") == 0){
        preencherMemoriaSextoFormato(14, reg0, reg1, mem);
    } else if(strcmp(instrucao, "st") == 0){
        preencherMemoriaSextoFormato(15, reg0, reg1, mem);
    } else if(strcmp(instrucao, "movil") == 0){
        preencherMemoriaSextoFormato(16, reg0, reg1, mem);
    } else if(strcmp(instrucao, "movih") == 0){
        preencherMemoriaSextoFormato(17, reg0, reg1, mem);
    } else if(strcmp(instrucao, "addi") == 0){
        preencherMemoriaSextoFormato(18, reg0, reg1, mem);
    } else if(strcmp(instrucao, "subi") == 0){
        preencherMemoriaSextoFormato(19, reg0, reg1, mem);
    } else if(strcmp(instrucao, "muli") == 0){
        preencherMemoriaSextoFormato(20, reg0, reg1, mem);
    } else if(strcmp(instrucao, "divi") == 0){
        preencherMemoriaSextoFormato(21, reg0, reg1, mem);
    } else if(strcmp(instrucao, "lsh") == 0){
        preencherMemoriaSextoFormato(22, reg0, reg1, mem);
    } else if(strcmp(instrucao, "rsh") == 0){
        preencherMemoriaSextoFormato(23, reg0, reg1, mem);
    } else if(strcmp(instrucao, "je") == 0){
        preencherMemoriaSetimoFormato(24, reg0, mem);
    } else if(strcmp(instrucao, "jne") == 0){
        preencherMemoriaSetimoFormato(25, reg0, mem);
    } else if(strcmp(instrucao, "jl") == 0){
        preencherMemoriaSetimoFormato(26, reg0, mem);
    } else if(strcmp(instrucao, "jle") == 0){
        preencherMemoriaSetimoFormato(27, reg0, mem);
    } else if(strcmp(instrucao, "jg") == 0){
        preencherMemoriaSetimoFormato(28, reg0, mem);
    } else if(strcmp(instrucao, "jge") == 0){
        preencherMemoriaSetimoFormato(29, reg0, mem);
    } else if(strcmp(instrucao, "jmp") == 0){
        preencherMemoriaSetimoFormato(30, reg0, mem);
    }
}

/**
 * Busca pelo arquivo instrucoes.txt no mesmo diretorio do executavel e processa o arquivo
 * inserindo a instrucao na memoria caso seja uma linha de instrucao ou guardando o valor na memoria
 * caso seja uma palavra de dado
 */
void processarArquivo(){
    unsigned int index;
    unsigned int valor;
    FILE *arq;
    unsigned char tipo, linha[99];
    unsigned int count = 0;
    arq = fopen("instrucoes.txt", "r");

    while (fgets(linha, sizeof(linha), arq) != NULL){// carrega linha por linha o conteudo do arquivo de instrucoes na variavel linha
        pt = strtok(linha, "; ");// quebra a linha em pedacos separando por espacos ou ;
        index = (int)strtol(pt,NULL,16);// converte de hexadecimal para inteiro o valor do primeiro peda�o da linha (corresponde ao endere�o da memoria)
        while(pt){// itera sobre os pedacos de uma linha
            if(count == 1){//se contador 1 for o pedaco que contem o tipo do comando se o instrucao (i) ou dado (d)
                tipo = *pt;
            }else if(count == 2){//se contador 2 contem ou o valor pra ser guardado na memoria ou a instrucao para ser armazenado na memoria
                if(tipo == 'd'){//verifica se o tipo do comando desta linha � de dado, caso seja deve armazenar na memoria
                    valor = (unsigned int)strtol(pt,NULL,16);//converte para inteiro o valor em hexadecimal contido terceira parte da linha de comando caso seja linha de dado
                    guardarValorParaMemoria(index, valor);
                }else{// se esse comando for do tipo 'i' deve armazenar a instrucao contido no ponteiro 'pt' na posicao de memoria 'index'
                    processarInstrucao(index);
                }
            }

            pt = strtok(NULL ,";");
            count++;
        }
        count = 0;
    }

    if (NULL == arq)
        printf("Arquivo instrucoes.txt nao encontrado.\n");
    fclose(arq);
}

/**
 * limpa a tela do terminal e move cursor para o inicio
 */
void limparTela() {
    printf("\033[2J\033[H"); // Limpa a tela e move o cursor para o inicio
}

/**
 * Interrompe a execucao do programa ate que Enter seja digitado, continuando a execucao e limpando a tela
 * apos ser digitado Enter. Cancela a execucao se qualquer outro caracter seja digitado
 */
void solicitaContinuar(){
    printf("\n\nDigite Enter para continuar (outra tecla para sair)...\n");
    int c = getchar();
    limparTela();
    if (c != '\n') { // Se nao for outro Enter, cancela a execucao
        printf("Programa encerrado pelo usuario.\n");
        exit(0);
    }
}

void imprimirRegistradores(){
    printf("                          Registradores                          \n");
    printf("-----------+----------+---------------+---------------+-----------------+\n");
    printf("| PC: %02x   | IR %02x    | MAR: %08x | MBR: %08x | IMM: %08x   | \n", pc, ir, mar, mbr, imm);
    printf("-----------+----------+---------------+---------------------------------+\n");
    printf("| E: %x     | L: %x     | G: %x          | RO0: %02x   | RO1: %02x |  RO2: %02x  |\n", e, l, g, ro0, ro1, ro2);
    printf("------------------------------------------------------------------------+\n");
    printf("| REG00: %08x | REG01: %08x | REG02: %08x | REG03: %08x |\n", reg[0], reg[1], reg[2], reg[3]);
    printf("------------------+-----------------+-----------------+-----------------+\n");
    printf("| REG04: %08x | REG05: %08x | REG06: %08x | REG07: %08x |\n", reg[4], reg[5], reg[6], reg[7]);
    printf("------------------+-----------------+-----------------+-----------------+\n");
    printf("| REG08: %08x | REG09: %08x | REG10: %08x | REG11: %08x |\n", reg[8], reg[9], reg[10], reg[11]);
    printf("------------------+-----------------+-----------------+-----------------+\n");
    printf("| REG12: %08x | REG13: %08x | REG14: %08x | REG15: %08x |\n", reg[12], reg[13], reg[14], reg[15]);
    printf("------------------+-----------------+-----------------+-----------------+\n");
}

void imprimirMemoria(){
    printf("\n                           Memoria                           ");
   	unsigned int i;

    for(  i = 0; i < 154; i++) {
        if (i%11 == 0)//adiciona quebra de linha a cada 11 elementos
            printf("\n----------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+\n|");
        printf(" %02x = %02x |", i, memoria[i]);
    };
}

/**
 * Imprime no terminal uma logo feita em codigo ANSI
 */
void imprimirLogo() {
    printf("\e[3mAlunos: Henrique Barros e Paulo David \e[0m \n");
    printf("\n                                  \033[31m 100001  \033[32m 1010101  10101011\n                                \033[31m 001111001 \033[32m 1     0  1      1\n                                \033[31m 111000111 \033[32m 0     0  0      1\n                                  \033[31m 11111   \033[32m 1101011  11010101\n\n                                \033[32m 1010101  10101011\n                                 1     0  1      1\n                                 0     0  0      1\n                                 1101011  11010101 \n\n                                 1010101  10101011  01010101\n                                 1     0  1      1  1      1\n                                 0     0  0      1  0      1\n                                 1101011  11010101  11010101\n                \n                                 1010101  10101011\n                                 1     0  1      1\n                                 0     0  0      1\n                                 1101011  11010101\n\n\n            \033[0m 1  00     0   00000  001001  0  001000  0   1  001000  1000\n             0  0  1   0  100       0     0    1     1   0    0    0    0\n             0  0   0  0     100    1     0    1     0   0    0    0    0\n             0  0    111  11100     0     1    0      101     1     1000\n\n                     0000  0000  0001   0000 00000   0     0\n                     0     0     0   0  0    0   0  1 0    0\n                     0000  000   0    0 000  00000  0  1   1 \n                     0     0     0   0  0    0  0  110001  1 \n                     0     0001  0000   0000 0   0 0     0 0000");

    solicitaContinuar();
}

int main(){
    imprimirLogo();

	processarArquivo();

    while(1){
        buscaDecodificaExecuta();
        imprimirRegistradores();
        imprimirMemoria();
        solicitaContinuar();
    }
}
