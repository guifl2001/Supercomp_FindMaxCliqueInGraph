
import networkx as nx
import random

# Parâmetros
num_vertices = 45  # Número de vértices no grafo
# Probabilidade de haver uma aresta entre dois vértices (ajuste conforme necessário)
probabilidade_conexao = 0.7

# Crie um grafo aleatório densamente conectado
grafo = nx.fast_gnp_random_graph(num_vertices, probabilidade_conexao)

# Nome do arquivo de saída
nome_arquivo = "grafo.txt"

# Abra o arquivo para escrita
with open(nome_arquivo, 'w') as arquivo:
    # Escreva a quantidade de vértices e número de arestas na primeira linha
    arquivo.write(f"{num_vertices} {grafo.number_of_edges()}\n")

    # Escreva as arestas no formato de lista de adjacência
    for aresta in grafo.edges():
        # +1 para ajustar os índices (começando em 1)
        arquivo.write(f"{aresta[0]+1} {aresta[1]+1}\n")

print(f"Grafo densamente conectado gerado e salvo em '{nome_arquivo}'.")
