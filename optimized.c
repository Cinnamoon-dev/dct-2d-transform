#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Para compilar: gcc main_optimized.c -o main_opt -lm -fopenmp -O2
// Comparação de complexidade:
//   Original:  O(N^4) — para N=2000: ~16 trilhões de operações
//   Otimizado: O(N^3) — para N=2000: ~8 bilhões de operações  (~2000x mais rápido)
//   + OpenMP + tabela de cossenos: ainda mais rápido

#ifdef _OPENMP
#include <omp.h>
#endif

#define Q 70.0
#define N 2000
#define PI 3.14159265358979323846

float input[N][N];
float output[N][N];
float reconstructed[N][N];

// Tabela de cossenos pré-computada: cos_table[u][x] = cos((2x+1)*u*pi / 2N)
// Evita recalcular cos() bilhões de vezes dentro dos loops
static float cos_table[N][N];

// Buffer intermediário para DCT separável (resultado após aplicar nas linhas)
static float temp[N][N];

// ============================================================
// Pré-computação da tabela de cossenos
// Custo: O(N²) — feito uma única vez antes das transformadas
// ============================================================
void precompute_cos_table() {
    for (int u = 0; u < N; u++) {
        for (int x = 0; x < N; x++) {
            cos_table[u][x] = (float)cos(((2 * x + 1) * u * PI) / (2.0 * N));
        }
    }
}

float alpha(int k) {
    return (k == 0) ? sqrtf(1.0f / N) : sqrtf(2.0f / N);
}

// ============================================================
// DCT-1D sobre uma linha (vetor de tamanho N)
// Complexidade: O(N²) por linha
// ============================================================
static void dct1D_row(const float *in, float *out) {
    for (int u = 0; u < N; u++) {
        float sum = 0.0f;
        const float *cos_u = cos_table[u]; // ponteiro para a linha u da tabela
        for (int x = 0; x < N; x++) {
            sum += in[x] * cos_u[x];
        }
        out[u] = alpha(u) * sum;
    }
}

// ============================================================
// DCT-2D separável (OTIMIZADA): O(N³) em vez de O(N⁴)
//
// Estratégia:
//   1. Aplica DCT-1D em cada LINHA de input  → temp[][]
//   2. Aplica DCT-1D em cada COLUNA de temp  → output[][]
//
// Fundamento matemático: DCT-2D(f)[u,v] = DCT-1D(DCT-1D(f, linhas), colunas)
// Isso funciona porque a fórmula 2D é o produto de dois cossenos independentes.
// ============================================================
void dct2D() {
    // Passo 1: DCT-1D em cada linha (paralelizado com OpenMP)
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        dct1D_row(input[i], temp[i]);
    }

    // Passo 2: DCT-1D em cada coluna de temp
    // Para acesso cache-friendly, trabalhamos com colunas usando um buffer local
    #pragma omp parallel for schedule(static)
    for (int v = 0; v < N; v++) {
        float col_in[N], col_out[N];

        // Extrai coluna v de temp
        for (int i = 0; i < N; i++) col_in[i] = temp[i][v];

        // DCT-1D na coluna
        dct1D_row(col_in, col_out);

        // Escreve resultado na coluna v de output
        for (int i = 0; i < N; i++) output[i][v] = col_out[i];
    }
}

// ============================================================
// IDCT-1D (DCT-III) sobre um vetor de tamanho N
// ============================================================
static void idct1D_row(const float *in, float *out) {
    for (int x = 0; x < N; x++) {
        float sum = 0.0f;
        for (int u = 0; u < N; u++) {
            sum += alpha(u) * in[u] * cos_table[u][x];
        }
        out[x] = sum;
    }
}

// ============================================================
// IDCT-2D separável: O(N³) em vez de O(N⁴)
// ============================================================
void idct2D() {
    // Passo 1: IDCT-1D em cada linha de output
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        float row_out[N];
        idct1D_row(output[i], row_out);
        memcpy(temp[i], row_out, N * sizeof(float));
    }

    // Passo 2: IDCT-1D em cada coluna de temp
    #pragma omp parallel for schedule(static)
    for (int v = 0; v < N; v++) {
        float col_in[N], col_out[N];
        for (int i = 0; i < N; i++) col_in[i] = temp[i][v];
        idct1D_row(col_in, col_out);
        for (int i = 0; i < N; i++) reconstructed[i][v] = col_out[i];
    }
}

// ============================================================
// I/O (sem alterações em relação ao original)
// ============================================================
void readPGM(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { printf("Erro ao abrir arquivo.\n"); exit(1); }

    char format[3];
    int width, height, maxval;
    fscanf(file, "%s", format);
    if (format[0] != 'P' || format[1] != '2') {
        printf("Formato inválido (use P2).\n"); exit(1);
    }
    fscanf(file, "%d %d %d", &width, &height, &maxval);
    if (width != N || height != N) {
        printf("Imagem deve ser %dx%d.\n", N, N); exit(1);
    }
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            fscanf(file, "%f", &input[i][j]);
    fclose(file);
}

void writeMatrix(float matrix[N][N], const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) { printf("Erro ao abrir arquivo.\n"); exit(1); }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            fprintf(file, "%f ", matrix[i][j]);
        fprintf(file, "\n");
    }
    fclose(file);
}

void writePGM(const char *filename, float mat[N][N]) {
    FILE *file = fopen(filename, "w");
    if (!file) { printf("Erro ao criar arquivo %s\n", filename); exit(1); }
    fprintf(file, "P2\n%d %d\n255\n", N, N);
    float min = mat[0][0], max = mat[0][0];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            if (mat[i][j] < min) min = mat[i][j];
            if (mat[i][j] > max) max = mat[i][j];
        }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            //int pixel = (int)(255 * (mat[i][j] - min) / (max - min + 1e-6f));
            int pixel = abs((int)round(mat[i][j]));
            fprintf(file, "%d ", pixel);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void quantize() {
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            output[u][v] = round(output[u][v] / Q);
        }
    }
}

void dequantize() {
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            output[u][v] = output[u][v] * Q;
        }
    }
}

// Teste 1: Verificar erro médio quadrático (MSE)
float calculateMSE() {
    float mse = 0.0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float diff = input[i][j] - reconstructed[i][j];
            mse += diff * diff;
        }
    }
    return mse / (N * N);
}

// Teste 2: Verificar erro máximo absoluto
float calculateMaxError() {
    float maxError = 0.0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float error = fabs(input[i][j] - reconstructed[i][j]);
            if (error > maxError) maxError = error;
        }
    }
    return maxError;
}

int main() {
    readPGM("mountain3.pgm");

    printf("Pré-computando tabela de cossenos...\n");
    precompute_cos_table();

    printf("Aplicando DCT-2D otimizada...\n");
    dct2D();

    quantize();
    dequantize();

    printf("Aplicando IDCT-2D otimizada...\n");
    idct2D();

    writeMatrix(output, "output_dct.txt");
    writePGM("reconstructed.pgm", reconstructed);

    float mse = calculateMSE();
    printf("  -> Teste MSE (Erro Medio Quadratico):\n");
    printf("     MSE = %.10f\n", mse);
    printf("     Status: %s\n\n", (mse < 0.0001) ? "PASSOU" : "FALHOU");
    
    float maxErr = calculateMaxError();
    printf("  -> Teste Erro Maximo Absoluto:\n");
    printf("     Max Error = %.10f\n", maxErr);
    printf("     Status: %s\n\n", (maxErr < 0.01) ? "PASSOU" : "FALHOU");
    

    printf("Concluído!\n");
    return 0;
}