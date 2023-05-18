#ifndef TICKETSYSTEM_SRC_VECTOR_H
#define TICKETSYSTEM_SRC_VECTOR_H

#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
#include"exceptions.h"

namespace sjtu {
    template<typename T>
    class vector {
    public:
        T *start; // 数组
        size_t length; // 实际长度
        size_t capacity; // 当前容量

        class const_iterator;

        class iterator {
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            iterator::pointer iter = nullptr; // 指针实际指向
            iterator::pointer start = nullptr; // 指针所指向vector数组中指向第一个元素的指针
        public:
            iterator(iterator::pointer iter_, iterator::pointer start_) : iter(iter_), start(start_) {}

        public:
            T *getIter() const {
                return iter;
            }

        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                iterator tmp(*this);
                tmp.iter = iter + n;
                return tmp;
            }

            iterator operator-(const int &n) const {
                iterator tmp(*this);
                tmp.iter = iter - n;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invalid_iterator.
            int operator-(const iterator &rhs) const {
                if (rhs.start != start) throw sjtu::invalid_iterator();
                return iter - rhs.iter;
            }

            iterator &operator+=(const int &n) {
                iter += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                iter -= n;
                return *this;
            }

            iterator operator++(int) {
                iterator tmp(*this);
                tmp.iter = iter + 1;
                return tmp;
            }

            iterator &operator++() {
                ++iter;
                return *this;
            }

            iterator operator--(int) {
                iterator tmp(*this);
                tmp.iter = iter - 1;
                return tmp;
            }

            iterator &operator--() {
                --iter;
                return *this;
            }

            T &operator*() const {
                return *iter;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return iter == rhs.iter;
            }

            bool operator==(const const_iterator &rhs) const {
                return iter == rhs.iter;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return iter != rhs.iter;
            }

            bool operator!=(const const_iterator &rhs) const {
                return iter != rhs.iter;
            }
        };

        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            const T *iter; // 指针实际指向
            const T *start; // 指针所指向vector数组中指向第一个元素的指针
        public:
            const_iterator(const const_iterator::pointer iter_, const const_iterator::pointer start_) : iter(iter_),
                                                                                                        start(start_) {}

        public:
            const T *getIter() const {
                return iter;
            }

        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {
                const_iterator tmp(*this);
                tmp.iter = iter + n;
                return tmp;
            }

            const_iterator operator-(const int &n) const {
                const_iterator tmp(*this);
                tmp.iter = iter - n;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invalid_iterator.
            int operator-(const const_iterator &rhs) const {
                if (rhs.start != start) throw sjtu::invalid_iterator();
                return iter - rhs.iter;
            }

            const_iterator &operator+=(const int &n) {
                iter += n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                iter -= n;
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator tmp(*this);
                tmp.iter = iter + 1;
                return tmp;
            }

            const_iterator &operator++() {
                ++iter;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(*this);
                tmp.iter = iter - 1;
                return tmp;
            }

            const_iterator &operator--() {
                --iter;
                return *this;
            }

            const T &operator*() const {
                return *iter;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return iter == rhs.iter;
            }

            bool operator==(const const_iterator &rhs) const {
                return iter == rhs.iter;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return iter != rhs.iter;
            }

            bool operator!=(const const_iterator &rhs) const {
                return iter != rhs.iter;
            }
        };

        vector() {
            start = (T *) calloc(16, sizeof(T)); // 初始设为16个单位
            length = 0;
            capacity = 16;
        }

        vector(const vector &other) {
            start = (T *) calloc((other.capacity), sizeof(T));
            length = other.length;
            capacity = other.capacity;
            for (int i = 0; i < length; ++i) {
                start[i] = other.start[i];
            }
        }

        ~vector() {
            destructor();
        }

        inline void destructor() { // 释放空间
            T *temp;
            for (int i = 0; i < length; ++i) {  // 在free vector数组前先执行T相关的析构函数
                temp = &start[i];
                temp->~T();
            }
            free(start);
        }

        vector &operator=(const vector &other) {
            if (this == &other) return *this;

            destructor();

            start = (T *) calloc(other.capacity, sizeof(T));
            length = other.length;
            capacity = other.capacity;
            for (int i = 0; i < length; ++i) {
                start[i] = other.start[i];
            }
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos < 0 or pos >= capacity) throw sjtu::index_out_of_bound();
            return start[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos < 0 or pos >= capacity) throw sjtu::index_out_of_bound();
            return start[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos < 0 or pos >= capacity) throw sjtu::index_out_of_bound();
            return start[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos < 0 or pos >= capacity) throw sjtu::index_out_of_bound();
            return start[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (length == 0) throw sjtu::container_is_empty();
            return start[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (length == 0) throw sjtu::container_is_empty();
            return start[length - 1];
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(start, start);
        }

        const_iterator cbegin() const {
            return const_iterator(start, start);
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(start + length, start);
        }

        const_iterator cend() const {
            return const_iterator(start + length, start);
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            return length == 0;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return length;
        }

        /**
         * clears the contents
         */
        void clear() {
            destructor();

            start = (T *) calloc(16, sizeof(T));
            length = 0;
            capacity = 16;
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            return insert(pos.getIter() - start, value);
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind > capacity) throw sjtu::index_out_of_bound();
            if (length == capacity) { // 所有容量已满
                T *tmp = (T *) calloc(2 * capacity, sizeof(T)); // 扩容为两倍
                for (int i = 0; i < ind; ++i) {
                    tmp[i] = start[i];
                }
                tmp[ind] = value;
                for (int i = ind; i < length; ++i) {
                    tmp[i + 1] = start[i];
                }

                destructor();

                start = tmp;
                ++length;
                capacity *= 2;
                return iterator(start + ind, start);
            } else { // 原来空间未满
                for (int i = length; i >= (int) ind; --i) {
                    start[i + 1] = start[i];
                }
                start[ind] = value;
                ++length;
                return iterator(start + ind, start);
            }
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            return erase(pos.getIter() - start);
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind >= length or ind < 0) throw sjtu::index_out_of_bound();
            if (capacity > 256 and length <= capacity / 4) { // 缩容
                T *tmp = (T *) calloc(capacity / 2, sizeof(T));
                for (int i = 0; i < ind; ++i) {
                    tmp[i] = start[i];
                }
                for (int i = ind + 1; i < length; ++i) {
                    tmp[i - 1] = start[i];
                }

                destructor();

                start = tmp;
                --length;
                capacity /= 2;
                return iterator(start + ind, start);
            } else {
                for (int i = ind + 1; i < length; ++i) {
                    start[i - 1] = start[i];
                }
                --length;

                T *temp = &start[length]; // 析构所删除元素
                temp->~T();

                return iterator(start + ind, start);
            }
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (length == capacity) { // 容量不够扩容
                T *tmp = (T *) calloc(2 * capacity, sizeof(T));
                for (int i = 0; i < length; ++i) {
                    tmp[i] = start[i];
                }
                tmp[length] = value;

                destructor();

                start = tmp;
                ++length;
                capacity *= 2;
            } else { // 不需扩容
                start[length] = value;
                ++length;
            }
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (length == 0) throw sjtu::container_is_empty();
            if (capacity > 256 and length <= capacity / 4) {
                T *tmp = (T *) calloc(capacity / 2, sizeof(T));
                for (int i = 0; i < length - 1; ++i) {
                    tmp[i] = start[i];
                }

                destructor();

                start = tmp;
                --length;
                capacity /= 2;
            } else {
                --length;

                T *temp = &start[length]; // 析构所删除元素
                temp->~T();
            }
        }
    };
}
#endif //TICKETSYSTEM_SRC_VECTOR_H
