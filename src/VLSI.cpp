#include <cstdlib>
#include <fstream>
#include <sstream>
#include <chrono>

#include "../include/VLSI.h"
#include "../include/utility.h"

CircuitNode::CircuitNode() : name(""), width(0), height(0), size(0), node_type(NodeTypeEnum::node) {
    net_names_unique = std::set<std::string>();
}

CircuitNode::CircuitNode(std::string name, double width, double height, NodeTypeEnum node_type) : name(name), width(width), height(height), node_type(node_type) {
    size = width * height;
    net_names_unique = std::set<std::string>();
}

NodePin::NodePin() : name(""), delta_width(0), delta_height(0) {}

NodePin::NodePin(std::string name, double delta_width, double delta_height) : name(name), delta_width(delta_width), delta_height(delta_height) {}

CircuitNet::CircuitNet() : name("") {
    node_pins = std::vector<NodePin>();
    node_names_unique = std::set<std::string>();
}

CircuitNet::CircuitNet(std::string name) : name(name) {
    node_pins = std::vector<NodePin>();
    node_names_unique = std::set<std::string>();
}

Circuit::Circuit() {
    nodes = std::unordered_map<std::string, CircuitNode>();
    nets = std::unordered_map<std::string, CircuitNet>();
}

void Circuit::load_nodes(std::string nodes_file_dir) {
    std::ifstream nodes_file(nodes_file_dir);
    if (!nodes_file.is_open()) {
        std::cerr << "Error: cannot open file " << nodes_file_dir << std::endl;
        exit(1);
    }
    std::string line;
    // ignore first 4 lines
    for (int i = 0; i < 4; i++) {
        std::getline(nodes_file, line);
    }
    // read line "NumNodes      :  847441", read the num_vertices
    int num_vertices;
    int num_terminals;
    nodes_file >> line >> line >> num_vertices;
    // read line "NumTerminals  :  82339", read the num_terminals
    std::getline(nodes_file, line);
    nodes_file >> line >> line >> num_terminals;
    // ignore an empty line
    std::getline(nodes_file, line);
    // read (num_vertices - mum_terminals) lines, each line is              o0             5             9, push 'o0' into vertex_names, push 5*9 into vertex_sizes
    for (int i = 0; i < num_vertices - num_terminals; i++) {
        std::string vertex_name;
        double width;
        double height;
        nodes_file >> vertex_name >> width >> height;
        CircuitNode node(vertex_name, width, height);
        nodes[vertex_name] = node;
    }
    // read num_terminals lines, each line is         o765102            10             9    terminal, push 'o765102' into vertex_names, push 10*9 into vertex_sizes
    for (int i = 0; i < num_terminals; i++) {
        std::string vertex_name;
        double width;
        double height;
        nodes_file >> vertex_name >> width >> height >> line;
        CircuitNode node(vertex_name, width, height, NodeTypeEnum::terminal);
        nodes[vertex_name] = node;
    }
}

