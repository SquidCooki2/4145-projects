#include <iostream>
#include <chrono>
#include <cstdlib>
#include <string>
using namespace std;

int* merge(int* left, int* right, int leftSize, int rightSize) {
    int i = 0, j = 0, k = 0;
    int* result = new int[leftSize + rightSize];

    while (i < leftSize && j < rightSize) {
        if (left[i] > right[j])
            result[k++] = right[j++];
        else
            result[k++] = left[i++];
    }

    while (i < leftSize)
        result[k++] = left[i++];

    while (j < rightSize)
        result[k++] = right[j++];

    return result;
}

void print(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        cout << *(arr + i) << ", ";
    }
}

int* mergeSort(int arr[], int size) {
    if (size <= 1) {
        int* base = new int[size];
        if (size == 1)
            base[0] = arr[0];
        return base;
    }

    int mid = size / 2;
    // int* left = new int[mid];
    // int* right = new int[size - mid];
    
    // cout << endl;
    // for (int i = 0; i < size; i++) {
    //     if (i < mid) {
    //         left[i] = arr[i];
    //     } else {
    //         right[i - mid] = arr[i];
    //     }
    // }

    // int* sortedLeft = mergeSort(left, mid);
    // int* sortedRight = mergeSort(right, size - mid);

    // delete[] right; delete[] left;

    int* left = mergeSort(arr, mid);
    int* right = mergeSort(arr + mid, size - mid);

    int* result = merge(left, right, mid, size - mid);
    delete[] left; delete[] right;

    return result;
}

int* generate(int samples, int min, int max) {
    int i;
    int* arr = new int[samples];
    for (i = 0; i < samples; i++) {
        arr[i] = (rand() % (max - min + 1)) + min;
    }
    return arr;
}

int* generate(int samples, int min) { return generate(samples, min, 100); }
int* generate(int samples) { return generate(samples, 1); }

int main(int argc, char* argv[]) {
    

    if (argc < 2) {
        cerr << "Error: ./merge_sort [number of items] [min] [max]" << endl;
        return 1;
    }

    int size = stoi(argv[1]);
    int* arr;
    if (argc == 2) {
        arr = generate(size);
    } else if (argc == 3) {
        arr = generate(size, stoi(argv[2]));
    } else {
        arr = generate(size, stoi(argv[2]), stoi(argv[3]));
    }
   
    auto start = chrono::steady_clock::now();
    int* sortedArr = mergeSort(arr, size);
    cout << "Runtime: " << chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count() << "μs" << endl;
    delete[] arr; delete[] sortedArr; 

    return 0;
}