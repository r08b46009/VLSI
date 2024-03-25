

#include <vector>
using namespace std;
#include <unordered_map>
#include <string>

struct Pin;
struct HB;
struct Net;

struct Pin
{
    string name;
    int x, y;

    Pin( string &N, int &coordinate_x, int &coordinate_y)
     {
        name = N, x =coordinate_x, y= coordinate_y;
     }
};


struct HB
{
     string name;
    int width, height, x, y;
    bool isRotated;
    Pin *pin;

    HB( string N, int &W, int &H)
    {
        name = N, width = W, height = H, x =0, y = 0, isRotated = 0;
        pin = new Pin(N, x, y);
    }
   
};

struct Net
{
     vector<Pin *> pins;

    int HPWL();
};

struct Node
{

    int type;
    HB *hardblock;
    Node *left, *right;
     vector< vector<int>> direc; // {{width, height, left choice, right choice}}

    Node(int  const &T, HB *hardblock = nullptr)
        : hardblock(hardblock)
    {
         left = nullptr; right = nullptr;
         type = T;
        if (hardblock != nullptr)
            direc = {{hardblock->width, hardblock->height, 0, 0},
                      {hardblock->height, hardblock->width, 1, 1}};
    }
    void update();
};



struct SAInput
{
     vector<HB *> hardblocks;
     vector<Net *> nets;
    double deadSpaceRatio;

    SAInput( vector<HB *>   &hardblocks,  vector<Net *>   &nets, double const &deadSpaceRatio)
        : hardblocks(hardblocks), nets(nets), deadSpaceRatio(deadSpaceRatio) {}
};

class Parser
{
public:
    vector<HB *> hardblocks;
    vector<Net *> nets;

    void readHB(string const &filename);
    void readPl(string const &filename);
    void readNet(string const &filename);


    Parser() {}
    SAInput *parse(char *argv[]);
};



class ResultWriter
{
    
    public:
    SAInput *input;
    ResultWriter(SAInput *input) : input(input) {}
    
    void write(string const &filename, int WL);
};




class SA
{

public:
    SAInput *input;
    vector<Node *> hardblocktype, cutidx;
    int outline;

    void Ini(vector<int> & post);
    void invertCut(vector<int> &post, int pos);
    bool check(vector<int> &post, int &pos)  ;
    vector<int> perturb(vector<int> post,int &type);
    void perturbinit(vector<int> post,int &type);
    Node *buildTree(vector<int>   & post);
    void position(Node *node, int &choice, int const &x, int const&y);
    int calCost(vector<int>   & post, bool const &WL);
    vector<int> algorithm(double const &c, double const &r, int const &k, vector<int>  const &post, bool  const  &WL);
    SA(SAInput *input);
    ResultWriter *solve();
};

//unordered_map<string, Pin *> map;