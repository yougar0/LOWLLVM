int main(int argc, char** argv) {
    int a = 0;
    int b = 1;
    for(; a < 10; a++) {
        b += 2;
    }
    if(b == 20) {
        a += b;
    }
}