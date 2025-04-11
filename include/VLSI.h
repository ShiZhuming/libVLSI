#pragma once

#include <list>
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>
#include <string>
#include <tuple>

enum class NodeTypeEnum{
    node,
    terminal,
    terminal_nl
};

struct CircuitNode {
public:
    std::string name;
    double width;
    double height;
    double size;
    NodeTypeEnum node_type;
    std::set<std::string> net_names_unique; // Some nets contains multiple pins from the same node
    CircuitNode();
    CircuitNode(std::string name, double width, double height, NodeTypeEnum node_type = NodeTypeEnum::node);
};

struct NodePin {
public:
    std::string name;
    double delta_width;
    double delta_height;
    NodePin();
    NodePin(std::string name, double delta_width, double delta_height);
};

struct CircuitNet {
public:
    std::string name;
    std::vector<NodePin> node_pins; // all pins in this net
    std::set<std::string> node_names_unique; // Some nets contains multiple pins from the same node
    CircuitNet();
    CircuitNet(std::string name);
};

class Circuit {
private:
    std::unordered_map<std::string, CircuitNode> nodes; // node_name -> CircuitNode
    std::unordered_map<std::string, CircuitNet> nets; // net_name -> CircuitNet
public:
    Circuit();
    void load_nodes(std::string nodes_file_dir);
    void load_nets(std::string nets_file_dir);
    void load(std::string aux_file_dir, int dump_level = 0);
    std::tuple<Circuit, Circuit, std::vector<CircuitNet> > Fiduccia_Mattheyses_bipartition(int area_constraint, int max_unbalanced_nodes, int dump_level);
    // void Timber_Wolf_placement();
    void dump(int level = 0) const;
};