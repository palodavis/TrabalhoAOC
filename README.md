# TrabalhoAOC

## Como executar o programa:
* Instalar o compilador GCC 
* Abrir a pasta do arquivo main.c no terminal
* Executar os dois comandos em sequencia: 
```
> gcc -o meu_programa main.c

> ./meu_programa
```
## Funcionamento

Ao executar o comando ./meu_programa, uma tela será aberta, exibindo a logo do IFG (Instituto Federal de Goiás), com uma solicitação para pressionar "Enter" para continuar a execução. Durante esse processo, os registradores e a memória serão apresentados em formato hexadecimal na tela. Para avançar a execução do programa passo a passo, é necessário pressionar "Enter" novamente.

Após a conclusão do programa, isto é, quando a instrução "hlt" for executada, o programa será encerrado automaticamente.

## Configurando a Execução do programa

O programa irá buscar e decodificar as instruções contidas no arquivo "instrucoes.txt" no mesmo diretório, esse arquivo deverá conter instruções em minemônicos correspondentes ao simulador da CPU. Esse simulador possui um conjunto de instruções com 30 instruções e contém 154 posições de memória.