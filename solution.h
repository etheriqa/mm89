#pragma once

#include <algorithm>
#include <functional>
#include <queue>
#include <random>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

using size_type = std::uint_least16_t;
using direction_type = std::uint_least8_t;
using coordinate_type = std::uint_least8_t;
using real_type = std::double_t;

const size_type kMaxCoordinate = 80;
const size_type kGridSize = kMaxCoordinate * kMaxCoordinate;

enum class Cell : std::uint_least8_t {
  straight = 0,
  right    = 1,
  turn     = 2,
  left     = 3,
  every    = 4,
  outside  = 5,
};

enum class Direction : direction_type {
  up    = 0,
  right = 1,
  down  = 2,
  left  = 3,
};

class Vertex {
private:
  coordinate_type x_, y_;
public:
  Vertex(coordinate_type x, coordinate_type y) : x_(x), y_(y) {}
  static size_type pack(coordinate_type x, coordinate_type y);
  static Vertex unpack(size_type v);
  coordinate_type& x() { return x_; }
  const coordinate_type& x() const { return x_; }
  coordinate_type& y() { return y_; }
  const coordinate_type& y() const { return y_; }
  bool operator==(const Vertex& v) const;
  bool operator!=(const Vertex& v) const;
  size_type pack() const;
  std::vector<Vertex> adjacents(coordinate_type w, coordinate_type h) const;
  Vertex move(Direction direction) const;
  Direction direction(const Vertex& adjacent) const;
};

namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(const Vertex& v) const;
};
}

using VertexSet = std::unordered_set<Vertex>;
using Path = std::vector<Vertex>;

class GridIterator;

template <typename T>
class Grid {
private:
  using reference = typename std::vector<T>::reference;
  using const_reference = typename std::vector<T>::const_reference;
  coordinate_type width_, height_;
  std::vector<T> grid_;
public:
  Grid(coordinate_type w, coordinate_type h, const T& value)
    : width_(w), height_(h), grid_(kGridSize, value) {}
  const coordinate_type& width() const { return width_; }
  const coordinate_type& height() const { return height_; }
  reference operator[](size_type v) { return grid_[v]; }
  const_reference operator[](size_type v) const { return grid_[v]; }
  reference at(const Vertex& v);
  const_reference at(const Vertex& v) const;
  reference at(coordinate_type x, coordinate_type y);
  const_reference at(coordinate_type x, coordinate_type y) const;
  GridIterator begin() const;
  GridIterator end() const;
};

class GridIterator {
private:
  coordinate_type width_, height_;
  Vertex v_;
  GridIterator(coordinate_type w, coordinate_type h, const Vertex& v)
    : width_(w), height_(h), v_(v) {}
public:
  static GridIterator begin(coordinate_type w, coordinate_type h);
  static GridIterator end(coordinate_type w, coordinate_type h);
  const Vertex& operator*() const { return v_; };
  GridIterator& operator++();
  bool operator==(const GridIterator& it) const;
  bool operator!=(const GridIterator& it) const;
};

class Maze : public Grid<Cell> {
private:
  Maze(const Grid<Cell>& grid) : Grid<Cell>(grid) {}
public:
  static Maze initialize(const Grid<Cell>& grid);
  Vertex forward(const Vertex& v0, const Vertex& v1) const;
  Vertex backward(const Vertex& v0, const Vertex& v1) const;
  std::vector<Path> searchComplete() const;
  std::vector<Path> searchLeading() const;
  std::vector<Path> searchTrailing() const;
private:
  std::vector<Path>
  search(std::function<Vertex(Maze, Vertex, Vertex)> next, bool complete) const;
};

class State {
private:
  Maze maze_;
public:
  explicit State(const Maze& maze) : maze_(maze) {}
  std::vector<std::tuple<Vertex, Cell>> fixes(const Maze& maze) const;
  size_type countNumberOfFixing(const Maze& original) const;
  real_type calculateEnergy() const;
  template <typename PRNG>
  State propose(const Maze& original, PRNG& random) const;
private:
  template <typename PRNG>
  std::tuple<Path, Path> sampleSubpathPair(PRNG& random) const;
  template <typename PRNG>
  void connectSubpathPair(const Path& leading, const Path& trailing, PRNG& random);
  void clean(const Maze& original);
};

