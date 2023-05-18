#ifndef TICKETSYSTEM_SRC_MAP_H
#define TICKETSYSTEM_SRC_MAP_H

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <cmath>
#include <iostream>
#include "utility.h"
#include "exceptions.h"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;

        class AvlNode {
        public:

            value_type data; // 数据
            int height; // 高度
            AvlNode *leftChild;
            AvlNode *rightChild;
            AvlNode *parent;

            AvlNode() : height(0), leftChild(nullptr), rightChild(nullptr), parent(nullptr) {}

            AvlNode(value_type data_, int height_ = 0, AvlNode *left = nullptr, AvlNode *right = nullptr,
                    AvlNode *pa = nullptr) : data(data_), height(height_), leftChild(left), rightChild(right),
                                             parent(pa) {}
        };

    private:
        AvlNode *root; // 根结点
        long long sizes = 0; // 大小
        Compare cmp; // 比较类对象
    public:
        AvlNode *start; // 最小数据对
        AvlNode *last; // 最大数据对
    public:
        //1.定义一个 type_traits 模板类
        template<typename T_>
        struct my_type_traits {
            using iterator_assignable = typename T_::iterator_assignable;
        };
        //2.定义两个类，表示迭代器是否可被赋值的特性(这一步也可以使用枚举变量来实现)
        struct my_true_type {
            static const bool if_assignable = true;
        };
        struct my_false_type {
            static const bool if_assignable = false;
        };
        //3.分别在可被赋值的迭代器和不可被赋值的迭代器中定义 iterator_assignable 类型
    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator {
        public:
            using iterator_assignable = my_true_type;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            AvlNode *iter; // 指向结点
            map *m; // 指向所在map
            // 令iter = nullptr代表end()
        public:
            AvlNode *getIter() const {
                return iter;
            }

            map *getMap() const {
                return m;
            }

        public:
            iterator() {
                // TODO
                iter = nullptr;
            }

            iterator(const iterator &other) {
                // TODO
                iter = other.getIter();
                m = other.getMap();
            }

            iterator(AvlNode *iter_, map *m_) : iter(iter_), m(m_) {}

            /**
             * TODO iter++
             */

            iterator operator++(int) {
                if (iter == nullptr) throw invalid_iterator(); // 指向end()或指向不存在的结点
                iterator tmp(*this);
                if (iter == m->last) { // 如果是最后一个则下一个是end()
                    iter = nullptr;
                } else if (iter->rightChild != nullptr) {
                    iter = iter->rightChild;
                    while (iter->leftChild != nullptr) {
                        iter = iter->leftChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->rightChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return tmp;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (iter == nullptr) throw invalid_iterator(); // 指向end()或指向不存在的结点
                if (iter == m->last) { // 如果是最后一个则下一个是end()
                    iter = nullptr;
                } else if (iter->rightChild != nullptr) {
                    iter = iter->rightChild;
                    while (iter->leftChild != nullptr) {
                        iter = iter->leftChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->rightChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                if (iter == m->start) throw invalid_iterator(); // 如果指向begin() 报错
                iterator tmp(*this);
                if (iter == nullptr) { // 如果指向end()，则返回最大数据对
                    iter = m->last;
                } else if (iter->leftChild != nullptr) {
                    iter = iter->leftChild;
                    while (iter->rightChild != nullptr) {
                        iter = iter->rightChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->leftChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return tmp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if (iter == m->start) throw invalid_iterator(); // 如果指向begin() 报错
                if (iter == nullptr) { // 如果指向end()，则返回最大数据对
                    iter = m->last;
                } else if (iter->leftChild != nullptr) {
                    iter = iter->leftChild;
                    while (iter->rightChild != nullptr) {
                        iter = iter->rightChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->leftChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return iter->data;
            }

            bool operator==(const iterator &rhs) const {
                return (iter == rhs.getIter() && m == rhs.getMap());
            }

            bool operator==(const const_iterator &rhs) const {
                return (iter == rhs.getIter() && m == rhs.getMap());
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (iter != rhs.getIter() || m != rhs.getMap());
            }

            bool operator!=(const const_iterator &rhs) const {
                return (iter != rhs.getIter() || m != rhs.getMap());
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                return &(iter->data);
            }
        };

        class const_iterator {
            // it should have similar member method as iterator.
            //  and it should be able to construct from an iterator.
        public:
            using iterator_assignable = my_false_type;
        private:
            // data members.
            const AvlNode *iter;
            const map *m;
        public:
            const AvlNode *getIter() const {
                return iter;
            }

            const map *getMap() const {
                return m;
            }

        public:
            const_iterator() : iter(nullptr), m(nullptr) {
                // TODO
            }

            const_iterator(const AvlNode *iter_, const map *m_) : iter(iter_), m(m_) {}

            const_iterator(const const_iterator &other) : iter(other.getIter()), m(other.getMap()) {
                // TODO
            }

            const_iterator(const iterator &other) : iter(other.getIter()), m(other.getMap()) {
                // TODO
            }
            // And other methods in iterator.
            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                if (iter == nullptr) throw invalid_iterator();
                const_iterator tmp(*this);
                if (iter == m->last) {
                    iter = nullptr;
                } else if (iter->rightChild != nullptr) {
                    iter = iter->rightChild;
                    while (iter->leftChild != nullptr) {
                        iter = iter->leftChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->rightChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return tmp;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                if (iter == nullptr) throw invalid_iterator();
                if (iter == m->last) {
                    iter = nullptr;
                } else if (iter->rightChild != nullptr) {
                    iter = iter->rightChild;
                    while (iter->leftChild != nullptr) {
                        iter = iter->leftChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->rightChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                if (iter == m->start) throw invalid_iterator();
                const_iterator tmp(*this);
                if (iter == nullptr) {
                    iter = m->last;
                } else if (iter->leftChild != nullptr) {
                    iter = iter->leftChild;
                    while (iter->rightChild != nullptr) {
                        iter = iter->rightChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->leftChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return tmp;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                if (iter == m->start) throw invalid_iterator();
                if (iter == nullptr) {
                    iter = m->last;
                } else if (iter->leftChild != nullptr) {
                    iter = iter->leftChild;
                    while (iter->rightChild != nullptr) {
                        iter = iter->rightChild;
                    }
                } else {
                    AvlNode *p = iter->parent;
                    while (p->leftChild == iter) {
                        iter = p;
                        p = p->parent;
                    }
                    iter = p;
                }
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            const value_type &operator*() const {
                return iter->data;
            }

            bool operator==(const iterator &rhs) const {
                return (iter == rhs.getIter() && m == rhs.getMap());
            }

            bool operator==(const const_iterator &rhs) const {
                return (iter == rhs.getIter() && m == rhs.getMap());
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (iter != rhs.getIter() || m != rhs.getMap());
            }

            bool operator!=(const const_iterator &rhs) const {
                return (iter != rhs.getIter() || m != rhs.getMap());
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type *operator->() const noexcept {
                return &(iter->data);
            }
        };

        /* used for type_traits_test */
        template<class T_>
        void test(T_ iter, bool assignable) {
            std::cout << "The iterator is judged " << (assignable ? "" : "not ") << "assignable." << std::endl;
        }

        template<class T_>
        void type_traits_test(T_ iter) {
            using iterator_assignable = typename my_type_traits<T_>::iterator_assignable;
            test(iter, iterator_assignable::if_assignable);
        }

        /**
         * TODO two constructors
         */
        map() { root = nullptr, start = nullptr, last = nullptr; }

        map(const map &other) {
            if (other.root == nullptr) {  // 复制空树
                root = nullptr, start = nullptr, last = nullptr;
                sizes = 0;
                return;
            }

            sizes = other.sizes;

            root = new AvlNode(other.root->data, other.root->height);
            if (other.root->leftChild != nullptr) {
                root->leftChild = makeQueue(other.root->leftChild);
                root->leftChild->parent = root;
            }
            if (other.root->rightChild != nullptr) {
                root->rightChild = makeQueue(other.root->rightChild);
                root->rightChild->parent = root;
            }

            start = last = root; // 更新start last
            while (start->leftChild != nullptr) start = start->leftChild;
            while (last->rightChild != nullptr) last = last->rightChild;
        }

        /**
         * TODO assignment operator
         */
        map &operator=(const map &other) {
            if (this == &other) return *this;
            clear(root);

            if (other.root == nullptr) { // 复制空树
                root = nullptr;
                return *this;
            }

            sizes = other.sizes;

            root = new AvlNode(other.root->data, other.root->height);
            if (other.root->leftChild != nullptr) {
                root->leftChild = makeQueue(other.root->leftChild);
                root->leftChild->parent = root;
            }
            if (other.root->rightChild != nullptr) {
                root->rightChild = makeQueue(other.root->rightChild);
                root->rightChild->parent = root;
            }

            start = last = root; // 更新start last
            while (start->leftChild != nullptr) start = start->leftChild;
            while (last->rightChild != nullptr) last = last->rightChild;

            return *this;
        }

        /**
         * TODO Destructors
         */
        ~map() { clear(root); }

    public:
        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            AvlNode *tmp = findKey(key);
            if (tmp == nullptr) throw sjtu::index_out_of_bound(); // 无法找到
            return tmp->data.second;
        }

        const T &at(const Key &key) const {
            const AvlNode *tmp = findKey(key);
            if (tmp == nullptr) throw index_out_of_bound();
            return tmp->data.second;
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */

        T &operator[](const Key &key) {
            AvlNode *tmp = findKey(key);
            if (tmp == nullptr) { // 无法找到则创建
                value_type v(key, T());
                return insert(v, root, nullptr).first.getIter()->data.second;
            } else return tmp->data.second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            const AvlNode *tmp = findKey(key);
            if (tmp == nullptr) throw index_out_of_bound();
            return tmp->data.second;
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            if (sizes == 0) return end(); // 空map
            return iterator(start, this);
        }

        const_iterator cbegin() const {
            if (sizes == 0) return cend();
            return const_iterator(start, this);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(nullptr, this);
        }

        const_iterator cend() const {
            return const_iterator(nullptr, this);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return sizes == 0;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return sizes;
        }

    private:
        AvlNode *makeQueue(AvlNode *source) { // 复制以source为根的树
            AvlNode *tmp = new AvlNode(source->data, source->height);
            if (source->leftChild != nullptr) {
                tmp->leftChild = makeQueue(source->leftChild);
                tmp->leftChild->parent = tmp;
            }
            if (source->rightChild != nullptr) {
                tmp->rightChild = makeQueue(source->rightChild);
                tmp->rightChild->parent = tmp;
            }
            return tmp;
        }

        void clear(AvlNode *root_) { // 释放空间
            if (root_ == nullptr) return;
            if (root_->leftChild != nullptr) clear(root_->leftChild);
            if (root_->rightChild != nullptr) clear(root_->rightChild);
            delete root_;
        }

        int getHeight(AvlNode *t) { // 返回子树高度，空结点高度-1
            if (t == nullptr) return -1;
            else return t->height;
        }

        int newHeight(AvlNode *t) { // 重新计算现在的高度
            int l = getHeight(t->leftChild), r = getHeight(t->rightChild);
            if (l < r) return r + 1;
            else return l + 1;
        }

        bool ifBalance(AvlNode *t) { // 现高度是否与调整前的高度一致
            return (t->height == newHeight(t));
        }

        void LL(AvlNode *&t) {
            AvlNode *t_ = t;
            AvlNode *tmp = t_->leftChild;

            if (t_ != root) {
                if (t_->parent->leftChild == t_) t_->parent->leftChild = tmp;
                else t_->parent->rightChild = tmp;
                tmp->parent = t_->parent;
            } else {
                root = tmp;
                tmp->parent = nullptr;
            }

            t_->leftChild = tmp->rightChild;
            if (tmp->rightChild != nullptr) tmp->rightChild->parent = t_;
            tmp->rightChild = t_;
            t_->parent = tmp;
            t_->height = newHeight(t_);
            tmp->height = newHeight(tmp);

            t = tmp;
        }

        void LR(AvlNode *&t) {
            RR(t->leftChild);
            LL(t);
        }

        void RL(AvlNode *&t) {
            LL(t->rightChild);
            RR(t);
        }

        void RR(AvlNode *&t) {
            AvlNode *t_ = t;
            AvlNode *tmp = t_->rightChild;

            if (t_ != root) {
                if (t_->parent->leftChild == t_) t_->parent->leftChild = tmp;
                else t_->parent->rightChild = tmp;
                tmp->parent = t_->parent;
            } else {
                root = tmp;
                tmp->parent = nullptr;
            }

            t_->rightChild = tmp->leftChild;
            if (tmp->leftChild != nullptr) tmp->leftChild->parent = t_;
            tmp->leftChild = t_;
            t_->parent = tmp;
            t_->height = newHeight(t_);
            tmp->height = newHeight(tmp);

            t = tmp;
        }

        AvlNode *findKey(const Key &key) const { // 找到元素所应在的位置
            AvlNode *tmp = root;
            while (tmp != nullptr && (cmp(tmp->data.first, key) == 1 || cmp(key, tmp->data.first) == 1)) {
                if (cmp(tmp->data.first, key)) tmp = tmp->rightChild;
                else tmp = tmp->leftChild;
            }
            return tmp;
        }

        pair<iterator, bool> insert(const value_type &value, AvlNode *&t, AvlNode *pa) {
            if (sizes == 0) { // 第一个结点
                t = new AvlNode(value);
                ++sizes;
                root = start = last = t;
                return pair<iterator, bool>{iterator(t, this), true};
            } else if (t == nullptr) { // 找到插入的位置
                t = new AvlNode(value);
                t->parent = pa;
                adjust(pa);
                ++sizes;
                AvlNode *temp = findKey(value.first);
                if (cmp(value.first, start->data.first)) start = temp;
                if (cmp(last->data.first, value.first)) last = temp;
                return pair<iterator, bool>{iterator(temp, this), true};
            } else if (cmp(value.first, t->data.first)) {
                return insert(value, t->leftChild, t);
            } else if (cmp(t->data.first, value.first)) {
                return insert(value, t->rightChild, t);
            } else return pair<iterator, bool>{iterator(t, this), false}; // map中已存在 插入失败
        }

        void adjust(AvlNode *pa) {
            if (ifBalance(pa)) { // 如果高度不变，可能会因为较矮子树变矮而不平衡
                if (getHeight(pa->leftChild) - getHeight(pa->rightChild) == 2) {
                    if (getHeight(pa->leftChild->leftChild) == getHeight(pa->leftChild->rightChild)) {
                        LL(pa);
                    } else if (getHeight(pa->leftChild->leftChild) > getHeight(pa->leftChild->rightChild)) {
                        LL(pa);
                        if (pa != root) adjust(pa->parent);
                    } else {
                        LR(pa);
                        if (pa != root) adjust(pa->parent);
                    }
                } else if (getHeight(pa->rightChild) - getHeight(pa->leftChild) == 2) {
                    if (getHeight(pa->rightChild->rightChild) == getHeight(pa->rightChild->leftChild)) {
                        RR(pa);
                    } else if (getHeight(pa->rightChild->rightChild) > getHeight(pa->rightChild->leftChild)) {
                        RR(pa);
                        if (pa != root) adjust(pa->parent);
                    } else {
                        RL(pa);
                        if (pa != root) adjust(pa->parent);
                    }
                }
            } else { // 如果高度变化，可能会因为较高子树变矮而不平衡
                if (getHeight(pa->leftChild) - getHeight(pa->rightChild) == 2) {
                    if (getHeight(pa->leftChild->leftChild) >
                        getHeight(pa->leftChild->rightChild)) { // leftChild左右子树不可能相等
                        LL(pa);
                    } else if (getHeight(pa->leftChild->leftChild) < getHeight(pa->leftChild->rightChild)) {
                        LR(pa);
                    }
                } else if (getHeight(pa->rightChild) - getHeight(pa->leftChild) == 2) {
                    if (getHeight(pa->rightChild->rightChild) >
                        getHeight(pa->rightChild->leftChild)) { // rightChild左右子树不可能相等
                        RR(pa);
                    } else if (getHeight(pa->rightChild->rightChild) < getHeight(pa->rightChild->leftChild)) {
                        RL(pa);
                    }
                } else { // 平衡则继续往上调整
                    pa->height = newHeight(pa);
                    if (pa != root) adjust(pa->parent);
                }
            }
        }

    public:

        /**
         * clears the contents
         */
        void clear() {
            clear(root);
            root = start = last = nullptr;
            sizes = 0;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            return insert(value, root, nullptr);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.getIter() == nullptr) throw invalid_iterator();
            if (pos.getMap() != this) throw invalid_iterator();

            if (sizes == 1) { // 只有一个结点
                delete root;
                root = start = last = nullptr;
                sizes = 0;
                return;
            }

            AvlNode *del = pos.getIter();
            if (del->leftChild == nullptr || del->rightChild == nullptr) { // 删除点只有一个儿子或没有
                --sizes;
                if (del == root) { // 删除的是根
                    if (del->rightChild == nullptr) {
                        if (del->leftChild != nullptr) { // 只有左儿子
                            root = del->leftChild;
                            root->parent = nullptr;
                            last = root;
                            while (last->rightChild != nullptr) last = last->rightChild;
                        }
                    } else { // 只有右儿子
                        root = del->rightChild;
                        root->parent = nullptr;
                        start = root;
                        while (start->leftChild != nullptr) last = last->leftChild;
                    }
                    delete del;
                    del = nullptr;
                } else { // 删除的是非根
                    if (del->rightChild == nullptr) {
                        if (del->leftChild != nullptr) { // 只有左儿子
                            if (del->parent->leftChild == del) {
                                del->parent->leftChild = del->leftChild;
                            } else del->parent->rightChild = del->leftChild;
                            del->leftChild->parent = del->parent;
                            if (del == last) {
                                last = last->leftChild;
                                while (last->rightChild != nullptr) last = last->rightChild;
                            }
                            adjust(del->parent);
                        } else { // 删除的点为叶节点
                            if (del == start) start = start->parent;
                            if (del == last) last = last->parent;
                            if (del->parent == root) { // 父亲是根节点
                                if (root->leftChild == del) {
                                    root->leftChild = nullptr;
                                    if (root->rightChild != nullptr) adjust(root);
                                    else root->height = 0;
                                } else {
                                    root->rightChild = nullptr;
                                    if (root->leftChild != nullptr) adjust(root);
                                    else root->height = 0;
                                }
                            } else if (del->parent->rightChild == nullptr ||
                                       del->parent->leftChild == nullptr) { // 父亲结点只有一个孩子
                                del->parent->rightChild = nullptr;
                                del->parent->leftChild = nullptr;
                                del->parent->height = 0;
                                if (del->parent->parent != nullptr) adjust(del->parent->parent);
                            } else if (del->parent->rightChild == del) { // 是父亲结点的右儿子
                                del->parent->rightChild = nullptr;
                                adjust(del->parent);
                            } else { // 是父亲结点的左儿子
                                del->parent->leftChild = nullptr;
                                adjust(del->parent);
                            }
                        }
                    } else { // 只有右儿子
                        if (del->parent->leftChild == del) {
                            del->parent->leftChild = del->rightChild;
                        } else del->parent->rightChild = del->rightChild;
                        del->rightChild->parent = del->parent;
                        if (del == start) {
                            start = start->rightChild;
                            while (start->leftChild != nullptr) start = start->leftChild;
                        }
                        adjust(del->parent);
                    }
                    delete del;
                    del = nullptr;
                }
            } else { // 有两个儿子
                if (del == root) { // 删除的点是根节点
                    AvlNode *replace = del->rightChild;
                    while (replace->leftChild != nullptr) replace = replace->leftChild; // 找到替身
                    del->leftChild->parent = replace;
                    if (del->rightChild != replace) { // 替身不是右孩子
                        del->rightChild->parent = replace;
                        del->parent = replace->parent;
                        replace->parent->leftChild = del;
                        replace->parent = nullptr;
                        AvlNode *r = replace->rightChild;
                        replace->leftChild = del->leftChild;
                        replace->rightChild = del->rightChild;
                        del->leftChild = nullptr;
                        del->rightChild = r;
                        if (r != nullptr) r->parent = del;
                    } else { // 替身是右孩子
                        replace->parent = nullptr;
                        replace->leftChild = del->leftChild;
                        del->rightChild = replace->rightChild;
                        if (replace->rightChild != nullptr) replace->rightChild->parent = del;
                        del->leftChild = nullptr;
                        replace->rightChild = del;
                        del->parent = replace;
                    }
                    int h = del->height;
                    del->height = replace->height;
                    replace->height = h;
                    root = replace;
                    erase(iterator(del, this));
                } else { // 非根结点
                    AvlNode *replace = del->rightChild;
                    while (replace->leftChild != nullptr) replace = replace->leftChild;
                    del->leftChild->parent = replace;
                    if (del->rightChild != replace) { // 替身不是右孩子
                        del->rightChild->parent = replace;
                        replace->parent->leftChild = del;
                        if (del->parent->rightChild == del) del->parent->rightChild = replace;
                        else del->parent->leftChild = replace;
                        AvlNode *p = replace->parent;
                        AvlNode *r = replace->rightChild;
                        replace->parent = del->parent;
                        replace->leftChild = del->leftChild;
                        replace->rightChild = del->rightChild;
                        del->leftChild = nullptr;
                        del->rightChild = r;
                        del->parent = p;
                        if (r != nullptr) r->parent = del;
                    } else { // 替身是右孩子
                        if (del->parent->rightChild == del) del->parent->rightChild = replace;
                        else del->parent->leftChild = replace;
                        del->rightChild = replace->rightChild;
                        if (replace->rightChild != nullptr) replace->rightChild->parent = del;
                        replace->leftChild = del->leftChild;
                        replace->leftChild->parent = replace;
                        replace->parent = del->parent;
                        del->leftChild = nullptr;
                        replace->rightChild = del;
                        del->parent = replace;
                    }
                    int h = del->height;
                    del->height = replace->height;
                    replace->height = h;
                    erase(iterator(del, this));
                }
            }
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            if (findKey(key) == nullptr) return 0;
            else return 1;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            return iterator(findKey(key), this);
        }

        const_iterator find(const Key &key) const {
            return const_iterator(findKey(key), this);
        }
    };

}

#endif //TICKETSYSTEM_SRC_MAP_H
