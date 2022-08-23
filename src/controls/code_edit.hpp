#ifndef CODE_EDIT_HPP
    #define CODE_EDIT_HPP

    #include "../application.hpp"
    #include "../renderer/renderer.hpp"
    #include "text_edit.hpp"

    struct Offset {
        u32 index = 0;

        Offset() {}

        void next() {
            advance(1);
        }

        void advance(u32 count) {
            index += count;
        }
    };

    struct Token {
        enum class Type : u32 {
            NUL = 0, SOH, STX, ETX, EOT, ENQ, ACK, BEL, BS, TAB, LF, VT, FF, CR, SO, SI, DLE, DC1, DC2, DC3, DC4, NAK, SYN, ETB, CAN, EM, SUB, ESC, FS, GS, RS, US,
            Space, ExclamationMark, DoubleQuotes, Pound, DollarSign, Modulo, Ampersand, SingleQuote, OpenParenthesis, CloseParenthesis, Asterisk, Plus, Comma, Minus,
            Dot, ForwardSlash, Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Colon, SemiColon, LessThan, Equals, GreaterThan, QuestionMark,
            At, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, OpenBracket, BackwardSlash, CloseBracket, Caret, Underscore,
            Backtick, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, OpenBrace, Pipe, CloseBrace, Tilde, DEL,

            String = 1000,
            Character,
            Escaped,
            Number,
            PreProcessorStatement,
            SingleLineComment,
            MultiLineComment,
            Function,

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
        HashMap<Slice<const char>, Token::Type> keywords = {
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

        Offset pos;
        Slice<const char> source;
        Slice<Token> tokens;

        Lexer();
        void lex(Slice<const char> source);
        bool peek(Token::Type type);
    };

    struct MinimapButton : public Widget {
        MinimapButton(Size min_size = Size(10, 10));
        ~MinimapButton();
        const char* name() override;
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;
        MinimapButton* setMinSize(Size size);
        Size minSize();
        i32 isFocusable() override;
    };

    struct Minimap : public Box {
        f64 m_min = 0.0;
        f64 m_max = 1.0;
        f64 m_value = 0.0;
        f64 m_step = 0.1;
        i32 m_origin = 0;

        i32 m_slider_button_size = 0;
        MinimapButton *m_slider_button = nullptr;
        EventListener<> onValueChanged = EventListener<>();

        std::shared_ptr<Texture> m_minimap_texture = nullptr;
        u32 m_minimap_width = 120;

        Minimap();
        ~Minimap();
        const char* name() override;
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;
        bool handleScrollEvent(ScrollEvent event) override;
    };

    struct CodeEdit : public TextEdit {
        Lexer m_lexer;
        bool m_overscroll = true;
        Minimap *m_minimap = nullptr;

        CodeEdit(String text = "", Size min_size = Size(100, 100));
        ~CodeEdit();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Point __automaticallyAddOrRemoveScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size);
        void __drawScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size, i32 extra);
        void __fillSingleLineColoredText(
            std::shared_ptr<Font> font,
            Slice<const char> text,
            Point point,
            u64 byte_offset,
            Color color,
            Renderer::Selection selection,
            Color selection_color
        );
        void __renderMinimap(Size size);
        bool handleScrollEvent(ScrollEvent event) override;
    };
#endif
