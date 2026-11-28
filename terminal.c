/* =============================================================================
 * PROJETO: Bíblia Digital Almeida 1911 em PT-BR (Domínio Público)
 *          Atualização Ortográfica e Gramatical realizada em 11/2026
 * OBJETIVO: Leitor de terminal para macOS (Intel e Apple Silicon). Mostra o
 *           menu, lê os capítulos (.md) e os exibe em páginas centralizadas.
 * TEXTO:   desde 28/11/2026 a limpeza do Markdown e a atualização ortográfica
 *          vêm do kernel da Bíblia (pasta kernel/): o mesmo texto do motor,
 *          cerca de 12 vezes mais rápido, já com os nomes próprios e a revisão
 *          dos versículos. As regras de ortografia NÃO ficam neste arquivo:
 *          elas são editadas no motor.biblia.c de origem (fora desta pasta) e
 *          o kernel é gerado de novo a partir dele (veja kernel/LEIAME.md).
 * AUTOR: Romulo CSR (GitHub: RomuloCSR)
 * AMBIENTE: macOS (Apple Clang), binário Universal x86_64 + arm64.
 * COMPILAR: clang -O2 terminal.c kernel/biblia_kernel.c -o motor.biblia
 * =============================================================================
 */

/* -----------------------------------------------------------------------------
 * 1. BIBLIOTECAS, LIMITES E CORES DO TERMINAL
 *    O que faz: inclui as bibliotecas do sistema e define o tamanho máximo de
 *    linha, a largura da página e as cores ANSI usadas na leitura.
 *    Por quê: a página fica centralizada e confortável em qualquer janela.
 *    Revisão: 28/11/2026 (sem mudança de comportamento).
 *    Fonte: motor.biblia.c original deste repositório (v1.1-macos).
 * ----------------------------------------------------------------------------- */
#ifndef __APPLE__
  #define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>

#ifdef __APPLE__
  #include <mach-o/dyld.h>
#endif

#ifdef _WIN32
  #include <windows.h>
#else
  #include <signal.h>
  #include <sys/ioctl.h>
  #include <termios.h>
#endif

#define MAX_LINHA 8192
#define LARGURA_PAGINA_MAX 74
#define LARGURA_PAGINA (obter_largura_pagina())

/* Códigos ANSI suaves para conforto de leitura */
#define COR_RESET   "\033[0m"
#define COR_NEGRITO "\033[1m"
#define COR_ITALICO "\033[3m"
#define COR_VERSO   "\033[1;36m"
#define COR_TITULO  "\033[1;33m"
#define COR_BORDA   "\033[36m"
#define COR_DIM     "\033[2m"

/* Redimensionamento dinâmico */
volatile sig_atomic_t redesenhar_tela = 0;


/* -----------------------------------------------------------------------------
 * 2. KERNEL DA BÍBLIA (livros, capítulos e texto)
 *    O que faz: substitui as antigas tabelas de livros e o antigo motor de
 *    texto (limpar_texto_verso, atualizar_ortografia e funções auxiliares).
 *    Nomes, pastas e total de capítulos: bk_livro_nome/pasta/capitulos(0..65),
 *    0..38 = Antigo Testamento, 39..65 = Novo Testamento.
 *    Texto de cada verso ou subtítulo: bk_verso() (limpa o Markdown e atualiza
 *    a ortografia). Os arquivos .md nunca são alterados; só lidos.
 *    Por quê: o mesmo texto do motor, cerca de 12 vezes mais rápido, com os
 *    nomes próprios e a revisão dos versículos, igual em todas as versões.
 *    Data: 28/11/2026.
 *    Fonte: kernel/ (biblia_kernel.h e biblia_kernel.c), gerado a partir do
 *    motor.biblia.c de origem; veja kernel/LEIAME.md.
 * ----------------------------------------------------------------------------- */
#include "kernel/biblia_kernel.h"

