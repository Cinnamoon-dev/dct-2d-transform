#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define Q 20.0
#define N 64
#define PI 3.14159265358979323846

float input[N][N];
float output[N][N];
float reconstructed[N][N];

float alpha(int k) {
    if (k == 0)
        return sqrt(1.0 / N);
    else
        return sqrt(2.0 / N);
}

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

// Leitura de PGM (P2)
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

// Escrita de PGM (P2)
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

// DCT-II 2D
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

// DCT-III 2D (Inversa da DCT-II)
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
