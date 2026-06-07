
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMEOUT_S 300.0   

static long long g_ops;   

/* utilidades  */
static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static void swap(int *a, int i, int j) {
    int tmp = a[i]; a[i] = a[j]; a[j] = tmp;
    g_ops++;                      
}

/*  Bubble Sort*/
static void bubble_sort(int *a, int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) { swap(a, j, j + 1); swapped = 1; }
        }
        if (!swapped) break;      
    }
}

/*  Quick Sort  */
static int partition(int *a, int lo, int hi) {
    int pivot = a[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (a[j] <= pivot) { i++; swap(a, i, j); }
    }
    swap(a, i + 1, hi);
    return i + 1;
}
static void quick_sort(int *a, int lo, int hi) {
    while (lo < hi) {             
        int p = partition(a, lo, hi);
        if (p - lo < hi - p) { quick_sort(a, lo, p - 1); lo = p + 1; }
        else                 { quick_sort(a, p + 1, hi); hi = p - 1; }
    }
}

/*  Merge Sort  */
static void merge(int *a, int *tmp, int lo, int mid, int hi) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        if (a[i] <= a[j]) tmp[k++] = a[i++];
        else              tmp[k++] = a[j++];
        g_ops++;                   
    }
    while (i <= mid) { tmp[k++] = a[i++]; g_ops++; }
    while (j <= hi)  { tmp[k++] = a[j++]; g_ops++; }
    for (int x = lo; x <= hi; x++) { a[x] = tmp[x]; g_ops++; } 
}
static void merge_sort_rec(int *a, int *tmp, int lo, int hi) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    merge_sort_rec(a, tmp, lo, mid);
    merge_sort_rec(a, tmp, mid + 1, hi);
    merge(a, tmp, lo, mid, hi);
}

/*  runner  */
static int *clone(const int *src, int n) {
    int *d = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) d[i] = src[i];
    return d;
}
static int is_sorted(const int *a, int n) {
    for (int i = 1; i < n; i++) if (a[i - 1] > a[i]) return 0;
    return 1;
}

int main(void) {
    int sizes[] = {1000, 10000, 100000};
    const char *names[] = {"Bubble Sort", "Quick Sort", "Merge Sort"};

    printf("algoritmo,n,exec,tempo_s,ops,ok\n");
    for (int s = 0; s < 3; s++) {
        int n = sizes[s]; 
        srand(12345 + s);        
        int *orig = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) orig[i] = rand();

        for (int alg = 0; alg < 3; alg++) {
            for (int run = 1; run <= 3; run++) {
                int *a = clone(orig, n);
                int *tmp = (alg == 2) ? malloc(n * sizeof(int)) : NULL;
                g_ops = 0;
                double t0 = now();
                if      (alg == 0) bubble_sort(a, n);
                else if (alg == 1) quick_sort(a, 0, n - 1);
                else               merge_sort_rec(a, tmp, 0, n - 1);
                double dt = now() - t0;
                int ok = is_sorted(a, n);
                printf("%s,%d,%d,%.6f,%lld,%d\n", names[alg], n, run, dt, g_ops, ok);
                fflush(stdout);
                free(a);
                free(tmp);
            }
        }
        free(orig);
    }
    return 0;
}
