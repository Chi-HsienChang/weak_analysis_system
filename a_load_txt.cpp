#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <algorithm>
#include <set>
#include <random>
#include <omp.h>  // OpenMP 用於多核心處理
#include <sys/statvfs.h>  // 獲取磁碟空間資訊
#include <cstdlib>  // exit()
#include <unistd.h> // sleep()
#include <fstream>
#include <sstream>
#include <map>
#include <regex>
using namespace std;

#define DEBUG 1

// **函式：檢查磁碟剩餘空間**
bool check_disk_space(const string& path, double min_free_gb) {
    struct statvfs stat;

    if (statvfs(path.c_str(), &stat) != 0) {
        cerr << "Error: Failed to check disk space!" << endl;
        return false; // 讀取失敗，避免誤判
    }

    // 計算剩餘空間 (GB)
    double free_space_gb = (stat.f_bavail * stat.f_frsize) / (1024.0 * 1024.0 * 1024.0);
    
    if (free_space_gb < min_free_gb) {
        cerr << "Warning: Low disk space! Only " << free_space_gb << "GB left. Stopping program." << endl;
        return false; // 返回 false，表示空間不足
    }

    return true; // 空間足夠，繼續運行
}

std::pair<std::set<char>, std::vector<std::string>> check_constraint_epi(int target_index, auto combination, auto enumeration, auto chromosomes){
    std::set<char> values_at_v;
    std::vector<std::string> highest_fitness_chromosomes;
    
    double max_fitness = -1;
    for (const auto& chromosome : chromosomes) 
    {
        bool fit_constraint = true;
        int enumeration_index = 0;
        for (int i : combination)
        {
            // cout << "note chromosome.first[i]: " << chromosome.first[i] << endl;
            // cout << "enumeration[i]: " << enumeration[enumeration_index] << endl;

            if (chromosome.first[i] - '0' != enumeration[enumeration_index]){
                fit_constraint = false;
                // cout << "不一樣" << chromosome.first[i] << endl;
                break;
            }
            // else{
            //     cout << "!一樣!" << chromosome.first[i] << endl;
            // }

            enumeration_index++;
        }


        if (fit_constraint) 
        {
            double fitness = chromosome.second;
            if (fitness > max_fitness) {
                max_fitness = fitness;
                values_at_v.clear();
                highest_fitness_chromosomes.clear();
                values_at_v.insert(chromosome.first[target_index]);
                highest_fitness_chromosomes.push_back(chromosome.first);
            } else if (fitness == max_fitness) {
                values_at_v.insert(chromosome.first[target_index]);
                highest_fitness_chromosomes.push_back(chromosome.first);
            } 

        }         
    
    }

    return {values_at_v, highest_fitness_chromosomes};

}

bool check_constrained_optima_epi(int target_index, auto combination, auto enumeration_original, auto combination_wo, auto enumeration_wo, auto chromosomes){

    auto constrained_optima_original = check_constraint_epi(target_index, combination, enumeration_original, chromosomes);
    auto constrained_optima_flip = check_constraint_epi(target_index, combination_wo, enumeration_wo, chromosomes);


    if (constrained_optima_original.first != constrained_optima_flip.first) {

        if (!constrained_optima_original.first.empty() && !constrained_optima_flip.first.empty()){ 
            return true;
        }
        else{
            return false;
        }
    }else{
        return false;
    }
}

int check_epi(int target_index, auto combination, auto enumerations, auto chromosomes) {

    int condition_holds = 0;
    for (int condition_index = 0; condition_index < combination.size(); condition_index++)
    {
        for (auto& enumeration : enumerations) // combination = [1, 2] and enumerations = { [0, 0], [0, 1], [1, 0], [1, 1]}
        { 
            auto enumeration_original = enumeration;
            // enumeration[condition_index] = 1 - enumeration[condition_index]; // flip bit  

            auto combination_wo = combination;
            auto enumeration_wo = enumeration;
            combination_wo.erase(combination_wo.begin() + condition_index);
            enumeration_wo.erase(enumeration_wo.begin() + condition_index);
            
            bool not_equal;
            not_equal = check_constrained_optima_epi(target_index, combination, enumeration_original, combination_wo, enumeration_wo, chromosomes);

            if (not_equal)
            {
                condition_holds++;
                break;
            }       
        }

    }

    if (condition_holds == combination.size())
        {
            if (DEBUG){  
                cout << "{ ";
                for (const auto& elem : combination) {
                    cout << elem << " ";
                }
                cout << "} -> "<< target_index << endl;           
            }

            return 1;
        }

    return 0;
}

