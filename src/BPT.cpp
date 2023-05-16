#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
#include "vector.h"
#include "function.h"

using index_type = char[64];
using value_type = int;

const int M = 120; // M叉树
const int M_min = 60;
const int BlockSize = 320; // 数据块大小
const int minSize = 160;


class BPT {
public:
    sjtu::vector<int> storage_T; // store unused tag of tree;
    sjtu::vector<int> storage_R; // store unused tag of storage;

public:
    // 最小数据单元
    struct Node {
        index_type index;
        value_type value = -1;

        Node() {
            memset(index, 0, sizeof(index));
        };

        Node(const index_type &index_, const value_type &value_) {
            memset(index, 0, sizeof(index));
            strcpy(index, index_);
            value = value_;
        }

        Node &operator=(const Node &rhs) {
            memset(index, 0, sizeof(index));
            strcpy(index, rhs.index);
            value = rhs.value;
            return *this;
        }

        bool operator<(const Node &rhs) const {
            if (strcmp(index, rhs.index) != 0) {
                if (strcmp(index, rhs.index) < 0) return true;
                return false;
            } else return value < rhs.value;
        }

        bool operator>(const Node &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const Node &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const Node &rhs) const {
            return !(*this < rhs);
        }

        bool operator==(const Node &rhs) const {
            if (strcmp(index, rhs.index) != 0)return false;
            if (value != rhs.value)return false;
            return true;
        }

        bool operator!=(const Node &rhs) const {
            return !(*this == rhs);
        }
    };

public:
    // M叉树结点
    struct BPTreeNode {
        int tag = -1;  // 记录序号
        int size = 0;
        bool ifLeaf = true;
        Node key[121]; // 关键词
        int son[121];

        BPTreeNode() {
            memset(son, -1, sizeof(int) * 121);
        }

        BPTreeNode(int tag_, int size_, bool ifLeaf_) {
            memset(son, -1, sizeof(int) * 121);
            tag = tag_;
            size = size_;
            ifLeaf = ifLeaf_;
        }

    };

public:
    // 数据块
    struct Record {
        int tag = -1; // 记录序号
        int size = 0;
        int next = -1; // 下一个数据块序号
        Node Block[BlockSize];

        Record() = default;

        Record(int tag_, int size_, int next_ = -1) {
            tag = tag_;
            size = size_;
            next = next_;
        }
    };

public:
    // 缓存当前树链
    struct cache {
        int size = 0;
        BPTreeNode link[100];
        int order[100]; //记录分支序号

        cache() {
            memset(order, -1, sizeof(int) * 100);
        }
    } ca;

    BPTreeNode *root = &ca.link[0];
    int head_tag = -1; // 记录第一个数据块

    int total_T = 0;
    int total_R = 0;

    std::fstream StorageOfTree;
    std::string FilenameOfTree;
    std::fstream StorageOfRecord;
    std::string FilenameOfRecord;
    std::fstream StorageOfInf;
    std::string FilenameOfInf;

    void readTree(BPTreeNode &read_, const int &location) {
        StorageOfTree.seekg(sizeof(BPTreeNode) * location);
        StorageOfTree.read(reinterpret_cast<char *>(&read_), sizeof(BPTreeNode));
    }

    void writeTree(BPTreeNode &write_, const int &location) {
        StorageOfTree.seekp(sizeof(BPTreeNode) * location);
        StorageOfTree.write(reinterpret_cast<char *>(&write_), sizeof(BPTreeNode));
    }

    void readRecord(Record &read_, const int &location) {
        StorageOfRecord.seekg(sizeof(Record) * location);
        StorageOfRecord.read(reinterpret_cast<char *>(&read_), sizeof(Record));
    }

    void writeRecord(Record &write_, const int &location) {
        StorageOfRecord.seekp(sizeof(Record) * location);
        StorageOfRecord.write(reinterpret_cast<char *>(&write_), sizeof(Record));
    }

    BPT() = default;

