import networkx as nx

# Nome do arquivo de entrada
nome_arquivo = "grafo.txt"

# Abrir o arquivo e pular a primeira linha
with open(nome_arquivo, 'r') as arquivo:
    next(arquivo)  # Pula a primeira linha

    # Lê o grafo a partir das linhas restantes
    G = nx.parse_adjlist(arquivo)

# Encontrar todas as cliques maximais
cliques_maximais = list(nx.find_cliques(G))

# Encontrar o tamanho da clique máxima
tamanho_clique_maxima = max(len(clique) for clique in cliques_maximais)

print("Cliques maximais encontradas:")
for clique in cliques_maximais:
    if len(clique) == tamanho_clique_maxima:
        clique.sort(reverse=True)
        print(clique)
