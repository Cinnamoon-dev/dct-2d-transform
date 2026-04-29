/**
 * @file main.c
 * @brief Implementação da Transformada Discreta do Cosseno (DCT) 2D
 * 
 * =============================================================================
 * PROJETO: DCT 2D - Transformada Discreta do Cosseno Bidimensional
 * =============================================================================
 * 
 * DESCRIÇÃO GERAL:
 * -----------------
 * Este programa implementa a Transformada Discreta do Cosseno (DCT) 
 * bidimensional, um algoritmo fundamental no processamento de imagens e 
 * compressão de dados. A DCT é amplamente utilizada em padrões de compressão 
 * como JPEG, MPEG e MP3.
 * 
 * O programa lê uma imagem no formato PGM (Portable Graymap), aplica a 
 * DCT-II 2D, realiza quantização e desquantização (simulando compressão), 
 * e depois aplica a DCT-III 2D (transformada inversa) para reconstruir 
 * a imagem.
 * 
 * =============================================================================
 * PERMISSÕES DE USO / COPYRIGHT:
 * =============================================================================
 * 
 * Este código foi desenvolvido para fins educacionais como parte da 
 * disciplina de Sistemas Embarcados.
 * 
 * Permissões:
 * - Uso acadêmico: Livre para uso em atividades educacionais
 * - Modificação: Permitida para fins de estudo e melhoria
 * - Distribuição: Vedada sem autorização dos autores
 * 
 * =============================================================================
 * COMO USAR:
 * =============================================================================
 * 
 * Compilação:
 *   gcc -o dct main.c -lm
 * 
 * Execução:
 *   ./dct
 * 
 * Entrada:
 *   - Arquivo de imagem: input.pgm (formato PGM, 64x64 pixels, tons de cinza)
 *   - A imagem deve ter exatamente 64x64 pixels com valores de 0 a 255
 * 
 * Saída:
 *   - output_dct.txt: Matriz com os coeficientes DCT (valores decimais)
 *   - reconstructed.pgm: Imagem reconstruída após DCT inversa
 * 
 * Formato do arquivo de entrada (input.pgm):
 *   P2
 *   64 64
 *   255
 *   [valores dos pixels de 0 a 255]
 * 
 * =============================================================================
 * ENTRADA E SAÍDA:
 * =============================================================================
 * 
 * Entrada (Input):
 *   - Tipo: Imagem PGM (Portable Graymap) em formato P2
 *   - Dimensões: 64x64 pixels
 *   - Valores: Inteiros de 0 a 255 (tons de cinza)
 *   - Arquivo: input.pgm
 * 
 * Saída (Output):
 *   - output_dct.txt: Matriz de coeficientes DCT (float)
 *   - reconstructed.pgm: Imagem reconstruída (PGM normalizado)
 * 
 * =============================================================================
 * AUTORES / ESTUDANTES:
 * =============================================================================
 * - Karen Menezes Ribeiro
 * - Pedro Leandro Campos Silva
 * 
 * =============================================================================
 * DATA:
 * =============================================================================
 * - Abril de 2026
 * 
 * =============================================================================
 * CONTEXTO DO DESENVOLVIMENTO:
 * =============================================================================
 * - Disciplina: Sistemas Operacionais
 * - Tipo: Trabalho acadêmico
 * - Plataforma alvo: Linux/Unix
 * - Compilador: GCC
 * 
 * =============================================================================
 * PLATAFORMA ALVO:
 * =============================================================================
 * - Sistemas Operacionais: Linux (testado), compatível com Unix
 * - Compilador: GCC com suporte a C99 ou superior
 * - Bibliotecas: stdlib.h, stdio.h, math.h (libm)
 * 
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * @def Q
 * @brief Fator de quantização usado na compressão DCT
 * 
 * Este valor determina o nível de compressão. Um valor maior resulta
 * em maior compressão (mais zeros na matriz de coeficientes), mas também
 * em maior perda de qualidade na reconstrução.
 * 
 */
#define Q 20.0