bool isSubset(const std::vector<int>& subset, const std::vector<int>& set) {
    for (const auto& elem : subset) {
        if (std::find(set.begin(), set.end(), elem) == set.end()) {
            // 如果找不到元素，則返回 false
            return false;
        }
    }
    // 所有元素都被找到，返回 true
    return true;
}

std::vector<std::vector<int>> generateBinarySequences(int n) {
    int totalSequences = 1 << n;  // 2^n
    std::vector<std::vector<int>> allSequences;

    for (int i = 0; i < totalSequences; ++i) {
        std::vector<int> sequence(n);

        for (int j = 0; j < n; ++j) {
            // 檢查第j位是否為1
            sequence[n - 1 - j] = (i >> j) & 1;
        }

        // 將序列添加到所有序列的vector中
        allSequences.push_back(sequence);
    }

    return allSequences;
}

std::vector<std::vector<int>> generateCombinations(int n, int k, int target_index) {
    std::vector<int> elements;

    // 建立不包含 target_index 的元素清單
    for (int i = 0; i <= n; ++i) { // n 表示實際範圍
        if (i != target_index) {
            elements.push_back(i);
        }
    }

    std::vector<int> bitmask(k, 1);            // 創建 k 個 1
    bitmask.resize(elements.size(), 0);       // 後面填充其餘為 0

    std::vector<std::vector<int>> combinations;

    do {
        std::vector<int> currentCombination;
        for (size_t i = 0; i < elements.size(); ++i) {
            if (bitmask[i]) {
                currentCombination.push_back(elements[i]);
            }
        }
        combinations.push_back(currentCombination);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end())); // 生成下一個排列

    return combinations;
}

std::pair<std::set<char>, std::vector<std::string>> check_constraint(int target_index, auto combination, auto enumeration, auto chromosomes){
    std::set<char> values_at_v;
    std::vector<std::string> highest_fitness_chromosomes;

    double max_fitness = -1;
    for (const auto& chromosome : chromosomes) 
    {
        bool fit_constraint = true;
        int enumeration_index = 0;
        for (int i : combination)
        {
            if (chromosome.first[i] - '0' != enumeration[enumeration_index]){
                fit_constraint = false;
                // cout << "不一樣" << chromosome.first[i] << endl;
                break;
            }
            // else{
            //     cout << "!一樣!" << chromosome.first[i] << endl;
            // }

            enumeration_index++;
        }


        if (fit_constraint) 
        {
            double fitness = chromosome.second;
            if (fitness > max_fitness) {
                max_fitness = fitness;
                values_at_v.clear();
                highest_fitness_chromosomes.clear();
                values_at_v.insert(chromosome.first[target_index]);
                highest_fitness_chromosomes.push_back(chromosome.first);
            } else if (fitness == max_fitness) {
                values_at_v.insert(chromosome.first[target_index]);
                highest_fitness_chromosomes.push_back(chromosome.first);
            } 
        }         
    
    }

    return {values_at_v, highest_fitness_chromosomes};

}


bool check_constrained_optima(int target_index, auto combination, auto enumeration_original, auto combination_wo, auto enumeration_wo, auto chromosomes){

    auto constrained_optima_original = check_constraint(target_index, combination, enumeration_original, chromosomes);
    auto constrained_optima_flip = check_constraint(target_index, combination_wo, enumeration_wo, chromosomes);

    if (constrained_optima_original.first != constrained_optima_flip.first) {

        if (!constrained_optima_original.first.empty() && !constrained_optima_flip.first.empty()){  
            return true;
        }
        else{
            return false;
        }
    }else{     
        return false;
    }
}

