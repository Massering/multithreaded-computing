#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <cstdio>

struct Point {
    double x = -1;
    double y = -1;
    double z = -1;
};

struct Vector {
    Point a;
    Point b;
};

double length(Vector v) {
    double dx = (v.a.x - v.b.x),
            dy = (v.a.y - v.b.y),
            dz = (v.a.z - v.b.z);
    return sqrt(dx * dx + dy * dy + dz * dz);
}

double scale_mul(Vector v1, Vector v2) {
    double dx1 = (v1.a.x - v1.b.x),
            dy1 = (v1.a.y - v1.b.y),
            dz1 = (v1.a.z - v1.b.z);
    double dx2 = (v2.a.x - v2.b.x),
            dy2 = (v2.a.y - v2.b.y),
            dz2 = (v2.a.z - v2.b.z);
    return (dx1 * dx2 + dy1 * dy2 + dz1 * dz2);
}

double height_of_octaedr(Point *p) {
    for (int i = 0; i < 3; i++) {
        Vector v1 = {p[i], p[(i + 1) % 3]};
        Point mid = {(v1.a.x + v1.b.x) / 2, (v1.a.y + v1.b.y) / 2, (v1.a.z + v1.b.z) / 2};
        Vector v2 = {p[(i + 2) % 3], mid};
        if (abs(scale_mul(v1, v2)) < 0.0001) {
            return length(v1) / 2;
        }
    }
}

unsigned long long MAX = 18446744073709551615ull;
unsigned int r;

unsigned long long next(unsigned long long t) {
    return t * 43112609 * r * 9178465377 + 2147483647;
}

double rand_float(unsigned long long &t) {
    t = next(t);
    return (double) t / MAX * 2 - 1;
}

int main(int arg, char *argv[]) {
    int input_number_of_threads = atoi(argv[1]);
    int number_of_threads;

    FILE *file_in = fopen(argv[2], "r");
    int N = 1e2;
    fscanf(file_in, "%i\n", &N);

    Point p[3];
    for (int i = 0; i < 3; i++) {
        fscanf(file_in, "(%lf %lf %lf)\n", &p[i].x, &p[i].y, &p[i].z);
    }

    double tstart = omp_get_wtime();

    double H = height_of_octaedr(p);
    int N1 = 0;
    srand(time(nullptr));
    r = rand();

    if (input_number_of_threads == -1) {
        number_of_threads = 0;
        unsigned long long t;

        for (int i = 0; i < N; i++) {
            t = i;
            double x = rand_float(t) * H;
            double y = rand_float(t) * H;
            double z = rand_float(t) * H;

            if (abs(x) + abs(y) + abs(z) <= H) {
                N1++;
            }
        }
    } else {
        if (input_number_of_threads == 0) {
            number_of_threads = omp_get_num_threads();
        } else {
            number_of_threads = input_number_of_threads;
        }
        omp_set_num_threads(number_of_threads);
        int a[number_of_threads];
        for (int i = 0; i < number_of_threads; i++) {
            a[i] = 0;
        }

#pragma omp parallel for schedule(guided, 200)
        for (int i = 0; i < N; i++) {
            unsigned long long t = i;
            double x = rand_float(t) * H;
            double y = rand_float(t) * H;
            double z = rand_float(t) * H;

            if (abs(x) + abs(y) + abs(z) <= H) {
                a[omp_get_thread_num()]++;
            }
        }
        for (int i = 0; i < number_of_threads; i++) {
            N1 += a[i];
        }
    }

    long double S = ((long double) N1 / N) * H * H * H * 8;

    double tend = omp_get_wtime();

    FILE *file_out = fopen(argv[3], "w");
    std::fprintf(file_out, "%g %g\n", S, H * H * H * 4 / 3);

    std::printf("Time (%i thread(s)): %g ms\n", number_of_threads, (tend - tstart) * 1000);

//    FILE *file_stat = fopen("stat.txt", "w");
//    std::fprintf(file_stat, "%g", (tend - tstart) * 1000);

    return 0;
}