/**
 * @def N
 * @brief Dimensão das matrizes de entrada e saída
 * 
 * Define o tamanho das matrizes quadradas usadas no processamento.
 * O programa processa imagens de 64x64 pixels.
 */
#define N 64

/**
 * @def PI
 * @brief Constante matemática Pi com alta precisão
 * 
 * Usada nos cálculos das funções cosseno na fórmula da DCT.
 */
#define PI 3.14159265358979323846

/**
 * @brief Matriz global de entrada
 * 
 * Armazena os valores dos pixels da imagem de entrada (input.pgm).
 * Cada posição [i][j] contém o valor de intensidade do pixel na
 * posição (i, j) da imagem, variando de 0 a 255.
 * 
 * Variáveis globais afetadas: input
 */
float input[N][N];

/**
 * @brief Matriz global de saída (coeficientes DCT)
 * 
 * Armazena os coeficientes resultantes da aplicação da DCT-II 2D.
 * Após a quantização e desquantização, contém os valores processados
 * que serão usados na DCT inversa.
 * 
 * Variáveis globais afetadas: output
 */
float output[N][N];

/**
 * @brief Matriz global reconstruída
 * 
 * Armazena a imagem reconstruída após a aplicação da DCT-III 2D
 * (transformada inversa). Contém os valores dos pixels da imagem
 * recuperada a partir dos coeficientes DCT.
 * 
 * Variáveis globais afetadas: reconstructed
 */
float reconstructed[N][N];

/**
 * @brief Função de normalização alpha para DCT
 * 
 * Calcula o fator de normalização alpha(k) usado na fórmula da 
 * Transformada Discreta do Cosseno. O valor depende se o índice k
 * é zero ou não:
 * - Para k = 0: sqrt(1/N)
 * - Para k > 0: sqrt(2/N)
 * 
 * Esta normalização garante que a transformada seja ortonormal.
 * 
 * @param k Índice de frequência (0 <= k < N)
 * @return float Valor do fator alpha(k)
 * 
 * Variáveis globais afetadas: Nenhuma
 * 
 * Exemplo de uso:
 *   float a = alpha(0);  // Retorna sqrt(1/64)
 *   float b = alpha(5);  // Retorna sqrt(2/64)
 */
float alpha(int k) {
    if (k == 0)
        return sqrt(1.0 / N);
    else
        return sqrt(2.0 / N);
}

/**
 * @brief Escreve uma imagem PGM com valores inteiros
 * 
 * Esta função cria um arquivo de imagem no formato PGM (P2) a partir
 * de uma matriz de valores floats. Os valores são arredondados para
 * inteiros antes de serem gravados.
 * 
 * ATENÇÃO: Esta função NÃO normaliza os valores. Assume que os valores
 * já estão no intervalo [0, 255]. Para imagens com outros intervalos,
 * use a função writePGM().
 * 
 * @param matrix Matriz NxN de valores float a serem gravados
 * @param filename Nome do arquivo de saída (ex: "imagem.pgm")
 * 
 * Variáveis globais afetadas: Nenhuma (função de escrita pura)
 * 
 * @note Esta função é usada para gravar a imagem reconstruída,
 *       que contém valores de pixels no intervalo válido.
 */
void writeNewPicture(float matrix[N][N], const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }
    
    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", N, N);
    fprintf(file, "255\n");

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            fprintf(file, "%d ", (int) round(matrix[i][j]));
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

/**
 * @brief Escreve uma matriz de valores em formato texto
 * 
 * Esta função exporta os valores da matriz DCT em um formato texto
 * simples, onde cada linha contém os valores de uma linha da matriz
 * separados por espaços. Útil para depuração e análise dos 
 * coeficientes DCT.
 * 
 * @param matrix Matriz NxN de valores float a serem gravados
 * @param filename Nome do arquivo de saída (ex: "output.txt")
 * 
 * Variáveis globais afetadas: Nenhuma (função de escrita pura)
 * 
 * @note O formato de saída é: "valor1 valor2 valor3 ... valorN"
 *       Uma linha por linha da matriz.
 */
