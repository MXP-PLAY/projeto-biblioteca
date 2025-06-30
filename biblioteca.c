#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITULO 100
#define MAX_AUTOR 100
#define MAX_ISBN 20
#define MAX_LEITOR 100
#define MAX_DATA 16

typedef enum { FICCAO = 1, DIDATICO, BIOGRAFIA,  ROMANCE,  TERROR,  CIENCIA,  HISTORIA,  FANTASIA,  AUTOAJUDA,  OUTRO
} Genero;

typedef enum { EM_ANDAMENTO = 1, CONCLUIDO } Status;

typedef struct {
    char titulo[MAX_TITULO];
    char autor[MAX_AUTOR];
    char isbn[MAX_ISBN];
    Genero genero;
    int disponivel;
} Livro;

typedef struct {
    int id;
    char leitor[MAX_LEITOR];
    char isbnLivro[MAX_ISBN];
    char data[MAX_DATA];
    char dataDevolucao[MAX_DATA];
    Status status;
} Emprestimo;

Livro* livros = NULL;
Emprestimo* emprestimos = NULL;
int totalLivros = 0;
int totalEmprestimos = 0;

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerString(const char* msg, char* destino, int tamanho) {
    printf("%s", msg);
    if (fgets(destino, tamanho, stdin)) {
        destino[strcspn(destino, "\n")] = '\0';
    } else {
        limparBuffer();
        destino[0] = '\0';
    }
}

int lerInt(const char* msg, int min, int max) {
    char linha[20];
    int valor;
    while (1) {
        printf("%s", msg);
        if (!fgets(linha, sizeof(linha), stdin)) {
            limparBuffer();
            continue;
        }
        if (sscanf(linha, "%d", &valor) == 1 && valor >= min && valor <= max)
            return valor;
        printf("Entrada invalida.\n");
    }
}

Genero escolherGenero() {
    printf("1. Ficcao\n");
    printf("2. Didatico\n");
    printf("3. Biografia\n");
    printf("4. Romance\n");
    printf("5. Terror\n");
    printf("6. Ciencia\n");
    printf("7. Historia\n");
    printf("8. Fantasia\n");
    printf("9. Autoajuda\n");
    printf("10. Outro\n");
    return (Genero)lerInt("Escolha o genero: ", 1, 10);
}

Status escolherStatus() {
    printf("1. Em andamento\n2. Concluido\n");
    return (Status)lerInt("Escolha o status: ", 1, 2);
}

void salvarLivros() {
    FILE* f = fopen("livros.dat", "wb");
    if (!f) return;
    fwrite(&totalLivros, sizeof(int), 1, f);
    fwrite(livros, sizeof(Livro), totalLivros, f);
    fclose(f);
}

void carregarLivros() {
    FILE* f = fopen("livros.dat", "rb");
    if (!f) return;
    fread(&totalLivros, sizeof(int), 1, f);
    livros = malloc(totalLivros * sizeof(Livro));
    fread(livros, sizeof(Livro), totalLivros, f);
    fclose(f);
}

void salvarEmprestimos() {
    FILE* f = fopen("emprestimos.dat", "wb");
    if (!f) return;
    fwrite(&totalEmprestimos, sizeof(int), 1, f);
    fwrite(emprestimos, sizeof(Emprestimo), totalEmprestimos, f);
    fclose(f);
}

void carregarEmprestimos() {
    FILE* f = fopen("emprestimos.dat", "rb");
    if (!f) return;
    fread(&totalEmprestimos, sizeof(int), 1, f);
    emprestimos = malloc(totalEmprestimos * sizeof(Emprestimo));
    fread(emprestimos, sizeof(Emprestimo), totalEmprestimos, f);
    fclose(f);
}

void cadastrarLivro() {
    livros = realloc(livros, (totalLivros + 1) * sizeof(Livro));
    Livro* l = &livros[totalLivros];

    lerString("Titulo: ", l->titulo, MAX_TITULO);
    lerString("Autor: ", l->autor, MAX_AUTOR);
    lerString("ISBN: ", l->isbn, MAX_ISBN);
    l->genero = escolherGenero();
    l->disponivel = 1;

    totalLivros++;
    salvarLivros();
    printf("Livro cadastrado com sucesso.\n");
}

void listarLivros() {
    printf("=== Lista de Livros ===\n");
    for (int i = 0; i < totalLivros; i++) {
        const char* generoStr =
            (livros[i].genero == FICCAO ? "Ficcao" :
             livros[i].genero == DIDATICO ? "Didatico" :
             livros[i].genero == BIOGRAFIA ? "Biografia" :
             livros[i].genero == ROMANCE ? "Romance" :
             livros[i].genero == TERROR ? "Terror" :
             livros[i].genero == CIENCIA ? "Ciencia" :
             livros[i].genero == HISTORIA ? "Historia" :
             livros[i].genero == FANTASIA ? "Fantasia" :
             livros[i].genero == AUTOAJUDA ? "Autoajuda" :
             "Outro");

        printf("%s | %s | %s | %s | %s\n",
               livros[i].titulo,
               livros[i].autor,
               livros[i].isbn,
               generoStr,
               livros[i].disponivel ? "Disponivel" : "Emprestado");
    }
}

