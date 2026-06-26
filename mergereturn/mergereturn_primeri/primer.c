#include <stdio.h>
#define MAX 100

int probnaFunkcija1() {
    int x;
    
    if (x < 0) {
        return -1;
    }

    if (x == 0) {
        return 0;
    }

    return x * 2;
}

float probnaFunkcija2() {
    float x;
    
    if (x < 0) {
        return -1;
    }

    if (x == 0) {
        return 0;
    }

    return x * 2;
}

void probnaFunkcija3() {
    int x;
    
    if (x < 0) {
        x = -1;
        return;
    }

    if (x == 0) {
        x = 0;
        return;
    }

    x = x * 2;
    return;
}

int main() {
    int n = probnaFunkcija1();
    float m = probnaFunkcija2();
    probnaFunkcija3();
}