void writeMatrix(float matrix[N][N], const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }
    
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            fprintf(file, "%f ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

/**
 * @brief Lê uma imagem no formato PGM (P2)
 * 
 * Esta função lê um arquivo de imagem no formato PGM (Portable Graymap)
 * variante P2 (ASCII tons de cinza). O arquivo deve ter:
 * - Cabeçalho P2
 * - Dimensões 64x64
 * - Valor máximo de pixel 255
 * - Valores dos pixels em formato ASCII
 * 
 * Os valores lidos são armazenados na matriz global input.
 * 
 * @param filename Nome do arquivo de entrada (ex: "input.pgm")
 * 
 * Variáveis globais afetadas: input (preenchida com os valores dos pixels)
 * 
 * @note O programa termina com erro se:
 *       - O arquivo não existir ou não puder ser aberto
 *       - O formato não for P2
 *       - As dimensões não forem 64x64
 */
void readPGM(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }

    char format[3];
    int width, height, maxval;

    fscanf(file, "%s", format);
    if (format[0] != 'P' || format[1] != '2') {
        printf("Formato inválido (use P2).\n");
        exit(1);
    }

    fscanf(file, "%d %d", &width, &height);
    fscanf(file, "%d", &maxval);

    if (width != N || height != N) {
        printf("Imagem deve ser %dx%d.\n", N, N);
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(file, "%f", &input[i][j]);
        }
    }

    fclose(file);
}

/**
 * @brief Escreve uma imagem PGM com normalização automática
 * 
 * Esta função cria um arquivo de imagem no formato PGM (P2) a partir
 * de uma matriz de valores floats. Os valores são automaticamente
 * normalizados para o intervalo [0, 255] antes de serem gravados.
 * 
 * A normalização é feita através da fórmula:
 * pixel = 255 * (valor - min) / (max - min)
 * 
 * @param filename Nome do arquivo de saída
 * @param mat Matriz NxN de valores float a serem gravados
 * 
 * Variáveis globais afetadas: Nenhuma (função de escrita pura)
 * 
 * @note Útil para visualizar matrizes DCT que podem ter valores
 *       negativos ou fora do intervalo [0, 255].
 */
void writePGM(const char *filename, float mat[N][N]) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao criar arquivo %s\n", filename);
        exit(1);
    }

    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", N, N);
    fprintf(file, "255\n");

    // Normalização para visualizar
    float min = mat[0][0], max = mat[0][0];

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (mat[i][j] < min) min = mat[i][j];
            if (mat[i][j] > max) max = mat[i][j];
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int pixel = (int)(255 * (mat[i][j] - min) / (max - min));
            fprintf(file, "%d ", pixel);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

/**
 * @brief Aplica quantização aos coeficientes DCT
 * 
 * A quantização é o processo de reduzir a precisão dos coeficientes DCT,
 * simulando a compressão com perda em algoritmos como JPEG.
 * 
 * Cada coeficiente é dividido pelo fator Q e arredondado para o
 * inteiro mais próximo. Isso faz com que muitos coeficientes de alta
 * frequência se tornem zero, resultando em compressão.
 * 
 * Fómula: output[u][v] = round(output[u][v] / Q)
 * 
 * Variáveis globais afetadas: output (coeficientes quantizados)
 * 
 * @note A quantização é irreversível - informação é perdida aqui.
 *       Este é o principal ponto de perda de qualidade na compressão.
 */
void quantize() {
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            output[u][v] = round(output[u][v] / Q);
        }
    }
}

/**
 * @brief Reverte a quantização (desquantização)
 * 
 * A desquantização multiplica os coeficientes quantizados pelo fator Q
 * para restaurar os valores aproximados originais.
 * 
 * Esta operação é necessária antes de aplicar a DCT inversa, pois
 * a IDCT espera os coeficientes no domínio da frequência.
 * 
 * Fórmula: output[u][v] = output[u][v] * Q
 * 
 * Variáveis globais afetadas: output (coeficientes desquantizados)
 * 
 * @note Os valores não são exatamente os originais devido à
 *       perda de precisão na quantização (arredondamento).
 */