class MazeFixing {
public:
  std::vector<std::string>
  improve(const std::vector<std::string>& grid, size_type max_fixing);
private:
  static Maze buildMaze(const std::vector<std::string>& grid);
};



size_type
Vertex::pack(coordinate_type x, coordinate_type y) {
  return x + y * kMaxCoordinate;
}

Vertex
Vertex::unpack(size_type v) {
  return Vertex(v % kMaxCoordinate, v / kMaxCoordinate);
}

bool
Vertex::operator==(const Vertex& v) const {
  return x_ == v.x_ && y_ == v.y_;
}

bool
Vertex::operator!=(const Vertex& v) const {
  return !(*this == v);
}

size_type
Vertex::pack() const {
  return pack(x_, y_);
}

std::vector<Vertex>
Vertex::adjacents(coordinate_type w, coordinate_type h) const {
  std::vector<Vertex> vs;
  if (y_ > 0) {
    vs.emplace_back(x_, y_ - 1);
  }
  if (x_ < w - 1) {
    vs.emplace_back(x_ + 1, y_);
  }
  if (y_ < h - 1) {
    vs.emplace_back(x_, y_ + 1);
  }
  if (x_ > 0) {
    vs.emplace_back(x_ - 1, y_);
  }
  return vs;
}

Vertex
Vertex::move(Direction direction) const {
  switch (direction) {
  case Direction::up:
    return Vertex(x_, y_ - 1);
  case Direction::right:
    return Vertex(x_ + 1, y_);
  case Direction::down:
    return Vertex(x_, y_ + 1);
  case Direction::left:
    return Vertex(x_ - 1, y_);
  default:
    throw std::logic_error("Invalid direction");
  }
}

Direction
Vertex::direction(const Vertex& adjacent) const {
  if (x_ == adjacent.x_ && y_ - 1 == adjacent.y_) { return Direction::up; }
  if (x_ + 1 == adjacent.x_ && y_ == adjacent.y_) { return Direction::right; }
  if (x_ == adjacent.x_ && y_ + 1 == adjacent.y_) { return Direction::down; }
  if (x_ - 1 == adjacent.x_ && y_ == adjacent.y_) { return Direction::left; }
  throw std::logic_error("Given vertex must be adjacent to this");
}

size_t
std::hash<Vertex>::operator()(const Vertex& v) const {
  return std::hash<size_type>()(v.pack());
}

template <typename T>
typename Grid<T>::reference
Grid<T>::at(const Vertex& v) {
  return (*this)[v.pack()];
}

template <typename T>
typename Grid<T>::const_reference
Grid<T>::at(const Vertex& v) const {
  return (*this)[v.pack()];
}

template <typename T>
typename Grid<T>::reference
Grid<T>::at(coordinate_type x, coordinate_type y) {
  return (*this)[Vertex::pack(x, y)];
}

template <typename T>
typename Grid<T>::const_reference
Grid<T>::at(coordinate_type x, coordinate_type y) const {
  return (*this)[Vertex::pack(x, y)];
}

template <typename T>
GridIterator
Grid<T>::begin() const {
  return GridIterator::begin(width_, height_);
}

template <typename T>
GridIterator
Grid<T>::end() const {
  return GridIterator::end(width_, height_);
}

GridIterator
GridIterator::begin(coordinate_type w, coordinate_type h) {
  return GridIterator(w, h, Vertex(0, 0));
}

GridIterator
GridIterator::end(coordinate_type w, coordinate_type h) {
  return GridIterator(w, h, Vertex(kMaxCoordinate, kMaxCoordinate));
}

GridIterator&
GridIterator::operator++() {
  v_.x()++;
  v_.y() += v_.x() / width_;
  v_.x() %= width_;
  if (v_.y() >= height_) {
    *this = end(width_, height_);
  }
  return *this;
}

bool
GridIterator::operator==(const GridIterator& it) const {
  return width_ == it.width_ && height_ == it.height_ && v_ == it.v_;
}

bool
GridIterator::operator!=(const GridIterator& it) const {
  return !(*this == it);
}

Maze
Maze::initialize(const Grid<Cell>& grid) {
  Maze maze(grid);
  VertexSet visited;
  std::stack<Vertex> stack;
  for (const auto& v : maze) {
    if (maze.at(v) == Cell::outside) {
      stack.push(v);
    }
  }
  while (!stack.empty()) {
    const auto v0 = stack.top();
    stack.pop();
    for (const auto v1 : v0.adjacents(maze.width(), maze.height())) {
      if (!visited.count(v1) && maze.at(v1) == Cell::every) {
        maze.at(v1) = Cell::outside;
        visited.insert(v1);
        stack.push(v1);
      }
    }
  }
  return maze;
}

