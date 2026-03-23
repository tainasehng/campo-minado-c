#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Definindo constantes
#define BOMB -1
#define MAX 100

typedef struct {
    int linhas;
    int colunas;
    int bombas;
    int tabuleiro[MAX][MAX];  // Armazena bombas e dicas
    int revelado[MAX][MAX];   // Armazena se a célula foi revelada
} CampoMinado;

// Variáveis globais para controle do jogo
int jogoPausado = 0;
int jogoIniciado = 0;  // Indica se o jogo já foi iniciado
int jogoTerminado = 0; // Indica se o jogo terminou
time_t tempoInicio, tempoPausa, tempoTotal;  // Variáveis de controle de tempo
time_t tempoAcumulado = 0; // Tempo acumulado antes de pausar

// Protótipos
void inicializarTabuleiro(CampoMinado *campo);
void imprimirTabuleiro(CampoMinado *campo);
void gerarBombas(CampoMinado *campo);
void contarBombasVizinhas(CampoMinado *campo);
int revelarCelula(CampoMinado *campo, int linha, int coluna);
int menu();
void iniciarJogo(CampoMinado *campo, int dificuldade);
int validarEntrada(char coluna, int linha, int max_colunas, int max_linhas);
void ajuda();
void dica();
void configuracoes(CampoMinado *campo);
void pausarJogo();
void continuarJogo();
void jogada(CampoMinado *campo);
void limparBuffer();
void mostrarTempo();
int verificarVitoria(CampoMinado *campo);

