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

double calculate_segment_onemax_weak(const string& segment, const string& method) {
        double weak_fiteness = 0;
        if (count(segment.begin(), segment.end(), '1') == 0)
            weak_fiteness = 1.5;
        else
            weak_fiteness = count(segment.begin(), segment.end(), '1');

        return weak_fiteness;
}

// Helper function for segment-based functions
double calculate_segment_fitness(const string& segment, const string& method) {
    double result = 0.0;
    if (method == "trap") {
        int ones = count(segment.begin(), segment.end(), '1');
        if (ones == segment.length()) {
            return 4.0;
        } else if (ones == 0) {
            return 3.0;
        } else {
            result = 3.0 - (ones * 3.0 / (segment.length() - 1));
            if (result < 0)
            {
                return 0.0;
            }

            return result;
        }
    } else if (method == "niah") {
        return all_of(segment.begin(), segment.end(), [](char bit) { return bit == '1'; }) ? 1.0 : 0.0;
    }
    return 0.0;
}

double calculate_segment_fitness_test(const string& segment) {
    // double result = 0.0;
  
    int ones = count(segment.begin(), segment.end(), '1');
    if (ones == segment.length()) {
        return 4.0;
    } else if (ones == 3) {
        return 3.0;
    } else if (ones == 2) {
        return 0.0;
    } else if (ones == 1) {
        return 1.0;
    } else {
        return 2.0;
    }
    // return 0.0;
}

// Helper function for segment-based functions
double calculate_onemax_weak2(const string& segment, const string& method) {
    double weak_fiteness = 0;
    if (count(segment.begin(), segment.end(), '1') == 0)
        weak_fiteness = 1.5;
    else
        weak_fiteness = count(segment.begin(), segment.end(), '1');

    return weak_fiteness;
}

double calculate_onemax_weak3(const string& segment, const string& method) {
    double weak_fiteness = 0;
    if (count(segment.begin(), segment.end(), '1') == 0)
        weak_fiteness = 1.5;
    else
        weak_fiteness = count(segment.begin(), segment.end(), '1');

    return weak_fiteness;
}

