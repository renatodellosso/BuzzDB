#include <vector>
#include <map>
#include <iostream>

class Tuple {
  public:
    int key; // ID
    int value; // Data
};

class BuzzDB {
  public:
    // Stores all tuples
    std::vector<Tuple> table;

    void insert(int key, int value) {
      Tuple newTuple = {key, value};
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
  BuzzDB db;

  db.insert(1, 100);
  db.insert(1, 200);

  db.insert(2, 50);
  
  db.insert(3, 200);
  db.insert(3, 200);
  db.insert(3, 100);
  
  db.insert(4, 500);

  db.selectGroupBySum();
  return 0;
}