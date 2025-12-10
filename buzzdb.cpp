#include <vector>
#include <map>
#include <iostream>
#include <fstream>

enum FieldType {
  INT,
  FLOAT,
  STRING
};

class Field {
  public:
    Field(int i) : type(INT), data{.i = i} {}
    Field(float f) : type(FLOAT), data{.f = f} {}
    Field(const std::string &s) : type(STRING) {
      data.s = new char[s.size() + 1]; // New is on heap, so we have to manually deallocate
      std::copy(s.begin(), s.end(), data.s);
      data.s[s.size()] = '\0'; // Null-terminate the string
    }

    ~Field() {
      if (type == STRING && data.s != nullptr) {
        delete[] data.s; // Free allocated memory for string
      }
    }

    FieldType type;
    union {
      int i;
      float f;
      char* s;
    } data;

    // Const means function will not modify the object
    void print() const {
      switch (type) {
        case INT:
          std::cout << data.i;
          break;
        case FLOAT:
          std::cout << data.f;
          break;
        case STRING:
          std::cout << data.s;
          break;
      }
    }
};

class Tuple {
  std::vector<Field> fields;

  public:
    ~Tuple() {
      for (auto& field : fields) {
        if (field.type == STRING) {
          field.~Field();
        }
      }
    }

    void addField(const Field& field) {
      fields.push_back(field);
    }

    void print() const {
      for (const auto& field : fields) {
        field.print();
        std::cout << " ";
      }
    }
};

class BuzzDB {
  public:
    // Stores all tuples
    std::vector<Tuple> table;

    void insert(int key, int value) {
      Tuple newTuple;
      newTuple.addField(Field(key)); // Add key field
      newTuple.addField(Field(value)); // Add value field
      newTuple.addField(Field(123.04f));

      table.push_back(newTuple); // Add to main table
      index[key].push_back(value); // Update index
    }

    void selectGroupBySum() {
      for (auto const &pair : index) {
        int sum = 0;
        for (auto const& value : pair.second) {
          sum += value; // Calculate sum for each key
        }
        std::cout << "Key: " << pair.first << ", Sum: " << sum << std::endl; // Output result
      }
    }
  
  private:
    std::map<int, std::vector<int>> index;
};

int main() {
  auto start = std::chrono::high_resolution_clock::now();
  BuzzDB db;

  std::ifstream inputFile("input.txt");

  if (!inputFile) {
    std::cerr << "Error opening file." << std::endl;
    return 1;
  }

  int field1, field2;
  while (inputFile >> field1 >> field2)
  {
    db.insert(field1, field2);
  }
  

  db.selectGroupBySum();

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

  return 0;
}