Vertex
Maze::forward(const Vertex& v0, const Vertex& v1) const {
  if (at(v1) == Cell::every || at(v1) == Cell::outside) {
    throw std::logic_error("v1 should be (straight|right|turn|left)");
  }
  const auto d0 = static_cast<direction_type>(v0.direction(v1));
  const auto d1 = static_cast<direction_type>(at(v1));
  return v1.move(static_cast<Direction>((d0 + d1) % 4));
}

Vertex
Maze::backward(const Vertex& v0, const Vertex& v1) const {
  if (at(v1) == Cell::every || at(v1) == Cell::outside) {
    throw std::logic_error("v1 should be (straight|right|turn|left)");
  }
  const auto d0 = static_cast<direction_type>(v0.direction(v1));
  const auto d1 = static_cast<direction_type>(at(v1));
  return v1.move(static_cast<Direction>((4 + d0 - d1) % 4));
}

std::vector<Path>
Maze::searchComplete() const {
  return search(std::mem_fn(&Maze::forward), true);
}

std::vector<Path>
Maze::searchLeading() const {
  return search(std::mem_fn(&Maze::forward), false);
}

std::vector<Path>
Maze::searchTrailing() const {
  return search(std::mem_fn(&Maze::backward), false);
}

std::vector<Path>
Maze::search(std::function<Vertex(Maze, Vertex, Vertex)> next, bool complete) const {
  std::vector<Path> subpaths;
  for (const auto& end : *this) {
    if (at(end) != Cell::every && at(end) != Cell::outside) {
      continue;
    }
    for (auto v1 : end.adjacents(width(), height())) {
      if (at(v1) == Cell::every || at(v1) == Cell::outside) {
        continue;
      }
      auto v0 = end;
      VertexSet visited({v0, v1});
      Path subpath({v0, v1});
      while (true) {
        std::tie(v0, v1) = std::make_tuple(v1, next(*this, v0, v1));
        if (visited.count(v1)) {
          if (!complete) {
            subpaths.push_back(subpath);
          }
          break;
        }
        visited.insert(v1);
        subpath.push_back(v1);
        if (at(v1) == Cell::every || at(v1) == Cell::outside) {
          if (complete) {
            subpaths.push_back(subpath);
          }
          break;
        }
      }
    }
  }
  return subpaths;
}

std::vector<std::tuple<Vertex, Cell>>
State::fixes(const Maze& original) const {
  std::vector<std::tuple<Vertex, Cell>> fs;
  for (const auto& v : original) {
    if (maze_.at(v) != original.at(v)) {
      fs.emplace_back(v, maze_.at(v));
    }
  }
  return fs;
}

size_type
State::countNumberOfFixing(const Maze& original) const {
  size_type count = 0;
  for (const auto& v : original) {
    if (maze_.at(v) != original.at(v)) {
      count++;
    }
  }
  return count;
}

real_type
State::calculateEnergy() const {
  Grid<bool> contribution(maze_.width(), maze_.height(), false);
  for (const auto subpath : maze_.searchComplete()) {
    for (const auto& v : subpath) {
      if (maze_.at(v) != Cell::outside) {
        contribution.at(v) = true;
      }
    }
  }
  real_type energy = 0;
  for (const auto& v : contribution) {
    if (contribution.at(v)) {
      energy -= 1;
    }
  }
  return energy;
}

template <typename PRNG>
State
State::propose(const Maze& original, PRNG& random) const {
  auto state = *this;
  Path leading, trailing;
  std::tie(leading, trailing) = state.sampleSubpathPair(random);
  state.connectSubpathPair(leading, trailing, random);
  state.clean(original);
  return state;
}

template <typename PRNG>
std::tuple<Path, Path>
State::sampleSubpathPair(PRNG& random) const {
  const auto leadings = maze_.searchLeading();
  const auto trailings = maze_.searchTrailing();
  while (true) {
    const auto leading =
      leadings[std::uniform_int_distribution<>(0, leadings.size() - 1)(random)];
    const auto trailing =
      trailings[std::uniform_int_distribution<>(0, trailings.size() - 1)(random)];
    const auto distance = std::abs(leading.back().x() - trailing.back().x()) +
                          std::abs(leading.back().y() - trailing.back().y());
    if (distance > 10) {
      continue;
    }
    return std::make_tuple(leading, trailing);
  }
}

