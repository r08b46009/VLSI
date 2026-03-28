#include "SA.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace {
const int kOperandNode = 0;
const int kVerticalCut = -2;
const int kHorizontalCut = -1;

const int kWidthIndex = 0;
const int kHeightIndex = 1;
const int kLeftChoiceIndex = 2;
const int kRightChoiceIndex = 3;

const int kFeasibilityPenaltyWeight = 10;
const unsigned int kRandomSeed = 100;

enum MoveType {
    kSwapOperands = 0,
    kToggleCutType = 1,
    kSwapAdjacentOperandAndOperator = 2
};

bool HeightDescending(const std::vector<int>& a, const std::vector<int>& b) {
    return a[kHeightIndex] > b[kHeightIndex];
}

void EnsureFileOpened(const std::ifstream& stream, const std::string& filename) {
    if (!stream.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
}
}  // namespace

HB::HB(const std::string& block_name, int block_width, int block_height)
    : name(block_name),
      width(block_width),
      height(block_height),
      x(0),
      y(0),
      isRotated(false),
      pin(nullptr) {
    pin = new Pin(block_name, x, y);
}

Node::Node(int node_type, HB* block)
    : type(node_type), hardblock(block), left(nullptr), right(nullptr) {
    if (block != nullptr) {
        direc.push_back(std::vector<int>{block->width, block->height, 0, 0});
        direc.push_back(std::vector<int>{block->height, block->width, 1, 1});
    }
}

SAInput::SAInput(const std::vector<HB*>& hardblocks,
                 const std::vector<Net*>& nets,
                 double dead_space_ratio)
    : hardblocks(hardblocks), nets(nets), deadSpaceRatio(dead_space_ratio) {}

void Parser::readHB(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    EnsureFileOpened(fin, filename);

    std::string temp;
    int hardblock_count = 0;
    int terminal_count = 0;
    fin >> temp >> temp >> hardblock_count;
    fin >> temp >> temp >> terminal_count;

    while (hardblock_count--) {
        int vertex_count = 0;
        int zero = 0;
        int x = 0;
        int y = 0;
        char c = '\0';
        std::string name;

        fin >> name >> temp;
        fin >> vertex_count;
        fin >> c;
        fin >> x;
        fin >> c;
        fin >> y;
        fin >> c;

        fin >> c;
        fin >> zero;
        fin >> c;
        fin >> y;
        fin >> c;

        fin >> c;
        fin >> x;
        fin >> c;
        fin >> y;
        fin >> c;

        fin >> c;
        fin >> x;
        fin >> c;
        fin >> zero;
        fin >> c;

        hardblocks.push_back(new HB(name, x, y));
        pin_map_.insert(std::make_pair(name, hardblocks.back()->pin));
    }
}

void Parser::readPl(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    EnsureFileOpened(fin, filename);

    std::string name;
    int x = 0;
    int y = 0;
    while (fin >> name >> x >> y) {
        pin_map_.insert(std::make_pair(name, new Pin(name, x, y)));
    }
}

void Parser::readNet(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    EnsureFileOpened(fin, filename);

    std::string identifier;
    while (fin >> identifier) {
        if (identifier == "NetDegree") {
            nets.push_back(new Net());
            std::string temp;
            size_t pin_count = 0;
            fin >> temp >> pin_count;
            for (size_t i = 0; i < pin_count; ++i) {
                std::string name;
                fin >> name;
                nets.back()->pins.push_back(pin_map_.at(name));
            }
        }
    }
}

SAInput* Parser::parse(char* argv[]) {
    readHB(argv[1]);
    readPl(argv[3]);
    readNet(argv[2]);
    return new SAInput(hardblocks, nets, std::stod(argv[5]));
}

void ResultWriter::write(const std::string& filename, int WL) {
    std::ofstream fout(filename.c_str());
    if (!fout.is_open()) {
        throw std::runtime_error("Failed to write output file: " + filename);
    }

    fout << "Wirelength " << WL << std::endl << "Blocks\n";
    for (std::vector<HB*>::const_iterator it = input->hardblocks.begin();
         it != input->hardblocks.end();
         ++it) {
        fout << (*it)->name << ' ' << (*it)->x << ' ' << (*it)->y << ' '
             << (*it)->isRotated << std::endl;
    }
}