/* -----------------------------------------------------------------------------
 * 3. UTILITÁRIOS DE JANELA, CENTRALIZAÇÃO E ENTRADA
 *    O que faz: mede a janela do Terminal, centraliza títulos e linhas, desenha
 *    separadores, mostra data e hora e lê números digitados com segurança.
 *    Por quê: a leitura se ajusta quando a janela muda de tamanho (SIGWINCH).
 *    Revisão: 28/11/2026 (sem mudança de comportamento).
 *    Fonte: motor.biblia.c original deste repositório (v1.1-macos).
 * ----------------------------------------------------------------------------- */
#ifndef _WIN32
void tratar_sigwinch(int sig) {
    (void)sig;
    redesenhar_tela = 1;
}
#endif

void obter_dimensoes_terminal(int *largura, int *altura) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        *largura = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *altura = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *largura = 80;
        *altura = 24;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0 && w.ws_row > 0) {
        *largura = w.ws_col;
        *altura = w.ws_row;
    } else {
        *largura = 80;
        *altura = 24;
    }
#endif
}

int obter_largura_pagina(void) {
    int cols = 80, rows = 24;
    obter_dimensoes_terminal(&cols, &rows);
    int larg = cols - 4;
    if (larg > LARGURA_PAGINA_MAX) larg = LARGURA_PAGINA_MAX;
    if (larg < 32) larg = 32;
    return larg;
}

int calcular_margem_esquerda(int largura_conteudo) {
    int cols = 80, rows = 24;
    obter_dimensoes_terminal(&cols, &rows);
    if (cols <= largura_conteudo) return 2;
    return (cols - largura_conteudo) / 2;
}

void imprimir_espacos(int qtd) {
    for (int i = 0; i < qtd; i++) putchar(' ');
}

int comprimento_visivel_utf8(const char *texto) {
    int visivel = 0;
    for (int i = 0; texto[i] != '\0'; i++) {
        if ((texto[i] & 0xC0) != 0x80) visivel++;
    }
    return visivel;
}

void imprimir_linha_centralizada(const char *texto, int largura_pagina) {
    int visivel = comprimento_visivel_utf8(texto);
    int margem = calcular_margem_esquerda(largura_pagina);
    int recuo_interno = (largura_pagina > visivel) ? (largura_pagina - visivel) / 2 : 0;
    imprimir_espacos(margem + recuo_interno);
    printf("%s\n", texto);
}

void imprimir_separador(int largura_pagina, char simbolo) {
    int margem = calcular_margem_esquerda(largura_pagina);
    imprimir_espacos(margem);
    printf("%s", COR_BORDA);
    for (int i = 0; i < largura_pagina; i++) putchar(simbolo);
    printf("%s\n", COR_RESET);
}

void pausar_tela(void) {
    int margem = calcular_margem_esquerda(LARGURA_PAGINA);
    printf("\n");
    imprimir_espacos(margem);
    printf("%s[Pressione ENTER para voltar ao menu...]%s", COR_DIM, COR_RESET);
    fflush(stdout);
    char tmp[32];
    if (fgets(tmp, sizeof(tmp), stdin) == NULL) return;
}

void exibir_data_hora(void) {
    time_t t = time(NULL);
    struct tm tm_local = *localtime(&t);

    const char *dias_semana[] = {
        "Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira",
        "Quinta-feira", "Sexta-feira", "Sábado"
    };
    const char *meses[] = {
        "Jan", "Fev", "Mar", "Abr", "Mai", "Jun",
        "Jul", "Ago", "Set", "Out", "Nov", "Dez"
    };

    char cabecalho[128];
    snprintf(cabecalho, sizeof(cabecalho), "%s, %02d de %s de %d  |  %02d:%02d",
             dias_semana[tm_local.tm_wday],
             tm_local.tm_mday,
             meses[tm_local.tm_mon],
             tm_local.tm_year + 1900,
             tm_local.tm_hour,
             tm_local.tm_min);

    printf("%s", COR_DIM);
    imprimir_linha_centralizada(cabecalho, LARGURA_PAGINA);
    printf("%s", COR_RESET);
}

void imprimir_coluna(int numero, const char *texto) {
    int tamanho_visivel = comprimento_visivel_utf8(texto);
    printf(" %2d - %s", numero, texto);
    int espacos = 20 - tamanho_visivel;
    for (int j = 0; j < espacos; j++) putchar(' ');
}

