#include <iostream>
#include "solution.h"

template <typename T>
T read()
{
  T value;
  std::cin >> value;
  return value;
}

int main()
{
  const auto H = read<size_t>();
  std::vector<std::string> maze(H);
  for (auto& row : maze) {
    row = read<std::string>();
  }
  const auto F = read<size_t>();

  MazeFixing solution;
  const auto ret = solution.improve(maze, F);

  std::cout << ret.size() << std::endl;
  for (const auto& row : ret) {
    std::cout << row << std::endl;
  }

  return 0;
}
