#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

/**
 * 用來儲存「no weak [X]」區塊的資料：
 *   - blockIndex: "[X]" 中的 X (可不需要存中括號也行)
 *   - rules: 例如 "{ 1 } -> 0", "{ 2 } -> 0", ... 全部以原字串存入
 *   - chromosomeFitness: 解析 "chromosomes & fitness" 後的 "111 8", "000 7", ...
 *
 *   注意：在判斷是否 "相同" 時，我們只比對 rules，不比對 chromosomeFitness。
 */
struct WeakBlock {
    string blockIndex;  
    vector<string> rules; 
    vector<pair<string, int>> chromosomeFitness; 

    // 只用 rules 判斷相等(重複)
    bool operator==(const WeakBlock &other) const {
        // 1) 先比 rules.size()
        if (rules.size() != other.rules.size()) return false;
        // 2) 再逐行比對內容
        for (size_t i = 0; i < rules.size(); i++) {
            if (rules[i] != other.rules[i]) return false;
        }
        // 不比對 chromosomeFitness
        return true;
    }
};

//----------------------------------------------------------------
// 幫助函式: 判斷是否是一行形如 "----- no weak [X] -----"
//           取出 X (e.g. "0", "1", "2")
//----------------------------------------------------------------
bool isNoWeakHeader(const string &line, string &indexOut) {
    // 範例: "----- no weak [0] -----"
    size_t startPos = line.find("----- no weak [");
    if (startPos == string::npos) return false;
    size_t endPos = line.find("] -----", startPos);
    if (endPos == string::npos) return false;

    size_t prefixLen = 15; // length of "----- no weak ["
    size_t xBegin = startPos + prefixLen;
    indexOut = line.substr(xBegin, endPos - xBegin);
    return true;
}

//----------------------------------------------------------------
// 幫助函式: 判斷是否是一行形如 "{ 1 2 } -> 0" 的規則
//----------------------------------------------------------------
bool isRuleLine(const string &line) {
    // 簡易判斷：只要包含 "->" 就視為規則
    return (line.find("->") != string::npos);
}

//----------------------------------------------------------------
// 幫助函式: 若該行形如 "111 8" 或 "000 7"，則 parse 成 (chromosome, fitness)
//           只接受二段，以 stringstream 簡易實作
//----------------------------------------------------------------
bool parseChromFitness(const string &line, pair<string,int> &outCF) {
    if (line.empty()) return false;
    stringstream ss(line);
    string chrom;
    int fit;
    if (!(ss >> chrom >> fit)) {
        return false; // parse 失敗
    }
    // 檢查 chrom 是否只包含 '0' or '1' (可自行拿掉或保留)
    for (char c : chrom) {
        if (c != '0' && c != '1') {
            return false;
        }
    }
    outCF = {chrom, fit};
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <txt file name>" << endl;
        return 1;
    }


    ifstream fin(argv[1]);
    if (!fin.is_open()) {
        cerr << "Error: cannot open file: " << argv[1] << endl;
        return 1;
    }

    vector<WeakBlock> blocks;
    WeakBlock currentBlock;
    bool readingBlock = false;    // 是否在讀某個 "no weak" 區塊
    bool readingFitness = false;  // 是否在讀 "chromosomes & fitness" 段

    string line;
    while (true) {
        if (!getline(fin, line)) {
            break; // EOF
        }

        // 檢查是否新區塊 header
        string indexStr;
        if (isNoWeakHeader(line, indexStr)) {
            // 表示上一區塊結束，把它放進 blocks
            if (readingBlock) {
                blocks.push_back(currentBlock);
            }
            // 開始新的區塊
            readingBlock = true;
            readingFitness = false;
            currentBlock = WeakBlock();
            currentBlock.blockIndex = indexStr;
        }
        else if (readingBlock) {
            // 已經在一個區塊中
            if (line.find("chromosomes & fitness") != string::npos) {
                readingFitness = true;
            }
            else if (readingFitness) {
                // 嘗試 parse 成 (chrom, fit)
                pair<string,int> cf;
                if (parseChromFitness(line, cf)) {
                    currentBlock.chromosomeFitness.push_back(cf);
                } 
                // 如果 parse 失敗就忽略(空行或其他)
            }
            else {
                // 檢查是否是 rule
                if (isRuleLine(line)) {
                    currentBlock.rules.push_back(line);
                }
                // 若不是 rule, 也不是 fitness, 可能是空行 -> 忽略
            }
        }
        // 若尚未讀到 "no weak" header, line 直接略過
    }

    // 若最後一個 block 還沒收攏，就 push_back
    if (readingBlock) {
        blocks.push_back(currentBlock);
    }

    fin.close();

    //----------------------------------------------------------------
    // 以 rules 判斷重複
    // 若兩個 blocks 的 rules 一模一樣，視為同一個 block，只保留第一個
    //----------------------------------------------------------------
    vector<WeakBlock> uniqueBlocks;
    for (auto &b : blocks) {
        bool foundSame = false;
        for (auto &ub : uniqueBlocks) {
            // 只比 rules，不比 chromosomeFitness
            if (b == ub) {
                foundSame = true;
                break;
            }
        }
        if (!foundSame) {
            uniqueBlocks.push_back(b);
        }
    }

    //----------------------------------------------------------------
    // 輸出：對每個 unique block，都印出 rules & fitness
    // 雖然過濾時不比對 fitness，但仍保留並印出
    //----------------------------------------------------------------
    int count = 0;
    for (auto &b : uniqueBlocks) {
        count++;
        cout << "----- no weak [" << b.blockIndex << "] (unique #" 
             << count << ") -----\n";

        // 規則
        for (auto &rule : b.rules) {
            cout << rule << "\n";
        }
        cout << "\n";

        // fitness (若有)
        if (!b.chromosomeFitness.empty()) {
            cout << "chromosomes & fitness\n";
            for (auto &cf : b.chromosomeFitness) {
                cout << cf.first << " " << cf.second << "\n";
            }
            cout << "\n";
        }
    }

    return 0;
}



