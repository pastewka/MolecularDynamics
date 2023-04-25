#include <iostream>

void set_to_one(int& value) {
  value = 1;
}

int main() {
  int * pointer = new int;
  set_to_one(*pointer);
  if (pointer = nullptr)
    std::cout << "pointer is nullptr\n";
  set_to_one(*pointer);
  return 0;
}