int Net::HPWL() {
    int count = 0;
    int min_x = 0;
    int max_x = 0;
    int min_y = 0;
    int max_y = 0;

    for (std::vector<Pin*>::iterator it = pins.begin(); it != pins.end(); ++it) {
        Pin* pin = *it;
        if (count == 0) {
            min_x = max_x = pin->x;
            min_y = max_y = pin->y;
        } else {
            if (min_x > pin->x) min_x = pin->x;
            if (min_y > pin->y) min_y = pin->y;
            if (max_x < pin->x) max_x = pin->x;
            if (max_y < pin->y) max_y = pin->y;
        }
        ++count;
    }
    return (max_x - min_x) + (max_y - min_y);
}

void Node::update() {
    direc.clear();

    if (type == kVerticalCut) {
        std::sort(left->direc.begin(), left->direc.end(), HeightDescending);
        std::sort(right->direc.begin(), right->direc.end(), HeightDescending);

        int left_idx = 0;
        int right_idx = 0;
        while (left_idx < static_cast<int>(left->direc.size()) &&
               right_idx < static_cast<int>(right->direc.size())) {
            const int left_width = left->direc[left_idx][kWidthIndex];
            const int right_width = right->direc[right_idx][kWidthIndex];
            const int left_height = left->direc[left_idx][kHeightIndex];
            const int right_height = right->direc[right_idx][kHeightIndex];

            direc.push_back(std::vector<int>{left_width + right_width,
                                             std::max(left_height, right_height),
                                             left_idx,
                                             right_idx});
            if (left_height > right_height) {
                ++left_idx;
            } else if (left_height < right_height) {
                ++right_idx;
            } else {
                ++left_idx;
                ++right_idx;
            }
        }
        return;
    }

    std::sort(left->direc.begin(), left->direc.end());
    std::sort(right->direc.begin(), right->direc.end());

    int left_idx = static_cast<int>(left->direc.size()) - 1;
    int right_idx = static_cast<int>(right->direc.size()) - 1;
    while (left_idx >= 0 && right_idx >= 0) {
        const int left_width = left->direc[left_idx][kWidthIndex];
        const int right_width = right->direc[right_idx][kWidthIndex];
        const int left_height = left->direc[left_idx][kHeightIndex];
        const int right_height = right->direc[right_idx][kHeightIndex];

        direc.push_back(std::vector<int>{std::max(left_width, right_width),
                                         left_height + right_height,
                                         left_idx,
                                         right_idx});
        if (left_width > right_width) {
            --left_idx;
        } else if (left_width < right_width) {
            --right_idx;
        } else {
            --left_idx;
            --right_idx;
        }
    }
}

bool SA::check(std::vector<int>& post, int& idx) {
    const int num = static_cast<int>(post.size());
    if (post[idx + 1] < 0) {
        int operator_count = 0;
        for (int i = 0; i <= idx + 1; ++i) {
            if (post[i] < 0) {
                ++operator_count;
            }
        }
        if (2 * operator_count >= idx + 1) {
            return false;
        }
    }

    if (post[idx + 1] < 0 && idx != 0) {
        return post[idx - 1] != post[idx + 1];
    }
    if (post[idx] < 0 && idx + 1 != num - 1) {
        return post[idx] != post[idx + 2];
    }
    return false;
}

