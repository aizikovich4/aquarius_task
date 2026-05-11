#include "process.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint8_t MAX_SIZE_WORD = 255;
static const char* QWERTY_KEYBOARD[] = {
    "qwertyuiop", 
    "asdfghjkl", 
    "zxcvbnm"};

const uint8_t QWERTY_ROW = sizeof(QWERTY_KEYBOARD)/sizeof(QWERTY_KEYBOARD[0]);
static_assert(sizeof(QWERTY_KEYBOARD)/sizeof(QWERTY_KEYBOARD[0]) == 3, "Unexpected QWERTY config");  // protect from accidently change QWERTY keyboard rows

typedef struct {
    uint8_t row;
    int16_t x_column;
} KeyPosition;

// compiler do not allow to use const as array size
#define ALHABET_COUNT 26 
// Its global object for this file, its filling on first countUniqueQwertyWords() call,
// no need to modify or update it. I dont like it, but make it global
// index - monotonnic increase key on keyboard ('q' is first, 'm' is last)
static KeyPosition key_positions[26];

static const int8_t KEY_OFFSET = 13; // offset for every key from previous
static const double ROW_OFFSET = 5; // offset for every row from previous (for example asdf... is offset from qwerty..., and so on)

// for every key we make the position in keyboard
// It will use for calculate the distance between key
static void init_key_positions(void) {    
    for (uint8_t i = 0; i < QWERTY_ROW; ++i) {
        char const* row_str = QWERTY_KEYBOARD[i];
        for (uint8_t j = 0; row_str[j] != '\0'; ++j) {
            char ch = row_str[j];
            key_positions[ch - 'a'].row = i;
            key_positions[ch - 'a'].x_column = j * KEY_OFFSET + i * ROW_OFFSET;
        }
    }

#ifdef DEBUG
    printf("KEYBOARD:\n");
    for (uint8_t i = 0; i < QWERTY_ROW; ++i) {
        char const* row_str = QWERTY_KEYBOARD[i];
        for (uint8_t j = 0; row_str[j] != '\0'; ++j) {
            char ch = row_str[j];
            printf("%c: %u, %d \t", ch, key_positions[ch-'a'].row, key_positions[ch-'a'].x_column);
        }
        printf("\n");
    }
#endif
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

    KeyPosition const* curr_key_pos = &key_positions[cur-'a'];
    KeyPosition const* new_key_pos = &key_positions[new-'a'];

    // calculate length of path between keys
    int8_t dist_row = abs(curr_key_pos->row - new_key_pos->row);
    //int8_t dist_column = abs(curr_key_pos.x_column - new_key_pos.x_column);
    double dist_x = abs(curr_key_pos->x_column - new_key_pos->x_column);

    if ((curr_key_pos->row  == new_key_pos->row) && (dist_x == KEY_OFFSET)) {
        // horizontal key-friends
        return true;
    } else if ( (dist_row == 1 && (dist_x < KEY_OFFSET+ROW_OFFSET))) {
        // vertical and diagonalis key-friends (up and down on keyboard)
        return true;
    }

    return false;
}

// this function find the substring which satisfise to ourcondition for word without spaces in begin and end
static char* makeWord(char* str) {
    if (!str) {
        return NULL;
    }
    // trim leading space
    while (str) {
        if (isspace(*str)) {
            ++str;
        } else {
            break;
        }
    }

    if (!str) {
        return NULL;
    }

    // trim trailing space
    char* end = str + strlen(str);
    while (end > str && isspace(*(end - 1))) {
        --end;
    }

    *end = '\0';
    return str;
}

static bool isGoodWord(char const* word)
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

#ifdef DEBUG
// this asserts is enough for testing main functions, no need to use special frameworks as gtest
void asserts()
{
    assert(isGoodWord("qwertyuiop"));
    assert(isGoodWord("poiuytrewq"));
    assert(isGoodWord("asdfghjkl"));
    assert(isGoodWord("zxcvbnm"));
    assert(isGoodWord("qws"));
    assert(isGoodWord("sw"));
    assert(isGoodWord("sz"));
    assert(isGoodWord("qwsxcdertgbnmkoiuygfcdewqazxs"));
    assert(!isGoodWord("zxf"));
    assert(!isGoodWord("sq"));
    assert(!isGoodWord("qs"));
    assert(!isGoodWord("zxq"));
    assert(!isGoodWord("wd"));
    assert(!isGoodWord("ht"));
    assert(!isGoodWord("bht"));
    assert(isGoodWord("fc"));
    assert(!isGoodWord("fcs"));
    return;
}
#endif

// This function does a lot of things, opening/closing file, reading context, trimming the lines - i dont like it,
// but I want to focus on solving the problem, so it'll all be here.
HandleResult countQwertyNeighborhoodWords(char const* path, uint64_t* res)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        return ERROR_OPEN_FILE;
    }

    char buf[MAX_SIZE_WORD];
    while (fgets(buf, sizeof(buf), file)) {
        // normalize string
        char* word = NULL;
        word = makeWord(buf);
        if (!word) {
            continue;
        }
        uint8_t len = strlen(word);
        if (len == 0) {
            continue;
        }

        // start for real work
        if (isGoodWord(buf)) {
#ifdef DEBUG
            printf("VALID word:%s\n", buf);
#endif
            ++(*res);
        } else {
#ifdef DEBUG
            printf("INVALID word:%s\n", buf);
#endif
        }
    }

    // we dont readed the file until the end, the result is bad
    if (ferror(file)) {
        fclose(file);
        return ERROR_READ_FILE;
    }

    fclose(file);
    return SUCCESS;
}