    BPT(const std::string &fileName1, const std::string &fileName2, const std::string &fileName3) {
        FilenameOfTree = fileName1;
        FilenameOfRecord = fileName2;
        FilenameOfInf = fileName3;
        StorageOfTree.open(fileName1);
        if (!StorageOfTree.good()) {
            StorageOfTree.open(fileName1, std::fstream::out);
            StorageOfTree.close();
            StorageOfTree.open(fileName1);
        }
        StorageOfRecord.open(fileName2);
        if (!StorageOfRecord.good()) {
            StorageOfRecord.open(fileName2, std::fstream::out);
            StorageOfRecord.close();
            StorageOfRecord.open(fileName2);
        }
        StorageOfInf.open(fileName3);
        if (!StorageOfInf.good()) {
            StorageOfInf.open(fileName3, std::fstream::out);
            StorageOfInf.close();
            StorageOfInf.open(fileName3);
        } else {
            StorageOfInf.seekg(0);
            StorageOfInf.read(reinterpret_cast <char *> (&total_T), sizeof(total_T));
            StorageOfInf.read(reinterpret_cast <char *> (&total_R), sizeof(total_R));
            StorageOfInf.read(reinterpret_cast <char *> (&ca.link[0]), sizeof(BPTreeNode));
            ca.size = 1;
        }
    }

    ~BPT() {
        StorageOfInf.seekp(0);
        StorageOfInf.write(reinterpret_cast <const char *> (&total_T), sizeof(int));
        StorageOfInf.write(reinterpret_cast <const char *> (&total_R), sizeof(int));
        StorageOfInf.write(reinterpret_cast <const char *> (&ca.link[0]), sizeof(BPTreeNode));
        writeTree(*root, root->tag);
        storage_T.clear();
        storage_R.clear();
        StorageOfTree.close();
        StorageOfRecord.close();
        StorageOfInf.close();
    }

    int AssignTreeTag() {
        if (storage_T.empty()) {
            return total_T++;
        } else {
            int tag_ = storage_T.back();
            storage_T.pop_back();
            return tag_;
        }
    }

    void RestoreTreeTag(int tag_) {
        storage_T.push_back(tag_);
    }

    int AssignRecordTag() {
        if (storage_R.empty()) {
            return total_R++;
        } else {
            int tag_ = storage_R.back();
            storage_R.pop_back();
            return tag_;
        }
    }

    void RestoreRecordTag(int tag_) {
        storage_R.push_back(tag_);
    }

    Record tmp_R;

    int FindTreeLocate(const Node &node_, BPTreeNode *Bnode_) {
        Node *tmp = sjtu::lower_bound(Bnode_->key, Bnode_->key + Bnode_->size - 1, node_);
        if (*tmp == node_) return tmp - Bnode_->key + 1;
        else return tmp - Bnode_->key;
    }

    int FindRecordLocate(const Node &node_, int tag_) {
        readRecord(tmp_R, tag_);
        return sjtu::lower_bound(tmp_R.Block, tmp_R.Block + tmp_R.size, node_) - tmp_R.Block;
    }

    bool insert_R(const Node &node_, int tag_) { // 插入数据块
        int location = FindRecordLocate(node_, tag_);
        if (node_ == tmp_R.Block[location]) return false; // 重复
        for (auto i = tmp_R.size - 1; i >= location; --i) {
            tmp_R.Block[i + 1] = tmp_R.Block[i];
        }
        tmp_R.Block[location] = node_;
        ++tmp_R.size;
        if (tmp_R.size < BlockSize) {
            writeRecord(tmp_R, tag_);
            return false;
        } else {
            return true;
        }
    }

    bool insert_leaf(const Node &node_, BPTreeNode *t) { // 插入m叉树的叶节点
        int location = FindTreeLocate(node_, t);
        if (insert_R(node_, t->son[location])) { // 数据块分裂
            Record tmp_R2;
            tmp_R2.tag = AssignRecordTag();
            tmp_R2.size = minSize;
            tmp_R2.next = tmp_R.next;
            tmp_R.next = tmp_R2.tag;

            for (auto i = minSize; i < BlockSize; ++i) {
                tmp_R2.Block[i - minSize] = tmp_R.Block[i];
                tmp_R.Block[i] = tmp_R2.Block[minSize];
            }
            tmp_R.size = minSize;
            writeRecord(tmp_R, tmp_R.tag);
            writeRecord(tmp_R2, tmp_R2.tag);

            for (int i = t->size - 2; i >= location; --i) {
                t->key[i + 1] = t->key[i];
                t->son[i + 2] = t->son[i + 1];
            }
            t->key[location] = tmp_R2.Block[0];
            t->son[location + 1] = tmp_R2.tag;
            ++t->size;
            if (t->size == M) {
                return true;
            } else {
                writeTree(*t, t->tag);
                return false;
            }
        }
        return false;
    }