// Calculate the fitness of a chromosome based on the selected method
double calculate_fitness(const string& chromosome, const string& method) {
    if (method == "onemax") {
        return count(chromosome.begin(), chromosome.end(), '1');
    } else if (method == "trap") {
        return calculate_segment_fitness(chromosome, "trap");
    } else if (method == "niah") {
        return calculate_segment_fitness(chromosome, "niah");
    } else if (method == "ctrap" || method == "cniah") {
        int segment_length = 4;
        double total_fitness = 0.0;
        for (size_t i = 0; i < chromosome.length(); i += segment_length) {
            string segment = chromosome.substr(i, min(segment_length, static_cast<int>(chromosome.length() - i)));
            total_fitness += calculate_segment_fitness(segment, method.substr(1));
        }

        return total_fitness;
    } else if (method == "cyctrap") {
        // cout << method << "!!"<< endl;
        int segment_length = 4;
        int overlap = 1;
        double total_fitness = 0.0;
        for (size_t i = 0; i < chromosome.length(); i += segment_length - overlap) {
            string segment;
            if (i + segment_length <= chromosome.length()) {
                segment = chromosome.substr(i, segment_length);
            } else {
                segment = chromosome.substr(i) + chromosome.substr(0, segment_length - (chromosome.length() - i));
            }

            total_fitness += calculate_segment_fitness(segment, "trap");

            if (i + segment_length >= chromosome.length() + overlap) {
                break;
            }
        }

        return total_fitness;
    } else if (method == "leadingones") {
        // cout << method << "!!" << endl;
        double leading_ones = 0;
        for (char bit : chromosome) {
            if (bit == '1') {
                leading_ones++;
            } else {
                break;
            }
        }

        return leading_ones;
    } else if (method == "leadingtraps") {
        int segment_length = 4;

        std::vector<int> L(chromosome.length(), 0); 
        L[0] = 1;
        std::vector<double> segment_fitness_record(chromosome.length(), 0); 

        double segment_fitness = 0.0;
        for (size_t i = 0; i < chromosome.length(); i += segment_length) {
            string segment = chromosome.substr(i, min(segment_length, static_cast<int>(chromosome.length() - i)));
            segment_fitness = calculate_segment_fitness(segment, "trap");
            segment_fitness_record[i] += segment_fitness;

            if (i == 0)
            {
                continue;
            }
           
            if (segment_fitness_record[i-4] == 4 && L[i-4] == 1) {
                L[i] = 1;
            }
            
        }

        double total_fitness = 0.0;
        for (size_t i = 0; i < chromosome.length(); i += segment_length) {
            total_fitness += L[i] * segment_fitness_record[i]; 
        }

        return total_fitness;
    
    } else if (method == "test") {
        // cout << method << "!!" << endl;
        
        double weak_fiteness = calculate_segment_fitness_test(chromosome);

        return weak_fiteness;
    } else if (method == "test_equal_fitness") {
        // cout << method << "!!" << endl;
        
        double weak_fiteness = 0;
        if (chromosome == "111")
            weak_fiteness = 4;
        else if (chromosome == "100")
            weak_fiteness = 3;
        else if (chromosome == "000")
            weak_fiteness = 3;
        else
            weak_fiteness = 0;
        return weak_fiteness;
    } else if (method == "onemax_weak") {
        // cout << method << "!!" << endl;
        
        double weak_fiteness = 0;
        // if (count(chromosome.begin(), chromosome.end(), '1') == 0)
        //     weak_fiteness = 1.5;
        if ((chromosome == "1111"))
            weak_fiteness = 16;
        else if((chromosome == "0111"))
            weak_fiteness = 15;
        // else if((chromosome == "1011"))
        //     weak_fiteness = 14;
        else if((chromosome == "1101"))
            weak_fiteness = 13;
        else if((chromosome == "1110"))
            weak_fiteness = 12;
        else if((chromosome == "0011") or (chromosome == "1011"))
            weak_fiteness = 11;
        else if((chromosome == "0101"))
            weak_fiteness = 10;
        else if((chromosome == "0110"))
            weak_fiteness = 9;
        else if((chromosome == "1001") or (chromosome == "0001"))
            weak_fiteness = 15.6;
        // else if((chromosome == "1010"))
        //     weak_fiteness = 7;
        else if((chromosome == "1100"))
            weak_fiteness = 6;
        else if((chromosome == "0001"))
            weak_fiteness = 5;
        else if((chromosome == "0010"))
            weak_fiteness = 4;
        else if((chromosome == "0100"))
            weak_fiteness = 3;
        else if((chromosome == "1001"))
            weak_fiteness = 15.6;
        else if((chromosome == "0000"))
            weak_fiteness = 1;
        else if((chromosome == "1101") or (chromosome == "0101"))
            weak_fiteness = 0;
        else
            weak_fiteness = 0;
        



        return weak_fiteness;
    } else if (method == "multi_weak") {
        double weak_fiteness = 0;

        std::string segment_weak2;
        segment_weak2 += chromosome[0];
        segment_weak2 += chromosome[1];
        segment_weak2 += chromosome[2]; 

        weak_fiteness += calculate_segment_onemax_weak(segment_weak2, method);

        std::string segment_weak3;
        segment_weak3 += chromosome[3];
        segment_weak3 += chromosome[4];
        segment_weak3 += chromosome[5];   
        segment_weak3 += chromosome[6];    
        weak_fiteness += calculate_segment_onemax_weak(segment_weak3, method);  

        std::string segment_weak4;
        segment_weak4 += chromosome[7];
        segment_weak4 += chromosome[8];
        segment_weak4 += chromosome[9];   
        segment_weak4 += chromosome[10];    
        segment_weak4 += chromosome[11]; 
        weak_fiteness += calculate_segment_onemax_weak(segment_weak4, method); 
      
        return weak_fiteness;
    }
    std::cerr << "Error: the problem does not exist!" << std::endl;
    exit(1);
    return 0.0;
}


