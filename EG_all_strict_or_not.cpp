#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <algorithm>
#include <set>

using namespace std;
#define DEBUG 0 // Uncomment this line if DEBUG is meant to be a macro

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


// Calculate the fitness of a chromosome based on the selected method
double calculate_fitness(const string& chromosome, const string& method) {
    if (method == "onemax") {
        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << count(chromosome.begin(), chromosome.end(), '1') << endl;
        }
        return count(chromosome.begin(), chromosome.end(), '1');
    } else if (method == "trap") {
        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << calculate_segment_fitness(chromosome, "trap") << endl;
        }
        return calculate_segment_fitness(chromosome, "trap");
    } else if (method == "niah") {
        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << calculate_segment_fitness(chromosome, "niah") << endl;
        }
        return calculate_segment_fitness(chromosome, "niah");
    } else if (method == "ctrap" || method == "cniah") {
        int segment_length = 4;
        double total_fitness = 0.0;
        for (size_t i = 0; i < chromosome.length(); i += segment_length) {
            string segment = chromosome.substr(i, min(segment_length, static_cast<int>(chromosome.length() - i)));
            total_fitness += calculate_segment_fitness(segment, method.substr(1));
        }
        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << total_fitness << endl;
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
        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << total_fitness << endl;
        }
        return total_fitness;
    } else if (method == "leadingone") {
        // cout << method << "!!" << endl;
        double leading_ones = 0;
        for (char bit : chromosome) {
            if (bit == '1') {
                leading_ones++;
            } else {
                break;
            }
        }
        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << leading_ones << endl;
        }
        return leading_ones;
    } else if (method == "weak") {
        // cout << method << "!!" << endl;
        
        double weak_fiteness = 0;
        if (chromosome == "100")
            weak_fiteness = 4;
        else if (chromosome == "111")
            weak_fiteness = 3;
        else if (chromosome == "001")
            weak_fiteness = 2;
        else if (chromosome == "010")
            weak_fiteness = 1;
        else
            weak_fiteness = 0;


        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << weak_fiteness << endl;
        }
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

        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << weak_fiteness << endl;
        }
        return weak_fiteness;
    } else if (method == "onemax_weak") {
        // cout << method << "!!" << endl;
        
        double weak_fiteness = 0;
        if (count(chromosome.begin(), chromosome.end(), '1') == 0)
            weak_fiteness = 1.5;
        else
            weak_fiteness = count(chromosome.begin(), chromosome.end(), '1');

        if (DEBUG)
        {
            cout << chromosome << endl;
            cout << weak_fiteness << endl;
        }
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

    if (DEBUG){
        cout << endl;
        cout << "++++++++++++ in check_constraint ++++++++++++" << endl;
    }

    if (DEBUG)
    {
        cout << "locus =  ";
        for (const auto& elem : combination) {
            cout << elem << " ";
        }
        cout << endl;
    }

    if (DEBUG)
    {
        cout << "allele =  ";
        for (const auto& elem : enumeration) {
            cout << elem << " ";
        }
        cout << endl;
    }   
    


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

            if (DEBUG){
                cout << "constraint = ";
                int _index = 0;
                for (int i : combination)
                {
                    cout << "(" << i << ", " << enumeration[_index] << ") ";
                    _index++;
                cout << endl;
                }
                cout << chromosome.first << endl;
            }
        }         
    
    }



    if (DEBUG)
    {
        cout << "highest_fitness_chromosomes: ";
        for (const auto& elem : highest_fitness_chromosomes) {
            cout << elem << " ";
        }
        cout << endl;
    }

    if (DEBUG)
    {
        cout<<"max_fitness = " << max_fitness << endl;
    }

    if (DEBUG)
    {
        cout << "values_at_v: ";
        for (const auto& elem : values_at_v) {
            cout << elem << " ";
        }
        cout << endl;
    }

    if (DEBUG){
        cout << "++++++++++++ out check_constraint ++++++++++++" << endl;
    }

    return {values_at_v, highest_fitness_chromosomes};

}


