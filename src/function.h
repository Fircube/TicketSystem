#ifndef TICKETSYSTEM_FUNCTION_H
#define TICKETSYSTEM_FUNCTION_H
#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
namespace sjtu {
    template<class T>
    void sort(T *first, T *last) {
        int l = 0, r = last - first - 1;
        if (!(l < r)) return;
        auto k = *first;
        do {
            while (l < r && !(*(first + r) < k)) --r;
            if (l < r) {
                *(first + l) = *(first + r);
                ++l;
            }
            while (l < r && !(k < *(first + l))) ++l;
            if (l < r) {
                *(first + r) = *(first + l);
                --r;
            }
        } while (l != r);
        *(first + l) = k;
        sort(first, (first + l));
        sort((first + l + 1), last);
        // O(nlogn)
    }

    template<class T>
    T *lower_bound(T *first, T *last, const T &value) {
        // TODO
        if (*(last - 1) < value) return last;
        if (last <= first) return first;
        int l = 0, r = last - first - 1;
        while (l < r) {
            int m = (l + r) / 2;
            if (*(first + m) < value) {
                l = m + 1;
            } else r = m;
        }
        return (first + r);
        // O(logn)
        return nullptr;
    }

}

#endif //TICKETSYSTEM_FUNCTION_H
