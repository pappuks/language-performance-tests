#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <iostream>
#include <queue>          // priority_queue
#include <vector>         // vector
#include <functional>
#include <chrono>
#include <future>
#include <pthread.h>

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

class CompressedOutput {
    public:
        vector<bool>* compressed;
        int inputLength;
};

string get_file_contents(const char *filename)
{
    ifstream in(filename, ios::in | ios::binary);
    if (in)
    {
        ostringstream contents;
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

CompressedOutput compress(const char * chars, int index, int inputLength, string* st) {
    vector<bool>* compressed = new vector<bool>();

    float div8 = float(inputLength) / 8;

    int start = index * int(div8);
    int end = (index + 1) * int(div8);

    if (index == 7) {
        end = inputLength;
    }

    for (int i = start; i < end; i++) {
        string code = st[chars[i]];
        const char * codeCh = code.c_str();
        for (int j = 0; j < code.length(); j++) {
            if (codeCh[j] == '0') {
                (compressed)->push_back(false);
            } else if (codeCh[j] == '1') {
                (compressed)->push_back(true);
            }
        }
    }

    cout << "Input size:" << end - start << " Compressed:" << (compressed)->size()/8 << endl;

    CompressedOutput retVal;
    retVal.compressed = compressed;
    retVal.inputLength = end - start;

    return retVal;
}

void buildTrieAndCode(const char * chars, int inputLength, Node** root, string** st) {
    // tabulate frequency counts
    int freq[256] {0};
    for (int i = 0; i < inputLength; i++) {
        freq[chars[i]]++;
    }

    *root = buildTrie(freq);

    *st = new string[256];
    buildCode(*st, *root, "");
}

string expand(int inputLength, Node* root, vector<bool>* compressed) {
    char* output = new char[inputLength + 1];

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
    output[inputLength] = '\0';
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
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();	
    
    Node* root;
    vector<bool>* compressed;

    string fileContent = get_file_contents("bible.txt");
    const char * chars = fileContent.c_str();
    int inputLength = fileContent.length();

    chrono::steady_clock::time_point endFile = chrono::steady_clock::now();
	cout << "Time File Read = " << chrono::duration_cast<chrono::microseconds>(endFile - begin).count() << "[µs]" << endl;

    string* st;

    buildTrieAndCode(chars, inputLength, &root,&st);

    vector<future<CompressedOutput>> futures;

    for (int i = 0; i < 8; i++) {
        futures.push_back(async(compress, chars, i, inputLength, st));
    }

    vector<CompressedOutput> compressedOutputs;

    for (int i = 0; i < 8; i++) {
        compressedOutputs.push_back(futures.at(i).get());
    }

    chrono::steady_clock::time_point endCompress = chrono::steady_clock::now();
	cout << "Time Compress = " << chrono::duration_cast<chrono::microseconds>(endCompress - begin).count() << "[µs]" << endl;

    vector<future<string>> expandFutures;

    for (int i = 0; i < 8; i++) {
        expandFutures.push_back(async(expand, compressedOutputs.at(i).inputLength, root, compressedOutputs.at(i).compressed));
    }

    string expanded;

    for (int i = 0; i < 8; i++) {
        expanded += expandFutures.at(i).get();
    }

    cout << fileContent.length() << ' ' << expanded.length() << endl;

    if (fileContent.compare(expanded) == 0) {
        cout << "Success" << endl;
    }

    delete compressed;
    cleanupNode(root);

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "[µs]" << endl;
    
}