std::vector<int> count_epi(int L, int target_index, auto chromosomes, const string& method)
{
    std::vector<std::vector<std::vector<int>>> weak_epi_set(L);
    std::vector<int> weak_epi_count(L, 0); 
    

    for (int epi_size = 1; epi_size < L; epi_size++)
    {  
        auto combinations = generateCombinations(L-1, epi_size, target_index); // combinations = { [1, 2], [1, 3], [2, 3] }

        for (auto& combination : combinations) // combination = [1, 2]
        { 
            bool not_find_smaller_epi;

            if (epi_size == 1)
            {
                not_find_smaller_epi = true;           
            }else{
                not_find_smaller_epi = true;  
                int smaller_epi_size = epi_size;
                bool is_subset;
                while(not_find_smaller_epi && smaller_epi_size>=1)
                {   
                    for (auto& previous : weak_epi_set[smaller_epi_size-1]) 
                    {
                        is_subset = isSubset(previous, combination);
                        not_find_smaller_epi = !is_subset;
                        if(is_subset) break;
                    }
                    smaller_epi_size--;
                }
            }

            if(not_find_smaller_epi)
            {
                auto enumerations = generateBinarySequences(epi_size); // enumerations = { [0, 0], [0, 1], [1, 0], [1, 1] }    

                int result = check_epi(target_index, combination, enumerations, chromosomes);
                weak_epi_count[epi_size] += result;
                if (result)
                {
                    // weak_epi_set[epi_size].push_back(combination);
                    // break;
                }
            
            
            }
        }
    }

    return weak_epi_count;
}

int check_weak(int target_index, auto combination, auto enumerations, auto chromosomes) {

    // auto enumeration_original = enumeration;
    int condition_holds = 0;
    for (int condition_index = 0; condition_index < combination.size(); condition_index++)
    {
        for (auto& enumeration : enumerations) // combination = [1, 2] and enumerations = { [0, 0], [0, 1], [1, 0], [1, 1]}
        { 
            auto enumeration_original = enumeration;
            // enumeration[condition_index] = 1 - enumeration[condition_index]; // flip bit  

            auto combination_wo = combination;
            auto enumeration_wo = enumeration;
            combination_wo.erase(combination_wo.begin() + condition_index);
            enumeration_wo.erase(enumeration_wo.begin() + condition_index);
            
            bool not_equal;
            not_equal = check_constrained_optima(target_index, combination, enumeration_original, combination_wo, enumeration_wo, chromosomes);

            if (not_equal)
            {
                condition_holds++;
                break;
            }       
        }

    }

    if (condition_holds == combination.size())
        {
            if (DEBUG){  
                // cout << "{";
                // for (const auto& elem : combination) {
                //     cout << elem << " ";
                // }
                // cout << "} -> "<< target_index << endl;           
            }
            return 1;
        }

    return 0;
}

std::vector<int> count_weak(int L, int target_index, auto chromosomes, const string& method)
{
    std::vector<std::vector<std::vector<int>>> weak_epi_set(L);
    std::vector<int> weak_epi_count(L, 0); 
    

    for (int epi_size = 1; epi_size < L; epi_size++)
    {  
        auto combinations = generateCombinations(L-1, epi_size, target_index); // combinations = { [1, 2], [1, 3], [2, 3] }

        for (auto& combination : combinations) // combination = [1, 2]
        { 
            bool not_find_smaller_epi;

            if (epi_size == 1)
            {
                not_find_smaller_epi = true;           
            }else{
                not_find_smaller_epi = true;  
                int smaller_epi_size = epi_size;
                bool is_subset;
                while(not_find_smaller_epi && smaller_epi_size>=1)
                {   
                    // cout << "previous:" << endl;
                    for (auto& previous : weak_epi_set[smaller_epi_size-1]) 
                    {
                        is_subset = isSubset(previous, combination);
                        not_find_smaller_epi = !is_subset;
                        if(is_subset) break;
                    }
                    smaller_epi_size--;
                }
            }

            if(not_find_smaller_epi)
            {
                auto enumerations = generateBinarySequences(epi_size); // enumerations = { [0, 0], [0, 1], [1, 0], [1, 1] }    

                int result = check_weak(target_index, combination, enumerations, chromosomes);
                weak_epi_count[epi_size] += result;
                if (result)
                {
                    weak_epi_set[epi_size].push_back(combination);
                    // break;
                }
            }
        }
    }

    return weak_epi_count;
}






