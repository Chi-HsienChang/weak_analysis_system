#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <algorithm>
#include <set>
#include <random>
#include <omp.h>          // OpenMP 用於多核心處理
#include <sys/statvfs.h>  // 獲取磁碟空間資訊
#include <cstdlib>        // exit()
#include <unistd.h>       // sleep()

using namespace std;

#define DEBUG 1

// ==================== 1. 檢查磁碟剩餘空間 ====================
bool check_disk_space(const string& path, double min_free_gb) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        cerr << "Error: Failed to check disk space!" << endl;
        return false; // 讀取失敗
    }
    double free_space_gb = (stat.f_bavail * stat.f_frsize) 
                           / (1024.0 * 1024.0 * 1024.0);

    if (free_space_gb < min_free_gb) {
        cerr << "Warning: Low disk space! Only " 
             << free_space_gb << "GB left. Stopping program." << endl;
        return false; // 空間不足
    }
    return true; // 空間足夠
}

// ===========================================================
// ================ 2. EPI 相關函式區塊 =======================
// ===========================================================

// ---------- 2.1 check_constraint_epi ----------
std::pair<std::set<char>, std::vector<std::string>> check_constraint_epi(
    int target_index, 
    const std::vector<int>& combination,
    const std::vector<int>& enumeration, 
    const vector<pair<string, double>>& chromosomes
){
    std::set<char> values_at_v;
    std::vector<std::string> highest_fitness_chromosomes;
    double max_fitness = -1;

    for (const auto& chromosome : chromosomes) {
        bool fit_constraint = true;
        int enumeration_index = 0;
        for (int i : combination) {
            if (chromosome.first[i] - '0' != enumeration[enumeration_index]) {
                fit_constraint = false;
                break;
            }
            enumeration_index++;
        }
        if (fit_constraint) {
            double fitness = chromosome.second;
            if (fitness > max_fitness) {
                max_fitness = fitness;
                values_at_v.clear();
                highest_fitness_chromosomes.clear();
                values_at_v.insert(chromosome.first[target_index]);
                highest_fitness_chromosomes.push_back(chromosome.first);
            } 
            else if (fitness == max_fitness) {
                values_at_v.insert(chromosome.first[target_index]);
                highest_fitness_chromosomes.push_back(chromosome.first);
            }
        }
    }
    return {values_at_v, highest_fitness_chromosomes};
}