    bool insert(const Node &node, BPTreeNode *t) {
        if (t->ifLeaf) {
            return insert_leaf(node, t);
        } else {
            int location = FindTreeLocate(node, t);
            ++ca.size;
            if (ca.order[ca.size - 1] != location) { // 判断是否读入缓存
                ca.order[ca.size - 1] = location;
                readTree(ca.link[ca.size - 1], t->son[location]);
                memset(ca.order + ca.size, -1, sizeof(int) * 90);
            }
            if (insert(node, &ca.link[ca.size - 1])) { // 节点分裂
                BPTreeNode tmp;
                tmp.tag = AssignTreeTag();
                tmp.size = M_min;
                tmp.ifLeaf = ca.link[ca.size - 1].ifLeaf;

                Node newKey = ca.link[ca.size - 1].key[M_min - 1];
                for (auto i = M_min; i < M; ++i) {
                    tmp.son[i - M_min] = ca.link[ca.size - 1].son[i];
                    ca.link[ca.size - 1].son[i] = tmp.son[M_min];
                }
                for (auto i = M_min; i < M - 1; ++i) {
                    tmp.key[i - M_min] = ca.link[ca.size - 1].key[i];
                    ca.link[ca.size - 1].key[i] = tmp.key[M_min];
                }
                ca.link[ca.size - 1].size = M_min;
                writeTree(tmp, tmp.tag);
                writeTree(ca.link[ca.size - 1], ca.link[ca.size - 1].tag);

                --ca.size;

                for (int i = t->size - 2; i >= location; --i) {
                    t->key[i + 1] = t->key[i];
                    t->son[i + 2] = t->son[i + 1];
                }
                t->key[location] = newKey;
                t->son[location + 1] = tmp.tag;
                ++t->size;
                t->ifLeaf = false;
                if (t->size == M) {
                    return true;
                } else {
                    writeTree(*t, t->tag);
                    return false;
                }
            } else {
                --ca.size;
                return false;
            }
        }
    }

    void insert(const index_type &index_, value_type value_) {
        Node node_(index_, value_);
        if (root->size == 0) { // 空树
            Record tmp_;
            tmp_.tag = AssignRecordTag();
            tmp_.size = 1;
            tmp_.Block[0] = node_;
            writeRecord(tmp_, tmp_.tag);

            root->tag = AssignTreeTag();
            root->size = 1;
            root->son[0] = tmp_.tag;

            ++ca.size;
            head_tag = root->son[0];
            return;
        } else {
            if (insert(node_, root)) { // 分裂根
                BPTreeNode tmp_;
                tmp_.tag = AssignTreeTag();
                tmp_.size = M_min;
                tmp_.ifLeaf = root->ifLeaf;

                Node newKey = root->key[M_min - 1];
                for (auto i = M_min; i < M; ++i) {
                    tmp_.son[i - M_min] = root->son[i];
                    root->son[i] = tmp_.son[M_min];
                }
                for (auto i = M_min; i < M - 1; ++i) {
                    tmp_.key[i - M_min] = root->key[i];
                    root->key[i] = tmp_.key[M_min];
                }
                root->key[M_min - 1] = tmp_.key[M_min];
                root->size = M_min;
                writeTree(tmp_, tmp_.tag);
                writeTree(ca.link[0], root->tag);

                BPTreeNode newRoot;
                newRoot.tag = AssignTreeTag();
                newRoot.size = 2;
                newRoot.son[0] = root->tag;
                newRoot.son[1] = tmp_.tag;
                newRoot.key[0] = newKey;
                newRoot.ifLeaf = false;
                ca.link[0] = newRoot;
                memset(ca.order, -1, sizeof(int) * 100);
            }
            ca.size = 1;
        }
    }

    bool erase_R(const Node &node_, int tag_) { // 从数据块中删除
        int location = FindRecordLocate(node_, tag_);
        if (node_ != tmp_R.Block[location]) return false;
        Node node;
        for (auto i = location; i < tmp_R.size - 1; ++i) {
            tmp_R.Block[i] = tmp_R.Block[i + 1];
        }
        tmp_R.Block[tmp_R.size - 1] = node;
        --tmp_R.size;
        if (tmp_R.size >= minSize) {
            writeRecord(tmp_R, tag_);
            return false;
        } else {
            return true;
        }
    }

