/* 
desenvolvido e testado em ambiente Linux.
 
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
    struct registro { int size; char flag; char* palavra; } registro;

    // construtor: abre arquivo. Essa aplicacao deveria ler o arquivo se existente ou criar um novo.
    // Entretando recriaremos o arquivo a cada execucao ("w+").
    MeuArquivo() {
        cabecalho.quantidade = 0;
        cabecalho.disponivel = -1;
        fd = fopen("dados.dat","w+");

        fwrite(&cabecalho, sizeof(cabecalho), 1, fd);
    }

    // Destrutor: fecha arquivo
    ~MeuArquivo() {
        fclose(fd);
    }

    // Insere uma nova palavra, consulta se há espaco disponível ou se deve inserir no final
    void inserePalavra(char *palavra) {
        substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0

        int sz = strlen(palavra);
        registro.size = max(sz, (int) sizeof(int)); //Ao ser excluído,campo palavra armazenará int
        registro.flag = ' ';
        registro.palavra = palavra;

        // printf("%d%c%s", registro.size, registro.flag, registro.palavra);

        fwrite(&registro, sizeof(registro), 1, fd);
    }

    // Marca registro como removido, atualiza lista de disponíveis, incluindo o cabecalho
    void removePalavra(int offset) {
        //Vai ao início e muda o cabeçalho
        fseek(fd, 0, SEEK_SET);
        int qtd;
        fread(&qtd, sizeof(int), 1, fd);
        fseek(fd, -sizeof(int), SEEK_CUR);
        qtd -= 1;
        fwrite(&qtd, sizeof(int), 1, fd);

        //Pega o segundo elemento do cabeçalho e substitui pelo offset da nova palavra deletada
        int lastDeletedOffset;
        fread(&lastDeletedOffset, sizeof(int), 1, fd);
        fseek(fd, -sizeof(int), SEEK_CUR);
        fwrite(&offset, sizeof(int), 1, fd);

        fseek(fd, offset - sizeof(char), SEEK_SET);
        char flag='*';
        fwrite(&flag, sizeof(char), 1, fd);
        fwrite(&lastDeletedOffset, sizeof(int), 1, fd);
    }

    // BuscaPalavra: retorno é o offset para o registro
    // Nao deve considerar registro removido
    int buscaPalavra(char *palavra) {
        substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0

        fseek(fd, sizeof(cabecalho), SEEK_SET);
        while (!feof(fd)){
            fread(&registro.size, sizeof(int), 1, fd);
            fread(&registro.flag, sizeof(char), 1, fd);
            printf("%c\n", registro.flag);
            if(registro.flag == '*') {
                fseek(fd, registro.size, SEEK_CUR);
                continue;
            }
            registro.palavra = (char*) malloc(registro.size);
            fread(registro.palavra, registro.size, 1, fd);
            if(strcmp(palavra, registro.palavra) == 0) {
                return ftell(fd);
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