#include "process.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

const uint8_t MAX_SIZE_WORD = 255;
static const char* QWERTY_KEYBOARD[] = {
    "qwertyuiop", 
    "asdfghjkl", 
    "zxcvbnm"};

const uint8_t QWERTY_ROW = sizeof(QWERTY_KEYBOARD)/sizeof(QWERTY_KEYBOARD[0]);
static_assert(sizeof(QWERTY_KEYBOARD)/sizeof(QWERTY_KEYBOARD[0]) == 3, "Unexpected QWERTY config");  // protect from accidently change QWERTY keyboard rows

typedef struct {
    int8_t row;
    int8_t column;
} KeyPosition;

// compiler do not allow to use const as array size
#define ALHABET_COUNT 26 
// its global object for all, its filling on first countUniqueQwertyWords() call, 
// no need to modify or update it. I dont like it, but place it here
static KeyPosition key_positions[26]; 



// for every key we make the position in keyboard
// It will use for calculate the length of path for neighborhood key
static void init_key_positions(void) {    
    for (uint8_t i = 0; i < QWERTY_ROW; ++i) {
        char const* row_str = QWERTY_KEYBOARD[i];
        for (uint8_t j = 0; row_str[j] != '\0'; ++j) {
            char ch = row_str[j];
                key_positions[ch - 'a'].row = i;
                key_positions[ch - 'a'].column = j;
        }
    }
}

// this function check the key in keyboard array, 
// and return true if new key is near with current
// All other case - return false
static bool isKeyNeighborhood(char current_key, char new_key)
{
    static bool positions_initialized = false;
    if (!positions_initialized) {
        init_key_positions();
        positions_initialized = true;
    }

    char cur = tolower(current_key);
    char new = tolower(new_key);

    if (cur == new) {
        return true; // allow double keys
    }

    KeyPosition curr_key_pos = key_positions[cur-'a'];
    KeyPosition new_key_pos = key_positions[new-'a'];

    // calculate length of path between keys
    int8_t dist_row = abs(curr_key_pos.row - new_key_pos.row);
    int8_t dist_column = abs(curr_key_pos.column - new_key_pos.column);

    if ((curr_key_pos.row  == new_key_pos.row) && (dist_column == 1)) {
        // horizontal key-friends
        return true;
    } else if ( (dist_row == 0 && (dist_column == 1)) || (dist_row == 1 && (dist_column == 0))) {
        // vertical key-friends (up and down on keyboard)
        return true;
    } else if ((dist_row == 1) && (dist_column == 1) )
    {
        // its diagonalis friends
        return true;
    }

    return false;
}

static char* trim(char* str) {
    // leading space
    while (isspace(*str)) {
        ++str;
    }

    if (!str) return NULL;

    char* end = str + strlen(str);
    while (end > str && isspace(*(end - 1))) {
        --end;
    }

    *end = '\0';
    return str;
}

bool isGoodWord(char const* word)
{
    if (!word) {
        return false;
    }
    uint8_t len = strlen(word);
    bool isValid = true;
    for (uint8_t i = 0; i < len - 1; ++i) {
        if (!isKeyNeighborhood(word[i], word[i+1])) {
            isValid = false;
            break;
        }
    }

    return isValid;
}
// This function does a lot of things, opening/closing file, reading context, trimming the lines - i dont like it,
// but I want to focus on solving the problem, so it'll all be here.
HandleResult countQwertyNeighborhoodWords(char const* path, uint64_t* resCount)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        return ERROR_OPEN_FILE;
    }

    char buf[MAX_SIZE_WORD];
    while (fgets(buf, sizeof(buf), file)) {
        // normalize string
        char* normalizeWord = NULL;
        normalizeWord = trim(buf);
        if (!normalizeWord) {
            continue;
        }
        uint8_t len = strlen(normalizeWord);
        if (len == 0) {
            continue;
        }

        // start for real work
        if (isGoodWord(buf)) {
            printf("VALID word:%s\n", buf);
            ++(*resCount);
        } else {
            printf("INVALID word:%s\n", buf);
        }
    }
    
    // we dont read file until the end, the result is bad
    if (ferror(file)) {
        fclose(file);
        return ERROR_READ_FILE;
    }

    fclose(file);
    return SUCCESS;
}

void asserts()
{
    assert(isGoodWord("sw"));
    assert(isGoodWord("sz"));
    assert(isGoodWord("qwertyuiop"));
    assert(isGoodWord("poiuytrewq"));
    assert(isGoodWord("asdfghjkl"));
    assert(isGoodWord("zxcvbnm"));
    assert(isGoodWord("qws"));
    assert(isGoodWord("qwsxcdertgbnmkoiuygfcdewqazxs"));
    assert(!isGoodWord("zxf"));
    assert(!isGoodWord("sq"));
    assert(!isGoodWord("qs"));
    assert(!isGoodWord("zxq"));
    assert(!isGoodWord("wd"));
    assert(!isGoodWord("ht"));
    return;

}