    bool erase_leaf(const Node &node_, BPTreeNode *t) {  // 从m叉树节点处删除
        int location = FindTreeLocate(node_, t);
        if (erase_R(node_, t->son[location])) {
            if (location != t->size - 1) {
                Record tmp_R2;
                readRecord(tmp_R2, t->son[location + 1]);
                if (tmp_R2.size > minSize) { // 向右边借结点
                    tmp_R.Block[minSize - 1] = tmp_R2.Block[0];
                    for (int i = 1; i < tmp_R2.size; ++i) {
                        tmp_R2.Block[i - 1] = tmp_R2.Block[i];
                    }
                    tmp_R2.Block[tmp_R2.size - 1] = tmp_R.Block[minSize];
                    tmp_R.size = minSize;
                    --tmp_R2.size;
                    writeRecord(tmp_R, tmp_R.tag);
                    writeRecord(tmp_R2, tmp_R2.tag);
                    t->key[location] = tmp_R2.Block[0];
                    writeTree(*t, t->tag);
                    return false;
                } else { // 与右边合并
                    for (int i = minSize - 1; i < BlockSize - 1; ++i) {
                        tmp_R.Block[i] = tmp_R2.Block[i - minSize + 1];
                    }
                    tmp_R.size = BlockSize - 1;
                    tmp_R.next = tmp_R2.next;
                    RestoreRecordTag(tmp_R2.tag);
                    Node n_tmp;
                    Record tmp;
                    writeRecord(tmp_R, tmp_R.tag);
                    writeRecord(tmp, tmp_R2.tag);

                    for (int i = location + 2; i < t->size; ++i) {
                        t->key[i - 2] = t->key[i - 1];
                        t->son[i - 1] = t->son[i];
                    }
                    t->key[t->size - 2] = n_tmp;
                    t->son[t->size - 1] = -1;
                    --t->size;
                    if (t->size >= M_min || root->ifLeaf) {
                        writeTree(*t, t->tag);
                        return false;
                    } else return true;
                }
            } else {
                Record tmp_R2;
                readRecord(tmp_R2, t->son[location - 1]);
                if (tmp_R2.size > minSize) { // 向左边借结点
                    for (int i = minSize - 1; i > 0; --i) {
                        tmp_R.Block[i] = tmp_R.Block[i - 1];
                    }
                    tmp_R.Block[0] = tmp_R2.Block[tmp_R2.size - 1];
                    tmp_R2.Block[tmp_R2.size - 1] = tmp_R.Block[minSize];
                    tmp_R.size = minSize;
                    --tmp_R2.size;
                    writeRecord(tmp_R, tmp_R.tag);
                    writeRecord(tmp_R2, tmp_R2.tag);
                    t->key[location - 1] = tmp_R.Block[0];
                    writeTree(*t, t->tag);
                    return false;
                } else { // 与左边合并
                    for (int i = minSize; i < BlockSize - 1; ++i) {
                        tmp_R2.Block[i] = tmp_R.Block[i - minSize];
                    }
                    tmp_R2.size = BlockSize - 1;
                    tmp_R2.next = tmp_R.next;
                    RestoreRecordTag(tmp_R.tag);
                    Node n_tmp;
                    Record tmp;
                    writeRecord(tmp_R2, tmp_R2.tag);
                    writeRecord(tmp, tmp_R.tag);

                    for (int i = location + 1; i < t->size; ++i) {
                        t->key[i - 2] = t->key[i - 1];
                        t->son[i - 1] = t->son[i];
                    }
                    t->key[t->size - 2] = n_tmp;
                    t->son[t->size - 1] = -1;
                    --t->size;
                    if (t->size >= M_min || root->ifLeaf) {
                        writeTree(*t, t->tag);
                        return false;
                    } else return true;
                }
            }
        }
        return false;
    }