bool check_constrained_optima(int target_index, auto combination, auto enumeration_original, auto enumeration, auto chromosomes){

        if (DEBUG)
        {
            cout << "A =  ";
            for (const auto& elem : enumeration_original) {
                cout << elem << " ";
            }
            cout << endl;
        }

        if (DEBUG)
        {
            cout << "B =  ";
            for (const auto& elem : enumeration) {
                cout << elem << " ";
            }
            cout << endl;
        }


    auto constrained_optima_original = check_constraint(target_index, combination, enumeration_original, chromosomes);

    auto constrained_optima_flip = check_constraint(target_index, combination, enumeration, chromosomes);


    if (DEBUG)
    {
        cout << "constrained_optima_original.first: ";
        for (const auto& elem : constrained_optima_original.first) {
            cout << elem << " ";
        }
        cout << endl;
    }

    if (DEBUG)
    {
        cout << "constrained_optima_flip.first: ";
        for (const auto& elem : constrained_optima_flip.first) {
            cout << elem << " ";
        }
        cout << endl;
    }

    if (constrained_optima_original.first != constrained_optima_flip.first) {

        std::set<char> intersection;
        std::set_intersection(constrained_optima_original.first.begin(), constrained_optima_original.first.end(),
                              constrained_optima_flip.first.begin(), constrained_optima_flip.first.end(),
                              std::inserter(intersection, intersection.begin()));
        if (!intersection.empty())
        {
            cout << "{";
            for (const auto& elem : combination) {
                cout << elem << " ";
            }
            cout << "} -> "<< target_index << " is non-strictly epistatic " << endl;            
        }else{
            cout << "{";
            for (const auto& elem : combination) {
                cout << elem << " ";
            }
            cout << "} -> "<< target_index << " is strictly epistatic" << endl;            
        }



        // cout << "omega_A_pattern: "<<endl;
        // for (const auto& elem : constrained_optima_original.second) {
        //     cout << elem << endl;
        // }

        // cout << "omega_B_pattern: "<<endl;;
        // for (const auto& elem : constrained_optima_flip.second) {
        //     cout << elem << endl;
        // }

        // cout << "omega_A_[target_index] = ";
        // for (const auto& elem : constrained_optima_original.first) {
        //     cout << elem << " ";
        // }
        // cout << endl;   

        // cout << "omega_B_[target_index] = ";
        // for (const auto& elem : constrained_optima_flip.first) {
        //     cout << elem << " ";
        // }
        // cout << endl;    
        // cout << endl;  
        return true;
    }else{
        // cout << "{";
        // for (const auto& elem : combination) {
        //     cout << elem << " ";
        // }
        // cout << "} !-> "<< target_index << endl;

        // cout << "omega_A_pattern: ";
        // for (const auto& elem : constrained_optima_original.second) {
        //     cout << elem << endl;
        // }

        // cout << "omega_B_pattern: ";
        // for (const auto& elem : constrained_optima_flip.second) {
        //     cout << elem << endl;
        // }

        // cout << "omega_A_[target_index] = ";
        // for (const auto& elem : constrained_optima_original.first) {
        //     cout << elem << " ";
        // }
        // cout << endl;   

        // cout << "omega_B_[target_index] = ";
        // for (const auto& elem : constrained_optima_flip.first) {
        //     cout << elem << " ";
        // }
        // cout << endl;    
        // cout << endl;        
        return false;
    }
}

