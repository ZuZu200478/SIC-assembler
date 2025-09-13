#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
using namespace std;

string intToHex(int num, int width) {
    stringstream ss;
    ss << setw(width) << setfill('0') << hex << uppercase << num;
    return ss.str();
}
struct Node {
    pair<string, int> data;  
    Node* left;
    Node* right;

    Node(pair<string, int> table) : data(table), left(NULL), right(NULL) {}
};

void insertNode(Node*& root, pair<string, int> table) 
{
    if (root == NULL) 
	{ 
        root = new Node(table);
        return;
    }

    if (table.first < root->data.first) 
	{
        insertNode(root->left, table);
    } 
	else if (table.first > root->data.first) 
	{
        insertNode(root->right, table);
    }
}

int search(Node* root, const string& key) {
    if (root == NULL) {
        return -1;  // �䤣��Ÿ��A��^-1
    }
    if (root->data.first == key) {
        return root->data.second;  // ���Ÿ��A��^�������a�}
    }
    if (key < root->data.first) {
        return search(root->left, key);  // �V���l��d��
    } else {
        return search(root->right, key); // �V�k�l��d��
    }
}
void printTree(Node* root) 
{
    if (root == NULL) return;
    printTree(root->left);
    cout << root->data.first << " " << intToHex(root->data.second , 4)<< endl; 
    printTree(root->right);
}

Node* root = NULL; 


// �إ�hashtable
class MyHashMap {
public:
    int modulo = 31;//�]�w��l�ƶq 
    vector<list<pair<string, string>>> table;

    MyHashMap() {
        table.resize(modulo + 1);//��l�j�p 
    }

    int hashFunction(const string& key) {//key ascill�����[�_��*7��31�o�쪺�l�ƴN�O��l�s�񪺦�l 
        int hash_value = 0;
        for (char ch : key) {
            hash_value = (hash_value * 7 + ch) % modulo;  // �ϥ� 31 �@������
        }
        return hash_value;
    }

    void put(string key, string value) {
        int index = hashFunction(key);
        for (auto &element : table[index]) {
            if (element.first == key) {
                element.second = value;
                return;
            }
        }
        table[index].emplace_back(make_pair(key, value));
    }

    string get(string key) {
        int index = hashFunction(key);
        for (auto  &element : table[index]) {
            if (element.first == key) {
                return element.second;
            }
        }
        return " "; // �Y�����A��^�Ŧr��
    }
    
void printAllBuckets() {//��X�Ҧ���l 
	    ofstream out("out.txt");//��X���G��out.txt 
	    //���ƪ����Ҧ����e�p�U�G
        for (int i = 0; i < table.size(); ++i) {
            if (!table[i].empty()) { // �u���L�D�ű�
                out << "Bucket " << i << ": ";
                for (const auto& element : table[i]) {
                    out << "(" << element.first << ", " << element.second << ") ";
                }
                out << endl;
            }
        }
    out.close();
    }
};

MyHashMap hashMap;  // �����ܼơA�x�s OPCODE ��������
void createHashtable() {
    ifstream in("opcode.txt");  // ���}�ɮ�
    if (!in) {
        cerr << "�L�k�}���ɮ�" << endl;
        return;
    }

    string key;
    string value;
    while (in >> key >> value) {  // Ū���ɮפ��� (key, value)
        hashMap.put(key, value);  // �s��� MyHashMap ��
    }

    in.close();  // �����ɮ�
}


