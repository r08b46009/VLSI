#ifndef HW3_SA_HPP
#define HW3_SA_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct Pin;
struct HB;
struct Net;
struct Node;
struct SAInput;

struct Pin {
    std::string name;
    int x;
    int y;

    Pin(const std::string& pin_name, int coordinate_x, int coordinate_y)
        : name(pin_name), x(coordinate_x), y(coordinate_y) {}
};

struct HB {
    std::string name;
    int width;
    int height;
    int x;
    int y;
    bool isRotated;
    Pin* pin;

    HB(const std::string& block_name, int block_width, int block_height);
};

struct Net {
    std::vector<Pin*> pins;
    int HPWL();
};

struct Node {
    int type;
    HB* hardblock;
    Node* left;
    Node* right;
    std::vector<std::vector<int> > direc;  // {width, height, left choice, right choice}

    explicit Node(int node_type, HB* block = nullptr);
    void update();
};

struct SAInput {
    std::vector<HB*> hardblocks;
    std::vector<Net*> nets;
    double deadSpaceRatio;

    SAInput(const std::vector<HB*>& hardblocks,
            const std::vector<Net*>& nets,
            double deadSpaceRatio);
};

class Parser {
public:
    std::vector<HB*> hardblocks;
    std::vector<Net*> nets;

    Parser() {}
    void readHB(const std::string& filename);
    void readPl(const std::string& filename);
    void readNet(const std::string& filename);
    SAInput* parse(char* argv[]);

private:
    std::unordered_map<std::string, Pin*> pin_map_;
};

class ResultWriter {
public:
    explicit ResultWriter(SAInput* input) : input(input) {}
    void write(const std::string& filename, int WL);

    SAInput* input;
};

class SA {
public:
    explicit SA(SAInput* input);
    ResultWriter* solve();

private:
    bool check(std::vector<int>& post, int& pos);
    std::vector<int> perturb(std::vector<int> post, int& type);
    Node* buildTree(std::vector<int>& post);
    void position(Node* node, int& choice, int x, int y);
    int calCost(std::vector<int>& post, bool focusWirelength);
    std::vector<int> algorithm(double c,
                               double r,
                               int k,
                               const std::vector<int>& post,
                               bool focusWirelength);

    SAInput* input;
    std::vector<Node*> hardblocktype;
    std::vector<Node*> cutidx;
    int outline;
};

#endif  // HW3_SA_HPP
