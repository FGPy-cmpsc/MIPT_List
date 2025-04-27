#pragma once
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  class BaseNode;
  class Node;

  template <bool IsConst>
  class BaseIterator;

 public:
  using value_type = T;
  using allocator_type = Allocator;
  List() = default;
  List(size_t count, const T& value, const Allocator& alloc = Allocator());

  explicit List(size_t count, const Allocator& alloc = Allocator());

  List(const List& other);

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

  ~List();

  Allocator get_allocator() const { return allocator_; }

  using iterator = BaseIterator<false>;
  using const_iterator = BaseIterator<true>;
  using reverse_iterator = std::reverse_iterator<BaseIterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<BaseIterator<true>>;

  iterator begin() { return {base_node_.get_next()}; }
  iterator end() { return {&base_node_}; }
  const_iterator cbegin() const { return {base_node_.get_next()}; }
  const_iterator cend() const { return {&base_node_}; }
  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  List& operator=(const List& other);

  T& front() { return static_cast<Node*>(base_node_.get_next())->get_value(); }
  const T& front() const {
    return static_cast<const Node*>(base_node_.get_next())->get_value();
  }
  T& back() { return static_cast<Node*>(base_node_.get_prev())->get_value(); }
  const T& back() const {
    return static_cast<const Node*>(base_node_.get_prev())->get_value();
  }
  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }

  Node* create_node(BaseNode* prev_node, BaseNode* next_node, const T& value);

  void delete_node(Node* node);

  void push_back(const T& value);

  void push_front(const T& value);

  void pop_back();

  void pop_front();

 private:
  BaseNode base_node_;
  [[no_unique_address]] Allocator allocator_;
  [[no_unique_address]]
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc_;
  using alloc_traits = std::allocator_traits<
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>>;
  size_t size_ = 0;
};

template <typename T, typename Allocator>
class List<T, Allocator>::BaseNode {
 public:
  BaseNode() : BaseNode(this, this) {}
  BaseNode(BaseNode* prev, BaseNode* next) : prev_(prev), next_(next) {}
  void set_prev(BaseNode* node) { prev_ = node; }
  void set_next(BaseNode* node) { next_ = node; }
  BaseNode* get_prev() { return prev_; }
  BaseNode* get_next() { return next_; }
  const BaseNode* get_prev() const { return prev_; }
  const BaseNode* get_next() const { return next_; }

 private:
  BaseNode* prev_ = nullptr;
  BaseNode* next_ = nullptr;
};

template <typename T, typename Allocator>
class List<T, Allocator>::Node : public BaseNode {
 public:
  Node(BaseNode* prev, BaseNode* next, const T& value)
      : BaseNode(prev, next), value_(value) {}
  Node(BaseNode* prev, BaseNode* next) : BaseNode(prev, next) {}
  T& get_value() { return value_; }
  const T& get_value() const { return value_; }
  T* get_value_pointer() { return &value_; }
  const T* get_value_pointer() const { return &value_; }

 private:
  T value_;
};

