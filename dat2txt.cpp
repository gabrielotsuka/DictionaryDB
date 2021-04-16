#include <stdio.h>
#include <stdlib.h>

struct cabecalho { int quantidade; int disponivel; } cabecalho;
struct registro { int size; char flag; char* palavra; } registro;

int main() {
    FILE* data = fopen("dados.dat", "rb");
    FILE* output = fopen("output.txt", "w+");

    if (data == NULL) {
        printf("Erro ao abrir arquivo.\n\n");
        return 0;
    }

    fread(&cabecalho, sizeof(cabecalho), 1, data);
    printf("%d %d\n", cabecalho.quantidade, cabecalho.disponivel);

    while(!feof(data)) {
        fread(&registro.size, sizeof(int), 1, data);
        printf("int %d\n", registro.size);
        fread(&registro.flag, sizeof(char), 1, data);
        printf("char %c\n", registro.flag);
        registro.palavra = (char*) malloc(registro.size + sizeof(char));
        fread(registro.palavra, registro.size + sizeof(char), 1, data);
        printf("string %s\n", registro.palavra);
    }
}