int check_weak(int target_index, auto combination, auto enumeration, auto chromosomes) {

    if (DEBUG){
        cout << endl;
        cout << "---------- in check_weak --------------" << endl;
    }


    if (DEBUG)
    {
        cout << "current combination index is ";
        for (const auto& elem : combination) {
            cout << elem << " ";
        }
        cout << endl;
    }
    
    
    
    if (DEBUG)
    {
        cout << "current enumeration is ";
        for (const auto& elem : enumeration) {
            cout << elem << " ";
        }
        cout << endl;
    }

    // cout << "current combination index is ";
    // for (const auto& elem : combination) {
    //     cout << elem << " ";
    // }
    // cout << endl;

    // cout << "current enumeration is ";
    // for (const auto& elem : enumeration) {
    //     cout << elem << " ";
    // }
    // cout << "target_index = "<< target_index <<endl;


    auto enumeration_original = enumeration;
    int condition_holds = 0;
    for (int condition_index = 0; condition_index < enumeration.size(); condition_index++)
    {
        // cout << "--------------------------------------" << endl;
        // cout << "enumeration_original" << endl;
        // for (const auto& elem : enumeration_original) {
        //     cout << elem << " ";
        // }
        // cout << endl;

        enumeration = enumeration_original;
        enumeration[condition_index] = 1 - enumeration[condition_index]; // flip bit 

        // if (DEBUG)
        // {
        //     cout << "A =  ";
        //     for (const auto& elem : enumeration_original) {
        //         cout << elem << " ";
        //     }
        //     cout << endl;
        // }

        // if (DEBUG)
        // {
        //     cout << "B =  ";
        //     for (const auto& elem : enumeration) {
        //         cout << elem << " ";
        //     }
        //     cout << endl;
        // }
        

        bool not_equal;
        not_equal = check_constrained_optima(target_index, combination, enumeration_original, enumeration, chromosomes);
        // cout << "not_equal: " << not_equal << endl;
        // cout << "######################################" << endl;

        if (not_equal)
        {
            condition_holds++;
        }else{
            return 0;
        }
    }

    if (condition_holds == enumeration.size())
    {


        // cout << "{";
        // for (const auto& elem : combination) {
        //     cout << elem << " ";
        // }
        // cout << "} -> "<< target_index << endl;        
        return 1;
    }

    if (DEBUG){
        cout << "---------- out check_weak --------------" << endl;
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

        // cout << "all combinations have " << combinations.size() << " pattern:"<< endl;
        // for (const auto& combination : combinations) {
        //     for (const auto& elem : combination) {
        //         cout << elem << " ";
        //     }
        //     cout << endl;
        // }    
        if (DEBUG)
        {
            cout << "all combinations have " << combinations.size() << " pattern:"<< endl;
            for (const auto& combination : combinations) {
                for (const auto& elem : combination) {
                    cout << elem << " ";
                }
                cout << endl;
            }
        }



        for (auto& combination : combinations) // combination = [1, 2]
        { 
            bool not_find_smaller_epi;

            if (epi_size == 1)
            {
                if (DEBUG)
                    cout << "epi_size == 1" << endl;
                not_find_smaller_epi = true;  
                
            }else{
                return weak_epi_count;
                if (DEBUG)
                    cout << "epi_size == " << epi_size <<endl;


                not_find_smaller_epi = true;  
                int smaller_epi_size = epi_size;
                bool is_subset;
                while(not_find_smaller_epi && smaller_epi_size>=1)
                {   
                    // cout << "previous!!" << endl;
                    for (auto& previous : weak_epi_set[smaller_epi_size-1]) 
                    {
                        // cout << "previous" << endl;

                        // for (const auto& elem : previous) {
                        //     cout << elem << " ";
                        // }
                        // cout<<endl;

                        is_subset = isSubset(previous, combination);
                        not_find_smaller_epi = !is_subset;
                        if(is_subset) break;
                    }
                    smaller_epi_size--;
                }
            }


            // std::cout<<"epi_size "<<epi_size<<std::endl;
            // std::cout<<"not_find_smaller_epi "<<not_find_smaller_epi<<std::endl;
            // 如果沒有找到更小的epi，則進入if >>> 這裡使用 weak 機制排除
            if(not_find_smaller_epi)
            {

                // cout << "not_find_smaller_epi" << endl;
                // for (const auto& elem : combination) {
                //     cout << elem << " ";
                // }
                // cout << endl;
                auto enumerations = generateBinarySequences(epi_size); // enumerations = { [0, 0], [0, 1], [1, 0], [1, 1] }    
                
                if (DEBUG)
                {
                    cout << "current combination index is ";
                    for (const auto& elem : combination) {
                        cout << elem << " ";
                    }
                    cout << endl;
                }
                
                
                
                if (DEBUG)
                {
                    cout << "enumerations have " << enumerations.size() << " pattern:"<< endl;
                    for (const auto& enumeration : enumerations) {
                        for (const auto& elem : enumeration) {
                            cout << elem << " ";
                        }
                        cout << endl;
                    }
                    cout << endl;
                }
                
                if (DEBUG)
                {
                    cout << "################ start count weak ####################" << endl;
                }
               
                // cout << "enumerations" << endl;
                // 遍歷所有的 pattern 不用 remove best_sequence 的 pattern
                for (auto& enumeration : enumerations) // combination = [1, 2] and enumerations = { [0, 0], [0, 1], [1, 0], [1, 1]}
                { 
                    int result = check_weak(target_index, combination, enumeration, chromosomes);
                    weak_epi_count[epi_size] += result;
                    if (result)
                    {
                        weak_epi_set[epi_size].push_back(combination);
                        break;
                    }

                    if (DEBUG)
                    {
                        cout << "is the enumeration weak? " <<  weak_epi_count[epi_size] << endl;
                    }
                }

                 if (DEBUG)
                {
                    cout << "################ end count weak ####################" << endl;
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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <Problem Length L> <Fitness Method>" << endl;
        return 1;
    }

    int L = stoi(argv[1]);
    string method = argv[2];

    auto chromosomes = generate_chromosomes(L, method);


    // 排序根據 chom.second 的值由高到低
    sort(chromosomes.begin(), chromosomes.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; // 由高到低排序
    });

    cout << "chromosomes & fitness" << endl;
    for (const auto& chom : chromosomes) {
        cout << chom.first << " " << chom.second << endl;
    }
    cout << endl;
    

    for (int target_index = 0; target_index < L; target_index++) {
        cout << "S -> " << target_index << endl;
        
        std::vector<int> weak_epi_count_results = count_weak(L, target_index, chromosomes, method);
        // cout << "order_0  order_1 order_2 ... order_(ell-1): "<< endl;
        for (int count : weak_epi_count_results) {
            cout << count << " ";
        }
        cout << endl;
    }
    cout << endl;
    return 0;
}
