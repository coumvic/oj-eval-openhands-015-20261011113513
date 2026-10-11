#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <cctype>

const std::string DATA_FILE = "database.dat";

// Structure to store key-value pairs
struct Record {
    std::string index;
    int value;
    
    bool operator<(const Record& other) const {
        if (index != other.index) return index < other.index;
        return value < other.value;
    }
};

// Database class for file-based storage
class FileDatabase {
private:
    std::string filename;
    
    // Load all records from file
    std::vector<Record> loadRecords() {
        std::vector<Record> records;
        std::ifstream file(filename, std::ios::binary);
        
        if (!file) {
            // File doesn't exist, return empty vector
            return records;
        }
        
        Record record;
        while (file.read(reinterpret_cast<char*>(&record), sizeof(Record))) {
            records.push_back(record);
        }
        
        file.close();
        return records;
    }
    
    // Save all records to file
    void saveRecords(const std::vector<Record>& records) {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        
        for (const auto& record : records) {
            file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
        }
        
        file.close();
    }

public:
    FileDatabase(const std::string& file) : filename(file) {}
    
    void insert(const std::string& index, int value) {
        auto records = loadRecords();
        
        // Check if this exact record already exists
        for (const auto& record : records) {
            if (record.index == index && record.value == value) {
                return; // Already exists, do nothing
            }
        }
        
        // Add new record
        Record newRecord;
        newRecord.index = index;
        newRecord.value = value;
        records.push_back(newRecord);
        
        // Sort records for consistent ordering
        std::sort(records.begin(), records.end());
        
        saveRecords(records);
    }
    
    void remove(const std::string& index, int value) {
        auto records = loadRecords();
        
        // Find and remove the record
        auto it = std::remove_if(records.begin(), records.end(), 
                                [&index, value](const Record& record) {
                                    return record.index == index && record.value == value;
                                });
        records.erase(it, records.end());
        
        saveRecords(records);
    }
    
    std::vector<int> find(const std::string& index) {
        std::vector<int> values;
        auto records = loadRecords();
        
        // Find all records with the given index
        for (const auto& record : records) {
            if (record.index == index) {
                values.push_back(record.value);
            }
        }
        
        // Sort values in ascending order
        std::sort(values.begin(), values.end());
        
        return values;
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