#ifndef _MARKET_SIM_H_
#define _MARKET_SIM_H_

//#define WITHOUT_CURSES

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <curl/curl.h>

#ifndef WITHOUT_CURSES
#include <curses.h>
#endif

#define ARRAYLEN(x) (sizeof(x) / sizeof(x[0]))

#define PROGRAM_NAME "market-sim"

/* VERSION_INFO is supplied by the compiler */
#define PROGRAM_VERSION "v0.3 (" VERSION_INFO ")"

#define ARG_LOADED (1<<0)
#define ARG_FAILURE (1<<1)
#define ARG_VERBOSE (1<<2)
#define ARG_NOCURSES (1<<3)
#define ARG_BATCHMODE (1<<4)
#define ARG_RESTRICTED (1<<5)
#define ARG_HTML (1<<6)

/* don't change this, it will corrupt existing saves */
#define EPOCH_YEAR 2000

#define SAVE_MAGIC "PORTv3"
#define MAGIC_LEN 6

#define BOLD_THRESHOLD 25
#define ABS(x) (((x)<0)?-(x):(x))

typedef unsigned long long ullong;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

/* money is represented internally as cents */
struct money_t {
    ullong cents;
};

enum history_action { BUY = 0, SELL };

struct history_time {
    ushort year; /* since EPOCH_YEAR */
    uchar month; /* 0 = jan, 11 = dec */
    uchar day;
    uchar hour; /* 0-23 */
    uchar minute;
    uchar second;
};

struct history_item {
    enum history_action action;
    ullong count;
    struct money_t price;

    struct history_time action_time;

    struct history_item *next;
};

struct stock_t {
    char *symbol;
    char *fullname;
    ullong count;
    struct money_t current_price;

    struct history_item *history;

    uint history_len;
};

struct player_t {
    struct money_t cash;
    uint portfolio_len;
    struct stock_t *portfolio;

    bool need_to_free_portfolio;
    char *filename; /* filename last loaded or written to, used for quicksave */
};

struct command_t {
    const char *name;
    const char *command;
    void (*handler)(struct player_t*);
};

/*** prototypes ***/

extern bool have_color;
extern bool html_out;

/* restricted mode disables things that would be considered "dangerous" when
   used in a web-facing script such as interactive loading/saving */
extern bool restricted;
extern bool batch_mode;

#define COL_NORM 0
#define COL_RED 1
#define COL_GREEN 2

bool get_stock_info(char *sym, struct money_t*, char **name);
char *(*read_string)(void);
char *csv_read(char**);
char *read_ticker(void);
extern int (*output)(const char*, ...);
extern void (*heading)(const char *text, ...);
extern void (*horiz_line)(void);
int compare_stocks(const void*, const void*);
struct stock_t *find_stock(struct player_t*, char*);
uint parse_args(int argc, char *argv[], char**);
uint16_t to_be16(uint16_t);
uint16_t to_sys16(uint16_t);
uint32_t to_be32(uint32_t);
uint32_t to_sys32(uint32_t);
uint64_t to_be64(uint64_t);
uint64_t to_sys64(uint64_t);
ullong read_int(void);
void add_hist(struct stock_t*, enum history_action, ullong count);
void all_lower(char*);
void all_upper(char*);
void batch_init(void);
void cleanup(void);
void curses_init(void);
void do_menu(struct player_t*, const struct command_t*, uint, const char*);
void fail(const char*, ...);;
void load_portfolio(struct player_t*, const char*);
void save_portfolio(struct player_t*, const char*);
void print_history(struct stock_t*);
void print_usage(int argc, char *argv[]);
void print_version(void);
void sig_handler(int);
void use_bold(void);
void stop_bold(void);

void use_color(int);
void stop_color(int);

void buy_handler(struct player_t*);
void info_handler(struct player_t*);
void load_handler(struct player_t*);
void print_handler(struct player_t*);
void quit_handler(struct player_t*);
void save_handler(struct player_t*);
void sell_handler(struct player_t*);
void update_handler(struct player_t*);

#endif
