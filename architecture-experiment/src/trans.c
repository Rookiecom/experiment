/*
 * trans.c - 矩阵转置B=A^T
 *每个转置函数都必须具有以下形式的原型：
 *void trans（int M，int N，int a[N][M]，int B[M][N]）；
 *通过计算，块大小为32字节的1KB直接映射缓存上的未命中数来计算转置函数。
 */
#include <stdio.h>

#include "cachelab.h"
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] = "Transpose submission";  // 请不要修改“Transpose_submission”

void transpose32X32(int M, int N, int A[N][M], int B[M][N]) {
    int row = 0, col = 0;

    for (; row + 8 <= N; row += 8) {
        for (col = 0; col + 8 <= M; col += 8) {
            int temp[8];
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    temp[j] = A[row + i][col + j];
                }
                for (int j = 0; j < 8; j++) {
                    B[col + j][row + i] = temp[j];
                }
            }
        }
    }
}

void transpose64X64(int M, int N, int A[N][M], int B[M][N]) {
    int row = 0, col = 0;

    for (; row + 8 <= N; row += 8) {
        for (col = 0; col + 8 <= M; col += 8) {
            if (row == col) {
                int temp[8];
                for (int i = 0; i < 8; i += 2) {
                    for (int j = 0; j < 8; j += 4) {
                        temp[0] = A[row + i][col + j];
                        temp[1] = A[row + i][col + j + 1];
                        temp[2] = A[row + i][col + j + 2];
                        temp[3] = A[row + i][col + j + 3];
                        temp[4] = A[row + i + 1][col + j];
                        temp[5] = A[row + i + 1][col + j + 1];
                        temp[6] = A[row + i + 1][col + j + 2];
                        temp[7] = A[row + i + 1][col + j + 3];
                        B[col + j][row + i] = temp[0];
                        B[col + j + 1][row + i] = temp[1];
                        B[col + j + 2][row + i] = temp[2];
                        B[col + j + 3][row + i] = temp[3];
                        B[col + j][row + i + 1] = temp[4];
                        B[col + j + 1][row + i + 1] = temp[5];
                        B[col + j + 2][row + i + 1] = temp[6];
                        B[col + j + 3][row + i + 1] = temp[7];
                    }
                }
                continue;
            }
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    B[col + j][row + i] = A[row + i][col + j];
                }
                for (int j = 4; j < 8; j++) {
                    B[col + j - 4][row + i + 4] = A[row + i][col + j];
                }
            }
            int temp[2][4];
            for (int j = 0; j < 4; j += 2) {
                for (int i = 4; i < 8; i++) {
                    // for A
                    temp[0][i - 4] = B[col + j][row + i];
                    temp[1][i - 4] = B[col + j + 1][row + i];
                    B[col + j][row + i] = A[row + i][col + j];
                    B[col + j + 1][row + i] = A[row + i][col + j + 1];
                }
                for (int k = 0; k < 4; k++) {
                    // j -> row, k -> col for B
                    B[col + j + 4][row + k] = temp[0][k];
                    B[col + j + 5][row + k] = temp[1][k];
                }
            }
            for (int i = 4; i < 8; i++) {
                for (int j = 4; j < 8; j++) {
                    B[col + j][row + i] = A[row + i][col + j];
                }
            }
        }
    }
    return;
}

void transpose61X67(int M, int N, int A[N][M], int B[M][N]) {
    int row = 0, col = 0;

    for (; row < N; row += 16) {
        for (col = 0; col < M; col += 16) {
            for (int i = row; i < row + 16 && i < N; i++) {
                for (int j = col; j < col + 16 && j < M; j++) {
                    B[j][i] = A[i][j];
                }
            }
        }
    }
    return;
}

// cache: 32 * 8 int, A和B的每一个位置都将映射到cache的同一组
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 32) {
        transpose32X32(M, N, A, B);
    } else if (M == 64) {
        transpose64X64(M, N, A, B);
    } else {
        transpose61X67(M, N, A, B);
    }
    return;
}
/*
 * 我们在下面定义了一个简单的方法来帮助您开始，您可以根据下面的例子把上面值置补充完整。
 */

/*
 * 简单的基线转置功能，未针对缓存进行优化。
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions-此函数向驱动程序注册转置函数。
 *在运行时，驱动程序将评估每个注册的函数并总结它们的性能。这是一种试验不同转置策略的简便方法。
 */
void registerFunctions() {
    /* 注册解决方案函数  */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* 注册任何附加转置函数 */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - 函数检查B是否是A的转置。在从转置函数返回之前，可以通过调用它来检查转置的正确性。
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