int ler_inteiro_seguro(int *resultado) {
    char input[64];
    if (fgets(input, sizeof(input), stdin) == NULL) return -1;
    if (input[0] == '\n') return 0;

    int i = 0;
    while (input[i] != '\n' && input[i] != '\0') {
        if (!isdigit((unsigned char)input[i])) return 0;
        i++;
    }

    *resultado = atoi(input);
    return 1;
}

/* -----------------------------------------------------------------------------
 * 4. LEITURA DOS CAPÍTULOS
 *    O que faz: encontra o arquivo .md do capítulo (pasta atual, pasta do
 *    binário, share e HOME), pagina a leitura e imprime versos e subtítulos.
 *    O texto de cada linha passa pelo kernel (bk_verso); se o kernel recusar
 *    a linha (retorno < 0), ela é pulada em vez de mostrar texto incompleto.
 *    Por quê: o texto mostrado é o mesmo do motor e das outras versões.
 *    Data: 28/11/2026.
 *    Fonte: motor.biblia.c original deste repositório + kernel/ (bk_verso).
 * ----------------------------------------------------------------------------- */
/* Busca inteligente do arquivo do capítulo (pasta atual, diretório do binário, share e HOME) */
FILE *abrir_arquivo_capitulo(const char *pasta_livro, int capitulo, char *caminho_out) {
    const char *formatos[] = { "%02d.md", "%03d.md" };
    char bases[5][PATH_MAX];
    int total_bases = 0;

    /* 1. Diretório atual */
    snprintf(bases[total_bases++], PATH_MAX, ".");

    /* 2. Diretório ao lado do executável (macOS / Linux) */
#ifdef __APPLE__
    char exe_path[PATH_MAX];
    uint32_t size = sizeof(exe_path);
    if (_NSGetExecutablePath(exe_path, &size) == 0) {
        char *slash = strrchr(exe_path, '/');
        if (slash) {
            *slash = '\0';
            snprintf(bases[total_bases++], PATH_MAX, "%s", exe_path);
            snprintf(bases[total_bases++], PATH_MAX, "%s/../share/biblia-1911", exe_path);
        }
    }
#endif

    /* 3. Caminhos padrão do sistema e do usuário */
    const char *home = getenv("HOME");
    if (home != NULL) {
        snprintf(bases[total_bases++], PATH_MAX, "%s/biblia-1911", home);
    }
    snprintf(bases[total_bases++], PATH_MAX, "/usr/local/share/biblia-1911");

    for (int b = 0; b < total_bases; b++) {
        for (int fmt = 0; fmt < 2; fmt++) {
            char arquivo[64];
            snprintf(arquivo, sizeof(arquivo), formatos[fmt], capitulo);
            if (strcmp(bases[b], ".") == 0) {
                snprintf(caminho_out, PATH_MAX, "%s/%s", pasta_livro, arquivo);
            } else {
                snprintf(caminho_out, PATH_MAX, "%s/%s/%s", bases[b], pasta_livro, arquivo);
            }
            FILE *f = fopen(caminho_out, "r");
            if (f != NULL) return f;
        }
    }

    snprintf(caminho_out, PATH_MAX, "%s/%02d.md", pasta_livro, capitulo);
    return NULL;
}

int verificar_pausa_pagina(int *linhas_na_pagina, int *modo_continuo, const char *nome_livro, int capitulo) {
    if (*modo_continuo) return 1;

    int cols = 80, rows = 24;
    obter_dimensoes_terminal(&cols, &rows);
    int limite_linhas = (rows > 8) ? (rows - 5) : 18;

    if (*linhas_na_pagina >= limite_linhas) {
        int margem = calcular_margem_esquerda(LARGURA_PAGINA);
        imprimir_separador(LARGURA_PAGINA, '-');
        imprimir_espacos(margem);
        printf("%s[ENTER] Próxima página  |  [T] Ler capítulo inteiro  |  [0] Voltar: %s",
               COR_DIM, COR_RESET);
        fflush(stdout);

        char resp[32];
        if (fgets(resp, sizeof(resp), stdin) == NULL) return 0;
        if (resp[0] == '0') return 0;
        if (resp[0] == 't' || resp[0] == 'T') {
            *modo_continuo = 1;
            return 2;
        }

        system("clear");
        char cabecalho[160];
        snprintf(cabecalho, sizeof(cabecalho), "%s — Capítulo %d (continuação)", nome_livro, capitulo);
        imprimir_separador(LARGURA_PAGINA, '=');
        printf("%s", COR_TITULO);
        imprimir_linha_centralizada(cabecalho, LARGURA_PAGINA);
        printf("%s", COR_RESET);
        imprimir_separador(LARGURA_PAGINA, '=');
        printf("\n");
        *linhas_na_pagina = 4;
    }
    return 1;
}