template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::BaseIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using node_pointer_type =
      std::conditional_t<IsConst, const BaseNode*, BaseNode*>;
  using real_node_pointer_type =
      std::conditional_t<IsConst, const Node*, Node*>;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using iterator_category = std::bidirectional_iterator_tag;

  BaseIterator(const BaseIterator& iter) : node_(iter.node_) {}
  BaseIterator(node_pointer_type node) : node_(node) {}

  operator BaseIterator<true>() const { return {node_}; }

  BaseIterator& operator++();
  BaseIterator operator++(int);
  BaseIterator& operator--();
  BaseIterator operator--(int);
  BaseIterator& operator+=(int count);
  BaseIterator& operator-=(int count);
  BaseIterator operator+(int count);
  BaseIterator operator-(int count);
  reference operator*() const {
    return static_cast<real_node_pointer_type>(node_)->get_value();
  }
  pointer operator->() const {
    return static_cast<real_node_pointer_type>(node_)->get_value_pointer();
  }
  bool operator==(const BaseIterator& iterator) const {
    return node_ == iterator.node_;
  }

 private:
  node_pointer_type node_;
};

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator++() {
  node_ = node_->get_next();
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator++(int) {
  BaseIterator copy = *this;
  ++*this;
  return copy;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator--() {
  node_ = node_->get_prev();
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator--(int) {
  BaseIterator copy = *this;
  --*this;
  return copy;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator+=(int count) {
  if (count >= 0) {
    for (int i = 0; i < count; ++i) {
      ++*this;
    }
  } else {
    count = -count;
    for (int i = 0; i < count; ++i) {
      --*this;
    }
  }
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator-=(int count) {
  *this += (-count);
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator+(int count) {
  BaseIterator new_iterator = *this;
  new_iterator += count;
  return new_iterator;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator-(int count) {
  BaseIterator new_iterator = *this;
  new_iterator -= count;
  return new_iterator;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc)
    : allocator_(alloc), alloc_(alloc) {
  BaseNode* prev = &base_node_;
  size_t created = 0;
  try {
    for (; created < count; ++created) {
      Node* new_node = alloc_traits::allocate(alloc_, 1);
      prev->set_next(new_node);
      alloc_traits::construct(alloc_, new_node, prev, nullptr, value);
      new_node->set_prev(prev);
      prev = new_node;
    }
    prev->set_next(&base_node_);
    base_node_.set_prev(prev);
  } catch (...) {
    BaseNode* current = prev;
    for (size_t i = 0; i <= created; ++i) {
      if (current->get_next() != nullptr) {
        if (i != 0) {
          alloc_traits::destroy(alloc_,
                                static_cast<Node*>(current->get_next()));
        }
        alloc_traits::deallocate(alloc_,
                                 static_cast<Node*>(current->get_next()), 1);
      }
      current->set_next(nullptr);
      current = current->get_prev();
    }
    base_node_.set_next(&base_node_);
    throw;
  }
  size_ = count;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc)
    : allocator_(alloc), alloc_(alloc) {
  BaseNode* prev = &base_node_;
  size_t created = 0;
  try {
    for (; created < count; ++created) {
      Node* new_node = alloc_traits::allocate(alloc_, 1);
      prev->set_next(new_node);
      alloc_traits::construct(alloc_, new_node, prev, nullptr);
      prev = new_node;
    }
    prev->set_next(&base_node_);
    base_node_.set_prev(prev);
  } catch (...) {
    BaseNode* current = prev;
    for (size_t i = 0; i <= created; ++i) {
      if (current->get_next() != nullptr) {
        if (i != 0) {
          alloc_traits::destroy(alloc_,
                                static_cast<Node*>(current->get_next()));
        }
        alloc_traits::deallocate(alloc_,
                                 static_cast<Node*>(current->get_next()), 1);
      }
      current->set_next(nullptr);
      current = current->get_prev();
    }
    base_node_.set_next(&base_node_);
    throw;
  }
  size_ = count;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other)
    : allocator_(std::allocator_traits<Allocator>::
                     select_on_container_copy_construction(other.allocator_)),
      alloc_(
          alloc_traits::select_on_container_copy_construction(other.alloc_)) {
  BaseNode* prev = &base_node_;
  const BaseNode* other_base_node = &other.base_node_;
  const BaseNode* current_other_node = other.base_node_.get_next();
  size_t copied = 0;
  try {
    while (current_other_node != other_base_node) {
      Node* new_node = alloc_traits::allocate(alloc_, 1);
      prev->set_next(new_node);
      alloc_traits::construct(
          alloc_, new_node, prev, nullptr,
          static_cast<const Node*>(current_other_node)->get_value());
      new_node->set_prev(prev);
      prev = new_node;
      current_other_node = current_other_node->get_next();
      ++copied;
    }
    prev->set_next(&base_node_);
    base_node_.set_prev(prev);
  } catch (...) {
    BaseNode* current = prev;
    for (size_t i = 0; i <= copied; ++i) {
      if (current->get_next() != nullptr) {
        if (i != 0) {
          alloc_traits::destroy(alloc_,
                                static_cast<Node*>(current->get_next()));
        }
        alloc_traits::deallocate(alloc_,
                                 static_cast<Node*>(current->get_next()), 1);
      }
      current->set_next(nullptr);
      current = current->get_prev();
    }
    base_node_.set_next(&base_node_);
    throw;
  }
  size_ = other.size_;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
    : allocator_(alloc), alloc_(alloc) {
  BaseNode* prev = &base_node_;
  size_t created = 0;
  try {
    for (auto iter = init.begin(); iter != init.end(); ++iter) {
      Node* new_node = alloc_traits::allocate(alloc_, 1);
      prev->set_next(new_node);
      alloc_traits::construct(alloc_, new_node, prev, nullptr, *iter);
      new_node->set_prev(prev);
      prev = new_node;
      ++created;
    }
    prev->set_next(&base_node_);
    base_node_.set_prev(prev);
  } catch (...) {
    BaseNode* current = prev;
    for (size_t i = 0; i <= created; ++i) {
      if (current->get_next() != nullptr) {
        if (i != 0) {
          alloc_traits::destroy(alloc_,
                                static_cast<Node*>(current->get_next()));
        }
        alloc_traits::deallocate(alloc_,
                                 static_cast<Node*>(current->get_next()), 1);
      }
      current->set_next(nullptr);
      current = current->get_prev();
    }
    base_node_.set_next(&base_node_);
    throw;
  }
  size_ = created;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  BaseNode* current = base_node_.get_next();
  while (current != &base_node_) {
    BaseNode* next = current->get_next();
    alloc_traits::destroy(alloc_, static_cast<Node*>(current));
    alloc_traits::deallocate(alloc_, static_cast<Node*>(current), 1);
    current = next;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node>
      new_alloc = alloc_traits::propagate_on_container_copy_assignment::value
                      ? other.alloc_
                      : alloc_;
  Allocator new_allocator =
      alloc_traits::propagate_on_container_copy_assignment::value
          ? other.allocator_
          : allocator_;
  BaseNode new_base_node;
  BaseNode* prev = new_base_node.get_next();
  const BaseNode* other_base_node = &other.base_node_;
  const BaseNode* current_other_node = other.base_node_.get_next();
  size_t copied = 0;
  try {
    while (current_other_node != other_base_node) {
      Node* new_node = alloc_traits::allocate(new_alloc, 1);
      prev->set_next(new_node);
      alloc_traits::construct(
          new_alloc, new_node, prev, nullptr,
          static_cast<const Node*>(current_other_node)->get_value());
      new_node->set_prev(prev);
      prev = new_node;
      current_other_node = current_other_node->get_next();
      ++copied;
    }
    new_base_node.set_prev(prev);
  } catch (...) {
    BaseNode* current = prev;
    for (size_t i = 0; i <= copied; ++i) {
      if (current->get_next() != nullptr) {
        if (i != 0) {
          alloc_traits::destroy(new_alloc,
                                static_cast<Node*>(current->get_next()));
        }
        alloc_traits::deallocate(new_alloc,
                                 static_cast<Node*>(current->get_next()), 1);
      }
      current->set_next(nullptr);
      current = current->get_prev();
    }
    throw;
  }
  BaseNode* current = base_node_.get_next();
  while (current != &base_node_) {
    BaseNode* next = current->get_next();
    alloc_traits::destroy(alloc_, static_cast<Node*>(current));
    alloc_traits::deallocate(alloc_, static_cast<Node*>(current), 1);
    current = next;
  }
  size_ = other.size_;
  base_node_ = new_base_node;
  new_base_node.get_next()->set_prev(&base_node_);
  prev->set_next(&base_node_);
  alloc_ = new_alloc;
  allocator_ = new_allocator;
  return *this;
}

template <typename T, typename Allocator>
typename List<T, Allocator>::Node* List<T, Allocator>::create_node(
    BaseNode* prev_node, BaseNode* next_node, const T& value) {
  Node* new_node = alloc_traits::allocate(alloc_, 1);
  alloc_traits::construct(alloc_, new_node, prev_node, next_node, value);
  return new_node;
}

template <typename T, typename Allocator>
void List<T, Allocator>::delete_node(Node* node) {
  alloc_traits::destroy(alloc_, node);
  alloc_traits::deallocate(alloc_, node, 1);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  Node* new_node = create_node(base_node_.get_prev(), &base_node_, value);
  base_node_.get_prev()->set_next(new_node);
  base_node_.set_prev(new_node);
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  Node* new_node = create_node(&base_node_, base_node_.get_next(), value);
  base_node_.get_next()->set_prev(new_node);
  base_node_.set_next(new_node);
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  Node* last = static_cast<Node*>(base_node_.get_prev());
  last->get_prev()->set_next(&base_node_);
  base_node_.set_prev(last->get_prev());
  delete_node(last);
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  Node* first = static_cast<Node*>(base_node_.get_next());
  first->get_next()->set_prev(&base_node_);
  base_node_.set_next(first->get_next());
  delete_node(first);
  --size_;
}
