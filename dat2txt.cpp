#include <stdio.h>

struct cabecalho { int quantidade; int disponivel; } cabecalho;

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
        int qtd;
        if (fread(&qtd, sizeof(int), 1, data) < 0) break;
        printf("int %d\n", qtd);
        char divider;
        if (fread(&divider, sizeof(char), 1, data) < 0) break;
        printf("char %c\n", divider);
        char palavra[qtd];
        if (fread(&palavra, qtd, 1, data) < 0) break;
        printf("string %s\n", palavra);
    }
}