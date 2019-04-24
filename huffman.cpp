//Date : 2018/12/29
//Purpose : Huffman

#include <iostream>
#include <queue>		//priority queue
#include <vector>
#include <functional>	//greater
#include <fstream>		//open file
#include <cstring>		//memset
#include <cstdlib>		//strtoull
#include <bitset>		//bitset
using namespace std;

int freq[256];		//frequency
string Code[256] = {""};		//code
unsigned long long originalBit = 0;		//origianl bit
unsigned long long compressBit = 0;		//compressed bit
struct node {		//for compress
	string data;	//code
	int freq;		//frequency

	node() : data(""), freq(0) {}
	node(string s, int f) :  data(s), freq(f) {}
	friend bool operator >(const node& a, const node& b) {		//for greater
		return a.freq == b.freq ? a.data > b.data : a.freq > b.freq;
	}
};
struct treeNode {	//for decompress
	int data;		//ascii
	string code;	
	bool leafNode;	//if it is a leaf node
	treeNode *left = NULL, *right = NULL;
	
	treeNode() : data(-1), code(""), leafNode(false) {}
	treeNode(int n, string i, bool leaf) : data(n), code(i), leafNode(leaf) {}
}*root = NULL;

void freqCount(string);		//count frequency
void encode();				//encode
void fileHeader(string);	//output file header
void compress(string, string);		//compress
void buildTree(string);		//build huffman search tree
void decompress(string, string);	//decompress

int main(int argc, char *argv[]) {

	if (argv[1][0] == '-' && argv[1][1] == 'c') {	//compress
		compress(argv[3], argv[5]);
		cout << "compress " << argv[3] << " to " << argv[5] << " successfully" << endl;
	} else if (argv[1][0] == '-' && argv[1][1] == 'u') { //decompress
		decompress(argv[3], argv[5]);
		cout << "decompress " << argv[3] << " to " << argv[5] << " successfully" << endl;
	}
	
	return 0;
}

void freqCount(string s) {
	fstream fin;
	fin.open(s, ios::binary | ios::in);
	if (fin.is_open()) {
		memset(freq, 0, sizeof(freq));	//reset to 0
		char c;
		while (fin.get(c)) {
			freq[(unsigned char)(c)]++;
			originalBit++;
		}
		originalBit *= 8;
	} else cout << "input file open fail : " << __LINE__ << endl;
	fin.close();
}

void encode() {
	priority_queue<node, vector<node>, greater<node>> pq;	//min heap
	queue<node*> q;
	
	for (int i = 0; i < 256; i++)
		if (freq[i]) pq.push(node(string(1, char(i)), freq[i]) );
	
	while (pq.size() > 1) {
		node a = pq.top();
		pq.pop();
		node b = pq.top();
		pq.pop();
		if (a.data > b.data) swap(a,b);
		node tmp = node(a.data + b.data, a.freq + b.freq);
		pq.push(tmp);
		for (int i = 0; i < a.data.length(); i++)	//encoding
			Code[(unsigned char)(a.data[i])] = "0" + Code[(unsigned char)(a.data[i])];
		for (int i = 0; i < b.data.length(); i++)
			Code[(unsigned char)(b.data[i])] = "1" + Code[(unsigned char)(b.data[i])];
	}
}

void fileHeader(string outfile) {
	fstream fout;
	fout.open(outfile, ios::out | ios::app | ios::binary);
	
	if (fout.is_open()) {
		for (int i = 0; i < 256; i++) {
			if (freq[i]) {
				fout << i << '=' << Code[i] << endl;
				compressBit += Code[i].length() * freq[i];
			}
		}
		fout << "Original bit : " << originalBit << endl;
		fout << "Compression bit : " << compressBit << endl;
		fout << "Compression ratio = " << (compressBit % 8 ? double(originalBit) / (compressBit + 8 - compressBit % 8) : double(originalBit) / compressBit) << endl;
	} else cout << "output compressed file fail : " << __LINE__ << endl;

	fout.close();
}

