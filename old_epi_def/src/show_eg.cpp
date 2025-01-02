#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <algorithm>
#include <set>

using namespace std;

// Generate all possible chromosomes based on the problem length L
vector<string> generate_chromosomes(int L) {
    vector<string> chromosomes;
    int num_combinations = pow(2, L);
    for (int i = 0; i < num_combinations; ++i) {
        string chromosome = bitset<32>(i).to_string().substr(32 - L);
        chromosomes.push_back(chromosome);
    }
    return chromosomes;
}

// Helper function for segment-based functions
double calculate_segment_fitness(const string& segment, const string& method) {
    if (method == "trap") {
        int ones = count(segment.begin(), segment.end(), '1');
        if (ones == segment.length()) {
            return 1.0;
        } else if (ones == 0) {
            return 0.8;
        } else {
            return 0.8 - (ones * 0.8 / (segment.length() - 1));
        }
    } else if (method == "niah") {
        return all_of(segment.begin(), segment.end(), [](char bit) { return bit == '1'; }) ? 1.0 : 0.0;
    }
    return 0.0;
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
    } else if (method == "leadingone") {
        int leading_ones = 0;
        for (char bit : chromosome) {
            if (bit == '1') {
                leading_ones++;
            } else {
                break;
            }
        }
        return leading_ones;
    }
    return 0.0;
}

// Find the values at index v with the highest fitness given the value at index u
set<char> find_values_at_v_with_highest_fitness(const vector<string>& chromosomes, int u, char u_value, int v, const string& method) {
    set<char> values_at_v;
    double max_fitness = -1.0;
    
    // Find the chromosome with the highest fitness under the given u_value
    for (const auto& chromosome : chromosomes) {
        if (chromosome[u] == u_value) {
            double fitness = calculate_fitness(chromosome, method);
            if (fitness > max_fitness) {
                max_fitness = fitness;
                values_at_v.clear();
                values_at_v.insert(chromosome[v]);
            } else if (fitness == max_fitness) {
                values_at_v.insert(chromosome[v]);
            }
        }
    }
    return values_at_v;
}


std::pair<std::set<char>, std::vector<std::string>> find_values_and_chromosomes_at_v_with_highest_fitness(const std::vector<std::string>& chromosomes, int u, char u_value, int v, const std::string& method) {
    std::set<char> values_at_v;
    std::vector<std::string> highest_fitness_chromosomes;
    double max_fitness = -1.0;

    for (const auto& chromosome : chromosomes) {
        if (chromosome[u] == u_value) {
            double fitness = calculate_fitness(chromosome, method);
            if (fitness > max_fitness) {
                max_fitness = fitness;
                values_at_v.clear();
                highest_fitness_chromosomes.clear();
                values_at_v.insert(chromosome[v]);
                highest_fitness_chromosomes.push_back(chromosome);
            } else if (fitness == max_fitness) {
                values_at_v.insert(chromosome[v]);
                highest_fitness_chromosomes.push_back(chromosome);
            }
        }
    }

    return {values_at_v, highest_fitness_chromosomes};
}

// Flip the value of a bit (0 to 1, 1 to 0)
string flip_value(char value) {
    return value == '0' ? "1" : "0";
}

// Check if there is a relationship between two indices
string has_relationship(const vector<string>& chromosomes, int u, int v, const string& method) {
    char u_value = chromosomes[0][u];
    auto result_before_flip = find_values_and_chromosomes_at_v_with_highest_fitness(chromosomes, u, u_value, v, method);
    auto values_at_v_before_flip = result_before_flip.first; 
    auto chromosomes_before_flip = result_before_flip.second; 
    
    char flipped_u_value = flip_value(u_value)[0];
    auto result_after_flip = find_values_and_chromosomes_at_v_with_highest_fitness(chromosomes, u, flipped_u_value, v, method);
    auto values_at_v_after_flip = result_after_flip.first; 
    auto chromosomes_after_flip = result_after_flip.second;

    if (values_at_v_before_flip != values_at_v_after_flip) {
        std::cout << "locus " << u << " is epistatic to " << "locus " <<  v << std::endl;
        std::cout << "because Ω{("<<u<<", "<<u_value<<")} = ";
        std::cout << "{";
        for (char value : values_at_v_before_flip) {
            for (const std::string& chromosome : chromosomes_before_flip) {       
                if (chromosome[v] == value) {
                    std::cout << chromosome << ", ";
                }
            }   
        }
        std::cout << "}";
        std::cout <<" ≠ Ω{("<<u<<", "<<flipped_u_value<<")} = ";
        std::cout << "{";
        for (char value : values_at_v_after_flip) {
            for (const std::string& chromosome : chromosomes_after_flip) {     
                if (chromosome[v] == value) {
                    std::cout << chromosome << ", ";
                }
            }   
        }
        std::cout << "}";      
        std::cout << std::endl; 
        //////////////////////////////////////////////////////////////////////
        std::cout << "because Ω{("<<u<<", "<<u_value<<")}["<<v<<"] = ";
        std::cout << "{";
        for (char value : values_at_v_before_flip) {
            std::cout << value << ", ";
        }   
        std::cout << "}";
        std::cout <<" ≠ Ω{("<<u<<", "<<flipped_u_value<<")}["<<v<<"] = ";
        std::cout << "{";
        for (char value : values_at_v_after_flip) {
           std::cout << value << ", ";
        }
        std::cout << "}";      
        std::cout << std::endl; 
        std::cout << std::endl; 

        return "O";  
    }else{

        std::cout << "locus " << u << " is not epistatic to " << "locus " <<  v << std::endl;
        std::cout << "because Ω{("<<u<<", "<<u_value<<")} = ";
        std::cout << "{";
        for (char value : values_at_v_before_flip) {
            for (const std::string& chromosome : chromosomes_before_flip) {
                
                if (chromosome[v] == value) {
                    std::cout << chromosome << ", ";
                }
            }   
        }
        std::cout << "}";
        std::cout <<" = Ω{("<<u<<", "<<flipped_u_value<<")} = ";
        std::cout << "{";
        for (char value : values_at_v_after_flip) {
            for (const std::string& chromosome : chromosomes_after_flip) {
                
                if (chromosome[v] == value) {
                    std::cout << chromosome << ", ";
                }
            }   
        }
        std::cout << "}";      
        std::cout << std::endl; 
        //////////////////////////////////////////////////////////////////////
        std::cout << "because Ω{("<<u<<", "<<u_value<<")}["<<v<<"] = ";
        std::cout << "{";
        for (char value : values_at_v_before_flip) {
            std::cout << value << ", ";
        }   
        std::cout << "}";
        std::cout <<" = Ω{("<<u<<", "<<flipped_u_value<<")}["<<v<<"] = ";
        std::cout << "{";
        for (char value : values_at_v_after_flip) {
           std::cout << value << ", ";
        }
        std::cout << "}";      
        std::cout << std::endl; 
        std::cout << std::endl; 
        
        return "X";  
    }
}

// Check all relationships between indices and return a matrix
vector<vector<string>> check_all_relationships(int L, const string& method) {
    vector<string> chromosomes = generate_chromosomes(L);
    vector<vector<string>> relationship_matrix(L, vector<string>(L, "X"));

    for (int u = 0; u < L; ++u) {
        for (int v = 0; v < L; ++v) {
            if (u != v) {  
                relationship_matrix[u][v] = has_relationship(chromosomes, u, v, method);
            }
        }
    }
    return relationship_matrix;
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

    vector<vector<string>> relationship_matrix = check_all_relationships(L, method); 
    print_matrix(relationship_matrix);  

    return 0;
}
