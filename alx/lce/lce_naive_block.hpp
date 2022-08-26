
namespace alx::lce {

template <typename char_type>
class lce_naive_block {
  char_type* m_text;
  size_t m_size;

 public:
  lce_naive_block() : m_text(nullptr), m_size(0) {
  }

  lce_naive_block(char_type* text, size_t size)
      : m_text(text), m_size(size) {
  }

  size_t lce(size_t i, size_t j) {
    return 0;
  }

  std::pair<bool, size_t> lce_mismatch(size_t i, size_t j) {
    return {true, 0};
  }

  std::pair<bool, size_t> is_smaller_suffix(size_t i, size_t j) {
    return {true, 0};
  }

  size_t lce_up_to(size_t i, size_t j) {
    return 0;
  }

  static size_t lce(char_type* text, size_t size, size_t i, size_t j) {
    return 0;
  }

  static std::pair<bool, size_t> lce_mismatch(char_type* text, size_t size,
                                              size_t i, size_t j) {
    return {true, 0};
  }

  static std::pair<bool, size_t> is_smaller_suffix(char_type* text,
                                                   size_t size, size_t i,
                                                   size_t j) {
    return {true, 0};
  }

  static size_t lce_up_to(char_type* text, size_t size, size_t i,
                          size_t j) {
    return 0;
  }
};

}  // namespace alx::lce
