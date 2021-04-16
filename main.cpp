/*
Desenvolvido e testado em ambiente Linux.

Autores:
    - Gabriel José Bueno Otsuka          11721BCC018
    - Marcos Felipe Belisário            11811BCC020
    - Pedro Henrique Bufulin de Almeida  11711BCC028
*/

#include <algorithm>  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

class MeuArquivo {
public:
    struct cabecalho { int quantidade; int disponivel; } cabecalho;

    // construtor: abre arquivo. Essa aplicacao deveria ler o arquivo se existente ou criar um novo.
    // Entretando recriaremos o arquivo a cada execucao ("w+").
    MeuArquivo() {
        this->cabecalho.quantidade = 0;
        this->cabecalho.disponivel = -1;
        fd = fopen("dados.dat","w+");

        fwrite(&this->cabecalho, sizeof(this->cabecalho), 1, this->fd);
    }

    // Destrutor: fecha arquivo
    ~MeuArquivo() {
        fclose(fd);
    }

    // Insere uma nova palavra, consulta se há espaco disponível ou se deve inserir no final
    void inserePalavra(char *palavra) {
        this->substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0

        fseek(fd, 0, SEEK_SET);
        fread(&cabecalho, sizeof(struct cabecalho), 1, fd);
        cabecalho.quantidade += 1;
        fseek(fd, 0, SEEK_SET);
        fwrite(&cabecalho, sizeof(struct cabecalho), 1, fd);
        
        int currentOffset = cabecalho.disponivel;
        int lastOffset = -1;
        bool success = false;
        while (currentOffset != -1) {
            fseek(fd, currentOffset - (sizeof(int) + sizeof(char)), SEEK_SET);
            int sz;
            fread(&sz, sizeof(int), 1, fd);
            fseek(fd, sizeof(char), SEEK_CUR);
            int next;
            fread(&next, sizeof(int), 1, fd);
            fseek(fd, sz - sizeof(int), SEEK_CUR);

            if(sz >= strlen(palavra)+1) {
                fseek(fd, -(sz + sizeof(char)), SEEK_CUR);
                char flag = ' ';
                fwrite(&flag, sizeof(char), 1, fd);
                fwrite(palavra, sz, 1, fd);
                if(lastOffset == -1){
                    fseek(fd, 0, SEEK_SET);
                    cabecalho.disponivel = next;
                    fwrite(&cabecalho, sizeof(struct cabecalho), 1, fd);
                }
                else {
                    fseek(fd, lastOffset, SEEK_SET);
                    fwrite(&next, sizeof(int), 1, fd);
                }
                success = true;
                break;
            }

            lastOffset = currentOffset;
            currentOffset = next;
        }
        if(success) return ;
        fseek(fd, 0, SEEK_END);
        int sz = strlen(palavra)+1; //é o tamanho exato da palavra
        sz = max(sz, (int) sizeof(int));
        fwrite(&sz, sizeof(int), 1, fd);
        char flag = ' ';
        fwrite(&flag, sizeof(char), 1, fd);
        fwrite(palavra, sz, 1, fd);
    }

    // Marca registro como removido, atualiza lista de disponíveis, incluindo o cabecalho
    void removePalavra(int offset) {
        //Vai ao início e muda o cabeçalho
        fseek(fd, 0, SEEK_SET);
        fread(&cabecalho, sizeof(struct cabecalho), 1, fd);
        cabecalho.quantidade -= 1;

        int lastDeletedOffset = cabecalho.disponivel;
        cabecalho.disponivel = offset;
        fseek(fd, 0, SEEK_SET);
        fwrite(&cabecalho, sizeof(struct cabecalho), 1, fd);

        fseek(fd, offset - sizeof(char), SEEK_SET);
        char flag='*';
        fwrite(&flag, sizeof(char), 1, fd);
        fwrite(&lastDeletedOffset, sizeof(int), 1, fd);
    }

    // BuscaPalavra: retorno é o offset para o registro
    // Nao deve considerar registro removido
    int buscaPalavra(char *palavra) {
        this->substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0

        fseek(fd, sizeof(struct cabecalho), SEEK_SET);
        int cont = 0;
        while (!feof(fd)){
            int sz;
            fread(&sz, sizeof(int), 1, fd);
            char flag;
            fread(&flag, sizeof(char), 1, fd);
            if(flag == '*') {
                fseek(fd, sz, SEEK_CUR);
                continue;
            }
            char now[sz];
            fread(now, sz, 1, fd);
            if(strcmp(palavra, now) == 0) {
                return ftell(fd) - sz;
            }
        }

        // retornar -1 caso nao encontrar
        return -1;
    }

private:
    // descritor do arquivo é privado, apenas métodos da classe podem acessa-lo
    FILE *fd;

    // funcao auxiliar substitui terminador por \0
    void substituiBarraNporBarraZero(char *str) {
        int tam = strlen(str); for (int i = 0; i < tam; i++) if (str[i] == '\n') str[i] = '\0';
    }
};

int main(int argc, char** argv) {
    // abrindo arquivo dicionario.txt
    FILE *f = fopen("dicionario.txt","rt");

    // se não abriu
    if (f == NULL) {
        printf("Erro ao abrir arquivo.\n\n");
        return 0;
    }

    char *palavra = new char[50];

    // criando arquivo de dados
    MeuArquivo *arquivo = new MeuArquivo();
    while (!feof(f)) {
        fgets(palavra,50,f);
        arquivo->inserePalavra(palavra);
    }

    // fechar arquivo dicionario.txt
    fclose(f);

    printf("Arquivo criado.\n\n");

    char opcao;
    do {
        printf("\n\n1-Insere\n2-Remove\n3-Busca\n4-Sair\nOpcao:");
        opcao = getchar();
        if (opcao == '1') {
            printf("Palavra: ");
            scanf("%s",palavra);
            arquivo->inserePalavra(palavra);
        }
        else if (opcao == '2') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0) {
                arquivo->removePalavra(offset);
                printf("Removido.\n\n");
            }
        }
        else if (opcao == '3') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0)
                printf("Encontrou %s na posição %d\n\n",palavra,offset);
            else
                printf("Não encontrou %s\n\n",palavra);
        }
        if (opcao != '4') opcao = getchar();
    } while (opcao != '4');

    printf("\n\nAte mais!\n\n");

    return (EXIT_SUCCESS);
}
