#include "LocalHelper.h"

String GET_TWO_DIGIT_STRING(int i) {
    String output = "";
    if (i < 10)
        output += "0";
    output += String(i);
    return output;
};

String GET_TWO_DIGIT_STRING(char *cstr) {
    return GET_TWO_DIGIT_STRING(atoi(cstr));
};

String GET_TWO_DIGIT_STRING(String str) {
    return GET_TWO_DIGIT_STRING(str.toInt());
};