void imprimir_verso_formatado(const char *versiculo_num, const char *texto, int *linhas_impressas) {
    int margem = calcular_margem_esquerda(LARGURA_PAGINA);
    char prefixo[32];
    snprintf(prefixo, sizeof(prefixo), "[%s] ", versiculo_num);
    int larg_prefixo = comprimento_visivel_utf8(prefixo);

    imprimir_espacos(margem);
    printf("%s%s%s", COR_VERSO, prefixo, COR_RESET);

    int col_atual = larg_prefixo;
    const char *p = texto;

    while (*p != '\0') {
        while (*p == ' ') p++;
        if (*p == '\0') break;

        const char *fim_palavra = p;
        int larg_palavra = 0;
        while (*fim_palavra != '\0' && *fim_palavra != ' ') {
            if ((*fim_palavra & 0xC0) != 0x80) larg_palavra++;
            fim_palavra++;
        }

        if (col_atual > larg_prefixo && (col_atual + 1 + larg_palavra) > LARGURA_PAGINA) {
            putchar('\n');
            (*linhas_impressas)++;
            imprimir_espacos(margem + larg_prefixo);
            col_atual = larg_prefixo;
        } else if (col_atual > larg_prefixo) {
            putchar(' ');
            col_atual++;
        }

        fwrite(p, 1, (size_t)(fim_palavra - p), stdout);
        col_atual += larg_palavra;
        p = fim_palavra;
    }

    printf("\n\n");
    (*linhas_impressas) += 2;
}

void imprimir_subtitulo_formatado(const char *texto, int *linhas_impressas) {
    int margem = calcular_margem_esquerda(LARGURA_PAGINA);
    imprimir_espacos(margem);
    printf("%s%s(%s)%s\n\n", COR_TITULO, COR_ITALICO, texto, COR_RESET);
    (*linhas_impressas) += 2;
}

int ler_capitulo(const char *nome_livro, const char *pasta_livro, int capitulo) {
    char caminho_ficheiro[PATH_MAX];
    FILE *ficheiro = abrir_arquivo_capitulo(pasta_livro, capitulo, caminho_ficheiro);

    if (ficheiro == NULL) {
        int margem = calcular_margem_esquerda(LARGURA_PAGINA);
        printf("\n");
        imprimir_espacos(margem);
        printf("(Erro: Não foi possível encontrar o arquivo '%s'.)\n", caminho_ficheiro);
        return 1;
    }

    int modo_continuo = 0;

reiniciar_leitura:
    rewind(ficheiro);
    system("clear");

    char cabecalho[160];
    snprintf(cabecalho, sizeof(cabecalho), "%s — Capítulo %d", nome_livro, capitulo);

    imprimir_separador(LARGURA_PAGINA, '=');
    printf("%s", COR_TITULO);
    imprimir_linha_centralizada(cabecalho, LARGURA_PAGINA);
    printf("%s", COR_RESET);
    imprimir_separador(LARGURA_PAGINA, '=');
    printf("\n");

    char linha[MAX_LINHA];
    char texto_limpo[MAX_LINHA];
    int linhas_na_pagina = 4;

    while (fgets(linha, sizeof(linha), ficheiro) != NULL) {
        if (redesenhar_tela) {
            redesenhar_tela = 0;
            goto reiniciar_leitura;
        }

        if (strncmp(linha, "---", 3) == 0) break;
        if (linha[0] == '#' || linha[0] == '[') continue;

        int idx = 0;
        while (linha[idx] == '*' || linha[idx] == ' ') idx++;

        if (linha[idx] >= '1' && linha[idx] <= '9') {
            int acao = verificar_pausa_pagina(&linhas_na_pagina, &modo_continuo, nome_livro, capitulo);
            if (acao == 0) {
                fclose(ficheiro);
                return 0;
            }
            if (acao == 2) goto reiniciar_leitura;

            char versiculo_num[20];
            int v_pos = 0;

            while (idx < MAX_LINHA && linha[idx] >= '0' && linha[idx] <= '9' && v_pos < 19) {
                versiculo_num[v_pos++] = linha[idx++];
            }
            versiculo_num[v_pos] = '\0';

            while (linha[idx] == '*' || linha[idx] == ' ') idx++;

            if (bk_verso(&linha[idx], texto_limpo, sizeof(texto_limpo)) < 0) continue;
            imprimir_verso_formatado(versiculo_num, texto_limpo, &linhas_na_pagina);
        } else if (strncmp(linha, "**", 2) == 0 && linha[idx] != '\0' && linha[idx] != '\n') {
            int acao = verificar_pausa_pagina(&linhas_na_pagina, &modo_continuo, nome_livro, capitulo);
            if (acao == 0) {
                fclose(ficheiro);
                return 0;
            }
            if (acao == 2) goto reiniciar_leitura;

            if (bk_verso(&linha[idx], texto_limpo, sizeof(texto_limpo)) < 0) continue;
            imprimir_subtitulo_formatado(texto_limpo, &linhas_na_pagina);
        }
    }

    imprimir_separador(LARGURA_PAGINA, '=');
    fclose(ficheiro);
    return 1;
}

