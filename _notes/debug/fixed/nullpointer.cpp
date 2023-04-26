#include <iostream>
#include <memory>

void set_to_one(int& value) {
  value = 1;
}

int main() {
  std::unique_ptr<int> pointer = std::make_unique<int>(0);
  set_to_one(*pointer);
  if (pointer == nullptr)
    std::cout << "pointer is nullptr\n";
  set_to_one(*pointer);
  return 0;
}
