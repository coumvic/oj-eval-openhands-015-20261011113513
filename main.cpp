#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <sstream>
#include <cctype>

const std::string DATA_FILE = "database.dat";

// Database class for file-based storage using append-only log
class FileDatabase {
private:
    std::string filename;
    
    // For a given index, scan the log and return active values
    std::set<int> findActiveValues(const std::string& targetIndex) {
        std::set<int> activeValues;
        std::ifstream file(filename);
        
        if (!file) {
            // File doesn't exist, return empty set
            return activeValues;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string index;
            int value;
            std::string status;
            
            if (iss >> index >> value) {
                // Only process records for the target index
                if (index == targetIndex) {
                    // Check if this is a deletion marker
                    if (iss >> status && status == "D") {
                        // This is a deletion, remove the value
                        activeValues.erase(value);
                    } else {
                        // This is an insertion, add the value
                        activeValues.insert(value);
                    }
                }
            }
        }
        
        file.close();
        return activeValues;
    }

public:
    FileDatabase(const std::string& file) : filename(file) {}
    
    void insert(const std::string& index, int value) {
        // Append the new record to the file
        std::ofstream file(filename, std::ios::app);
        file << index << " " << value << "\n";
        file.close();
    }
    
    void remove(const std::string& index, int value) {
        // Append a deletion marker for this record
        std::ofstream file(filename, std::ios::app);
        file << index << " " << value << " D\n";
        file.close();
    }
    
    std::vector<int> find(const std::string& index) {
        // Get active values for this index
        auto activeValues = findActiveValues(index);
        
        // Convert set to vector (already sorted due to set properties)
        return std::vector<int>(activeValues.begin(), activeValues.end());
    }
};

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Helper function to split string by whitespace
std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

int main() {
    FileDatabase db(DATA_FILE);
    
    int n;
    std::cin >> n;
    std::cin.ignore(); // Ignore the newline after n
    
    for (int i = 0; i < n; i++) {
        std::string line;
        std::getline(std::cin, line);
        
        auto tokens = split(line);
        
        if (tokens.empty()) continue;
        
        if (tokens[0] == "insert" && tokens.size() == 3) {
            std::string index = tokens[1];
            int value = std::stoi(tokens[2]);
            db.insert(index, value);
        }
        else if (tokens[0] == "delete" && tokens.size() == 3) {
            std::string index = tokens[1];
            int value = std::stoi(tokens[2]);
            db.delete(index, value);
        }
        else if (tokens[0] == "find" && tokens.size() == 2) {
            std::string index = tokens[1];
            auto values = db.find(index);
            
            if (values.empty()) {
                std::cout << "null" << std::endl;
            } else {
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) std::cout << " ";
                    std::cout << values[j];
                }
                std::cout << std::endl;
            }
        }
    }
    
    return 0;
}