/* -----------------------------------------------------------------------------
 * 5. MENU PRINCIPAL (main)
 *    O que faz: mostra o menu (Antigo Testamento, Novo Testamento, Sair), a
 *    lista de livros e pede o capítulo. Livros vêm do kernel:
 *    base 0 = Antigo Testamento (39 livros), base 39 = Novo Testamento (27).
 *    O menu mostra a linha de doação espontânea via PIX.
 *    Por quê: uma única fonte para nomes, pastas e capítulos (o kernel).
 *    Data: 28/11/2026.
 *    Fonte: motor.biblia.c original deste repositório + kernel/.
 * ----------------------------------------------------------------------------- */
int main(void) {
#ifndef _WIN32
    struct sigaction sa;
    sa.sa_handler = tratar_sigwinch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGWINCH, &sa, NULL);
#endif

    int opcao_principal = 0;
    int opcao_livro = 0;
    int opcao_capitulo = 0;

    do {
        system("clear");
        exibir_data_hora();

        int margem = calcular_margem_esquerda(LARGURA_PAGINA);

        imprimir_separador(LARGURA_PAGINA, '=');
        printf("%s", COR_TITULO);
        imprimir_linha_centralizada("BÍBLIA DIGITAL ALMEIDA 1911 EM PT-BR (DOMÍNIO PÚBLICO)", LARGURA_PAGINA);
        imprimir_linha_centralizada("Atualização Ortográfica e Gramatical realizada em 11/2026", LARGURA_PAGINA);
        printf("%s%s", COR_RESET, COR_DIM);
#ifdef __APPLE__
        imprimir_linha_centralizada("Motor C Universal — macOS", LARGURA_PAGINA);
#else
        imprimir_linha_centralizada("(Intel/Apple Silicon)", LARGURA_PAGINA);
#endif
        imprimir_linha_centralizada("Autor: Romulo CSR", LARGURA_PAGINA);
        imprimir_linha_centralizada("Download free: github.com/RomuloCSR", LARGURA_PAGINA);
        imprimir_linha_centralizada("Doação espontânea para a obra via PIX: 21980934118", LARGURA_PAGINA);
        printf("%s", COR_RESET);
        imprimir_separador(LARGURA_PAGINA, '=');
        printf("\n");

        if (redesenhar_tela) {
            redesenhar_tela = 0;
            continue;
        }

        imprimir_espacos(margem + 4); printf("1 - Antigo Testamento\n");
        imprimir_espacos(margem + 4); printf("2 - Novo Testamento\n");
        imprimir_espacos(margem + 4); printf("3 - Sair\n\n");
#ifdef __APPLE__
        imprimir_espacos(margem + 4); printf("%sZoom da fonte: Cmd + (+) para aumentar  |  Cmd + (-) para diminuir%s\n\n", COR_DIM, COR_RESET);
#else
        imprimir_espacos(margem + 4); printf("%sZoom da fonte: Ctrl + Shift + (+) ou pinça na tela%s\n\n", COR_DIM, COR_RESET);
#endif
        imprimir_espacos(margem + 4); printf("Escolha uma opção: ");
        fflush(stdout);

        int status_menu = ler_inteiro_seguro(&opcao_principal);
        if (status_menu == -1) break;
        if (status_menu == 0 || redesenhar_tela) {
            redesenhar_tela = 0;
            continue;
        }

        if (opcao_principal == 1 || opcao_principal == 2) {
            int base = (opcao_principal == 1) ? 0 : 39;
            int total_livros = (opcao_principal == 1) ? 39 : 27;

            system("clear");
            imprimir_separador(LARGURA_PAGINA, '=');
            printf("%s", COR_TITULO);
            imprimir_linha_centralizada((opcao_principal == 1) ? "ANTIGO TESTAMENTO" : "NOVO TESTAMENTO", LARGURA_PAGINA);
            printf("%s", COR_RESET);
            imprimir_separador(LARGURA_PAGINA, '=');
            printf("\n");

            int cols_grade = (LARGURA_PAGINA >= 72) ? 3 : ((LARGURA_PAGINA >= 46) ? 2 : 1);
            for (int i = 0; i < total_livros; i++) {
                if (i % cols_grade == 0) imprimir_espacos(margem);
                imprimir_coluna(i + 1, bk_livro_nome(base + i));
                if ((i + 1) % cols_grade == 0) printf("\n");
            }

            if (total_livros % cols_grade != 0) printf("\n");

            printf("\n");
            imprimir_espacos(margem); printf(" 0 - Voltar\n");
            imprimir_espacos(margem); printf(" Escolha um livro: ");
            fflush(stdout);

            int status_livro = ler_inteiro_seguro(&opcao_livro);
            if (status_livro == -1) break;
            if (status_livro == 0 || redesenhar_tela) continue;

            if (opcao_livro >= 1 && opcao_livro <= total_livros) {
                int total_do_livro = bk_livro_capitulos(base + opcao_livro - 1);

                printf("\n");
                imprimir_espacos(margem);
                printf("%s selecionado. Digite o capítulo (1 a %d): ", bk_livro_nome(base + opcao_livro - 1), total_do_livro);
                fflush(stdout);

                int status_cap = ler_inteiro_seguro(&opcao_capitulo);
                if (status_cap == -1) break;
                if (redesenhar_tela) continue;

                if (status_cap == 1 && opcao_capitulo >= 1 && opcao_capitulo <= total_do_livro) {
                    int leu_ate_o_fim = ler_capitulo(bk_livro_nome(base + opcao_livro - 1), bk_livro_pasta(base + opcao_livro - 1), opcao_capitulo);
                    if (leu_ate_o_fim) pausar_tela();
                } else {
                    printf("\n");
                    imprimir_espacos(margem);
                    printf("Capítulo inválido! O livro de %s possui capítulos de 1 a %d.\n", bk_livro_nome(base + opcao_livro - 1), total_do_livro);
                    pausar_tela();
                }
            } else if (opcao_livro != 0) {
                printf("\n");
                imprimir_espacos(margem);
                printf("Opção de livro inválida!\n");
                pausar_tela();
            }
        } else if (opcao_principal == 3) {
            int margem_saida = calcular_margem_esquerda(LARGURA_PAGINA);
            printf("\n");
            imprimir_espacos(margem_saida);
            printf("Saindo do aplicativo...\n");
        } else {
            int margem_inv = calcular_margem_esquerda(LARGURA_PAGINA);
            printf("\n");
            imprimir_espacos(margem_inv);
            printf("Opção inválida! Tente novamente.\n");
            pausar_tela();
        }

    } while (opcao_principal != 3);

    return 0;
}
