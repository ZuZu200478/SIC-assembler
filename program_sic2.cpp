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
        return -1;  // 找不到符號，返回-1
    }
    if (root->data.first == key) {
        return root->data.second;  // 找到符號，返回對應的地址
    }
    if (key < root->data.first) {
        return search(root->left, key);  // 向左子樹查找
    } else {
        return search(root->right, key); // 向右子樹查找
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


// 建立hashtable
class MyHashMap {
public:
    int modulo = 31;//設定桶子數量 
    vector<list<pair<string, string>>> table;

    MyHashMap() {
        table.resize(modulo + 1);//桶子大小 
    }

    int hashFunction(const string& key) {//key ascill全部加起來*7除31得到的餘數就是桶子存放的位子 
        int hash_value = 0;
        for (char ch : key) {
            hash_value = (hash_value * 7 + ch) % modulo;  // 使用 31 作為乘數
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
        return " "; // 若未找到，返回空字串
    }
    
void printAllBuckets() {//輸出所有桶子 
	    ofstream out("out.txt");//輸出結果到out.txt 
	    //哈希表中的所有桶內容如下：
        for (int i = 0; i < table.size(); ++i) {
            if (!table[i].empty()) { // 只打印非空桶
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

MyHashMap hashMap;  // 全域變數，儲存 OPCODE 對應的值
void createHashtable() {
    ifstream in("opcode.txt");  // 打開檔案
    if (!in) {
        cerr << "無法開啟檔案" << endl;
        return;
    }

    string key;
    string value;
    while (in >> key >> value) {  // 讀取檔案中的 (key, value)
        hashMap.put(key, value);  // 存放到 MyHashMap 中
    }

    in.close();  // 關閉檔案
}


//pass1
void pass1() {
    ifstream inputFile("source.txt"); // 打開源文件
    ofstream intermediateFile("intermediate.txt"); // 輸出中間文件
    string label, opcode, operand;
    int LOCCTR = 0;
    int startingAddress = 0;

    // 讀取第一行
    inputFile >> label >> opcode >> operand;

    // 處理 START 指令
    if (opcode == "START") {
        startingAddress = stoi(operand, nullptr, 16); // 解析操作數並獲得起始地址（十六進制）
        LOCCTR = startingAddress; // 設置 LOCCTR 為起始地址
        intermediateFile << intToHex(LOCCTR, 4) << " " << label << " " << opcode << " " << operand << endl;
    } else {
        LOCCTR = 0;
    }

    // 處理後續行
    while (opcode != "END" && inputFile >> label >> opcode >> operand) {
        intermediateFile << intToHex(LOCCTR, 4) << " " << label << " " << opcode << " " << operand << endl;

        if (!label.empty() && label[0] != '.') {
            insertNode(root, {label, LOCCTR}); // 插入二叉樹
        }

        // 根據操作碼計算 LOCCTR
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
    // 打??入和?出文件
    ifstream intermediateFile("intermediate.txt");  // ?取中?文件
    ofstream objectFile("objectcode.txt");         // ?出目?代?到 objectcode.txt
    ofstream objectProgram("objectprogram.txt");   // ?出目?程序到 objectprogram.txt

    // ?查文件是否成功打?
    if (!intermediateFile) {
        cerr << "Error opening intermediate.txt" << endl;
        return;
    }

    // 初始化必要的?量
    int LOCCTR = 0, startingAddress = 0, programSize = 0;  // LOCCTR：?前位置，startingAddress：程序起始地址，programSize：程序大小
    string label, opcode, operand, objectCode, textRecord = "";  // label：??，opcode：操作?，operand：操作?，objectCode：目?代?，textRecord：文本??
    bool errorFlag = false;  // ???志
    int textRecordStart = 0, textRecordLength = 0;  // textRecordStart：文本??的起始地址，textRecordLength：文本???度

    // Pass 2 ?始前，?取 "ProgramSize"
    string line;
    while (getline(intermediateFile, line)) {
        // 查找包含 "ProgramSize" 的行，并提取程序大小
        if (line.find("ProgramSize") != string::npos) {
            programSize = stoi(line.substr(line.find(" ") + 1), nullptr, 16);  // ?取十六?制的程序大小
            break;  // 找到后跳出循?
        }
    }

    // ?文件指?重置到文件?始?，准??始?理每一行
    intermediateFile.seekg(0);
    intermediateFile >> hex >> LOCCTR >> label >> opcode >> operand;  // ?取第一行，?查操作?是否? "START"

    // 如果操作?是 "START"，?初始化程序起始地址和文本??的起始地址
    if (opcode == "START") {
        startingAddress = LOCCTR;  // ?置程序起始地址
        textRecordStart = LOCCTR;  // ?置文本??的起始地址
        objectProgram << "H^" << label << "^" << intToHex(LOCCTR, 6) << "^" << intToHex(programSize, 6) << endl;  // ?入目?程序???
        intermediateFile >> hex >> LOCCTR >> label >> opcode >> operand;  // ?取下一行
    }

    // ?理每一行直到 "END" 操作?
    while (opcode != "END") {
        objectCode.clear();  // 清空目?代?
        
        if (hashMap.get(opcode) != " ") {  // 查找操作?是否在哈希表中
            objectCode = hashMap.get(opcode);  // ?取操作?的目?代?
            if (!operand.empty()) {  // 如果操作?不?空
                int symbolAddress = search(root, operand);  // 查找操作?的符?地址
                if (symbolAddress != -1) {
                    objectCode += intToHex(symbolAddress, 4);  // 添加符?地址到目?代?
                } else if (operand[operand.length() - 1] == 'X' && operand[operand.length() - 2] == ',') {  // ?查是否?索引寄存器
                    size_t commaPos = operand.find(',');
                    string part1 = operand.substr(0, commaPos);
                    objectCode += intToHex(search(root, part1) + 0x8000, 4);  // 添加索引寄存器的目?代?
                } else if (opcode == "RSUB") {
                    	objectCode += "0000";  // 如果符?未定?，添加空的目?代?
                    errorFlag = true;  // ?置???志
                }
            } else {
                objectCode += "0000";  // 如果?有操作?，添加空的目?代?
            }
        } else if (opcode == "WORD") {
            objectCode = intToHex(stoi(operand), 6);  // ?理 "WORD" 操作?
        } else if (opcode == "BYTE") {
            if (operand[0] == 'C') {
                for (size_t i = 2; i < operand.size() - 1; ++i) {
                    objectCode += intToHex((int)operand[i], 2);  // ?理字符常量
                }
            } else if (operand[0] == 'X') {
                objectCode += operand.substr(2, operand.size() - 3);  // ?理十六?制常量
            }
        }
        
        // 生成目?程序
        if (textRecordLength + (objectCode.size()/2)  > 30 || objectCode.empty()) {  // 如果?前文本??已?，或目?代??空
            // ?入?前的文本??到目?程序文件
            if (!textRecord.empty()) {
                objectProgram << "T^" << intToHex(textRecordStart, 6) << "^" << intToHex(textRecordLength, 2) << "^" << textRecord << endl;
                textRecord.clear();  // 清空文本??
            }
            textRecordStart = LOCCTR;  // ?始新的文本??
            textRecordLength = 0;  // 重置文本???度
        }

        // ?目?代?追加到文本??中
        if (!objectCode.empty()) {
            if (!textRecord.empty()) textRecord += "^";  // 如果文本??不?空，添加分隔符
            textRecord += objectCode;  // 追加目?代?
            textRecordLength += objectCode.length() / 2;  // 增加文本???度
        }

        // ?入?前行到目?代?文件
        objectFile << intToHex(LOCCTR, 4) << setw(8) << label << setw(8) << opcode << setw(12) << operand << setw(8) << objectCode << endl;
        intermediateFile >> hex >> LOCCTR >> label >> opcode >> operand;  // ?取下一行
    }

    // ?入最后一?文本??（如果有）
    if (!textRecord.empty()) {
        objectProgram << "T^" << intToHex(textRecordStart, 6) << "^" << intToHex(textRecordLength, 2) << "^" << textRecord << endl;
    }
    
    // ?入?束??
    objectProgram << "E^" << intToHex(startingAddress, 6) << endl;

    // ??文件
    intermediateFile.close();
    objectFile.close();
    objectProgram.close();
    
    // 如果?生??，?出??信息
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
