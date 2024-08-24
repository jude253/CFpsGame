#include <stdio.h>
#include <stdbool.h>
#include <math.h>


void printfExamples() {
    int a = 120;
    int b = 65;
    float float_a_b = (float)a/b;
    double double_a_b = (double)a/b;
    float double_to_float_a_b = (float)double_a_b;
    
    printf("a/b=%lf\n", ((double)a/b));
    printf("Precision int: %.*d\n", 5, 10);
    printf("Precision int: %.5d\n", 10);
    printf("Precision int: %.5d\n", 1234567);


    // I can't remember exactly why the float isn't just truncated at a
    // shorter amount than the double, but I believe it has to do with
    // how floats and doubles are stored in memory as binary not decimal.
    printf("Precision double: %.60f\n", (double)1234567);
    printf("Precision float_a_b: %.60f\n", float_a_b);
    printf("Precision double_a_b: %.60f\n", double_a_b);
    printf("Precision double_to_float_a_b: %.60f\n", double_to_float_a_b);

    // Prints whatever happened to be in that location of memory before:
    float uninitialized_float;
    printf("Uninitialized float: %f\n", uninitialized_float);
}


typedef struct {
    bool defined;
    float x;
    float y;
} Point;

void printPoint(Point point) {
    printf("(%f, %f), defined=%d\n", point.x, point.y, point.defined);
}

Point LIST_OF_POINTS[100];

/*
Run with:
gcc test/test.cpp -o test.out && ./test.out
*/
int main() {
    for (int i = 0; i < 100; i++) {
        printPoint(LIST_OF_POINTS[i]);
    }
    return 0;
}