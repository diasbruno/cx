#define nil NULL

int is_space(int ch) {
  return ch == ' ' || ch == '\t';
}

int is_identifier(int ch) {
  return ch >= 'a' && ch <= 'z';
}

int is_number(int ch) {
  return ch >= '0' && ch <= '9';
}

int is_valid_identifier(int ch) {
  return is_identifier(ch) || is_number(ch) ||
    ch == '_';
}

int is_octal_num(int ch) {
  return ch >= '0' && ch <= '7';
}

int is_binary_num(int ch) {
  return ch >= '0' && ch <= '1';
}

int is_hex_num(int ch) {
  return
    (ch >= '0' && ch <= '1') ||
    (ch >= 'a' && ch <= 'f');
}