// Sample n chromosomes randomly from all_chromosomes
vector<pair<string, double>> sample_chromosomes(const vector<pair<string, double>>& all_chromosomes, int n) {
    // Create a mutable copy of all_chromosomes
    vector<pair<string, double>> sampled_chromosomes = all_chromosomes;

    // Shuffle the vector
    random_device rd;
    mt19937 gen(rd());
    shuffle(sampled_chromosomes.begin(), sampled_chromosomes.end(), gen);

    // Resize to keep only n elements
    if (n < sampled_chromosomes.size()) {
        sampled_chromosomes.resize(n);
    }

    return sampled_chromosomes;
}

// 顯示進度條
void show_progress_bar(double progress) {
    #pragma omp critical  // 確保多執行緒時不會交錯輸出
    {
        std::cerr << "\rProgress: " << int(progress * 100.0)<< "%";
        std::cerr.flush();
    }
}

// 處理單個排列
void process_permutation(vector<pair<string, double>> chromosomes, long long &permutation_count, long long total_permutations) {
    static int no_weak_id = 0; // 計數器
    int L = chromosomes[0].first.size(); // 染色體長度
    string method = "onemax";

    bool all_no_weak = true;

    for (int target_index = 0; target_index < L; target_index++) {
        std::vector<int> weak_epi_count_results = count_weak(L, target_index, chromosomes, method);

        for (int i = 2; i < L; i++) {
            if (weak_epi_count_results[i] > 0) {
                all_no_weak = false;
                break;
            }
        }

        if (!all_no_weak) break;
    }

    if (all_no_weak) {
        #pragma omp critical  // 避免多執行緒輸出混亂
        {
            cout << "----- no weak [" << no_weak_id << "] ----- " << endl;
            no_weak_id++;

            for (int target_index = 0; target_index < L; target_index++) {
                std::vector<int> epi_count_results = count_epi(L, target_index, chromosomes, method);
            }

            // 根據 fitness 排序
            sort(chromosomes.begin(), chromosomes.end(), [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

            cout << "chromosomes & fitness" << endl;
            for (const auto& chom : chromosomes) {
                cout << chom.first << " " << chom.second << endl;
            }
            cout << endl;
        }
    }

    // 更新進度計數
    #pragma omp atomic
    permutation_count++;

    // 更新進度條
    show_progress_bar(double(permutation_count) / total_permutations);
}

//----------------- 自行依需求修改的適應值計算函式 -------------------
double calculate_fitness_emu(const string &chromosome, const string &method) {
    // 計算 '1' 的數量
    double fitness = 0.0;
    int count = 0;
    
    for (char c : chromosome) {
        if (c == '1') count++;
    }

    // 如果全部都是 '1'，回傳 chromosome.size()，否則回傳 0
    return (count == chromosome.size()) ? static_cast<double>(pow(2, chromosome.size())) : 0.0;
}


//----------------- 產生所有 2^L 染色體並計算 fitness ----------------
vector<pair<string, double>> generate_chromosomes_emu(int L, const string &method) {
    vector<pair<string, double>> chromosomes;
    // 2^L 個組合
    int num_combinations = static_cast<int>(pow(2, L));

    for (int i = 0; i < num_combinations; ++i) {
        // 用 bitset 產生二進位字串，再擷取後 L 位
        string chromosome = bitset<32>(i).to_string().substr(32 - L);

        // 計算此染色體的適應值
        double fitness = calculate_fitness_emu(chromosome, method);

        // 儲存 (染色體字串, fitness) 組
        chromosomes.push_back({chromosome, fitness});
    }
    return chromosomes;
}

// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <string>
// #include <map>
// #include <vector>
// #include <regex>
// #include <set>
// #include <algorithm>

// using namespace std;

// // 讀取並解析適應值規則的函式
// map<int, set<int>> read_fitness_rules(const string &filename) {
//     map<int, set<int>> index_to_chromosomes; // key: index, value: set<int> (避免重複)
//     ifstream file(filename);
//     string line;
//     regex pattern(R"(\{\s*([\d\s]+)\s*\}\s*->\s*(\d+))"); // 匹配 `{}` 內的數字 + `->` 右邊的 index

//     if (!file) {
//         cerr << "無法開啟檔案：" << filename << endl;
//         return index_to_chromosomes;
//     }

//     while (getline(file, line)) {
//         smatch match;
//         if (regex_search(line, match, pattern)) {
//             string chromosome_str = match[1].str();  // `{}` 內的數字
//             int index = stoi(match[2].str()); // `->` 右側的數字當作 index

//             // 解析 chromosome_str，轉換為數字集合
//             stringstream ss(chromosome_str);
//             int num;
//             while (ss >> num) {
//                 index_to_chromosomes[index].insert(num); // 存入 set<int> 避免重複
//             }
//         }
//     }

//     file.close();
//     return index_to_chromosomes;
// }

// // 輸出字典格式
// map<int, vector<int>> get_output_dict(map<int, set<int>> &index_map) {
//     map<int, vector<int>> output_dict;

//     for (const auto &group : index_map) {
//         vector<int> sorted_chromosomes(group.second.begin(), group.second.end());
//         sort(sorted_chromosomes.begin(), sorted_chromosomes.end()); // 確保數字順序
//         output_dict[group.first] = sorted_chromosomes;
//     }

//     return output_dict;
// }

// // 顯示 map 內容
// void print_output_dict(const map<int, vector<int>> &output_dict) {
//     cout << "輸出字典內容：" << endl;
//     for (const auto &pair : output_dict) {
//         cout << pair.first << " <- [";
//         for (size_t i = 0; i < pair.second.size(); i++) {
//             if (i > 0) cout << ", ";
//             cout << pair.second[i];
//         }
//         cout << "]" << endl;
//     }
// }

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <regex>
#include <set>
#include <algorithm>

using namespace std;

// 讀取並解析適應值規則的函式
map<int, set<int>> read_fitness_rules(const string &filename) {
    map<int, set<int>> index_to_chromosomes; // key: index, value: set<int> (避免重複)
    ifstream file(filename);
    string line;
    regex pattern(R"(\{\s*([\d\s]+)\s*\}\s*->\s*(\d+))"); // 匹配 `{}` 內的數字 + `->` 右邊的 index

    if (!file) {
        cerr << "無法開啟檔案：" << filename << endl;
        return index_to_chromosomes;
    }

    while (getline(file, line)) {
        smatch match;
        if (regex_search(line, match, pattern)) {
            string chromosome_str = match[1].str();  // `{}` 內的數字
            int index = stoi(match[2].str()); // `->` 右側的數字當作 index

            // 解析 chromosome_str，轉換為數字集合
            stringstream ss(chromosome_str);
            int num;
            vector<int> chromosome_numbers;
            while (ss >> num) {
                chromosome_numbers.push_back(num);
            }
            
            // 將 vector<int> 轉換為數字串（例如 `{1 2}` 變 `12`）
            sort(chromosome_numbers.begin(), chromosome_numbers.end());
            stringstream combined;
            for (int n : chromosome_numbers) {
                combined << n;
            }
            int combined_number = stoi(combined.str());

            // 存入 map，以 index 為 key，chromosome 為 value
            index_to_chromosomes[index].insert(combined_number);
        }
    }

    file.close();
    return index_to_chromosomes;
}

// 將數據存入 map<int, vector<int>>，並確保數字順序
map<int, vector<int>> get_output_dict(map<int, set<int>> &index_map) {
    map<int, vector<int>> output_dict;

    for (const auto &group : index_map) {
        vector<int> sorted_chromosomes(group.second.begin(), group.second.end());
        sort(sorted_chromosomes.begin(), sorted_chromosomes.end()); // 確保數字順序
        output_dict[group.first] = sorted_chromosomes;
    }

    return output_dict;
}

// 顯示 map 內容
void print_output_dict(const map<int, vector<int>> &output_dict) {
    cout << "輸出字典內容：" << endl;
    for (const auto &pair : output_dict) {
        cout << pair.first << " <- [";
        for (size_t i = 0; i < pair.second.size(); i++) {
            if (i > 0) cout << ", ";
            cout << pair.second[i];
        }
        cout << "]" << endl;
    }
}





int main(int argc, char* argv[]) {


    string input_filename = "input.txt"; // 輸入檔案

    // 讀取數據
    map<int, set<int>> index_map = read_fitness_rules(input_filename);
    
    // 取得輸出字典
    map<int, vector<int>> output_dict = get_output_dict(index_map);

    // 顯示輸出字典內容
    print_output_dict(output_dict);

    exit(0);


    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <Problem Length L> <Fitness Method> <Num Threads>" << endl;
        return 1;
    }

    int L = stoi(argv[1]);
    string method = argv[2];
    int num_threads = stoi(argv[3]);  // 使用者輸入要用的執行緒數量
    int n = pow(2, L);
    double min_free_gb = 1.0;
    // double min_free_gb = 366;

    omp_set_num_threads(num_threads);  // 設定 OpenMP 使用的核心數量

    // 取得全部 2^L 染色體
    vector<pair<string, double>> base_chromosomes = generate_chromosomes_emu(L, method);

    // cout << "chromosomes & fitness" << endl;
    // for (const auto& chom : base_chromosomes) {
    //     cout << chom.first << " " << chom.second << endl;
    // }
    // cout << endl;

   

    // 找出 "最佳解" (fitness 最大)
    auto it_best = max_element(
        base_chromosomes.begin(), base_chromosomes.end(),
        [](auto &a, auto &b) { return a.second < b.second; }
    );
    auto best_chromosome = *it_best;
    base_chromosomes.erase(it_best);

    // 排序剩下的染色體（為了 `next_permutation`）
    sort(base_chromosomes.begin(), base_chromosomes.end(),
         [](auto &a, auto &b) {
             if (a.second != b.second) return a.second < b.second; 
             return a.first < b.first;
         });

    long long permutation_count = 0;
    long long total_permutations = 1;  // 計算排列數
    for (int i = 2; i <= base_chromosomes.size(); i++) {
        total_permutations *= i;
    }



    // 直接在 next_permutation 內部並行處理
    #pragma omp parallel
    {
        vector<pair<string, double>> local_chromosomes = base_chromosomes;

        do {

            // **在每次排列計算前，檢查磁碟空間**
            if (!check_disk_space("/", min_free_gb)) {
                cerr << "Stopping computation due to low disk space." << endl;
                exit(1); // 結束程式
            }


            vector<pair<string, double>> this_perm;
            this_perm.push_back(best_chromosome);
            for (auto &c : local_chromosomes) {
                this_perm.push_back(c);
            }

            for (int i = 0; i < this_perm.size(); i++) {
                this_perm[i].second = this_perm.size() - i;
            }

            #pragma omp task firstprivate(this_perm)
            {
                process_permutation(this_perm, permutation_count, total_permutations);
            }

        } while (next_permutation(local_chromosomes.begin(), local_chromosomes.end(),
                     [](auto &a, auto &b) {
                         if (a.second != b.second) return a.second < b.second;
                         return a.first < b.first;
                     }));

        #pragma omp taskwait  // 確保所有任務完成
    }

    cout << endl;
    cout << "Total permutations = " << permutation_count << endl;

    return 0;
}