std::vector<int> SA::perturb(std::vector<int> post, int& type) {
    const int num = static_cast<int>(post.size());
    std::vector<int> candidate_indices;
    int idx1 = 0;
    int idx2 = 0;
    int violation_count = 0;

    if (type == kSwapOperands) {
        for (int i = 0; i < num; ++i) {
            if (post[i] >= 0) {
                candidate_indices.push_back(i);
            }
        }

        idx1 = rand() % candidate_indices.size();
        idx2 = rand() % candidate_indices.size();
        while (idx1 == idx2) {
            idx2 = rand() % candidate_indices.size();
        }
        std::swap(post[candidate_indices[idx1]], post[candidate_indices[idx2]]);
        return post;
    }

    if (type == kToggleCutType) {
        for (int i = 1; i < num; ++i) {
            if (post[i - 1] >= 0 && post[i] < 0) {
                candidate_indices.push_back(i);
            }
        }

        idx1 = candidate_indices[rand() % candidate_indices.size()];
        while (idx1 < num && post[idx1] < 0) {
            post[idx1] = (post[idx1] == kVerticalCut) ? kHorizontalCut : kVerticalCut;
            ++idx1;
        }
        return post;
    }

    for (int i = 0; i < num - 1; ++i) {
        if ((post[i] >= 0 && post[i + 1] < 0) ||
            (post[i] < 0 && post[i + 1] >= 0)) {
            candidate_indices.push_back(i);
        }
    }

    do {
        idx1 = rand() % candidate_indices.size();
        ++violation_count;
    } while (!check(post, candidate_indices[idx1]) &&
             violation_count < static_cast<int>(candidate_indices.size()));

    if (violation_count < static_cast<int>(candidate_indices.size())) {
        idx1 = candidate_indices[idx1];
        idx2 = idx1 + 1;
        std::swap(post[idx1], post[idx2]);
    }

    return post;
}

Node* SA::buildTree(std::vector<int>& post) {
    int cut_index = 0;
    std::vector<Node*> inorder_nodes;

    for (std::vector<int>::iterator it = post.begin(); it != post.end(); ++it) {
        const int type = *it;
        if (type >= 0) {
            inorder_nodes.push_back(hardblocktype[type]);
        } else {
            Node* node = cutidx[cut_index++];
            node->type = type;
            node->right = inorder_nodes.back();
            inorder_nodes.pop_back();
            node->left = inorder_nodes.back();
            inorder_nodes.pop_back();
            inorder_nodes.push_back(node);
            node->update();
        }
    }
    return inorder_nodes.back();
}

void SA::position(Node* node, int& choice, int x, int y) {
    if (node->type == kOperandNode) {
        const int margin_x = node->direc[choice][kWidthIndex];
        const int margin_y = node->direc[choice][kHeightIndex];
        node->hardblock->x = x;
        node->hardblock->y = y;
        node->hardblock->isRotated = !(node->hardblock->width >= margin_x &&
                                       node->hardblock->height >= margin_y);
        node->hardblock->pin->x = x + margin_x / 2;
        node->hardblock->pin->y = y + margin_y / 2;
        return;
    }

    position(node->left, node->direc[choice][kLeftChoiceIndex], x, y);

    int offset_x = 0;
    int offset_y = 0;
    if (node->type == kVerticalCut) {
        offset_x = node->left->direc[node->direc[choice][kLeftChoiceIndex]][kWidthIndex];
    } else {
        offset_y = node->left->direc[node->direc[choice][kLeftChoiceIndex]][kHeightIndex];
    }

    const int new_x = x + offset_x;
    const int new_y = y + offset_y;
    position(node->right, node->direc[choice][kRightChoiceIndex], new_x, new_y);
}

int SA::calCost(std::vector<int>& post, bool focusWirelength) {
    Node* root = buildTree(post);
    int min_area_penalty = 0;
    int best_choice = 0;

    for (int i = 0; i < static_cast<int>(root->direc.size()); ++i) {
        const std::vector<int>& shape = root->direc[i];
        const int width_overflow = shape[kWidthIndex] - outline;
        const int height_overflow = shape[kHeightIndex] - outline;

        int area_penalty = 0;
        if (width_overflow > 0 && height_overflow > 0) {
            area_penalty = shape[kWidthIndex] * shape[kHeightIndex] - outline * outline;
        } else if (width_overflow <= 0 && height_overflow > 0) {
            area_penalty = outline * height_overflow;
        } else if (width_overflow > 0 && height_overflow <= 0) {
            area_penalty = width_overflow * outline;
        }

        if (i == 0 || min_area_penalty > area_penalty) {
            min_area_penalty = area_penalty;
            best_choice = i;
        }
    }

    if (!focusWirelength) {
        return min_area_penalty * kFeasibilityPenaltyWeight;
    }

    position(root, best_choice, 0, 0);
    int wirelength = 0;
    for (std::vector<Net*>::iterator it = input->nets.begin(); it != input->nets.end(); ++it) {
        wirelength += (*it)->HPWL();
    }
    return min_area_penalty * kFeasibilityPenaltyWeight + wirelength;
}

