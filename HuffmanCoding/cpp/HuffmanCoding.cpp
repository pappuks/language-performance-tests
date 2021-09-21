#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <iostream>
#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <functional>
#include <chrono>

using namespace std;

class Node {
    public:
        char ch;
        int freq;
        Node* left; 
        Node* right;

        Node(char ch, int freq, Node* left, Node* right) {
            this->ch = ch;
            this->freq = freq;
            this->left = left;
            this->right = right;
        }

        // is the node a leaf node?
        bool isLeaf() {
            return (left == 0) && (right == 0);
        }

        
};

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::ostringstream contents;
        contents << in.rdbuf();
        in.close();
        return (contents.str());
    }
    throw(errno);
}

bool compare (Node* lhs, Node* rhs)
{
    return (lhs->freq > rhs->freq);
}

Node* buildTrie(int* freq) {
    priority_queue<Node*,vector<Node*>, function<bool(Node*,Node*)>> pq(compare);

    for (int c = 0; c < 256; c++) {
        if (freq[c] > 0) {
            pq.push(new Node(c, freq[c], 0, 0));
        }
    }

    while(pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();
        pq.push(new Node('0', left->freq + right->freq, left, right));
    }

    return pq.top();
}

void buildCode(string* st, Node* x, string s) {
    if (!(x->isLeaf())) {
        buildCode(st, x->left, s + '0');
        buildCode(st, x->right, s + '1');
    } else {
        st[x->ch] = s;
    }
}

void compress(const char * chars, int inputLength, Node** root, vector<bool>** compressed) {
    

    // tabulate frequency counts
    int freq[256] {0};
    for (int i = 0; i < inputLength; i++) {
        freq[chars[i]]++;
    }

    *root = buildTrie(freq);

    string st[256];
    buildCode(st, *root, "");

    *compressed = new vector<bool>();

    for (int i = 0; i < inputLength; i++) {
        string code = st[chars[i]];
        const char * codeCh = code.c_str();
        for (int j = 0; j < code.length(); j++) {
            if (codeCh[j] == '0') {
                (*compressed)->push_back(false);
            } else if (codeCh[j] == '1') {
                (*compressed)->push_back(true);
            }
        }
    }

    cout << "Input size:" << inputLength << " Compressed:" << (*compressed)->size()/8 << endl;
}

string expand(int inputLength, Node* root, vector<bool>* compressed) {
    char* output = new char[inputLength];

    int j = 0;
    for (int i = 0; i < inputLength; i++) {
        Node * x = root;
        while(!(x->isLeaf())) {
            bool bit = compressed->at(j++);
            if (bit) {
                x = x->right;
            } else {
                x = x->left;
            }
        }
        output[i] = x->ch;
    }
    return string(output);
}

void cleanupNode(Node * x) {
    if (x->left != 0) {
        cleanupNode(x->left);
        x->left = 0;
    }
    if (x->right != 0) {
        cleanupNode(x->right);
        x->right = 0;
    }
    delete x;
}

int main()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();	
    
    Node* root;
    vector<bool>* compressed;

    string fileContent = get_file_contents("tale.txt");
    const char * chars = fileContent.c_str();
    int inputLength = fileContent.length();

    std::chrono::steady_clock::time_point endFile = std::chrono::steady_clock::now();
	std::cout << "Time File Read = " << std::chrono::duration_cast<std::chrono::microseconds>(endFile - begin).count() << "[µs]" << std::endl;

    compress(chars, inputLength, &root, &compressed);

    std::chrono::steady_clock::time_point endCompress = std::chrono::steady_clock::now();
	std::cout << "Time Compress = " << std::chrono::duration_cast<std::chrono::microseconds>(endCompress - begin).count() << "[µs]" << std::endl;

    string expanded = expand(inputLength, root, compressed);
    if (fileContent.compare(expanded) == 0) {
        cout << "Success" << endl;
    }

    delete compressed;
    cleanupNode(root);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    
}