// Generate all possible chromosomes based on the problem length L
vector<pair<string, double>> generate_chromosomes(int L, const string& method) {
    vector<pair<string, double>> chromosomes;
    int num_combinations = pow(2, L);

    for (int i = 0; i < num_combinations; ++i) {
        // Generate chromosome using bitset and convert it to a string
        string chromosome = bitset<32>(i).to_string().substr(32 - L);
        
        // Calculate fitness for the chromosome
        double fitness = calculate_fitness(chromosome, method);
        
        // Store the chromosome and its fitness as a pair
        chromosomes.push_back({chromosome, fitness});
    }

    return chromosomes;
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

// Print the matrix
void print_matrix(const vector<vector<string>>& matrix) {
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

//----------------- 自行依需求修改的適應值計算函式 -------------------
double calculate_fitness_emu(const string &chromosome, const string &method) {
    // 這裡範例只把 '1' 的數量當作 fitness，實際可依照你的需求實作
    double fitness = 0.0;
    for (char c : chromosome) {
        if (c == '1') fitness += 1.0;
    }
    return fitness;
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

// void process_permutation(vector<pair<string, double>>& chromosomes) {
//     static int no_weak_id = 0; // 用來標記無弱相互作用的計數器
//     int L = chromosomes[0].first.size(); // 染色體長度
//     string method = "onemax"; // 可改為你要的計算適應值的方法

//     bool all_no_weak = true; // 假設所有目標位點都沒有弱相互作用

//     for (int target_index = 0; target_index < L; target_index++) {
//         std::vector<int> weak_epi_count_results = count_weak(L, target_index, chromosomes, method);

//         for (int i = 2; i < L; i++) {
//             if (weak_epi_count_results[i] > 0) {
//                 all_no_weak = false;
//                 break;
//             }
//         }

//         if (!all_no_weak) break;
//     }

//     if (all_no_weak) {
//         cout << "----- no weak [" << no_weak_id << "] ----- " << endl;
//         no_weak_id++;

//         // 計算所有目標位點的強相互作用
//         for (int target_index = 0; target_index < L; target_index++) {
//             std::vector<int> epi_count_results = count_epi(L, target_index, chromosomes, method);
//         }

//         // 根據 fitness 排序
//         sort(chromosomes.begin(), chromosomes.end(), [](const auto& a, const auto& b) {
//             return a.second > b.second;
//         });

//         // cout << endl;

//         cout << "chromosomes & fitness" << endl;
//         for (const auto& chom : chromosomes) {
//             cout << chom.first << " " << chom.second << endl;
//         }
//         cout << endl;
//     }
// }

///////////////



// 顯示進度條
// void show_progress_bar(double progress) {
//     #pragma omp critical  // 確保多執行緒時不會交錯輸出
//     {
//         std::cerr << int(progress * 100.0) << "%\r";
//         std::cerr.flush();
//     }
// }


// void show_progress_bar(double progress) {
//     std::cerr << int(progress * 100.0) << "%";
//     std::cerr.flush();  // 立即輸出，確保不受緩衝影響
// }

// 顯示進度條
void show_progress_bar(double progress) {
    #pragma omp critical  // 確保多執行緒時不會交錯輸出
    {
        std::cerr << "\rProgress: " << int(progress * 100.0)<< "%";
        std::cerr.flush();
    }
}

// // 處理單個排列
// void process_permutation(vector<pair<string, double>> chromosomes) {
//     static int no_weak_id = 0; // 計數器
//     int L = chromosomes[0].first.size(); // 染色體長度
//     string method = "onemax";

//     bool all_no_weak = true;

//     for (int target_index = 0; target_index < L; target_index++) {
//         std::vector<int> weak_epi_count_results = count_weak(L, target_index, chromosomes, method);

//         for (int i = 2; i < L; i++) {
//             if (weak_epi_count_results[i] > 0) {
//                 all_no_weak = false;
//                 break;
//             }
//         }

//         if (!all_no_weak) break;
//     }

//     if (all_no_weak) {
//         #pragma omp critical  // 避免多執行緒輸出混亂
//         {
//             cout << "----- no weak [" << no_weak_id << "] ----- " << endl;
//             no_weak_id++;

//             for (int target_index = 0; target_index < L; target_index++) {
//                 std::vector<int> epi_count_results = count_epi(L, target_index, chromosomes, method);
//             }

//             // 根據 fitness 排序
//             sort(chromosomes.begin(), chromosomes.end(), [](const auto& a, const auto& b) {
//                 return a.second > b.second;
//             });

//             cout << "chromosomes & fitness" << endl;
//             for (const auto& chom : chromosomes) {
//                 cout << chom.first << " " << chom.second << endl;
//             }
//             cout << endl;
//         }
//     }
// }

// int main(int argc, char* argv[]) {
//     if (argc != 4) {
//         cerr << "Usage: " << argv[0] << " <Problem Length L> <Fitness Method> <Num Threads>" << endl;
//         return 1;
//     }

//     int L = stoi(argv[1]);
//     string method = argv[2];
//     int num_threads = stoi(argv[3]);  // 使用者輸入要用的執行緒數量
//     int n = pow(2, L);

//     omp_set_num_threads(num_threads);  // 設定 OpenMP 使用的核心數量

//     // 取得全部 2^L 染色體
//     vector<pair<string, double>> base_chromosomes = generate_chromosomes_emu(L, method);

//     // 找出 "最佳解" (fitness 最大)
//     auto it_best = max_element(
//         base_chromosomes.begin(), base_chromosomes.end(),
//         [](auto &a, auto &b) { return a.second < b.second; }
//     );
//     auto best_chromosome = *it_best;
//     base_chromosomes.erase(it_best);

//     // 排序剩下的染色體（為了 `next_permutation`）
//     sort(base_chromosomes.begin(), base_chromosomes.end(),
//          [](auto &a, auto &b) {
//              if (a.second != b.second) return a.second < b.second; 
//              return a.first < b.first;
//          });

//     long long permutation_count = 0;
//     long long total_permutations = 1;  // 計算排列數
//     for (int i = 2; i <= base_chromosomes.size(); i++) {
//         total_permutations *= i;
//     }

//     vector<vector<pair<string, double>>> all_permutations;
    
//     // 生成所有排列並存入 all_permutations
//     do {
//         vector<pair<string, double>> this_perm;
//         this_perm.push_back(best_chromosome);
//         for (auto &c : base_chromosomes) {
//             this_perm.push_back(c);
//         }
//         for (int i = 0; i < this_perm.size(); i++) {
//             this_perm[i].second = this_perm.size() - i;
//         }
//         all_permutations.push_back(this_perm);
//     } while (next_permutation(base_chromosomes.begin(), base_chromosomes.end(),
//                  [](auto &a, auto &b) {
//                      if (a.second != b.second) return a.second < b.second;
//                      return a.first < b.first;
//                  }));

//     // 使用 OpenMP 並行處理排列組合
//     #pragma omp parallel for schedule(dynamic)
//     for (size_t i = 0; i < all_permutations.size(); i++) {
//         process_permutation(all_permutations[i]);

//         // 更新進度條
//         show_progress_bar(double(i) / all_permutations.size());
//     }

//     cout << endl;
//     cout << "Total permutations = " << permutation_count << endl;

//     return 0;
// }



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

int main(int argc, char* argv[]) {
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