std::vector<int> SA::algorithm(double c,
                               double r,
                               int k,
                               const std::vector<int>& post,
                               bool focusWirelength) {
    int initial_wirelength = 0;
    for (std::vector<Net*>::iterator it = input->nets.begin(); it != input->nets.end(); ++it) {
        initial_wirelength += (*it)->HPWL();
    }
    std::cout << "Initialized\nwirelength: " << initial_wirelength << "\n\n";

    const int move_limit = k * static_cast<int>(input->hardblocks.size());
    std::vector<int> best_expression = post;
    std::vector<int> current_expression = post;
    int best_cost = calCost(current_expression, focusWirelength);
    int current_cost = best_cost;

    do {
        double temperature = 100.0;
        do {
            int total_moves = 0;
            int uphill_moves = 0;
            int rejected_moves = 0;

            do {
                int move_type = kSwapOperands;
                if (!focusWirelength) {
                    move_type = rand() % 3;
                }

                std::vector<int> neighbor = perturb(current_expression, move_type);
                ++total_moves;

                const int new_cost = calCost(neighbor, focusWirelength);
                const int delta = new_cost - current_cost;
                const double accept_prob = std::exp(-1.0 * delta / temperature);
                if (delta < 0 || static_cast<double>(rand()) / RAND_MAX < accept_prob) {
                    if (delta > 0) {
                        ++uphill_moves;
                    }
                    current_expression = neighbor;
                    current_cost = new_cost;

                    if (current_cost < best_cost) {
                        best_expression = current_expression;
                        best_cost = current_cost;
                        if (best_cost == 0) {
                            calCost(best_expression, true);
                            return best_expression;
                        }
                    }
                } else {
                    ++rejected_moves;
                }
            } while (uphill_moves <= move_limit && total_moves <= 2 * move_limit);

            temperature = r * temperature;
            if (total_moves == 0) {
                break;
            }
            if (static_cast<double>(rejected_moves) / total_moves > 0.95) {
                break;
            }
        } while (temperature >= c);
    } while (!focusWirelength);

    calCost(best_expression, true);

    int final_wirelength = 0;
    for (std::vector<Net*>::iterator it = input->nets.begin(); it != input->nets.end(); ++it) {
        final_wirelength += (*it)->HPWL();
    }
    std::cout << "find a possible solution\nwirelength: " << final_wirelength << "\n\n";

    return best_expression;
}

SA::SA(SAInput* input)
    : input(input), outline(0) {
    int total_area = 0;
    for (std::vector<HB*>::iterator it = input->hardblocks.begin(); it != input->hardblocks.end(); ++it) {
        HB* hardblock = *it;
        hardblocktype.push_back(new Node(kOperandNode, hardblock));
        total_area += hardblock->width * hardblock->height;
    }

    outline = static_cast<int>(std::sqrt(total_area * (1 + input->deadSpaceRatio)));
    for (int i = 0; i < static_cast<int>(input->hardblocks.size()) - 1; ++i) {
        cutidx.push_back(new Node(kVerticalCut));
    }
}

ResultWriter* SA::solve() {
    srand(kRandomSeed);

    std::vector<int> post;
    int row_width = 0;
    int vertical_cut_count = 0;
    int horizontal_cut_count = 0;
    int cut_count = 0;
    const int num_blocks = static_cast<int>(input->hardblocks.size());

    for (int i = 0; i < num_blocks; ++i) {
        HB* hardblock = input->hardblocks[i];
        row_width += hardblock->width;
        if (row_width >= outline) {
            ++horizontal_cut_count;
            if (horizontal_cut_count >= 2) {
                post.push_back(kHorizontalCut);
                ++cut_count;
                horizontal_cut_count = 1;
            }
            row_width = hardblock->width;
            vertical_cut_count = 0;
        }

        post.push_back(i);
        ++vertical_cut_count;
        if (vertical_cut_count >= 2) {
            post.push_back(kVerticalCut);
            ++cut_count;
            vertical_cut_count = 1;
        }
    }

    for (int i = cut_count; i < num_blocks - 1; ++i) {
        post.push_back(kHorizontalCut);
    }

    post = algorithm(0.1, 0.9, 10, post, false);
    std::cout << "SA start" << std::endl;
    algorithm(1.0, 0.95, 5, post, true);

    return new ResultWriter(input);
}
