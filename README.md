# dct-2d-transform
Como rodar o projeto:

```bash
# Compila o arquivo
gcc file_main.c -o main -lm
./main
```

---

## Como funciona o DCT-II

### 1. A Ideia Central (A Analogia)

Imagine que sua imagem é um **"barulho complexo"**. O DCT-II funciona como um **"equalizador de som"**: ele decompõe esse barulho em **frequências puras** (cossenos) de diferentes "velocidades".

- **Baixas frequências** (u,v pequenos) = mudanças suaves na imagem
- **Altas frequências** (u,v grandes) = detalhes finos e bordas

### 2. Como Funciona o Cálculo

Para cada pixel de saída `(u,v)` na matriz resultante:

```
coeficiente[u][v] = α(u) × α(v) × Σ (pixel[x][y] × cos1 × cos2)
```

Onde:
- `cos1 = cos((2x+1) × u × π / 2N)` → onda horizontal
- `cos2 = cos((2y+1) × v × π / 2N)` → onda vertical
- `α()` → normalização (mais peso para baixas frequências)

**Intuição:** Cada coeficiente mede *"quanto aquela frequência específica está presente na imagem"* — como testar se uma nota musical está presente em uma música.

### 3. O que o Programa Faz

| Etapa | O que acontece |
|-------|---------------|
| **Entrada** | Gera matriz 64×64 com padrão de teste `(i+j) % 256` |
| **DCT-II** | Transforma pixels → **coeficientes de frequência** |
| **Saída** | Matriz onde valores no canto superior esquerdo são as "componentes principais" da imagem |

### Sobre a DCT-III (Inversa)

A **DCT-III** é simplesmente o **caminho de volta**: pega os coeficientes de frequência e reconstrói a imagem original pixel por pixel.

Ela está presente apenas para motivos de teste, para que se possa reconstruir a imagem anterior a partir das frequências. Isso garante a execução correta do algoritmo.

> **No trabalho:** DCT-II faz a análise. DCT-III só serve para **verificar se deu certo** — se reconstruir a imagem e ela for igual à original, a implementação está correta.

---

**Curiosidade:** A DCT-II é a base do JPEG — é assim que a compressão descarta frequências invisíveis ao olho humano.
