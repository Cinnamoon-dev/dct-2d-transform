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

---

## Testes de Validação

O arquivo `test_validation.c` implementa 4 testes matemáticos que provam que a DCT foi implementada corretamente.

### 1. MSE (Mean Squared Error)

**O que é:** Média do quadrado das diferenças entre cada pixel original e reconstruído.

```c
mse = Σ(input[i][j] - reconstructed[i][j])² / (N×N)
```

**Por que prova que funciona:**
- A DCT-II e a DCT-III são **transformações matemáticas inversas** uma da outra
- Se aplicar DCT → IDCT, deve recuperar a imagem original **perfeitamente**
- Erro pequeno (~0.00000001) significa: `input ≈ reconstructed`
- Erro grande = bug na implementação

**Critério de sucesso:** MSE < 0.0001

---

### 2. Max Error (Erro Máximo Absoluto)

**O que é:** Maior diferença absoluta entre qualquer pixel original e reconstruído.

```c
maxError = máximo de |input[i][j] - reconstructed[i][j]|
```

**Por que prova que funciona:**
- MSE pode "esconder" erros grandes em poucos pixels se a maioria está certa
- Max Error pega o **pior caso** de qualquer pixel
- Se o pior erro é < 0.01, **todos** os pixels estão corretos
- Garante que não há "outliers" com erro significativo

---
### Resumo: Por que esses testes são suficientes?

| Teste | O que valida |
|-------|--------------|
| **MSE** | A transformação inversa recupera a original (integridade matemática) |
| **Max Error** | Nenhum pixel tem erro significativo (qualidade garantida) |

A DCT-II transforma pixels em frequências. A DCT-III faz o caminho inverso. Se reconstruir a imagem original com erro ~0, a matemática foi implementada corretamente.