    bool erase(const Node &node, BPTreeNode *t) {
        if (t->ifLeaf) {
            return erase_leaf(node, t);
        } else {
            int location = FindTreeLocate(node, t);
            ++ca.size;
            if (ca.order[ca.size - 1] != location) {
                ca.order[ca.size - 1] = location;
                readTree(ca.link[ca.size - 1], t->son[location]);
                memset(ca.order + ca.size, -1, sizeof(int) * 90);
            }
            BPTreeNode *tmp_T = &ca.link[ca.size - 1];
            if (erase(node, tmp_T)) {
                Node n_tmp;
                if (location != t->size - 1) {
                    BPTreeNode tmp_T2;
                    readTree(tmp_T2, t->son[location + 1]);
                    if (tmp_T2.size > M_min) { // 向右边借结点
                        tmp_T->key[M_min - 2] = t->key[location];
                        t->key[location] = tmp_T2.key[0];
                        tmp_T->son[M_min - 1] = tmp_T2.son[0];
                        for (int i = 2; i < tmp_T2.size; ++i) {
                            tmp_T2.key[i - 2] = tmp_T2.key[i - 1];
                            tmp_T2.son[i - 2] = tmp_T2.son[i - 1];
                        }
                        tmp_T2.son[tmp_T2.size - 2] = tmp_T2.son[tmp_T2.size - 1];
                        tmp_T2.key[tmp_T2.size - 2] = n_tmp;
                        tmp_T2.son[tmp_T2.size - 1] = -1;
                        tmp_T->size = M_min;
                        --tmp_T2.size;
                        writeTree(ca.link[ca.size - 1], tmp_T->tag);
                        writeTree(tmp_T2, tmp_T2.tag);
                        writeTree(*t, t->tag);
                        --ca.size;
                        return false;
                    } else { // 与右边合并
                        tmp_T->key[M_min - 2] = t->key[location];
                        for (int i = M_min - 1; i < M - 2; ++i) {
                            tmp_T->key[i] = tmp_T2.key[i - M_min + 1];
                            tmp_T->son[i] = tmp_T2.son[i - M_min + 1];
                        }
                        tmp_T->son[M - 2] = tmp_T2.son[M_min - 1];
                        tmp_T->size = M - 1;
                        RestoreTreeTag(tmp_T2.tag);
                        BPTreeNode tmp;
                        writeTree(*tmp_T, tmp_T->tag);
                        writeTree(tmp, tmp_T2.tag);

                        for (int i = location + 2; i < t->size; ++i) {
                            t->key[i - 2] = t->key[i - 1];
                            t->son[i - 1] = t->son[i];
                        }
                        t->key[t->size - 2] = n_tmp;
                        t->son[t->size - 1] = -1;
                        --t->size;
                        --ca.size;
                        if (t->size >= M_min) {
                            writeTree(*t, t->tag);
                            return false;
                        } else return true;
                    }
                } else { // 向左边借结点
                    BPTreeNode tmp_T2;
                    readTree(tmp_T2, t->son[location - 1]);
                    if (tmp_T2.size > M_min) {
                        tmp_T->son[M_min - 1] = tmp_T->son[M_min - 2];
                        for (int i = M_min - 2; i > 0; --i) {
                            tmp_T->key[i] = tmp_T->key[i - 1];
                            tmp_T->son[i] = tmp_T->son[i - 1];
                        }
                        tmp_T->son[0] = tmp_T2.son[tmp_T2.size - 1];
                        tmp_T->key[0] = t->key[location - 1];
                        t->key[location - 1] = tmp_T2.key[tmp_T2.size - 2];
                        tmp_T2.key[tmp_T2.size - 2] = n_tmp;
                        tmp_T2.son[tmp_T2.size - 1] = -1;
                        tmp_T->size = M_min;
                        --tmp_T2.size;
                        writeTree(ca.link[ca.size - 1], tmp_T->tag);
                        writeTree(tmp_T2, tmp_T2.tag);
                        writeTree(*t, t->tag);
                        --ca.size;
                        return false;
                    } else { // 与左边合并
                        tmp_T2.key[M_min - 1] = t->key[location - 1];
                        for (int i = M_min; i < M - 2; ++i) {
                            tmp_T2.key[i] = tmp_T->key[i - M_min];
                            tmp_T2.son[i] = tmp_T->son[i - M_min];
                        }
                        tmp_T2.son[M - 2] = tmp_T->son[M_min - 2];
                        tmp_T2.size = M - 1;
                        RestoreTreeTag(tmp_T->tag);
                        BPTreeNode tmp;
                        writeTree(tmp_T2, tmp_T2.tag);
                        writeTree(tmp, tmp_T->tag);

                        for (int i = location + 1; i < t->size; ++i) {
                            t->key[i - 2] = t->key[i - 1];
                            t->son[i - 1] = t->son[i];
                        }
                        t->key[t->size - 2] = n_tmp;
                        t->son[t->size - 1] = -1;
                        --t->size;
                        ca.link[ca.size - 1] = tmp_T2;
                        ca.order[ca.size - 1] = location - 1;
                        --ca.size;
                        if (t->size >= M_min) {
                            writeTree(*t, t->tag);
                            return false;
                        } else return true;
                    }
                }
            } else {
                --ca.size;
                return false;
            }
        }
    }

