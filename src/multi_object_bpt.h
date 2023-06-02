#ifndef TICKETSYSTEM_SRC_MULTI_OBJECT_MultiBPT_H
#define TICKETSYSTEM_SRC_MULTI_OBJECT_MultiBPT_H

#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
#include "vector.h"
#include "function.h"




template<typename index_type, typename value_type,class CmpI = std::less<index_type>, class CmpV = std::less<value_type>>
class MultiBPT {
public:
    const int kBranch = 120; // M叉树
    const int kHalfBranch = 60;
    const int kBindBranch = 40;
    static const int kBlockSize = 320; // 数据块大小
    const int kHalfBlockSize = 160;
    const int kBindBlockSize = 106;
    sjtu::vector<int> storage_of_tree_tag_; // store unused tag of tree;
    sjtu::vector<int> storage_of_block_tag_; // store unused tag of storage;
public:
    // 最小数据单元
    struct Node {
        index_type index;
        value_type value=-1;

        Node() {};

        Node(const index_type &index_, const value_type &value_) : index(index_), value(value_) {}

        bool operator<(const Node &rhs) const {
            if (index != rhs.index) return index < rhs.index;
            else return value < rhs.value;
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
            return index == rhs.index && value == rhs.value;
        }

        bool operator!=(const Node &rhs) const {
            return !(*this == rhs);
        }
    };

public:
    // M叉树结点
    struct MultiBPTreeNode {
        int tag = -1;  // 记录序号
        int size = 0;
        bool ifLeaf = true;
        Node key[121]; // 关键词
        int son[121];

        MultiBPTreeNode() {
            memset(son, -1, sizeof(int) * 121);
        }

        MultiBPTreeNode(int tag_, int size_, bool ifLeaf_) {
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
        Node Block[kBlockSize];

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
        MultiBPTreeNode link[100];
        int order[100]; //记录分支序号

        cache() {
            memset(order, -1, sizeof(int) * 100);
        }
    } ca;

    MultiBPTreeNode *root = &ca.link[0];
    int head_tag = -1; // 记录第一个数据块

    int total_T = 0;
    int total_R = 0;

    std::fstream storage_of_tree_;
    std::string filename_of_tree_;
    std::fstream storage_of_record_;
    std::string filename_of_record_;
    std::fstream storage_of_inf_;
    std::string filename_of_inf_;

    void readTree(MultiBPTreeNode &read_, const int &location) {
        storage_of_tree_.seekg(sizeof(MultiBPTreeNode) * location);
        storage_of_tree_.read(reinterpret_cast<char *>(&read_), sizeof(MultiBPTreeNode));
    }

    void writeTree(MultiBPTreeNode &write_, const int &location) {
        storage_of_tree_.seekp(sizeof(MultiBPTreeNode) * location);
        storage_of_tree_.write(reinterpret_cast<char *>(&write_), sizeof(MultiBPTreeNode));
    }

    void readRecord(Record &read_, const int &location) {
        storage_of_record_.seekg(sizeof(Record) * location);
        storage_of_record_.read(reinterpret_cast<char *>(&read_), sizeof(Record));
    }

    void writeRecord(Record &write_, const int &location) {
        storage_of_record_.seekp(sizeof(Record) * location);
        storage_of_record_.write(reinterpret_cast<char *>(&write_), sizeof(Record));
    }

    MultiBPT() = default;

    MultiBPT(const std::string &fileName1, const std::string &fileName2, const std::string &fileName3) {
        filename_of_tree_ = fileName1;
        filename_of_record_ = fileName2;
        filename_of_inf_ = fileName3;
        storage_of_tree_.open(fileName1);
        if (!storage_of_tree_.good()) {
            storage_of_tree_.open(fileName1, std::fstream::out);
            storage_of_tree_.close();
            storage_of_tree_.open(fileName1);
        }
        storage_of_record_.open(fileName2);
        if (!storage_of_record_.good()) {
            storage_of_record_.open(fileName2, std::fstream::out);
            storage_of_record_.close();
            storage_of_record_.open(fileName2);
        }
        storage_of_inf_.open(fileName3);
        if (!storage_of_inf_.good()) {
            storage_of_inf_.open(fileName3, std::fstream::out);
            storage_of_inf_.close();
            storage_of_inf_.open(fileName3);
        } else {
            storage_of_inf_.seekg(0);
            storage_of_inf_.read(reinterpret_cast <char *> (&total_T), sizeof(total_T));
            storage_of_inf_.read(reinterpret_cast <char *> (&total_R), sizeof(total_R));
            storage_of_inf_.read(reinterpret_cast <char *> (&ca.link[0]), sizeof(MultiBPTreeNode));
            ca.size = 1;
        }
    }

