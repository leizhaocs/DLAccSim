import numpy as np
import sys

np.set_printoptions(threshold=sys.maxsize)

data = open('data.txt', 'r')

# load inputs

line = data.readline()

input_info = line.split()
N = int(input_info[0])
C = int(input_info[1])
H = int(input_info[2])
W = int(input_info[3])

inputs = np.empty([N, C, H, W], dtype=int)

for n in range(N):
    for c in range(C):
        line = data.readline()
        for h in range(H):
            line = data.readline()
            elements = line.split()
            for w in range(W):
                inputs[n][c][h][w] = int(elements[w])
    line = data.readline()
    line = data.readline()

# load filters

line = data.readline()

filter_info = line.split()
M = int(filter_info[0])
C = int(filter_info[1])
R = int(filter_info[2])
S = int(filter_info[3])

filters = np.empty([M, C, R, S], dtype=int)

for m in range(M):
    for c in range(C):
        line = data.readline()
        for r in range(R):
            line = data.readline()
            elements = line.split()
            for s in range(S):
                filters[m][c][r][s] = int(elements[s])
    line = data.readline()
    line = data.readline()

data.close()

# convolution

outputs = np.empty([N, M, H-R+1, W-S+1], dtype=int)

for n in range(N):
    for m in range(M):
        for r in range(H-R+1):
            for c in range(W-S+1):

                result = 0
                for i in range(R):
                    for j in range(S):
                        for k in range(C):
                            result += filters[m][k][i][j] * inputs[n][k][r+i][c+j]
                outputs[n][m][r][c] = result

print(outputs)