void editarLivro() {
    char isbn[MAX_ISBN];
    lerString("ISBN do livro a editar: ", isbn, MAX_ISBN);

    for (int i = 0; i < totalLivros; i++) {
        if (strcmp(livros[i].isbn, isbn) == 0) {
            printf("Editando livro: %s\n", livros[i].titulo);
            lerString("Novo titulo: ", livros[i].titulo, MAX_TITULO);
            lerString("Novo autor: ", livros[i].autor, MAX_AUTOR);
            livros[i].genero = escolherGenero();
            salvarLivros();
            printf("Livro atualizado com sucesso.\n");
            return;
        }
    }
    printf("Livro nao encontrado.\n");
}

void removerLivro() {
    char isbn[MAX_ISBN];
    lerString("ISBN do livro a remover: ", isbn, MAX_ISBN);

    for (int i = 0; i < totalLivros; i++) {
        if (strcmp(livros[i].isbn, isbn) == 0) {
            if (!livros[i].disponivel) {
                printf("Livro esta emprestado. Nao pode ser removido.\n");
                return;
            }
            for (int j = i; j < totalLivros - 1; j++) {
                livros[j] = livros[j + 1];
            }
            totalLivros--;
            livros = realloc(livros, totalLivros * sizeof(Livro));
            salvarLivros();
            printf("Livro removido com sucesso.\n");
            return;
        }
    }
    printf("Livro nao encontrado.\n");
}

void registrarEmprestimo() {
    char isbn[MAX_ISBN];
    lerString("ISBN do livro: ", isbn, MAX_ISBN);

    int index = -1;
    for (int i = 0; i < totalLivros; i++) {
        if (strcmp(livros[i].isbn, isbn) == 0 && livros[i].disponivel) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Livro nao disponivel ou nao encontrado.\n");
        return;
    }

    emprestimos = realloc(emprestimos, (totalEmprestimos + 1) * sizeof(Emprestimo));
    Emprestimo* e = &emprestimos[totalEmprestimos];

    lerString("Nome do leitor: ", e->leitor, MAX_LEITOR);
    strcpy(e->isbnLivro, isbn);
    lerString("Data do emprestimo (dd/mm/aaaa): ", e->data, MAX_DATA);
    strcpy(e->dataDevolucao, "");

    e->id = totalEmprestimos + 1;
    e->status = EM_ANDAMENTO;

    livros[index].disponivel = 0;
    totalEmprestimos++;

    salvarLivros();
    salvarEmprestimos();
    printf("Emprestimo registrado com sucesso.\n");
}

void marcarDevolucao() {
    int id = lerInt("ID do emprestimo: ", 1, totalEmprestimos);
    for (int i = 0; i < totalEmprestimos; i++) {
        if (emprestimos[i].id == id && emprestimos[i].status == EM_ANDAMENTO) {
            lerString("Data da devolucao (dd/mm/aaaa): ", emprestimos[i].dataDevolucao, MAX_DATA);
            emprestimos[i].status = CONCLUIDO;

            for (int j = 0; j < totalLivros; j++) {
                if (strcmp(livros[j].isbn, emprestimos[i].isbnLivro) == 0) {
                    livros[j].disponivel = 1;
                    break;
                }
            }

            salvarLivros();
            salvarEmprestimos();
            printf("Devolucao registrada com sucesso.\n");
            return;
        }
    }
    printf("Emprestimo nao encontrado ou ja finalizado.\n");
}

void listarEmprestimosPorStatus() {
    Status s = escolherStatus();
    printf("=== Emprestimos (Status %d) ===\n", s);
    for (int i = 0; i < totalEmprestimos; i++) {
        if (emprestimos[i].status == s) {
            printf("ID: %d | Leitor: %s | ISBN: %s | Data Emprestimo: %s",
                   emprestimos[i].id,
                   emprestimos[i].leitor,
                   emprestimos[i].isbnLivro,
                   emprestimos[i].data);

            if (emprestimos[i].status == CONCLUIDO && strlen(emprestimos[i].dataDevolucao) > 0) {
                printf(" | Data Devolucao: %s", emprestimos[i].dataDevolucao);
            }

            printf("\n");
        }
    }
}

int main() {
    carregarLivros();
    carregarEmprestimos();

    int opcao;
    do {
        printf("\n===== MENU BIBLIOTECA =====\n");
        printf("1. Cadastrar Livro\n");
        printf("2. Listar Livros\n");
        printf("3. Editar Livro\n");
        printf("4. Remover Livro\n");
        printf("5. Registrar Emprestimo\n");
        printf("6. Marcar Devolucao\n");
        printf("7. Listar Emprestimos por Status\n");
        printf("0. Sair\n");
        opcao = lerInt("Opcao: ", 0, 7);

        switch (opcao) {
            case 1: cadastrarLivro(); break;
            case 2: listarLivros(); break;
            case 3: editarLivro(); break;
            case 4: removerLivro(); break;
            case 5: registrarEmprestimo(); break;
            case 6: marcarDevolucao(); break;
            case 7: listarEmprestimosPorStatus(); break;
        }
    } while (opcao != 0);

    free(livros);
    free(emprestimos);
    printf("Sistema encerrado.\n");
    return 0;
}
