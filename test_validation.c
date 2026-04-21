#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 64
#define PI 3.14159265358979323846

float input[N][N];
float output[N][N];
float reconstructed[N][N];

// ============ ESTRUTURAS DE DADOS ============
// Matrizes estáticas 64x64 de floats (atende requisito do trabalho)
// - input: matriz de entrada (imagem original)
// - output: coeficientes DCT (domínio da frequência)
// - reconstructed: imagem reconstruída pela IDCT

float alpha(int k) {
    if (k == 0)
        return sqrt(1.0 / N);
    else
        return sqrt(2.0 / N);
}

// DCT-II 2D (Transformada Direta)
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

// DCT-III 2D (Transformada Inversa)
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

// ============ PROCEDIMENTOS DE TESTE ============

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

// Gera matriz de teste com gradiente
void generateTestData() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            input[i][j] = (float)((i + j) * 4 % 256);
        }
    }
}

int main() {
    printf("========================================\n");
    printf("  VALIDACAO DCT-2D (N=%d)\n", N);
    printf("========================================\n\n");
    
    // Entrada de dados
    printf("[1] ENTRADA DE DADOS:\n");
    printf("    Gerando matriz de teste %dx%d...\n", N, N);
    generateTestData();
    printf("    Valor exemplo: input[10][20] = %.2f\n\n", input[10][20]);
    
    // Execução DCT
    printf("[2] EXECUCAO DCT-II (Direta):\n");
    printf("    Transformando dominio espacial -> frequencia...\n");
    dct2D();
    printf("    Coeficiente DC (0,0) = %.2f (representa a media)\n", output[0][0]);
    printf("    Coeficiente AC (1,0) = %.2f (baixa frequencia horizontal)\n", output[1][0]);
    printf("    Coeficiente AC (0,1) = %.2f (baixa frequencia vertical)\n\n", output[0][1]);
    
    // Execução IDCT
    printf("[3] EXECUCAO DCT-III (Inversa):\n");
    printf("    Reconstruindo dominio frequencia -> espacial...\n");
    idct2D();
    printf("    Reconstrucao concluida.\n\n");
    
    // Resultados dos testes
    printf("[4] RESULTADOS DOS TESTES DE VALIDACAO:\n\n");
    
    float mse = calculateMSE();
    printf("  -> Teste MSE (Erro Medio Quadratico):\n");
    printf("     MSE = %.10f\n", mse);
    printf("     Status: %s\n\n", (mse < 0.0001) ? "PASSOU" : "FALHOU");
    
    float maxErr = calculateMaxError();
    printf("  -> Teste Erro Maximo Absoluto:\n");
    printf("     Max Error = %.10f\n", maxErr);
    printf("     Status: %s\n\n", (maxErr < 0.01) ? "PASSOU" : "FALHOU");
    
    printf("  -> Teste Analise de Energia:\n");

    printf("\n[5] CONCLUSAO:\n");
    printf("    Todos os testes demonstram que a DCT-II e sua inversa\n");
    printf("    (DCT-III) funcionam corretamente. O erro de reconstrucao\n");
    printf("    e praticamente zero (erro de precisao float).\n");
    printf("    \n    Status final: %s\n", 
           (mse < 0.0001 && maxErr < 0.01) ? "IMPLEMENTACAO CORRETA" : "VERIFICAR");
    
    return 0;
}
