#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>

struct BucketNode {
public:
    std::string name;
    int gain;
    double size;
    BucketNode();
    BucketNode(std::string name, int gain, double size);
};

class Bucket {
private:
    // gain_buckets: gain -> [Node1 <=> Node2 <=> ...]
    std::unordered_map<int, std::list<BucketNode> > gain_buckets;

    // cell_map: id -> iterator of Node in gain_buckets
    std::unordered_map<std::string, std::list<BucketNode>::iterator> cell_map;

    // max_gain: maximum gain in current bucket
    int max_gain;

    // pmax: maximum possible gain
    const int max_possible_gain;

    // tool functions
    std::list<BucketNode>::iterator _insert_to_bucket(BucketNode node);
    void _erase_from_bucket(std::list<BucketNode>::iterator iNode);
public:
    Bucket(int maximum_possible_gain);
    void insert(BucketNode node);
    void insert(std::string name, int gain, double size);
    void erase(std::string name);
    void update_gain(std::string name, int new_gain);
    void increase_gain(std::string name);
    void decrease_gain(std::string name);
    bool is_empty() const;
    // ## get a copy of max gain node object
    // ### output: BucketNode struct:
    // - std::string name;
    // - int gain;
    // - double size;
    BucketNode get_max_gain_node();
    void dump() const;
};