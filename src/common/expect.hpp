#ifndef AGRO_EXPECT_HPP
    #define AGRO_EXPECT_HPP

    #include <stdio.h>

    #define expect(expr) __expect(expr, #expr, __FILE__, __LINE__)

    static const char *RED = "\033[91m";
    static const char *GREEN = "\033[92m";
    static const char *YELLOW = "\033[93m";
    static const char *BOLD = "\033[1m";
    static const char *END = "\033[0m";

    bool __expect(bool expr, const char *expr_string, const char *file, int line) {
        if (expr) {
            printf(
                "{ %s%sPASS%s } Expr: %s, File: %s, Line: %d\n",
                GREEN, BOLD, END, expr_string, file, line
            );
            return true;
        } else {
            printf(
                "{ %s%sFAIL%s } Expr: %s%s%s%s, File: %s%s%s%s, Line: %s%s%d%s\n",
                RED, BOLD, END, YELLOW, BOLD, expr_string, END, YELLOW, BOLD, file, END, YELLOW, BOLD, line, END
            );
            return false;
        }
    }
#endif