void Circuit::dump(int level) const {
    if (level == 0) {
        std::cout << "Circuit briefly dump:" << std::endl;
        std::cout << "Nodes:" << std::endl;
        // dump first and last 5 nodes
        int count = 0;
        for (const auto& pair : nodes) {
            const CircuitNode& node = pair.second;
            if (count < 5 || count >= nodes.size() - 5) {
                std::cout << "Name: " << node.name << ", Width: " << node.width << ", Height: " << node.height << ", Size: " << node.size << ", Type: " << (node.node_type==NodeTypeEnum::terminal?"Terminal":"Node") << std::endl;
            }
            count++;
        }
        std::cout << "Nets:" << std::endl;
        // dump first and last 5 nets
        count = 0;
        for (const auto& pair : nets) {
            const CircuitNet& net = pair.second;
            if (count < 5 || count >= nets.size() - 5) {
                std::cout << "Name: " << net.name << ", Pins: ";
                for (const auto& pin : net.node_pins) {
                    std::cout << pin.name << " ";
                }
                std::cout << std::endl;
            }
            count++;
        }
        std::cout << "Total nodes: " << nodes.size() << ", Total nets: " << nets.size() << std::endl;
    } 
    else if (level == 1) {

    std::cout << "Circuit dump:" << std::endl;
        std::cout << "Nodes:" << std::endl;
        for (const auto& pair : nodes) {
            const CircuitNode& node = pair.second;
            std::cout << "Name: " << node.name << ", Width: " << node.width << ", Height: " << node.height << ", Size: " << node.size << ", Type: " << (node.node_type==NodeTypeEnum::terminal?"Terminal":"Node") << std::endl;
        }
        std::cout << "Nets:" << std::endl;
        for (const auto& pair : nets) {
            const CircuitNet& net = pair.second;
            std::cout << "Name: " << net.name << ", Pins: ";
            for (const auto& pin : net.node_pins) {
                std::cout << pin.name << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Total nodes: " << nodes.size() << ", Total nets: " << nets.size() << std::endl;
    }
}

void Circuit::load_nets(std::string nets_file_dir) {
    std::ifstream nets_file(nets_file_dir);
    if (!nets_file.is_open()) {
        std::cerr << "Error: cannot open file " << nets_file_dir << std::endl;
        exit(1);
    }
    std::string line;
    // ignore first 4 lines
    for (int i = 0; i < 4; i++) {
        std::getline(nets_file, line);
    }
    // read line "NumNets  :  822744", read the num_hyperedges
    int num_nets;
    int num_pins;
    nets_file >> line >> line >> num_nets;
    // read line "NumPins  :  2861188", read the num_pins
    std::getline(nets_file, line);
    nets_file >> line >> line >> num_pins;
    // ignore an empty line
    std::getline(nets_file, line);
    // read num_hyperedges units, for each unit, its like
    // NetDegree  :  9    n0
    //     o816034   I  :      0.0000     -1.5000
    //      p17299   I  :      0.0000      0.0000
    //      p17298   I  :      0.0000      0.0000
    //      p17297   I  :      0.0000      0.0000
    //      p17296   I  :      0.0000      0.0000
    //          p3   O  :      0.0000      0.0000
    //          p2   O  :      0.0000      0.0000
    //          p1   O  :      0.0000      0.0000
    //          p0   O  :      0.0000      0.0000
    for (int i = 0; i < num_nets; i++) {
        std::string net_name;
        int degree;
        nets_file >> line >> line >> degree >> net_name;
        CircuitNet net(net_name);
        float delta_width, delta_height;
        for (int j = 0; j < degree; j++) {
            std::string node_name;
            nets_file >> node_name >> line >> line >> delta_width >> delta_height;
            net.node_pins.push_back(NodePin(node_name, delta_width, delta_height));
            net.node_names_unique.insert(node_name);
            // push this net to its corresponding node in nodes
            nodes[node_name].net_names_unique.insert(net_name);
        }
        nets[net_name] = net;
    }
    // if still exists some more content, error
    if (std::getline(nets_file, line) && line.size() > 0) {
        std::cout << "Error: still exists some more content in nets_file\n";
        exit(1);
    }
}
    
void Circuit::load(std::string aux_file_dir, int dump_level) {
    std::ifstream aux_file(aux_file_dir);
    if (!aux_file.is_open()) {
        std::cerr << "Error: cannot open file " << aux_file_dir << std::endl;
        exit(1);
    }
    std::string nodes_file_dir = aux_file_dir.substr(0, aux_file_dir.find_last_of('.')) + ".nodes";
    std::string nets_file_dir = aux_file_dir.substr(0, aux_file_dir.find_last_of('.')) + ".nets";
    auto start = std::chrono::high_resolution_clock::now(); // track time
    load_nodes(nodes_file_dir);
    auto end = std::chrono::high_resolution_clock::now();
    if (dump_level == 0) {
        std::cout << "Time to load nodes: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }
    start = std::chrono::high_resolution_clock::now();
    load_nets(nets_file_dir);
    end = std::chrono::high_resolution_clock::now();
    if (dump_level == 0) {
        std::cout << "Time to load nets: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }
}

// ## Fiduccia-Mattheyses bipartition
// ### input:
//      - area_constraint: area constraint for each partition, can be 0: the number of nodes, or 1: the total area of the partition
//      - max_unbalanced_nodes: maximum number of nodes/sum of area that can be more/less than half of the total
//      - dump_level: dump level, 0 for brief dump, 1 for full dump
// ### output:
//      - partition1: Circuit class, the circuit object for first partition
//      - partition2: Circuit class, the circuit object for second partition
//      - Cut: the CircuitNet objects that are on the cut
std::tuple<Circuit, Circuit, std::vector<CircuitNet> > Circuit::Fiduccia_Mattheyses_bipartition(int area_constraint = 1, int max_unbalanced_nodes = 500, int dump_level = 0) {
    int num_nodes = nodes.size();
    int num_nets = nets.size();
    double max_node_size = 0;
    Bucket buckets[2] = {Bucket(num_nets+1), Bucket(num_nets+1)};
    double partition_size[2] = {0, 0};
    double balance_size;
    // current cut size
    int cut_size = 0;
    // current partition: name of node -> partition 0 or 1
    auto partition = std::unordered_map<std::string, int>();
    // min cut size, initialized as the cut size of the initial circuit
    int min_cut_size;
    // min cut partition, initialized as the cut size of the initial circuit, update when a better partition is found
    std::unordered_map<std::string, int> min_cut_partition;
    // record the changes of the partitions after last update min cut, includes the change of current iteration, excludes the change excuted during the last update
    auto pending_moves = std::unordered_map<std::string, int>();
    // each edge has a counter for the number of pins in each partition, name of net -> vector(num_pins_in_partition_0, num_pins_in_partition_1)
    auto num_of_nodes_in_partition = std::unordered_map<std::string, std::vector<int> >();
    // record if a node is locked
    auto node_locked = std::unordered_map<std::string, bool>();

    auto start = std::chrono::high_resolution_clock::now(); // track time
    
    // ramdomly assign nodes to partition 0 or 1, and initialize partition size and the max node size
    std::srand(13);
    for (const auto& pair : nodes) {
        const CircuitNode& node = pair.second;
        // if (node.node_type == NodeTypeEnum::terminal) {
        //     continue;
        // }
        int partition_id = rand() % 2;
        // initialize the partition
        partition[node.name] = partition_id;
        // initialize the max node size
        max_node_size = max_node_size > node.size ? max_node_size : node.size;
        // initialize the partition size
        partition_size[partition_id] += node.size;
        // initialize the node locked status
        node_locked[node.name] = false;
    }
    // initialize num_of_nodes_in_partition and cut_size, count the number of nodes in each partition
    for (const auto& pair : nets) {
        const CircuitNet& net = pair.second;
        num_of_nodes_in_partition[net.name] = std::vector<int>(2, 0);
        // go throuth all the nodes involved in this net
        for (const auto& node_name : net.node_names_unique) {
            num_of_nodes_in_partition[net.name][partition[node_name]]++;
        }
        // if this net has nodes on both sides, it is a cut
        if (num_of_nodes_in_partition[net.name][0] > 0 && num_of_nodes_in_partition[net.name][1] > 0) {
            cut_size++;
        }
    }
    
    // initialize the balance size, check balance condition for initial partition
    balance_size = (partition_size[0] + partition_size[1]) / 2;
    if (partition_size[0] > balance_size + max_unbalanced_nodes * max_node_size || partition_size[0] < balance_size - max_unbalanced_nodes * max_node_size) {
        std::cerr << "Error: the initial partition is unbalanced, please check the input parameters\n";
        std::cerr << "Partition size: " << partition_size[0] << " , " << partition_size[1] << ", balance size: " << balance_size << ", max node size: " << max_node_size << std::endl;
        exit(1);
    }

    // initialize the min_cut_size and min_cut_partition
    min_cut_size = cut_size;
    min_cut_partition = partition;
    // initialize the pending_moves
    pending_moves.clear();

    // calculate the initial gain for each node, insert them into the buckets
    for (const auto& pair : nodes) {
        const CircuitNode& node = pair.second;
        // if (node.node_type == NodeTypeEnum::terminal) {
        //     continue;
        // }
        int gain = 0;
        // go through all the nets this node is involved in
        for (const auto& net_name : node.net_names_unique) {
            const CircuitNet& net = nets[net_name];
            // if this net has only one node in the node's partition, increase gain
            if (num_of_nodes_in_partition[net_name][partition[node.name]] == 1) {
                gain++;
            }
            // if this net has nothing on the other side, decrease gain
            if (num_of_nodes_in_partition[net_name][1 - partition[node.name]] == 0) {
                gain--;
            }
        }
        buckets[partition[node.name]].insert(node.name, gain, node.size);
    }

    auto end = std::chrono::high_resolution_clock::now();
    if (dump_level == 0) {
        std::cout << "Time to initialize in FM algo: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }

    auto start_main_loop = std::chrono::high_resolution_clock::now();

    // main loop, total iterations are the number of nodes
    for (int i = 0; i < num_nodes; i++) {
        start = std::chrono::high_resolution_clock::now();

        bool eligible_for_move[2] = {false, false};
        // find the max gain nodes of each part
        for (int part = 0; part < 2; part++) {
            // node exists in the bucket
            eligible_for_move[part] = !buckets[part].is_empty() 
            // remove the max gain node will not cause the partition too small
            && partition_size[part] - buckets[part].get_max_gain_node().size >= balance_size - max_unbalanced_nodes * max_node_size
            // add the max gain node will not cause the partition too large
            && partition_size[1-part] + buckets[part].get_max_gain_node().size <= balance_size + max_unbalanced_nodes * max_node_size;
        }
        int from_part;
        // if neither eligible for move, finished, break
        if (!eligible_for_move[0] && !eligible_for_move[1]) {
            break;
        }
        // if only one part is eligible for move, move from this part
        else if (eligible_for_move[0] && !eligible_for_move[1]) {
            from_part = 0;
        }
        else if (!eligible_for_move[0] && eligible_for_move[1]) {
            from_part = 1;
        }
        // if both parts are eligible for move, choose the one with the max gain node
        else {
            BucketNode node1 = buckets[0].get_max_gain_node();
            BucketNode node2 = buckets[1].get_max_gain_node();
            if (node1.gain >= node2.gain) {
                from_part = 0;
            }
            else {
                from_part = 1;
            }
        }
        int to_part = 1 - from_part;
        auto bucket_node_to_move = buckets[from_part].get_max_gain_node();
        const CircuitNode & circuit_node_to_move = nodes[bucket_node_to_move.name];

        // excute movement (excluding the upodate of num_of_nodes_in_partition)
        buckets[from_part].erase(bucket_node_to_move.name);
        partition_size[from_part] -= bucket_node_to_move.size;
        partition_size[to_part] += bucket_node_to_move.size;
        cut_size -= bucket_node_to_move.gain;
        partition[bucket_node_to_move.name] = to_part;
        pending_moves[bucket_node_to_move.name] = to_part;
        node_locked[bucket_node_to_move.name] = true;

        // update the min cut if the cut size is smaller than the min cut size, excute all pending move
        if (cut_size < min_cut_size) {
            // update the min cut size and min cut partition
            min_cut_size = cut_size;
            // apply all changes since last update, clear the pending_moves
            for (const auto& pair : pending_moves) {
                min_cut_partition[pair.first] = pair.second;
            }
            pending_moves.clear();
        }

        // go through all the nets this node is involved in, check critical nets, update gains and num_of_nodes_in_partition
        for (const auto& net_name : circuit_node_to_move.net_names_unique) {
            // check critical nets before the move
            // T(n) == 0 then increase gain of all free cells on this net
            if (num_of_nodes_in_partition[net_name][to_part] == 0) {
                // increase gains of all free nodes on this net
                for (const auto& node_name : nets[net_name].node_names_unique) {
                    if (node_locked[node_name] == false) {
                        buckets[partition[node_name]].increase_gain(node_name);
                    }
                }
            }
            // else of T(n) == 1 THEN decrement gain of the only T cell on net(n), if it is free
            else if (num_of_nodes_in_partition[net_name][to_part] == 1) {
                // decrease gains of all free to_part nodes on this net
                for (const auto& node_name : nets[net_name].node_names_unique) {
                    if (node_locked[node_name] == false && partition[node_name] == to_part) {
                        buckets[to_part].decrease_gain(node_name);
                    }
                }
            }
            // update the num_of_nodes_in_partition to reflect the movement
            num_of_nodes_in_partition[net_name][from_part]--;
            num_of_nodes_in_partition[net_name][to_part]++;
            // check critical nets after the move
            // F(n) == 0 then decrease gain of all free cells on this net
            if (num_of_nodes_in_partition[net_name][from_part] == 0) {
                // decrease gains of all free nodes on this net
                for (const auto& node_name : nets[net_name].node_names_unique) {
                    if (node_locked[node_name] == false) {
                        buckets[partition[node_name]].decrease_gain(node_name);
                    }
                }
            }
            // ELSE IF F(n) = 1 THEN increment gain of the only F cell on net(n), if it is free
            else if (num_of_nodes_in_partition[net_name][from_part] == 1) {
                // increase gains of all free from_part nodes on this net
                for (const auto& node_name : nets[net_name].node_names_unique) {
                    if (node_locked[node_name] == false && partition[node_name] == from_part) {
                        buckets[from_part].increase_gain(node_name);
                    }
                }
            }
        }

        end = std::chrono::high_resolution_clock::now();
        if (dump_level == 1) {
            std::cout << "Time to update in FM algo: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
        }

        if (dump_level == 1) {
            std::cout << "Move node " << bucket_node_to_move.name << " from partition " << from_part << " to partition " << to_part << std::endl;
            std::cout << "Current cut size: " << cut_size << ", min cut size: " << min_cut_size << std::endl;
            std::cout << "Current partition size: " << partition_size[0] << ", " << partition_size[1] << std::endl;
        }
    }

    auto end_main_loop = std::chrono::high_resolution_clock::now();
    if (dump_level == 0) {
        std::cout << "Time to run main loop in FM algo: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_main_loop - start_main_loop).count() << " ms" << std::endl;
    }

    start = std::chrono::high_resolution_clock::now();

    // build the two partitions and the cut, follow min_cut_partition
    Circuit parts[2] = {Circuit(), Circuit()};
    std::vector<CircuitNet> cut;
    // go throuth all the nodes, insert them into the corresponding partition
    for (const auto& pair : nodes) {
        const CircuitNode& node = pair.second;
        // if (node.node_type == NodeTypeEnum::terminal) {
        //     continue;
        // }
        parts[min_cut_partition[node.name]].nodes[node.name] = node;
    }
    // go through all the nets, check if a net is:
    // 1. all in one partition, add it to the corresponding partition
    // 2. in both partitions, add it to the cut, insert two new nets into the corresponding partition
    for (const auto& pair : nets) {
        const CircuitNet& net = pair.second;
        CircuitNet new_nets[2] = {CircuitNet(net.name), CircuitNet(net.name)};
        bool contains_nodes_from_part[2] = {false, false};
        // go through all the pins in this net, insert them into the corresponding partition
        for (const auto& pin : net.node_pins) {
            int its_part = min_cut_partition[pin.name];
            new_nets[its_part].node_pins.push_back(pin);
            new_nets[its_part].node_names_unique.insert(pin.name);
            contains_nodes_from_part[its_part] = true;
        }
        if (contains_nodes_from_part[0] && contains_nodes_from_part[1]) {
            cut.push_back(net);
        }
        for (int part = 0; part < 2; part++) {
            // if this net is in one partition, add it to the corresponding partition
            if (contains_nodes_from_part[part]) {
                parts[part].nets[net.name] = new_nets[part];
            }
        }
    }

    end = std::chrono::high_resolution_clock::now();
    if (dump_level == 0) {
        std::cout << "Time to build the two partitions and the cut: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }

    return std::make_tuple(parts[0], parts[1], cut);
}