    ~MultiBPT() {
        storage_of_inf_.seekp(0);
        storage_of_inf_.write(reinterpret_cast <const char *> (&total_T), sizeof(int));
        storage_of_inf_.write(reinterpret_cast <const char *> (&total_R), sizeof(int));
        storage_of_inf_.write(reinterpret_cast <const char *> (&ca.link[0]), sizeof(MultiBPTreeNode));
        writeTree(*root, root->tag);
        storage_of_tree_tag_.clear();
        storage_of_block_tag_.clear();
        storage_of_tree_.close();
        storage_of_record_.close();
        storage_of_inf_.close();
    }
private:
    int AssignTreeTag() {
        if (storage_of_tree_tag_.empty()) {
            return total_T++;
        } else {
            int tag_ = storage_of_tree_tag_.back();
            storage_of_tree_tag_.pop_back();
            return tag_;
        }
    }

    void RestoreTreeTag(int tag_) {
        storage_of_tree_tag_.push_back(tag_);
    }

    int AssignRecordTag() {
        if (storage_of_block_tag_.empty()) {
            return total_R++;
        } else {
            int tag_ = storage_of_block_tag_.back();
            storage_of_block_tag_.pop_back();
            return tag_;
        }
    }

    void RestoreRecordTag(int tag_) {
        storage_of_block_tag_.push_back(tag_);
    }

    Record tmp_R;

    int FindTreeLocate(const Node &node_, MultiBPTreeNode *Bnode_) {
        Node *tmp = sjtu::lower_bound(Bnode_->key, Bnode_->key + Bnode_->size - 1, node_);
        if (*tmp == node_) return tmp - Bnode_->key + 1;
        else return tmp - Bnode_->key;
    }

    int FindRecordLocate(const Node &node_, int tag_) {
        readRecord(tmp_R, tag_);
        return sjtu::lower_bound(tmp_R.Block, tmp_R.Block + tmp_R.size, node_) - tmp_R.Block;
    }
private:
    bool insert_R(const Node &node_, int tag_) { // 插入数据块
        int location = FindRecordLocate(node_, tag_);
        if (node_ == tmp_R.Block[location]) return false; // 重复
        for (auto i = tmp_R.size - 1; i >= location; --i) {
            tmp_R.Block[i + 1] = tmp_R.Block[i];
        }
        tmp_R.Block[location] = node_;
        ++tmp_R.size;
        if (tmp_R.size < kBlockSize) {
            writeRecord(tmp_R, tag_);
            return false;
        } else {
            return true;
        }
    }

    bool insert_leaf(const Node &node_, MultiBPTreeNode *t) { // 插入m叉树的叶节点
        int location = FindTreeLocate(node_, t);
        if (insert_R(node_, t->son[location])) { // 数据块分裂
            Record tmp_R2;
            tmp_R2.tag = AssignRecordTag();
            tmp_R2.size = kHalfBlockSize;
            tmp_R2.next = tmp_R.next;
            tmp_R.next = tmp_R2.tag;

            for (auto i = kHalfBlockSize; i < kBlockSize; ++i) {
                tmp_R2.Block[i - kHalfBlockSize] = tmp_R.Block[i];
                tmp_R.Block[i] = tmp_R2.Block[kHalfBlockSize];
            }
            tmp_R.size = kHalfBlockSize;
            writeRecord(tmp_R, tmp_R.tag);
            writeRecord(tmp_R2, tmp_R2.tag);

            for (int i = t->size - 2; i >= location; --i) {
                t->key[i + 1] = t->key[i];
                t->son[i + 2] = t->son[i + 1];
            }
            t->key[location] = tmp_R2.Block[0];
            t->son[location + 1] = tmp_R2.tag;
            ++t->size;
            if (t->size == kBranch) {
                return true;
            } else {
                writeTree(*t, t->tag);
                return false;
            }
        }
        return false;
    }