void dequantize() {
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            output[u][v] = output[u][v] * Q;
        }
    }
}

/**
 * @brief Transformada Discreta do Cosseno 2D (DCT-II)
 * 
 * Aplica a DCT-II bidimensional à matriz de entrada. A DCT-II é a
 * transformada usada em compressão de imagens (JPEG).
 * 
 * A DCT 2D é computada como o produto de duas DCTs 1D:
 * - Primeiro nas linhas, depois nas colunas (ou vice-versa)
 * 
 * Fórmula:
 * F(u,v) = α(u)α(v) * Σ Σ f(x,y) * cos((2x+1)uπ/2N) * cos((2y+1)vπ/2N)
 * 
 * onde α(k) = sqrt(1/N) para k=0, sqrt(2/N) para k>0
 * 
 * Variáveis globais afetadas: output (preenchida com coeficientes DCT)
 *                            input (lida, não modificada)
 * 
 * @note Complexidade: O(N^4) = 64^4 = 16,777,216 operações
 *      
 */
void dct2D() {
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {

            float sum = 0.0;

            for (int x = 0; x < N; x++) {
                for (int y = 0; y < N; y++) {

                    float cos1 = cos(((2 * x + 1) * u * PI) / (2 * N));
                    float cos2 = cos(((2 * y + 1) * v * PI) / (2 * N));

                    sum += input[x][y] * cos1 * cos2;
                }
            }

            output[u][v] = alpha(u) * alpha(v) * sum;
        }
    }
}

/**
 * @brief Transformada Inversa do Cosseno 2D (DCT-III)
 * 
 * Aplica a DCT-III bidimensional, que é a inversa exata da DCT-II.
 * Usada para reconstruir a imagem original a partir dos coeficientes
 * no domínio da frequência.
 * 
 * A DCT-III é matematicamente a transposta da DCT-II (com fatores
 * de normalização apropriados), garantindo que:
 * IDCT(DCT(input)) ≈ input
 * 
 * Fórmula:
 * f(x,y) = Σ Σ α(u)α(v) * F(u,v) * cos((2x+1)uπ/2N) * cos((2y+1)vπ/2N)
 * 
 * Variáveis globais afetadas: reconstructed (preenchida com imagem)
 *                            output (lida, não modificada)
 * 
 * @note Devido à quantização, a imagem reconstruída não será idêntica
 *       à original - este é o efeito da compressão com perda.
 */
void idct2D() {
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {

            float sum = 0.0;

            for (int u = 0; u < N; u++) {
                for (int v = 0; v < N; v++) {

                    float cos1 = cos(((2 * x + 1) * u * PI) / (2 * N));
                    float cos2 = cos(((2 * y + 1) * v * PI) / (2 * N));

                    sum += alpha(u) * alpha(v) * output[u][v] * cos1 * cos2;
                }
            }

            reconstructed[x][y] = sum;
        }
    }
}

int main() {
    /**
     * @brief Função principal do programa
     * 
     * Executa o pipeline completo de processamento DCT:
     * 1. Lê a imagem de entrada (input.pgm)
     * 2. Aplica a DCT-II 2D
     * 3. Aplica quantização
     * 4. Reverte a quantização (desquantização)
     * 5. Aplica a DCT-III 2D (inversa)
     * 6. Grava os resultados
     * 
     * @return int Código de saída (0 = sucesso)
     * 
     * Variáveis globais afetadas: input, output, reconstructed
     * 
     * Fluxo de execução:
     * input (64x64) -> DCT-II -> output (coeficientes) -> quantização
     * -> desquantização -> IDCT -> reconstructed (64x64)
     */
    readPGM("input.pgm");

    dct2D();

    quantize();
    dequantize();

    idct2D();

    writeMatrix(output, "output_dct.txt");
    writeNewPicture(reconstructed, "reconstructed.pgm");

    printf("DCT aplicada com sucesso! Veja output_dct.pgm\n");
    return 0;
}
