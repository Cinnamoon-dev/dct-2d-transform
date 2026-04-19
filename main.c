#include <stdio.h>
#include <math.h>

#define N 64

// Matrizes estáticas (atende requisito do trabalho)
float input[N][N];
float output[N][N];

// Função alfa (normalização)
float alpha(int k) {
    if (k == 0)
        return sqrt(1.0 / N);
    else
        return sqrt(2.0 / N);
}

// Função DCT 2D
void dct2D() {
    int u, v, x, y;

    // Calcula cada coeficiente de saída
    for (u = 0; u < N; u++) { // 0,0
        for (v = 0; v < N; v++) {
            float sum = 0.0;

            // Percorre a imagem original
            // Calcula a soma
            for (x = 0; x < N; x++) {
                for (y = 0; y < N; y++) {

                    float cos1 = cos(((2 * x + 1) * u * M_PI) / (2 * N));
                    float cos2 = cos(((2 * y + 1) * v * M_PI) / (2 * N));

                    sum += input[x][y] * cos1 * cos2;
                }
            }

            output[u][v] = alpha(u) * alpha(v) * sum;
        }
    }
}

// Função para gerar dados de teste
void generateInput() {
    int i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            input[i][j] = (float)((i + j) % 256); // padrão simples
        }
    }
}

// Função para imprimir parte da matriz
void printMatrix(float mat[N][N]) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%8.2f ", mat[i][j]);
        }
        printf("\n");
    }
}

int main() {

    generateInput();

    printf("Input (8x8 parcial):\n");
    printMatrix(input);

    dct2D();

    printf("\nDCT Output (8x8 parcial):\n");
    printMatrix(output);

    return 0;
}