// ---------- 2.2 check_constrained_optima_epi ----------
bool check_constrained_optima_epi(
    int target_index, 
    const std::vector<int>& combination, 
    const std::vector<int>& enumeration_original, 
    const std::vector<int>& combination_wo, 
    const std::vector<int>& enumeration_wo, 
    const vector<pair<string, double>>& chromosomes
){
    auto constrained_optima_original = check_constraint_epi(
        target_index, 
        combination, 
        enumeration_original, 
        chromosomes
    );
    auto constrained_optima_flip = check_constraint_epi(
        target_index, 
        combination_wo, 
        enumeration_wo, 
        chromosomes
    );

    if (constrained_optima_original.first != constrained_optima_flip.first) {
        if (!constrained_optima_original.first.empty() 
            && !constrained_optima_flip.first.empty()) { 
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// ---------- 2.3 check_epi ----------
int check_epi(
    int target_index, 
    const std::vector<int>& combination, 
    const vector<vector<int>>& enumerations, 
    const vector<pair<string, double>>& chromosomes, 
    bool isPrint
){
    int condition_holds = 0;
    for (int condition_index = 0; condition_index < (int)combination.size(); condition_index++) {
        for (auto enumeration : enumerations) {
            // enumeration_original
            auto enumeration_original = enumeration;

            // 去掉 flip 該位後的 combination & enumeration
            auto combination_wo = combination;
            auto enumeration_wo = enumeration;
            combination_wo.erase(combination_wo.begin() + condition_index);
            enumeration_wo.erase(enumeration_wo.begin() + condition_index);
            
            bool not_equal = check_constrained_optima_epi(
                target_index, 
                combination,
                enumeration_original, 
                combination_wo, 
                enumeration_wo, 
                chromosomes
            );
            if (not_equal) {
                condition_holds++;
                break;
            }
        }
    }

    if (condition_holds == (int)combination.size()) {
        if (isPrint){  
            cout << "{ ";
            for (auto elem : combination) {
                cout << elem << " ";
            }
            cout << "} -> "<< target_index << endl;           
        }
        return 1;
    }
    return 0;
}

// ---------- 2.4 isSubset ----------
bool isSubset(const std::vector<int>& subset, const std::vector<int>& set) {
    for (const auto& elem : subset) {
        if (std::find(set.begin(), set.end(), elem) == set.end()) {
            return false;
        }
    }
    return true;
}

// ---------- 2.5 generateBinarySequences (列出 2^n 所有 bit 序列) ----------
std::vector<std::vector<int>> generateBinarySequences(int n) {
    int totalSequences = 1 << n;  // 2^n
    std::vector<std::vector<int>> allSequences;
    allSequences.reserve(totalSequences);

    for (int i = 0; i < totalSequences; ++i) {
        std::vector<int> sequence(n);
        for (int j = 0; j < n; ++j) {
            sequence[n - 1 - j] = (i >> j) & 1;
        }
        allSequences.push_back(sequence);
    }
    return allSequences;
}

// ---------- 2.6 generateCombinations (從 0..n(不含target_index) 中挑 k 個) ----------
std::vector<std::vector<int>> generateCombinations(int n, int k, int target_index) {
    // n: L-1
    std::vector<int> elements;
    for (int i = 0; i <= n; ++i) {
        if (i != target_index) {
            elements.push_back(i);
        }
    }
    std::vector<int> bitmask(k, 1);
    bitmask.resize(elements.size(), 0);

    std::vector<std::vector<int>> combinations;
    do {
        std::vector<int> currentCombination;
        for (size_t i = 0; i < bitmask.size(); ++i) {
            if (bitmask[i]) {
                currentCombination.push_back(elements[i]);
            }
        }
        combinations.push_back(currentCombination);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    return combinations;
}

// ---------- 2.7 check_constraint ----------
std::pair<std::set<char>, std::vector<std::string>> check_constraint(
    int target_index, 
    const std::vector<int>& combination, 
    const std::vector<int>& enumeration, 
    const vector<pair<string, double>>& chromosomes
){
    std::set<char> values_at_v;
    std::vector<std::string> highest_fitness_chromosomes;
    double max_fitness = -1;

    for (const auto& chromosome : chromosomes) {
        bool fit_constraint = true;
        int enumeration_index = 0;
        for (int i : combination) {
            if (chromosome.first[i] - '0' != enumeration[enumeration_index]) {
                fit_constraint = false;
                break;
            }
            enumeration_index++;
        }
        if (fit_constraint) {
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

// ---------- 2.8 check_constrained_optima ----------
bool check_constrained_optima(
    int target_index,
    const std::vector<int>& combination,
    const std::vector<int>& enumeration_original,
    const std::vector<int>& combination_wo,
    const std::vector<int>& enumeration_wo,
    const vector<pair<string, double>>& chromosomes
){
    auto constrained_optima_original = check_constraint(
        target_index, combination, enumeration_original, chromosomes
    );
    auto constrained_optima_flip = check_constraint(
        target_index, combination_wo, enumeration_wo, chromosomes
    );

    if (constrained_optima_original.first != constrained_optima_flip.first) {
        if (!constrained_optima_original.first.empty() 
            && !constrained_optima_flip.first.empty()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// ---------- 2.9 count_epi (針對每個 target_index，計算互作 EPI 情況) ----------
std::vector<int> count_epi(
    int L, 
    int target_index, 
    const vector<pair<string, double>>& chromosomes,
    const string& method, 
    bool isPrint
){
    std::vector<std::vector<std::vector<int>>> weak_epi_set(L);
    std::vector<int> weak_epi_count(L, 0);

    for (int epi_size = 1; epi_size < L; epi_size++) {
        auto combinations = generateCombinations(L-1, epi_size, target_index);

        for (auto& combination : combinations) {
            bool not_find_smaller_epi;
            if (epi_size == 1) {
                not_find_smaller_epi = true;
            } else {
                not_find_smaller_epi = true;
                int smaller_epi_size = epi_size;
                bool is_subset_flag;
                while(not_find_smaller_epi && smaller_epi_size >= 1) {
                    for (auto& previous : weak_epi_set[smaller_epi_size-1]) {
                        is_subset_flag = isSubset(previous, combination);
                        not_find_smaller_epi = !is_subset_flag;
                        if(is_subset_flag) break;
                    }
                    smaller_epi_size--;
                }
            }

            if(not_find_smaller_epi) {
                auto enumerations = generateBinarySequences(epi_size);
                int result = check_epi(
                    target_index, 
                    combination, 
                    enumerations, 
                    chromosomes, 
                    isPrint
                );
                weak_epi_count[epi_size] += result;
                // 若要記錄可在此 push_back
                // if (result) {
                //     weak_epi_set[epi_size].push_back(combination);
                // }
            }
        }
    }
    return weak_epi_count;
}

// ---------- 2.10 check_weak (檢查「弱互作」條件) ----------
int check_weak(
    int target_index,
    const std::vector<int>& combination,
    const std::vector<std::vector<int>>& enumerations,
    const vector<pair<string, double>>& chromosomes
){
    int condition_holds = 0;
    for (int condition_index = 0; condition_index < (int)combination.size(); condition_index++) {
        for (auto enumeration : enumerations) {
            auto enumeration_original = enumeration;

            auto combination_wo = combination;
            auto enumeration_wo = enumeration;
            combination_wo.erase(combination_wo.begin() + condition_index);
            enumeration_wo.erase(enumeration_wo.begin() + condition_index);

            bool not_equal = check_constrained_optima(
                target_index, 
                combination, 
                enumeration_original, 
                combination_wo, 
                enumeration_wo, 
                chromosomes
            );
            if (not_equal) {
                condition_holds++;
                break;
            }
        }
    }

    if (condition_holds == (int)combination.size()) {
        if (DEBUG){  
            // 可在此輸出除錯資訊
        }
        return 1;
    }
    return 0;
}

// ---------- 2.11 count_weak (統計弱互作) ----------
std::vector<int> count_weak(
    int L,
    int target_index,
    const vector<pair<string, double>>& chromosomes,
    const string& method
){
    std::vector<std::vector<std::vector<int>>> weak_epi_set(L);
    std::vector<int> weak_epi_count(L, 0);

    for (int epi_size = 1; epi_size < L; epi_size++) {
        auto combinations = generateCombinations(L-1, epi_size, target_index);

        for (auto& combination : combinations) {
            bool not_find_smaller_epi;
            if (epi_size == 1) {
                not_find_smaller_epi = true;           
            } else {
                not_find_smaller_epi = true;
                int smaller_epi_size = epi_size;
                bool is_subset_flag;
                while(not_find_smaller_epi && smaller_epi_size >= 1) {
                    for (auto& previous : weak_epi_set[smaller_epi_size-1]) {
                        is_subset_flag = isSubset(previous, combination);
                        not_find_smaller_epi = !is_subset_flag;
                        if(is_subset_flag) break;
                    }
                    smaller_epi_size--;
                }
            }

            if(not_find_smaller_epi) {
                auto enumerations = generateBinarySequences(epi_size);
                int result = check_weak(target_index, combination, enumerations, chromosomes);
                weak_epi_count[epi_size] += result;
                if (result) {
                    weak_epi_set[epi_size].push_back(combination);
                }
            }
        }
    }
    return weak_epi_count;
}

// ===========================================================
// =========== 3. 產生染色體、fitness 計算函式 ================
// ===========================================================

// ---- 3.1 calculate_fitness_emu ----
double calculate_fitness_emu(const string &chromosome, const string &method) {
    // 範例：僅計算 '1' 的數量
    double fitness = 0.0;
    for (char c : chromosome) {
        if (c == '1') fitness += 1.0;
    }
    return fitness;
}

// ---- 3.2 generate_chromosomes_emu ----
vector<pair<string, double>> generate_chromosomes_emu(int L, const string &method) {
    vector<pair<string, double>> chromosomes;
    int num_combinations = (int)pow(2, L);
    chromosomes.reserve(num_combinations);

    for (int i = 0; i < num_combinations; ++i) {
        // 用 bitset 產生二進位字串，再擷取後 L 位
        string chromosome = bitset<32>(i).to_string().substr(32 - L);
        double fitness = calculate_fitness_emu(chromosome, method);
        chromosomes.push_back({chromosome, fitness});
    }
    return chromosomes;
}

// ===========================================================
// ================= 4. 顯示進度條函式 ========================
// ===========================================================
void show_progress_bar(double progress) {
    #pragma omp critical
    {
        cerr << "\rProgress: " << int(progress * 100.0) << "%";
        cerr.flush();
    }
}

// ===========================================================
// =========== 5. 處理單個 permutation 函式 ===================
// ===========================================================
void process_permutation(
    vector<pair<string, double>> chromosomes, 
    long long &permutation_count, 
    long long total_permutations,
    long long this_perm_id // <--- 用來標示「第幾個 this_perm」
) {
    // 在這裡印出「第幾個 permutation」
    // #pragma omp critical
    // {
    //     cerr << "[Thread " << omp_get_thread_num() << "] "
    //          << "Handling permutation #" << this_perm_id << endl;
    // }
    #pragma omp critical
    {
        // 1. 印出「執行緒編號」與「第幾個 permutation」
        // cerr << "[Thread " << omp_get_thread_num() << "] "
        cerr  << "#" << this_perm_id <<endl;

        // // 2. 迴圈印出整個 this_perm
        // cerr << " { ";
        // for (auto &ch : chromosomes) {
        //     // ch.first = 染色體字串, ch.second = fitness
        //     cerr << "(" << ch.first << ", " << ch.second << ") ";
        // }
        // cerr << "} " << endl;
    }

    // 取得染色體長度
    int L = (int)chromosomes[0].first.size();
    string method = "onemax";

    // ---- 檢查是否有弱互作 ----
    bool all_no_weak = true;
    for (int target_index = 0; target_index < L; target_index++) {
        vector<int> weak_epi_count_results = count_weak(L, target_index, chromosomes, method);
        for (int i = 2; i < L; i++) {
            if (weak_epi_count_results[i] > 0) {
                all_no_weak = false;
                break;
            }
        }
        if (!all_no_weak) break;
    }

    if (all_no_weak) {
        // 如果沒有弱互作
        #pragma omp critical
        {
            bool epi_stop = false;
            // bool epi_stop = true;
            vector<int> epi_count_results(L, 0);
            
            for (int target_index = 0; target_index < L; target_index++) {
                // if (epi_stop) break;
                auto epi_results = count_epi(L, target_index, chromosomes, method, false);
                // for (int i = 2; i < L; i++) {
                //     if (epi_results[i] > 0) {
                //         epi_stop = true;
                //         break;
                //     }
                // }
                // for (int i = 0; i < L; i++) {
                //     epi_count_results[i] += epi_results[i];
                // }
            }

            if(!epi_stop) {
                int epi_size_2_L = 0;
                // for (int index = 2; index < L; index++) {
                //     epi_size_2_L += epi_count_results[index];
                // }

                if (epi_size_2_L == 0) {
                    static int no_weak_id = 0; // 用來記錄「找到第幾個 no_weak 組合」
                    cout << "----- no weak [" << no_weak_id << "] ----- " << endl;
                    no_weak_id++;

                    // 印出所有 EPI 結果
                    for (int target_index = 0; target_index < L; target_index++) {
                        count_epi(L, target_index, chromosomes, method, true);
                    }

                    // 根據 fitness 排序並印出
                    sort(chromosomes.begin(), chromosomes.end(),
                        [](auto &a, auto &b) {
                            return a.second > b.second;
                        });

                    cout << "chromosomes & fitness\n";
                    for (auto &chom : chromosomes) {
                        cout << chom.first << " " << chom.second << endl;
                    }
                    cout << endl;

                    // 找到目標後直接結束程式
                    // exit(0);
                }
            }
        }
    }

    // 更新 permutation_count
    #pragma omp atomic
    permutation_count++;

    // 更新進度條
    // show_progress_bar(double(permutation_count) / total_permutations);
}

// ===========================================================
// ==================== 6. 主程式 main =======================
// ===========================================================
int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] 
             << " <Problem Length L> <Fitness Method> <Num Threads>" << endl;
        return 1;
    }

    // 參數解析
    int L = stoi(argv[1]);
    string method = argv[2];
    int num_threads = stoi(argv[3]);

    // 最小磁碟空間需求 (GB)，自行調整
    double min_free_gb = 1.0; 

    // 設定執行緒數量
    omp_set_num_threads(num_threads);

    // 產生 2^L 染色體
    vector<pair<string, double>> base_chromosomes = generate_chromosomes_emu(L, method);

    // 找出 "最佳解" (fitness 最大)
    auto it_best = max_element(
        base_chromosomes.begin(), 
        base_chromosomes.end(),
        [](auto &a, auto &b) { return a.second < b.second; }
    );
    auto best_chromosome = *it_best;
    base_chromosomes.erase(it_best);

    // 排序剩下染色體（給 next_permutation 用）
    sort(base_chromosomes.begin(), base_chromosomes.end(),
         [](auto &a, auto &b) {
             if (a.second != b.second) return a.second < b.second; 
             return a.first < b.first;
         });

    // 計算 permutations 數量
    long long permutation_count = 0;
    long long total_permutations = 1;
    for (int i = 2; i <= (int)base_chromosomes.size(); i++) {
        total_permutations *= i;
    }

    // ================ OpenMP 並行區域 =================
    #pragma omp parallel
    {
        // 只有一個執行緒進行 next_permutation
        #pragma omp single
        {
            // 用來標記「第幾個 this_perm」的計數器
            static long long perm_id = 0;

            vector<pair<string, double>> local_chromosomes = base_chromosomes;

            do {
                // 檢查磁碟空間
                if (!check_disk_space("/", min_free_gb)) {
                    cerr << "Stopping computation due to low disk space." << endl;
                    exit(1);
                }

                // 建立當前 this_perm
                vector<pair<string, double>> this_perm;
                this_perm.push_back(best_chromosome);
                for (auto &c : local_chromosomes) {
                    this_perm.push_back(c);
                }
                // 給定一個唯一排序的 fitness 值，避免重複
                // for (int i = 0; i < (int)this_perm.size(); i++) {
                //     this_perm[i].second = (double)this_perm.size() - i;
                // }
                
                // 給定一個唯一排序的 fitness 值，避免重複
                for (int i = 0; i < (int)this_perm.size(); i++) {
                    if (i == 0){
                        this_perm[i].second = this_perm.size() - i;
                    }else{
                        this_perm[i].second = this_perm.size() - (this_perm.size()-i);
                    }
                }

                // 捕捉目前的 perm_id（第幾個 permutation）
                long long this_perm_id;
                #pragma omp atomic capture
                {
                    perm_id++;
                    this_perm_id = perm_id;
                }

                // 建立一個 task，讓任意執行緒處理
                #pragma omp task firstprivate(this_perm, this_perm_id)
                {
                    process_permutation(this_perm, permutation_count, total_permutations, this_perm_id);
                }

            } while (next_permutation(
                         local_chromosomes.begin(), 
                         local_chromosomes.end(),
                         [](auto &a, auto &b) {
                             if (a.second != b.second) 
                                 return a.second < b.second;
                             return a.first < b.first;
                         }
                     ));

            // 等待所有任務完成
            #pragma omp taskwait
        } // end single
    } // end parallel

    cout << endl << "Total permutations = " << permutation_count << endl;

    return 0;
}
