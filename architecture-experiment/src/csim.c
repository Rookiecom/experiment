/*
 *csim.c-使用C编写一个Cache模拟器，它可以处理来自Valgrind的跟踪和输出统计
 *息，如命中、未命中和逐出的次数。更换政策是LRU。
 * 设计和假设:
 *  1. 每个加载/存储最多可导致一个缓存未命中。（最大请求是8个字节。）
 *  2. 忽略指令负载（I），因为我们有兴趣评估trace.c内容中数据存储性能。
 *  3. 数据修改（M）被视为加载，然后存储到同一地址。因此，M操作可能导致两次缓存命中，或者一次未命中和一次命中，外加一次可能的逐出。
 * 使用函数printSummary() 打印输出，输出hits, misses and evictions 的数，这对结果评估很重要
 */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cachelab.h"
//                    请在此处添加代码
//****************************Begin*********************

int s, e, b;
int hit, miss, evictions;
int logicClock = 0;
char file[20];

const int cacheNULL = 0;

typedef struct cacheLine {
    int valid;
    int setTag;
    int time;
    char* data;
} cacheLine;

cacheLine** cache;

void initCache() {
    cache = (cacheLine**)malloc((1 << s) * e * sizeof(cacheLine*));
    for (int i = 0; i < (1 << s); i++) {
        for (int j = 0; j < e; j++) {
            cacheLine* newLine = (cacheLine*)malloc(sizeof(cacheLine));
            newLine->valid = cacheNULL;
            newLine->time = -1;
            char* data = (char*)malloc(1 << b);
            newLine->data = data;
            cache[i * e + j] = newLine;
        }
    }
    return;
}

void access(unsigned address, int size) {
    address = address >> b;
    int sidx = address % (1 << s);
    int setTag = address / (1 << s);
    cacheLine* pcache ;
    int minTime = 0x7fffffff, minTimeIdx;
    for (int i = 0; i < e; i++, pcache++) {
        pcache = cache[sidx * e + i];
        if (pcache->valid == cacheNULL) {
            miss++;
            pcache->valid = !cacheNULL;
            pcache->setTag = setTag;
            pcache->time = logicClock;
            logicClock++;
            return;
        }
        if (pcache->setTag == setTag) {
            hit++;
            pcache->time = logicClock;
            logicClock++;
            return;
        }
        if (pcache->time < minTime) {
            minTime = pcache->time;
            minTimeIdx = i;
        }
    }
    pcache = cache[sidx * e + minTimeIdx];
    pcache->setTag = setTag;
    pcache->time = logicClock;
    logicClock++;
    miss++;
    evictions++;
    return;
}

void modify(int address, int size) {
    access(address, size);
    access(address, size);
}

void freeCache() {
    for (int i = 0; i < (1 << s); i++) {
        for (int j = 0; j < e; j++) {
            free(cache[i * e + j]->data);
            free(cache[i * e + j]);
        }
    }
    free(cache);
    return;
}

int main(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch (opt) {
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            e = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            strcpy(file, optarg);
            break;
        default:
            printf("wrong argument\n");
            break;
        }
    }
    initCache();

    FILE* pfile;
    pfile = fopen(file, "r");

    char identifier;
    unsigned address;
    int size;

    while (fscanf(pfile, "%c %x, %d", &identifier, &address, &size) > 0) {
        switch (identifier) {
        case 'L':
            access(address, size);
            break;
        case 'S':
            access(address, size);
            break;
        case 'I':
            break;
        case 'M':
            modify(address, size);
            break;
        default:
            break;
        }
    }
    fclose(pfile);

    printSummary(hit, miss, evictions);  // 输出hit、miss和evictions数量统计
    return 0;
}
//****************************End**********************#