void compress(string infile, string outfile) {	//compress
	freqCount(infile);
	encode();
	fileHeader(outfile);
	
	fstream fin, fout;
	fin.open(infile, ios::in | ios::binary);	
	if (fin.is_open()) {
		fout.open(outfile, ios::out | ios::app | ios::binary);
		if (fout.is_open ()) {
			unsigned long long num;
			char c;
			string tmp = "";
			while (fin.get(c)) {
				tmp = tmp + Code[(unsigned char)(c)];
				while (tmp.length() >= 8) {
					char *wanted = (char*)(tmp.substr(0, 8).c_str());
					num = strtoull(wanted, NULL, 2);	//8 bit binary string transfer to unsigned char
					fout << (unsigned char)(num);
					if (tmp.length() > 8) tmp = &tmp[8];
					else tmp = "";
				}
			}
			if (tmp.length()) {
				while(8 - tmp.length()) tmp = tmp + "0";	//if it's not enough to transfer to unsigned char
				char *wanted = (char*)(tmp.substr(0, 8).c_str());
				num = strtoull(wanted, NULL, 2);
				fout << (unsigned char)(num);
			}
		} else cout << "output compressed file fail : " << __LINE__ << endl;		
		fout.close();
	} else cout << "input file open fail : " << __LINE__ << endl;
	
	fin.close();
}

void buildTree(string s) {
	fstream fin;

	fin.open(s, ios::in | ios::binary);
	if (fin.is_open()) {
		int data;
		string input;
		root = new treeNode;
		while (fin >> data >>input && input != "Original") {
			treeNode *newNode = new treeNode(data, &input[1], true);	//decode
			treeNode *cur = root;
			for (int i = 0; i < newNode->code.length() - 1; i++) {		//build tree
				if (newNode->code[i] == '0') {
					if (!cur->left) {
						treeNode *tmp = new treeNode;
						cur->left = tmp;
					}
					cur = cur->left;
				} else {
					if (!cur->right) {
						treeNode *tmp = new treeNode;
						cur->right = tmp;
					}
					cur = cur->right;
				}
			}
			if (newNode->code[newNode->code.length() - 1] == '0') cur->left = newNode;
			else cur->right = newNode;
		}
	} else cout << "input file open fail : " << __LINE__ << endl;
	fin.close();
}

void decompress(string infile, string outfile) {
	buildTree(infile);
	
	fstream fin, fout;
	fin.open(infile, ios::in | ios::binary);
	if (fin.is_open()) {
		fout.open(outfile, ios::out | ios::binary);
		if (fout.is_open()) {
			string junk;
			while (fin >> junk && junk != "Compression");
			fin >> junk >> junk >> compressBit >> junk >> junk >> junk >> junk;
			getline(fin, junk);		//'\n','\r',...
			unsigned long long cnt = 0;
			char c;
			string s = "";
			while (fin.get(c)) {
				int index = 0;
				bitset<8> tmp(c);	//char to binary
				s = s + tmp.to_string();
				cnt += 8;
				treeNode *cur = root;
				
				if (cnt > compressBit - 8) s = s.substr(0, s.length() + (compressBit % 8));		//xxxx0000
				while (index < s.length()) {
					//cout << __LINE__ << endl;
					cur = (s[index] == '0' ? cur->left : cur->right);
					//cout << __LINE__ << endl;
					index++;
					if (cur->leafNode) {
						fout << (unsigned char)(cur->data);
						s = (index == s.length() ? "" : &s[index]);
						index = 0;
						cur = root;
					}
					//cout << __LINE__ << endl;
				}
			}
		} else cout << "output decompressed file fail : " << __LINE__ << endl;
		fout.close();
	} else cout << "input file open fail : " << __LINE__ << endl;
	fin.close();
}
