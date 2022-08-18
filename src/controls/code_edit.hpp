#ifndef CODE_EDIT_HPP
    #define CODE_EDIT_HPP

    #include "../application.hpp"
    #include "../renderer/renderer.hpp"
    #include "text_edit.hpp"

    struct Position {
        u32 line = 0;
        u32 column = 0;
        u32 index = 0;

        Position() {}

        void next() {
            advance(1);
        }

        void advance(u32 count) {
            column += count;
            index += count;
        }

        void newline() {
            line++;
            column = 0;
            index++;
        }
    };

    struct Token {
        enum class Type : u32 {
            Null = '\0',
            Newline = '\n',
            CarriageReturn = '\r',
            Tab = '\t',
            Space = ' ',

            OpenParenthesis = '(',
            CloseParenthesis = ')',
            OpenBrace = '{',
            CloseBrace = '}',
            OpenBracket = '[',
            CloseBracket = ']',
            LessThan = '<',
            GreaterThan = '>',

            Pound = '#',
            Colon = ':',
            SemiColon = ';',
            Plus = '+',
            Minus = '-',
            Asterisk = '*',
            Ampersand = '&',
            Comma = ',',
            Dot = '.',
            Equals = '=',
            ExclamationMark = '!',
            QuestionMark = '?',
            Pipe = '|',
            Caret = '^',
            Underscore = '_',
            At = '@',
            Tilde = '~',
            GraveAccent = '`',
            Modulo = '%',
            Dollar = '$',

            ForwardSlash = '/',
            BackwardSlash = '\\',

            DoubleQuotes = '"',
            SingleQuote = '\'',

            StringBegin = 1000,
            StringEnd,
            CharacterBegin,
            CharacterEnd,
            Escaped,
            NumberBegin,
            NumberEnd,
            PreProcessorStatementBegin,
            PreProcessorStatementEnd,
            SingleLineCommentBegin,
            SingleLineCommentEnd,
            MultiLineCommentBegin,
            MultiLineCommentEnd,

            Identifier = 2000,
            Auto,
            Break,
            Case,
            Char,
            Const,
            Continue,
            Default,
            Do,
            Double,
            Else,
            Enum,
            Extern,
            Float,
            For,
            Goto,
            If,
            Inline,
            Int,
            Long,
            Register,
            Restrict,
            Return,
            Short,
            Signed,
            Sizeof,
            Static,
            Struct,
            Switch,
            Typedef,
            Union,
            Unsigned,
            Void,
            Volatile,
            While,
            Alignas,
            Alignof,
            Atomic,
            Bool,
            Complex,
            Decimal128,
            Decimal32,
            Decimal64,
            Generic,
            Imaginary,
            Noreturn,
            StaticAssert,
            ThreadLocal,
        };

        Type type;
        u32 index;

        Token() {}
        Token(Type type, u32 index) : type{type}, index{index} {}
    };

    struct Lexer {
        enum class State {
            Default,
            Identifier,
            SingleLineComment,
            MultiLineComment,
            StringLiteral,
            CharacterLiteral,
            Number,
            PreProcessor,
        };

        HashMap<String, Token::Type> keywords = {
            {"auto", Token::Type::Auto},
            {"break", Token::Type::Break},
            {"case", Token::Type::Case},
            {"char", Token::Type::Char},
            {"const", Token::Type::Const},
            {"continue", Token::Type::Continue},
            {"default", Token::Type::Default},
            {"do", Token::Type::Do},
            {"double", Token::Type::Double},
            {"else", Token::Type::Else},
            {"enum", Token::Type::Enum},
            {"extern", Token::Type::Extern},
            {"float", Token::Type::Float},
            {"for", Token::Type::For},
            {"goto", Token::Type::Goto},
            {"if", Token::Type::If},
            {"inline", Token::Type::Inline},
            {"int", Token::Type::Int},
            {"long", Token::Type::Long},
            {"register", Token::Type::Register},
            {"restrict", Token::Type::Restrict},
            {"return", Token::Type::Return},
            {"short", Token::Type::Short},
            {"signed", Token::Type::Signed},
            {"sizeof", Token::Type::Sizeof},
            {"static", Token::Type::Static},
            {"struct", Token::Type::Struct},
            {"switch", Token::Type::Switch},
            {"typedef", Token::Type::Typedef},
            {"union", Token::Type::Union},
            {"unsigned", Token::Type::Unsigned},
            {"void", Token::Type::Void},
            {"volatile", Token::Type::Volatile},
            {"while", Token::Type::While},
            {"_Alignas", Token::Type::Alignas},
            {"_Alignof", Token::Type::Alignof},
            {"_Atomic", Token::Type::Atomic},
            {"_Bool", Token::Type::Bool},
            {"_Complex", Token::Type::Complex},
            {"_Decimal128", Token::Type::Decimal128},
            {"_Decimal32", Token::Type::Decimal32},
            {"_Decimal64", Token::Type::Decimal64},
            {"_Generic", Token::Type::Generic},
            {"_Imaginary", Token::Type::Imaginary},
            {"_Noreturn", Token::Type::Noreturn},
            {"_Static_assert", Token::Type::StaticAssert},
            {"_Thread_local", Token::Type::ThreadLocal},
        };

        State state;
        u32 state_begin;
        Position pos;
        Slice<const char> source;
        Slice<Token> tokens;

        Lexer();
        void lex(Slice<const char> source);
        bool peek(Token::Type type);
    };

    struct CodeEdit : public TextEdit {
        Lexer lexer;

        CodeEdit(String text = "", Size min_size = Size(100, 100));
        ~CodeEdit();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
        void __fillSingleLineColoredText(std::shared_ptr<Font> font, Slice<const char> text, Point point, u64 byte_offset, Color color, Renderer::Selection selection, Color selection_color);
    };
#endif
