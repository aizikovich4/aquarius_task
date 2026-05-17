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
static KeyPosition key_positions[ALHABET_COUNT];

static const int8_t KEY_OFFSET = 13; // offset for every key from previous
static const double ROW_OFFSET = 5; // offset for every row from previous (for example asdf... is offset from qwerty..., and so on)

static uint8_t get_index(char const key)
{
    return key - 'a';
}

// for every key we make the position in keyboard
// It will use for calculate the distance between key
static void init_key_positions(void) {    
    for (uint8_t i = 0; i < QWERTY_ROW; ++i) {
        char const* row_str = QWERTY_KEYBOARD[i];
        for (uint8_t j = 0; row_str[j] != '\0'; ++j) {
            char ch = row_str[j];
            key_positions[get_index(ch)].row = i;
            key_positions[get_index(ch)].x_column = j * KEY_OFFSET + i * ROW_OFFSET;
        }
    }

#ifdef DEBUG
    printf("KEYBOARD:\n");
    for (uint8_t i = 0; i < QWERTY_ROW; ++i) {
        char const* row_str = QWERTY_KEYBOARD[i];
        for (uint8_t j = 0; row_str[j] != '\0'; ++j) {
            char ch = row_str[j];
            printf("%c: %u, %d \t", ch, key_positions[get_index(ch)].row, key_positions[get_index(ch)].x_column);
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

    if (!isalpha(current_key) || !isalpha(new_key)) {
        return false;
    }

    current_key = tolower(current_key);
    new_key = tolower(new_key);

    if (current_key == new_key) {
        return true; // allow double keys
    }

    KeyPosition const* curr_key_pos = &key_positions[get_index(current_key)];
    KeyPosition const* new_key_pos = &key_positions[get_index(new_key)];

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
static uint8_t makeNormalizeWord(char* out, uint8_t len, char const* str)
{
    if(len == 0) {
        return 0;
    }

    // Trim leading space
    while(isspace((unsigned char)*str)) {
        ++str;
    }

    if(*str == '\0') {
        return 0;
    }

    // copy trailing symbols, but break on first space
    uint8_t len_res=0;
    for (uint8_t i=0; i < strlen(str); ++i, ++len_res) {
        if (!isspace((unsigned char)str[i])) {
            out[i] = str[i];
        } else
        {
            out[i] = '\0';
            break;
        }
    }
    return len_res;
}

static bool isGoodWord(char* str)
{
    if (!str) {
        return false;
    }

    // normalize string
    char normalizeWord[MAX_SIZE_WORD];
    memset(normalizeWord, '\0', MAX_SIZE_WORD);
    uint8_t len = makeNormalizeWord(normalizeWord, MAX_SIZE_WORD, str);
    if (len == 0) {
        return false;
    }

    len = strlen(normalizeWord);
    if (len == 0) {
        return false;
    }
    if (len == 1) {
        return true;
    }

    bool isValid = true;
    for (uint8_t i = 0; i < len - 1; ++i) {
        if (!isKeyNeighborhood(normalizeWord[i], normalizeWord[i+1])) {
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
    assert(!isGoodWord(""));
    assert(isGoodWord(" q"));
    assert(isGoodWord(" q "));
    assert(isGoodWord("q "));
    assert(!isGoodWord("cfb"));
    assert(!isGoodWord(" cfb"));
    assert(!isGoodWord(" cfb "));
    assert(!isGoodWord("cfb "));
    assert(!isGoodWord("tgn"));
    assert(isGoodWord("qwertyuiop"));
    assert(isGoodWord("   asdfg"));
    assert(isGoodWord("qwerty    "));
    assert(isGoodWord("dddfffggg"));
    assert(isGoodWord("   hhh   "));
    assert(isGoodWord("fff"));
    assert(isGoodWord(" yyyy "));
    assert(isGoodWord("qqwweerrttyyuu"));
    assert(isGoodWord("  qqwweerrttyyuu"));
    assert(isGoodWord("qqwweerrttyyuu  "));
    assert(isGoodWord("   qqwweerrttyyuu   "));
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
    assert(!isGoodWord("FCS"));
    assert(isGoodWord("FC"));
    assert(isGoodWord("QWERTY"));
    assert(isGoodWord("  QWERTY"));
    assert(isGoodWord("QWERTY  "));
    assert(isGoodWord("  QWERTY  "));

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
        if (isGoodWord(buf)) {
#ifdef DEBUG
            printf("VALID word:%s", buf);
#endif
            ++(*res);
        } else {
#ifdef DEBUG
            printf("INVALID word:%s", buf);
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
