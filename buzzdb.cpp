#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <memory>

enum FieldType {
  INT,
  FLOAT,
  STRING
};

class Field {
  public:
    FieldType type;
    std::unique_ptr<char[]> data;
    size_t data_length;

    Field(int i) : type(INT) {
      data_length = sizeof(int);
      data = std::make_unique<char[]>(data_length);
      std::memcpy(data.get(), &i, data_length);
    }
    
    Field(float f) : type(FLOAT) {
      data_length = sizeof(float);
      data = std::make_unique<char[]>(data_length);
      std::memcpy(data.get(), &f, data_length);
    }

    Field(const std::string &s) : type(STRING) {
      data_length = s.size() + 1; // +1 for null terminator
      data = std::make_unique<char[]>(data_length);
      std::strcpy(data.get(), s.c_str());
    }

    // Copy constructor
    Field(Field&& other) {
      type = other.type;
      data = std::make_unique<char[]>(other.data_length);
      std::memcpy(data.get(), other.data.get(), other.data_length);
    }

    // Assignment operator
    Field& operator=(Field&& other) {
      if (&other == this) {
        return *this;
      }
      
      type = other.type;
      data = std::make_unique<char[]>(other.data_length);
      std::memcpy(data.get(), other.data.get(), other.data_length);

      return *this;
    }

    FieldType getType() const {
      return type;
    }

    int asInt() const {
      return *reinterpret_cast<int*>(data.get());
    }

    float asFloat() const {
      return *reinterpret_cast<float*>(data.get());
    }

    std::string asString() const {
      return std::string(data.get());
    }

    // Const means function will not modify the object
    void print() const {
      switch (type) {
        case INT:
          std::cout << asInt();
          break;
        case FLOAT:
          std::cout << asFloat();
          break;
        case STRING:
          std::cout << asString();
          break;
      }
    }
};

class Tuple {
  public:
    std::vector<std::unique_ptr<Field>> fields;

    void addField(std::unique_ptr<Field> field) {
      fields.push_back(std::move(field));
    }

    size_t getSize() const {
      size_t total_size = 0;
      for (const auto& field : fields) {
        total_size += field->data_length;
      }
      return total_size;
    }

    void print() const {
      for (const auto& field : fields) {
        field->print();
        std::cout << " ";
      }
    }
};

const size_t PAGE_SIZE = 4096; // 4KB page size

class Page {
  public:
    size_t used_size = 0;
    std::vector<std::unique_ptr<Tuple>> tuples;

    bool addTuple(std::unique_ptr<Tuple> tuple) {
      size_t tuple_size = tuple->getSize(); // Calculate size of tuple
      if (used_size + tuple_size > PAGE_SIZE) {
        // Garbage collect
      }

      if (used_size + tuple_size > PAGE_SIZE) {
        std::cout << "Page is full, cannot add tuple." << "\n";
        std::cout << "Page contains " << tuples.size() << " tuples." << "\n";
        return false; // Not enough space
      }

      tuples.push_back(std::move(tuple));
      used_size += tuple_size;
      return true;
    }

    void write(const std::string &filename) const {
      std::ofstream out(filename);

      // Write # of tuples
      size_t num_tuples = tuples.size();
      out.write(reinterpret_cast<const char*>(&num_tuples), sizeof(num_tuples));

      // Write each tuple
      for (const auto& tuple : tuples) {
        size_t num_fields = tuple->fields.size();
        out.write(reinterpret_cast<const char*>(&num_fields), sizeof(num_fields));

        for (const auto& field : tuple->fields) {
          out.write(reinterpret_cast<const char*>(&field->type), sizeof(field->type));
          out.write(reinterpret_cast<const char*>(&field->data_length), sizeof(field->data_length));
          out.write(field->data.get(), field->data_length);
        }
      }
    }

    void read(const std::string &filename) {
    }
};

class BuzzDB {
  public:
    // Stores all tuples
    std::vector<std::unique_ptr<Tuple>> table;
    Page page;

    void insert(int key, int value) {
      auto newTuple = std::make_unique<Tuple>();
      std::unique_ptr<Field> keyField = std::make_unique<Field>(key);
      newTuple->addField(std::move(keyField)); // Add key field
      std::unique_ptr<Field> valueField = std::make_unique<Field>(value);
      newTuple->addField(std::move(valueField)); // Add value field
      std::unique_ptr<Field> floatField = std::make_unique<Field>(123.04f);
      newTuple->addField(std::move(floatField));
      std::unique_ptr<Field> stringField = std::make_unique<Field>("example_string");
      newTuple->addField(std::move(stringField));

      // table.push_back(newTuple); // Add to main table
      index[key].push_back(value); // Update index
      page.addTuple(std::move(newTuple)); // Add to page
    }

    void selectGroupBySum() {
      for (auto const &pair : index) {
        int sum = 0;
        for (auto const& value : pair.second) {
          sum += value; // Calculate sum for each key
        }
        std::cout << "Key: " << pair.first << ", Sum: " << sum << "\n"; // Output result
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
    std::cerr << "Error opening file." << "\n";
    return 1;
  }

  int field1, field2;
  while (inputFile >> field1 >> field2)
  {
    db.insert(field1, field2);
  }
  

  db.selectGroupBySum();

  std::string filename = "page.dat";
  db.page.write(filename);

  Page page2;
  page2.read(filename);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " seconds" << "\n";

  return 0;
}