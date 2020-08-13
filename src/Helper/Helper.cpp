#include "Helper/Helper.h"

namespace Helper
{
    char *GET_TWO_DIGIT_STRING(int i)
    {
        static char out[4];
        // get c str of value
        char val[4];
        itoa(i, val, 10);

        if (i < 10)
        {
            strcpy(out, "0");
            strcat(out, val);
        }
        else
        {
            strcpy(out, val);
        }
        return out;
    };

    char *GET_TWO_DIGIT_STRING(const char *cstr)
    {
        return GET_TWO_DIGIT_STRING(atoi(cstr));
    };

    void CHAR_TO_CSTR(char *out, const char c)
    {
        if (sizeof(out) < 2)
            return;

        out[0] = c;
        out[1] = '\0';
    };

    int IDXOF(const char *str, char c, int start)
    {
        for (int i = start; i < strlen(str); ++i)
        {
            if (str[i] == c)
                return i;
        }
        return -1;
    };

    int IDXOF(const char *str, char c)
    {
        return IDXOF(str, c, 0);
    };

    int SUBSTR(char *out, const char *in, int from, int to)
    {
        int bytesCopied = 0;
        // check that from and to values are in range
        if (from < 0 || from >= to || to > strlen(in))
            return bytesCopied; // 0

        // copy substring
        int outPtr = 0;
        int inPtr = from;
        while (inPtr < to)
        {
            out[outPtr] = in[inPtr];
            ++outPtr;
            ++inPtr;
            ++bytesCopied;
        }
        out[outPtr] = '\0'; // terminate string;
        return bytesCopied;
    };

    int SUBSTR(char *out, const char *in, int from)
    {
        return SUBSTR(out, in, from, strlen(in));
    };

    char *GEN_UUID()
    {
        // creates a 5 digit alphanumeric c-str
        static char uuid[6];
        randomSeed(analogRead(millis()));

        for (int i = 0; i < 5; ++i)
        {
            byte randVal = random(0, 35);
            char letter = randVal + 'a';
            if (randVal > 25)
                letter = (randVal - 26) + '0';
            uuid[i] = letter;
        }
        uuid[6] = '\0';
        return uuid;
    };

}; // namespace Helper