// Função Principal
int main() {
    CampoMinado campo;
    int opcao, dificuldade;

    while (1) {
        opcao = menu();

        switch (opcao) {
            case 1:
                ajuda();
                break;
            case 2:
                dica();
                break;
            case 3:
                configuracoes(&campo);
                break;
            case 4:
                printf("Escolha o nível de dificuldade (1: Fácil, 2: Médio, 3: Difícil): ");
                scanf("%d", &dificuldade);
                limparBuffer();
                iniciarJogo(&campo, dificuldade);
                break;
            case 5:
                pausarJogo();
                break;
            case 6:
                continuarJogo();
                break;
            case 7:
                jogada(&campo);
                break;
            case 8:
                printf("Saindo do jogo...\n");
                return 0;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// Função para o menu principal
int menu() {
    int opcao;
    int leituraValida;

    do {
        printf("\n==== Campo Minado ====\n");
        printf("1. Ajuda\n");
        printf("2. Dica\n");
        printf("3. Configurações\n");
        printf("4. Iniciar\n");
        printf("5. Pausar\n");
        printf("6. Continuar\n");
        printf("7. Jogada\n");
        printf("8. Sair\n");
        printf("Escolha uma opção: ");

        leituraValida = scanf("%d", &opcao);
        limparBuffer();

        if (leituraValida != 1 || opcao < 1 || opcao > 8) {
            printf("Opção inválida! Por favor, escolha uma opção entre 1 e 8.\n");
        }
    } while (leituraValida != 1 || opcao < 1 || opcao > 8);

    return opcao;
}

// Função de Ajuda
void ajuda() {
    printf("==== Ajuda ====\n");
    printf("O objetivo do jogo é evitar as bombas e revelar todas as outras células.\n");
}

// Função de Dica
void dica() {
    printf("==== Dica ====\n");
    printf("Evite clicar nas bordas se você não tiver certeza sobre as bombas.\n");
}

// Função de Configurações
void configuracoes(CampoMinado *campo) {
    printf("==== Configurações ====\n");
    printf("Atualmente, você só pode ajustar o nível de dificuldade ao iniciar uma nova partida.\n");
}

// Função para pausar o jogo
void pausarJogo() {
    if (!jogoIniciado) {
        printf("Nenhum jogo em andamento para pausar.\n");
        return;
    }

    if (!jogoPausado) {
        jogoPausado = 1;
        tempoPausa = time(NULL);  // Armazena o momento em que o jogo foi pausado
        tempoAcumulado += tempoPausa - tempoInicio;  // Atualiza o tempo acumulado
        printf("Jogo pausado.\n");
    } else {
        printf("O jogo já está pausado.\n");
    }
}

// Função para continuar o jogo
void continuarJogo() {
    if (!jogoIniciado) {
        printf("Nenhum jogo em andamento para continuar.\n");
        return;
    }

    if (jogoPausado) {
        jogoPausado = 0;
        tempoInicio = time(NULL);  // Reinicia o tempo a partir do momento em que o jogo foi continuado
        printf("Continuando o jogo...\n");
    } else {
        printf("O jogo não está pausado.\n");
    }
}

// Inicializa o jogo com base no nível de dificuldade
void iniciarJogo(CampoMinado *campo, int dificuldade) {
    jogoPausado = 0;
    jogoIniciado = 1;
    jogoTerminado = 0;  // Reinicia o estado do jogo terminado
    tempoAcumulado = 0; // Reinicia o tempo acumulado
    tempoInicio = time(NULL);  // Marca o tempo de início do jogo

    switch (dificuldade) {
        case 1: // Fácil
            campo->linhas = 9;
            campo->colunas = 9;
            campo->bombas = 10;
            break;
        case 2: // Médio
            campo->linhas = 16;
            campo->colunas = 16;
            campo->bombas = 40;
            break;
        case 3: // Difícil
            campo->linhas = 16;
            campo->colunas = 30;
            campo->bombas = 99;
            break;
        default:
            printf("Dificuldade inválida, escolhendo nível Fácil.\n");
            campo->linhas = 9;
            campo->colunas = 9;
            campo->bombas = 10;
    }

    inicializarTabuleiro(campo);
    gerarBombas(campo);
    contarBombasVizinhas(campo);
    imprimirTabuleiro(campo);
}

// Inicializa todas as células como não reveladas
void inicializarTabuleiro(CampoMinado *campo) {
    for (int i = 0; i < campo->linhas; i++) {
        for (int j = 0; j < campo->colunas; j++) {
            campo->tabuleiro[i][j] = 0;
            campo->revelado[i][j] = 0;
        }
    }
}

// Gera bombas aleatoriamente no tabuleiro
void gerarBombas(CampoMinado *campo) {
    srand(time(NULL));
    int bombasColocadas = 0;
    while (bombasColocadas < campo->bombas) {
        int linha = rand() % campo->linhas;
        int coluna = rand() % campo->colunas;

        if (campo->tabuleiro[linha][coluna] != BOMB) {
            campo->tabuleiro[linha][coluna] = BOMB;
            bombasColocadas++;
        }
    }
}

// Conta o número de bombas vizinhas para cada célula
void contarBombasVizinhas(CampoMinado *campo) {
    for (int i = 0; i < campo->linhas; i++) {
        for (int j = 0; j < campo->colunas; j++) {
            if (campo->tabuleiro[i][j] == BOMB) continue;

            int bombasVizinhas = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < campo->linhas && nj >= 0 && nj < campo->colunas && campo->tabuleiro[ni][nj] == BOMB) {
                        bombasVizinhas++;
                    }
                }
            }
            campo->tabuleiro[i][j] = bombasVizinhas;
        }
    }
}