    void erase(const index_type &index_, value_type value_) {
        Node node_(index_, value_);
        if (root->size == 0) return;
        if (root->size == 1) { // 只有一个数据块
            int location = FindRecordLocate(node_, root->son[0]);
            if (node_ != tmp_R.Block[location]) return;
            if (tmp_R.size == 1) {
                RestoreRecordTag(tmp_R.tag);
                Record tmp;
                writeRecord(tmp, tmp_R.tag);
                RestoreTreeTag(root->tag);
                BPTreeNode tmp_;
                ca.link[0] = tmp_;
            } else {
                Node node;
                for (auto i = location; i < tmp_R.size - 1; ++i) {
                    tmp_R.Block[i] = tmp_R.Block[i + 1];
                }
                tmp_R.Block[tmp_R.size - 1] = node;
                --tmp_R.size;
                writeRecord(tmp_R, tmp_R.tag);
            }
            return;
        } else if (root->size == 2 && !root->ifLeaf) {
            erase(node_, root);
            if (root->size == 1) { // 降深度
                int newRoot = root->son[0];
                RestoreTreeTag(root->tag);
                readTree(ca.link[0], newRoot);
            }
            ca.size = 1;
            memset(ca.order, -1, sizeof(int) * 100);
        } else {
            erase(node_, root);
            ca.size = 1;
        }
    }


    void find(const index_type &index_) {
        Node tmp(index_, -1);
        if (!root->size) {
            std::cout << "null\n";
            return;
        }
        int point;
        ca.size = 1;
        while (!ca.link[ca.size - 1].ifLeaf) {
            point = FindTreeLocate(tmp, &ca.link[ca.size - 1]);
            ++ca.size;
            ca.order[ca.size - 1] = point;
            readTree(ca.link[ca.size - 1], ca.link[ca.size - 2].son[point]);
        }
        point = FindTreeLocate(tmp, &ca.link[ca.size - 1]);
        point = ca.link[ca.size - 1].son[point];
        ca.size = 1;
        if (point != -1) {
            int location = FindRecordLocate(tmp, point);
            if (location == tmp_R.size && strcmp(index_, tmp_R.Block[location].index) != 0) {
                point = tmp_R.next;
                if (point == -1) {
                    std::cout << "null\n";
                    return;
                }
                readRecord(tmp_R, point);
                location = 0;
            }
            bool flag = true;
            if (strcmp(index_, tmp_R.Block[location].index) != 0) {
                std::cout << "null\n";
                return;
            } else {
                std::cout << tmp_R.Block[location].value << ' ';
                while (location < tmp_R.size) {
                    ++location;
                    if (location == tmp_R.size) break;
                    if (strcmp(index_, tmp_R.Block[location].index) == 0) {
                        std::cout << tmp_R.Block[location].value << ' ';
                    } else {
                        flag = false;
                        break;
                    }
                }
                while (flag and point != -1) {
                    point = tmp_R.next;
                    if (point != -1) {
                        readRecord(tmp_R, point);
                        location = -1;
                        while (location < tmp_R.size) {
                            ++location;
                            if (location == tmp_R.size) break;
                            if (strcmp(index_, tmp_R.Block[location].index) == 0) {
                                std::cout << tmp_R.Block[location].value << ' ';
                            } else {
                                flag = false;
                                break;
                            }
                        }
                    }
                }
            }
            std::cout << '\n';
            return;

        }
        std::cout << "null\n";
    }

};

int main() {
//    freopen("b.in", "r", stdin);
//    freopen("b.out", "w", stdout);
    std::ios::sync_with_stdio(false);
    BPT bpt("0tree", "0record", "0inf");
    int n;
    std::string instruct;
    char index[64];
    int value;
    std::cin >> n;
    while (n--) {
        memset(index, 0, sizeof(index));
        std::cin >> instruct >> index;

        if (instruct == "find") {
            bpt.find(index);
            continue;
        }
        std::cin >> value;
        if (instruct == "insert") {
            bpt.insert(index, value);
        } else if (instruct == "delete") {
            bpt.erase(index, value);
        }
    }
    return 0;
}