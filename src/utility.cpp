#include "../include/utility.h"

BucketNode::BucketNode() : name(""), gain(0), size(0) {}

BucketNode::BucketNode(std::string name, int gain, double size) : name(name), gain(gain), size(size) {}

std::list<BucketNode>::iterator Bucket::_insert_to_bucket(BucketNode node) {
    // check if the gain exists
    if (gain_buckets.find(node.gain) == gain_buckets.end()) {
        // if not, create a new list
        gain_buckets[node.gain] = std::list<BucketNode>();
    }
    // push the node to the list back
    gain_buckets[node.gain].push_back(node);
    // update max_gain
    if (node.gain > max_gain) {
        max_gain = node.gain;
    }
    // return the iterator of the node
    auto it = --gain_buckets[node.gain].end();
    return it;
}

void Bucket::_erase_from_bucket(std::list<BucketNode>::iterator iNode) {
    // get the gain of the node
    int gain = iNode->gain;
    // erase the node from the list
    gain_buckets[gain].erase(iNode);
    // if the list is empty, erase it from the map
    if (gain_buckets[gain].empty()) {
        gain_buckets.erase(gain);
        // update max_gain
        if (gain == max_gain) {
            max_gain = -max_possible_gain;
            for (auto it = gain_buckets.begin(); it != gain_buckets.end(); ++it) {
                if (it->first > max_gain) {
                    max_gain = it->first;
                }
            }
        }
    }
}

Bucket::Bucket(int max_possible_gain) : max_possible_gain(max_possible_gain) {
    max_gain = -max_possible_gain;
    gain_buckets = std::unordered_map<int, std::list<BucketNode> >();
    cell_map = std::unordered_map<std::string, std::list<BucketNode>::iterator>();
}

void Bucket::insert(BucketNode node) {
    cell_map[node.name] = _insert_to_bucket(node);
}

void Bucket::insert(std::string name, int gain, double size) {
    cell_map[name] = _insert_to_bucket(BucketNode(name, gain, size));
}

void Bucket::erase(std::string name) {
    auto it = cell_map[name];
    _erase_from_bucket(it);
    cell_map.erase(name);
}

void Bucket::update_gain(std::string name, int new_gain) {
    // get the iterator of the node
    auto it = cell_map[name];
    auto new_node = *it;
    new_node.gain = new_gain;
    // erase the node from the bucket
    _erase_from_bucket(it);
    // insert the new node to the bucket and update the cell_map
    cell_map[name] = _insert_to_bucket(new_node);
}

void Bucket::increase_gain(std::string name) {
    auto it = cell_map[name];
    auto new_node = *it;
    new_node.gain++;
    // erase the node from the bucket
    _erase_from_bucket(it);
    // insert the new node to the bucket and update the cell_map
    cell_map[name] = _insert_to_bucket(new_node);
}

void Bucket::decrease_gain(std::string name) {
    auto it = cell_map[name];
    auto new_node = *it;
    new_node.gain--;
    // erase the node from the bucket
    _erase_from_bucket(it);
    // insert the new node to the bucket and update the cell_map
    cell_map[name] = _insert_to_bucket(new_node);
}

bool Bucket::is_empty() const {
    return gain_buckets.empty();
}

BucketNode Bucket::get_max_gain_node() {
    if (is_empty() || gain_buckets[max_gain].empty()) {
        std::cerr << "Error: Bucket is empty!" << std::endl;
        exit(1);
    }
    return gain_buckets[max_gain].back();
}

void Bucket::dump() const {
    std::cout << "Bucket dump:" << std::endl;
    for (const auto& pair : gain_buckets) {
        std::cout << "Gain: " << pair.first << " -> ";
        for (const auto& node : pair.second) {
            std::cout << "(" << node.name << ", " << node.gain << ", " << node.size << ") ";
        }
        std::cout << std::endl;
    }
}