template <typename PRNG>
void
State::connectSubpathPair(const Path& leading, const Path& trailing, PRNG& random) {
  VertexSet visited;
  for (const auto& v : leading) {
    visited.insert(v);
  }
  for (const auto& v : trailing) {
    visited.insert(v);
  }
  std::queue<Path> queue({{*(leading.rbegin() + 1), *leading.rbegin()}});
  while (!queue.empty()) {
    auto subpath = queue.front();
    queue.pop();
    const auto& v0 = subpath.back();
    auto adjacents = v0.adjacents(maze_.width(), maze_.height());
    std::shuffle(adjacents.begin(), adjacents.end(), random);
    for (const auto& v1 : adjacents) {
      subpath.push_back(v1);
      if (v1 == *trailing.rbegin()) {
        subpath.push_back(*(trailing.rbegin() + 1));
        for (size_type i = 0; i < subpath.size() - 2; i++) {
          const auto& u0 = subpath[i];
          const auto& u1 = subpath[i + 1];
          const auto& u2 = subpath[i + 2];
          const auto d01 = static_cast<size_type>(u0.direction(u1));
          const auto d12 = static_cast<size_type>(u1.direction(u2));
          maze_.at(u1) = static_cast<Cell>((4 + d12 - d01) % 4);
        }
        return;
      }
      const auto& cell = maze_.at(v1);
      if (!visited.count(v1) && cell != Cell::every && cell != Cell::outside) {
        visited.insert(v1);
        queue.push(subpath);
      }
      subpath.pop_back();
    }
  }
}

void
State::clean(const Maze& original) {
  Grid<bool> contribution(maze_.width(), maze_.height(), false);
  for (const auto subpath : maze_.searchComplete()) {
    for (const auto& v : subpath) {
      contribution.at(v) = true;
    }
  }
  for (const auto& v : contribution) {
    if (!contribution.at(v)) {
      maze_.at(v) = original.at(v);
    }
  }
}

std::vector<std::string>
MazeFixing::improve(const std::vector<std::string>& cells, size_type max_fixing) {
  std::mt19937_64 random(0);
  const auto maze = buildMaze(cells);
  State state(maze);
  auto energy = state.calculateEnergy();
  for (size_type i = 0; i < 500; i++) {
    const auto new_state = state.propose(maze, random);
    const auto new_energy = new_state.calculateEnergy();
    if (new_energy > energy) {
      continue;
    }
    const auto n_fixing = new_state.countNumberOfFixing(maze);
    if (n_fixing > max_fixing) {
      continue;
    }
    state = new_state;
    energy = new_energy;
  }
  std::vector<std::string> fixes;
  for (const auto fix : state.fixes(maze)) {
    Vertex v(0, 0);
    Cell cell;
    std::tie(v, cell) = fix;
    std::string s;
    s += std::to_string(static_cast<int>(v.y())) + " ";
    s += std::to_string(static_cast<int>(v.x())) + " ";
    switch (cell) {
    case Cell::straight:
      s += "S";
      break;
    case Cell::right:
      s += "R";
      break;
    case Cell::turn:
      s += "U";
      break;
    case Cell::left:
      s += "L";
      break;
    default:
      throw std::logic_error("");
    }
    fixes.push_back(s);
  }
  return fixes;
}

Maze
MazeFixing::buildMaze(const std::vector<std::string>& grid) {
  const auto width = grid[0].size();
  const auto height = grid.size();
  Grid<Cell> cell_grid(width, height, Cell::outside);
  for (size_type y = 0; y < height; y++) {
    for (size_type x = 0; x < width; x++) {
      switch (grid[y][x]) {
      case 'S':
        cell_grid.at(x, y) = Cell::straight;
        break;
      case 'R':
        cell_grid.at(x, y) = Cell::right;
        break;
      case 'U':
        cell_grid.at(x, y) = Cell::turn;
        break;
      case 'L':
        cell_grid.at(x, y) = Cell::left;
        break;
      case 'E':
        cell_grid.at(x, y) = Cell::every;
        break;
      }
    }
  }
  return Maze::initialize(cell_grid);
}