//pass1
void pass1() {
    ifstream inputFile("source.txt"); // ���}�����
    ofstream intermediateFile("intermediate.txt"); // ��X�������
    string label, opcode, operand;
    int LOCCTR = 0;
    int startingAddress = 0;

    // Ū���Ĥ@��
    inputFile >> label >> opcode >> operand;

    // �B�z START ���O
    if (opcode == "START") {
        startingAddress = stoi(operand, nullptr, 16); // �ѪR�ާ@�ƨ���o�_�l�a�}�]�Q���i��^
        LOCCTR = startingAddress; // �]�m LOCCTR ���_�l�a�}
        intermediateFile << intToHex(LOCCTR, 4) << " " << label << " " << opcode << " " << operand << endl;
    } else {
        LOCCTR = 0;
    }

    // �B�z�����
    while (opcode != "END" && inputFile >> label >> opcode >> operand) {
        intermediateFile << intToHex(LOCCTR, 4) << " " << label << " " << opcode << " " << operand << endl;

        if (!label.empty() && label[0] != '.') {
            insertNode(root, {label, LOCCTR}); // ���J�G�e��
        }

        // �ھھާ@�X�p�� LOCCTR
        try {
            if (opcode == "WORD") {
                LOCCTR += 3;
            } else if (opcode == "RESW") {
                LOCCTR += 3 * stoi(operand); 
            } else if (opcode == "RESB") {
                LOCCTR += stoi(operand); 
            } else if (opcode == "BYTE") {
                if (operand[0] == 'C') {
                    LOCCTR += (operand.length() - 3); 
                } else if (operand[0] == 'X') {
                    LOCCTR += (operand.length() - 3) / 2; 
                } else {
                    cout << "Error: Invalid BYTE format: " << operand << endl;
                }
            } else if (hashMap.get(opcode) != " ") {
                LOCCTR += 3; 
            }
        } catch (const std::invalid_argument& e) {
            cout << "Error in operand conversion: " << operand << endl;
        }
    }

    int programSize = LOCCTR - startingAddress;
	intermediateFile << "ProgramSize " << intToHex(programSize, 4) << endl;
    
    printTree(root);
    inputFile.close();
    intermediateFile.close();
}