    bool insert(const Node &node, MultiBPTreeNode *t) {
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
                MultiBPTreeNode tmp;
                tmp.tag = AssignTreeTag();
                tmp.size = kHalfBranch;
                tmp.ifLeaf = ca.link[ca.size - 1].ifLeaf;

                Node newKey = ca.link[ca.size - 1].key[kHalfBranch - 1];
                for (auto i = kHalfBranch; i < kBranch; ++i) {
                    tmp.son[i - kHalfBranch] = ca.link[ca.size - 1].son[i];
                    ca.link[ca.size - 1].son[i] = tmp.son[kHalfBranch];
                }
                for (auto i = kHalfBranch; i < kBranch - 1; ++i) {
                    tmp.key[i - kHalfBranch] = ca.link[ca.size - 1].key[i];
                    ca.link[ca.size - 1].key[i] = tmp.key[kHalfBranch];
                }
                ca.link[ca.size - 1].size = kHalfBranch;
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
                if (t->size == kBranch) {
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
public:
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
                MultiBPTreeNode tmp_;
                tmp_.tag = AssignTreeTag();
                tmp_.size = kHalfBranch;
                tmp_.ifLeaf = root->ifLeaf;

                Node newKey = root->key[kHalfBranch - 1];
                for (auto i = kHalfBranch; i < kBranch; ++i) {
                    tmp_.son[i - kHalfBranch] = root->son[i];
                    root->son[i] = tmp_.son[kHalfBranch];
                }
                for (auto i = kHalfBranch; i < kBranch - 1; ++i) {
                    tmp_.key[i - kHalfBranch] = root->key[i];
                    root->key[i] = tmp_.key[kHalfBranch];
                }
                root->key[kHalfBranch - 1] = tmp_.key[kHalfBranch];
                root->size = kHalfBranch;
                writeTree(tmp_, tmp_.tag);
                writeTree(ca.link[0], root->tag);

                MultiBPTreeNode newRoot;
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
private:
    bool erase_R(const Node &node_, int tag_) { // 从数据块中删除
        int location = FindRecordLocate(node_, tag_);
        if (node_ != tmp_R.Block[location]) return false;
        Node node;
        for (auto i = location; i < tmp_R.size - 1; ++i) {
            tmp_R.Block[i] = tmp_R.Block[i + 1];
        }
        tmp_R.Block[tmp_R.size - 1] = node;
        --tmp_R.size;
        if (tmp_R.size >= kBindBlockSize) {
            writeRecord(tmp_R, tag_);
            return false;
        } else {
            return true;
        }
    }

    bool erase_leaf(const Node &node_, MultiBPTreeNode *t) {  // 从m叉树节点处删除
        int location = FindTreeLocate(node_, t);
        if (erase_R(node_, t->son[location])) {
            if (location != t->size - 1) {
                Record tmp_R2;
                readRecord(tmp_R2, t->son[location + 1]);
                if (tmp_R2.size > kBindBlockSize) { // 向右边借结点
                    tmp_R.Block[kBindBlockSize - 1] = tmp_R2.Block[0];
                    for (int i = 1; i < tmp_R2.size; ++i) {
                        tmp_R2.Block[i - 1] = tmp_R2.Block[i];
                    }
                    tmp_R2.Block[tmp_R2.size - 1] = tmp_R.Block[kBindBlockSize];
                    tmp_R.size = kBindBlockSize;
                    --tmp_R2.size;
                    writeRecord(tmp_R, tmp_R.tag);
                    writeRecord(tmp_R2, tmp_R2.tag);
                    t->key[location] = tmp_R2.Block[0];
                    writeTree(*t, t->tag);
                    return false;
                } else { // 与右边合并
                    for (int i = kBindBlockSize - 1; i < 2 * kBindBlockSize - 1; ++i) {
                        tmp_R.Block[i] = tmp_R2.Block[i - kBindBlockSize + 1];
                    }
                    tmp_R.size = 2 * kBindBlockSize - 1;
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
                    if (t->size >= kBindBranch || root->ifLeaf) {
                        writeTree(*t, t->tag);
                        return false;
                    } else return true;
                }
            } else {
                Record tmp_R2;
                readRecord(tmp_R2, t->son[location - 1]);
                if (tmp_R2.size > kBindBlockSize) { // 向左边借结点
                    for (int i = kBindBlockSize - 1; i > 0; --i) {
                        tmp_R.Block[i] = tmp_R.Block[i - 1];
                    }
                    tmp_R.Block[0] = tmp_R2.Block[tmp_R2.size - 1];
                    tmp_R2.Block[tmp_R2.size - 1] = tmp_R.Block[kBindBlockSize];
                    tmp_R.size = kBindBlockSize;
                    --tmp_R2.size;
                    writeRecord(tmp_R, tmp_R.tag);
                    writeRecord(tmp_R2, tmp_R2.tag);
                    t->key[location - 1] = tmp_R.Block[0];
                    writeTree(*t, t->tag);
                    return false;
                } else { // 与左边合并
                    for (int i = kBindBlockSize; i < 2 * kBindBlockSize - 1; ++i) {
                        tmp_R2.Block[i] = tmp_R.Block[i - kBindBlockSize];
                    }
                    tmp_R2.size = 2 * kBindBlockSize - 1;
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
                    if (t->size >= kBindBranch || root->ifLeaf) {
                        writeTree(*t, t->tag);
                        return false;
                    } else return true;
                }
            }
        }
        return false;
    }

    bool erase(const Node &node, MultiBPTreeNode *t) {
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
            MultiBPTreeNode *tmp_T = &ca.link[ca.size - 1];
            if (erase(node, tmp_T)) {
                Node n_tmp;
                if (location != t->size - 1) {
                    MultiBPTreeNode tmp_T2;
                    readTree(tmp_T2, t->son[location + 1]);
                    if (tmp_T2.size > kBindBranch) { // 向右边借结点
                        tmp_T->key[kBindBranch - 2] = t->key[location];
                        t->key[location] = tmp_T2.key[0];
                        tmp_T->son[kBindBranch - 1] = tmp_T2.son[0];
                        for (int i = 2; i < tmp_T2.size; ++i) {
                            tmp_T2.key[i - 2] = tmp_T2.key[i - 1];
                            tmp_T2.son[i - 2] = tmp_T2.son[i - 1];
                        }
                        tmp_T2.son[tmp_T2.size - 2] = tmp_T2.son[tmp_T2.size - 1];
                        tmp_T2.key[tmp_T2.size - 2] = n_tmp;
                        tmp_T2.son[tmp_T2.size - 1] = -1;
                        tmp_T->size = kBindBranch;
                        --tmp_T2.size;
                        writeTree(ca.link[ca.size - 1], tmp_T->tag);
                        writeTree(tmp_T2, tmp_T2.tag);
                        writeTree(*t, t->tag);
                        --ca.size;
                        return false;
                    } else { // 与右边合并
                        tmp_T->key[kBindBranch - 2] = t->key[location];
                        for (int i = kBindBranch - 1; i < 2 * kBindBranch - 2; ++i) {
                            tmp_T->key[i] = tmp_T2.key[i - kBindBranch + 1];
                            tmp_T->son[i] = tmp_T2.son[i - kBindBranch + 1];
                        }
                        tmp_T->son[2 * kBindBranch - 2] = tmp_T2.son[kBindBranch - 1];
                        tmp_T->size = 2 * kBindBranch - 1;
                        RestoreTreeTag(tmp_T2.tag);
                        MultiBPTreeNode tmp;
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
                        if (t->size >= kBindBranch) {
                            writeTree(*t, t->tag);
                            return false;
                        } else return true;
                    }
                } else { // 向左边借结点
                    MultiBPTreeNode tmp_T2;
                    readTree(tmp_T2, t->son[location - 1]);
                    if (tmp_T2.size > kBindBranch) {
                        tmp_T->son[kBindBranch - 1] = tmp_T->son[kBindBranch - 2];
                        for (int i = kBindBranch - 2; i > 0; --i) {
                            tmp_T->key[i] = tmp_T->key[i - 1];
                            tmp_T->son[i] = tmp_T->son[i - 1];
                        }
                        tmp_T->son[0] = tmp_T2.son[tmp_T2.size - 1];
                        tmp_T->key[0] = t->key[location - 1];
                        t->key[location - 1] = tmp_T2.key[tmp_T2.size - 2];
                        tmp_T2.key[tmp_T2.size - 2] = n_tmp;
                        tmp_T2.son[tmp_T2.size - 1] = -1;
                        tmp_T->size = kBindBranch;
                        --tmp_T2.size;
                        writeTree(ca.link[ca.size - 1], tmp_T->tag);
                        writeTree(tmp_T2, tmp_T2.tag);
                        writeTree(*t, t->tag);
                        --ca.size;
                        return false;
                    } else { // 与左边合并
                        tmp_T2.key[kBindBranch - 1] = t->key[location - 1];
                        for (int i = kBindBranch; i < 2 * kBindBranch - 2; ++i) {
                            tmp_T2.key[i] = tmp_T->key[i - kBindBranch];
                            tmp_T2.son[i] = tmp_T->son[i - kBindBranch];
                        }
                        tmp_T2.son[2 * kBindBranch - 2] = tmp_T->son[kBindBranch - 2];
                        tmp_T2.size = 2 * kBindBranch - 1;
                        RestoreTreeTag(tmp_T->tag);
                        MultiBPTreeNode tmp;
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
                        if (t->size >= kBindBranch) {
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
public:
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
                MultiBPTreeNode tmp_;
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
    void find(const index_type &index_,sjtu::vector<value_type> &indexes) {
        Node tmp(index_, -1);
        if (!root->size) {
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
            if (location == tmp_R.size && index_!= tmp_R.Block[location].index) {
                point = tmp_R.next;
                if (point == -1) {
                    return ;
                }
                readRecord(tmp_R, point);
                location = 0;
            }
            bool flag = true;
            if (index_!= tmp_R.Block[location].index) {
                return ;
            } else {
                indexes.push_back(tmp_R.Block[location].value);
                while (location < tmp_R.size) {
                    ++location;
                    if (location == tmp_R.size) break;
                    if (index_== tmp_R.Block[location].index) {
                        indexes.push_back(tmp_R.Block[location].value);
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
                            if (index_== tmp_R.Block[location].index) {
                                indexes.push_back( tmp_R.Block[location].value);
                            } else {
                                flag = false;
                                break;
                            }
                        }
                    }
                }
            }
            return;
        }
    }

    void find_o(const index_type &index_,sjtu::vector<value_type> &indexes,sjtu::vector<index_type> &record) {
        Node tmp(index_, -1);
        if (!root->size) {
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
            if (location == tmp_R.size && index_!= tmp_R.Block[location].index) {
                point = tmp_R.next;
                if (point == -1) {
                    return ;
                }
                readRecord(tmp_R, point);
                location = 0;
            }
            bool flag = true;
            if (index_!= tmp_R.Block[location].index) {
                return ;
            } else {
                indexes.push_back(tmp_R.Block[location].value);
                record.push_back(tmp_R.Block[location].index);
                while (location < tmp_R.size) {
                    ++location;
                    if (location == tmp_R.size) break;
                    if (index_== tmp_R.Block[location].index) {
                        indexes.push_back(tmp_R.Block[location].value);
                        record.push_back(tmp_R.Block[location].index);
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
                            if (index_== tmp_R.Block[location].index) {
                                indexes.push_back( tmp_R.Block[location].value);
                                record.push_back(tmp_R.Block[location].index);
                            } else {
                                flag = false;
                                break;
                            }
                        }
                    }
                }
            }
            return;
        }
    }


    void clear(){
        storage_of_tree_tag_.clear();
        storage_of_block_tag_.clear();
        ca.size=0;
        MultiBPTreeNode tmp_;
        ca.link[0] = tmp_;
        head_tag = -1; // 记录第一个数据块

        total_T = 0;
        total_R = 0;
    }
};
#endif //TICKETSYSTEM_SRC_MULTI_OBJECT_MultiBPT_H
