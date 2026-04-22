import numpy as np
from scipy.fftpack import dct, idct
from PIL import Image

N = 64

def read_pgm(filename):
    """Lê arquivo PGM (P2)"""
    try:
        img = Image.open(filename)
        if img.mode != 'L':
            img = img.convert('L')
        arr = np.array(img, dtype=np.float32)
        
        if arr.shape != (N, N):
            raise ValueError(f"Imagem deve ser {N}x{N}, mas é {arr.shape}")
        
        return arr
    except Exception as e:
        print(f"Erro ao abrir arquivo: {e}")
        exit(1)

def write_pgm(filename, mat):
    """Escreve arquivo PGM (P2)"""
    # Normalização para visualizar
    min_val = np.min(mat)
    max_val = np.max(mat)
    
    if max_val - min_val == 0:
        normalized = np.zeros_like(mat)
    else:
        normalized = (mat - min_val) / (max_val - min_val)
    
    # Converte para 0-255
    pixel_data = (normalized * 255).astype(np.uint8)
    
    # Cria imagem e salva
    img = Image.fromarray(pixel_data, mode='L')
    img.save(filename)

def dct_2d(input_mat):
    """Aplica DCT-II 2D"""
    # scipy.fftpack.dct usa a normalização ortonormal por padrão
    # DCT na dimensão x (linhas)
    dct_x = dct(input_mat, axis=0, norm='ortho')
    # DCT na dimensão y (colunas)
    dct_2d_result = dct(dct_x, axis=1, norm='ortho')
    
    return dct_2d_result

def idct_2d(dct_mat):
    """Aplica IDCT (DCT-III) 2D"""
    # IDCT na dimensão x (linhas)
    idct_x = idct(dct_mat, axis=0, norm='ortho')
    # IDCT na dimensão y (colunas)
    idct_2d_result = idct(idct_x, axis=1, norm='ortho')
    
    return idct_2d_result

def main():
    # Lê imagem de entrada
    print("Lendo imagem...")
    input_data = read_pgm("input.pgm")
    
    # Aplica DCT
    print("Aplicando DCT-II 2D...")
    dct_output = dct_2d(input_data)
    
    # Aplica IDCT (reconstrução)
    print("Aplicando IDCT (reconstrução)...")
    reconstructed = idct_2d(dct_output)
    
    # Salva resultados
    print("Salvando resultados...")
    write_pgm("output_dct_py.pgm", dct_output)
    write_pgm("reconstructed_py.pgm", reconstructed)
    
    # Calcula erro de reconstrução
    error = np.mean((input_data - reconstructed) ** 2)
    print(f"Erro médio quadrático na reconstrução: {error:.2e}")
    print("DCT aplicada com sucesso! Veja output_dct_py.pgm e reconstructed_py.pgm")

if __name__ == "__main__":
    main()