// Função que lida com a jogada do jogador
void jogada(CampoMinado *campo) {
    if (!jogoIniciado) {
        printf("Inicie um jogo primeiro.\n");
        return;
    }

    if (jogoPausado) {
        printf("O jogo está pausado. Continue ou despause para jogar.\n");
        return;
    }

    if (jogoTerminado) {
        printf("O jogo terminou. Não é possível fazer jogadas.\n");
        return;
    }

    char coluna;
    int linha;
    int leituraValida;

    printf("Informe a célula para revelar (Ex: A1): ");

    // Leitura da entrada do usuário
    leituraValida = scanf(" %c%d", &coluna, &linha);

    // Limpa o buffer de entrada após leitura
    limparBuffer();

    // Verifica se a entrada foi lida corretamente
    if (leituraValida != 2) {
        printf("Entrada inválida. Tente novamente.\n");
        return;
    }

    coluna = tolower(coluna) - 'a';
    linha -= 1;

    if (validarEntrada(coluna, linha, campo->colunas, campo->linhas)) {
        if (!revelarCelula(campo, linha, coluna)) {
            imprimirTabuleiro(campo);
            mostrarTempo();  // Exibe o tempo de jogo após a jogada
        }
    } else {
        printf("Entrada inválida. Tente novamente.\n");
    }

    // Verifica vitória
    if (verificarVitoria(campo)) {
        printf("Parabéns, você venceu!\n");
        jogoTerminado = 1;
        mostrarTempo();  // Exibe o tempo final
    }
}

// Função que exibe o tempo total de jogo
void mostrarTempo() {
    time_t tempoAtual = time(NULL);
    time_t tempoDecorrido;

    if (jogoPausado) {
        tempoDecorrido = tempoAcumulado;  // Se estiver pausado, usamos o tempo acumulado
    } else {
        tempoDecorrido = tempoAcumulado + (tempoAtual - tempoInicio);
    }

    int minutos = tempoDecorrido / 60;
    int segundos = tempoDecorrido % 60;

    printf("Tempo de jogo: %02d:%02d\n", minutos, segundos);
}

// Verifica se a jogada está dentro dos limites
int validarEntrada(char coluna, int linha, int max_colunas, int max_linhas) {
    return coluna >= 0 && coluna < max_colunas && linha >= 0 && linha < max_linhas;
}

// Limpa o buffer de entrada
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Imprime o tabuleiro de jogo
void imprimirTabuleiro(CampoMinado *campo) {
    printf("  ");
    for (int j = 0; j < campo->colunas; j++) {
        printf("%c ", 'A' + j);
    }
    printf("\n");

    for (int i = 0; i < campo->linhas; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < campo->colunas; j++) {
            if (campo->revelado[i][j]) {
                if (campo->tabuleiro[i][j] == BOMB) {
                    printf("* ");
                } else {
                    printf("%d ", campo->tabuleiro[i][j]);
                }
            } else {
                printf("- ");
            }
        }
        printf("\n");
    }
}

// Revela uma célula e retorna se o jogo acabou ou não
int revelarCelula(CampoMinado *campo, int linha, int coluna) {
    if (campo->revelado[linha][coluna]) {
        printf("Célula já revelada. Escolha outra.\n");
        return 0;
    }

    campo->revelado[linha][coluna] = 1;

    if (campo->tabuleiro[linha][coluna] == BOMB) {
        printf("Bomba! Você perdeu!\n");
        jogoTerminado = 1;
        mostrarTempo();  // Exibe o tempo final em caso de derrota
        return 1;
    }

    // Revelação automática de células vazias adjacentes (expansão)
    if (campo->tabuleiro[linha][coluna] == 0) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                int ni = linha + di;
                int nj = coluna + dj;
                if (ni >= 0 && ni < campo->linhas && nj >= 0 && nj < campo->colunas && !campo->revelado[ni][nj]) {
                    revelarCelula(campo, ni, nj);
                }
            }
        }
    }

    return 0;
}

// Verifica se o jogador venceu
int verificarVitoria(CampoMinado *campo) {
    for (int i = 0; i < campo->linhas; i++) {
        for (int j = 0; j < campo->colunas; j++) {
            if (campo->tabuleiro[i][j] != BOMB && !campo->revelado[i][j]) {
                return 0;  // Ainda existem células seguras não reveladas
            }
        }
    }
    return 1;  // Todas as células seguras foram reveladas
}
