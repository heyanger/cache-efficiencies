#include <map>
#include <memory>

class CacheMetrics {
  public:
    int hits = 0;
    int miss = 0;

    double hitratio() {
      if (hits == 0 && miss == 0) {
        return 0;
      }
      return (double) hits / (hits + miss);
    }

    void debug() {
      std::cout << hits << std::endl;
      std::cout << miss << std::endl;
    }
};

class Node {
 public:
  std::shared_ptr<Node> left;
  std::shared_ptr<Node> right;
  int key;
  int value;
};

class LRUCache {
 public:
  explicit LRUCache(int capacity) {
    head_.reset(new Node());
    tail_.reset(new Node());

    head_->right = tail_;
    tail_->left = head_;
    capacity_ = capacity;
  }

  int get(int key) {
    if (map_.find(key) == map_.end()) {
      metrics.miss += 1;
      return -1;
    }
    metrics.hits += 1;
    refer(key);
    return map_[key]->value;
  }

  void put(int key, int value) {
    // Not found and over capacity
    if (map_.find(key) == map_.end() && map_.size() >= capacity_) {
      evict();
    }
    std::shared_ptr<Node> node = refer(key);
    map_[key] = node;
    node->value = value;
  }

  CacheMetrics metrics;

 private:
  void evict() {
    std::shared_ptr<Node> node = tail_->left;
    map_.erase(node->key);
    std::shared_ptr<Node> front = node->left;
    front->right = tail_;
    tail_->left = front;
  }

  std::shared_ptr<Node> refer(int key) {
    std::shared_ptr<Node> node;
    if (map_.find(key) == map_.end()) {
      node.reset(new Node());
      node->key = key;
    } else {
      node = map_[key];
      std::shared_ptr<Node> node_left = node->left;
      std::shared_ptr<Node> node_right = node->right;
      node->left.reset();
      node->right.reset();
      node_left->right = node_right;
      node_right->left = node_left;
    }
    std::shared_ptr<Node> front = head_->right;

    head_->right = node;
    node->left = head_;
    node->right = front;
    front->left = node;
    return node;
  }

  std::shared_ptr<Node> head_;
  std::shared_ptr<Node> tail_;
  std::map<int, std::shared_ptr<Node>> map_;
  int capacity_;
};