void pass2() {
    // ��??�J�M?�X���
    ifstream intermediateFile("intermediate.txt");  // ?����?���
    ofstream objectFile("objectcode.txt");         // ?�X��?�N?�� objectcode.txt
    ofstream objectProgram("objectprogram.txt");   // ?�X��?�{�Ǩ� objectprogram.txt

    // ?�d���O�_���\��?
    if (!intermediateFile) {
        cerr << "Error opening intermediate.txt" << endl;
        return;
    }

    // ��l�ƥ��n��?�q
    int LOCCTR = 0, startingAddress = 0, programSize = 0;  // LOCCTR�G?�e��m�AstartingAddress�G�{�ǰ_�l�a�}�AprogramSize�G�{�Ǥj�p
    string label, opcode, operand, objectCode, textRecord = "";  // label�G??�Aopcode�G�ާ@?�Aoperand�G�ާ@?�AobjectCode�G��?�N?�AtextRecord�G�奻??
    bool errorFlag = false;  // ???��
    int textRecordStart = 0, textRecordLength = 0;  // textRecordStart�G�奻??���_�l�a�}�AtextRecordLength�G�奻???��

    // Pass 2 ?�l�e�A?�� "ProgramSize"
    string line;
    while (getline(intermediateFile, line)) {
        // �d��]�t "ProgramSize" ����A�}�����{�Ǥj�p
        if (line.find("ProgramSize") != string::npos) {
            programSize = stoi(line.substr(line.find(" ") + 1), nullptr, 16);  // ?���Q��?��{�Ǥj�p
            break;  // ���Z���X�`?
        }
    }

    // ?����?���m����?�l?�A��??�l?�z�C�@��
    intermediateFile.seekg(0);
    intermediateFile >> hex >> LOCCTR >> label >> opcode >> operand;  // ?���Ĥ@��A?�d�ާ@?�O�_? "START"

    // �p�G�ާ@?�O "START"�A?��l�Ƶ{�ǰ_�l�a�}�M�奻??���_�l�a�}
    if (opcode == "START") {
        startingAddress = LOCCTR;  // ?�m�{�ǰ_�l�a�}
        textRecordStart = LOCCTR;  // ?�m�奻??���_�l�a�}
        objectProgram << "H^" << label << "^" << intToHex(LOCCTR, 6) << "^" << intToHex(programSize, 6) << endl;  // ?�J��?�{��???
        intermediateFile >> hex >> LOCCTR >> label >> opcode >> operand;  // ?���U�@��
    }

    // ?�z�C�@�檽�� "END" �ާ@?
    while (opcode != "END") {
        objectCode.clear();  // �M�ť�?�N?
        
        if (hashMap.get(opcode) != " ") {  // �d��ާ@?�O�_�b���ƪ�
            objectCode = hashMap.get(opcode);  // ?���ާ@?����?�N?
            if (!operand.empty()) {  // �p�G�ާ@?��?��
                int symbolAddress = search(root, operand);  // �d��ާ@?����?�a�}
                if (symbolAddress != -1) {
                    objectCode += intToHex(symbolAddress, 4);  // �K�[��?�a�}���?�N?
                } else if (operand[operand.length() - 1] == 'X' && operand[operand.length() - 2] == ',') {  // ?�d�O�_?���ޱH�s��
                    size_t commaPos = operand.find(',');
                    string part1 = operand.substr(0, commaPos);
                    objectCode += intToHex(search(root, part1) + 0x8000, 4);  // �K�[���ޱH�s������?�N?
                } else if (opcode == "RSUB") {
                    	objectCode += "0000";  // �p�G��?���w?�A�K�[�Ū���?�N?
                    errorFlag = true;  // ?�m???��
                }
            } else {
                objectCode += "0000";  // �p�G?���ާ@?�A�K�[�Ū���?�N?
            }
        } else if (opcode == "WORD") {
            objectCode = intToHex(stoi(operand), 6);  // ?�z "WORD" �ާ@?
        } else if (opcode == "BYTE") {
            if (operand[0] == 'C') {
                for (size_t i = 2; i < operand.size() - 1; ++i) {
                    objectCode += intToHex((int)operand[i], 2);  // ?�z�r�ű`�q
                }
            } else if (operand[0] == 'X') {
                objectCode += operand.substr(2, operand.size() - 3);  // ?�z�Q��?��`�q
            }
        }
        
        // �ͦ���?�{��
        if (textRecordLength + (objectCode.size()/2)  > 30 || objectCode.empty()) {  // �p�G?�e�奻??�w?�A�Υ�?�N??��
            // ?�J?�e���奻??���?�{�Ǥ��
            if (!textRecord.empty()) {
                objectProgram << "T^" << intToHex(textRecordStart, 6) << "^" << intToHex(textRecordLength, 2) << "^" << textRecord << endl;
                textRecord.clear();  // �M�Ť奻??
            }
            textRecordStart = LOCCTR;  // ?�l�s���奻??
            textRecordLength = 0;  // ���m�奻???��
        }

        // ?��?�N?�l�[��奻??��
        if (!objectCode.empty()) {
            if (!textRecord.empty()) textRecord += "^";  // �p�G�奻??��?�šA�K�[���j��
            textRecord += objectCode;  // �l�[��?�N?
            textRecordLength += objectCode.length() / 2;  // �W�[�奻???��
        }

        // ?�J?�e����?�N?���
        objectFile << intToHex(LOCCTR, 4) << setw(8) << label << setw(8) << opcode << setw(12) << operand << setw(8) << objectCode << endl;
        intermediateFile >> hex >> LOCCTR >> label >> opcode >> operand;  // ?���U�@��
    }

    // ?�J�̦Z�@?�奻??�]�p�G���^
    if (!textRecord.empty()) {
        objectProgram << "T^" << intToHex(textRecordStart, 6) << "^" << intToHex(textRecordLength, 2) << "^" << textRecord << endl;
    }
    
    // ?�J?��??
    objectProgram << "E^" << intToHex(startingAddress, 6) << endl;

    // ??���
    intermediateFile.close();
    objectFile.close();
    objectProgram.close();
    
    // �p�G?��??�A?�X??�H��
    if (errorFlag) {
        cerr << "Errors encountered during Pass 2." << endl;
    }
}



int main(int argc, char** argv) {
	createHashtable();
    hashMap.printAllBuckets();
	pass1();
	pass2();
	return 0;
}
