#ifndef RECT_HPP
    #define RECT_HPP

    #include <ostream>

    struct Rect {
        int x;
        int y;
        int w;
        int h;

        Rect(int x = 0.0f, int y = 0.0f, int w = 0.0f, int h = 0.0f) {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }

        void shrink(int amount) {
            this->x += amount;
            this->y += amount;
            this->w -= amount * 2;
            this->h -= amount * 2;
        }

        Rect clipTo(Rect parent) {
            Rect new_clip = *this;

            if (new_clip.x < parent.x) {
                int diff = parent.x - new_clip.x;
                new_clip.x += diff;
                if (diff < new_clip.w) {
                    new_clip.w = new_clip.w - diff;
                } else {
                    new_clip.w = 0;
                }
            } else if (new_clip.x > parent.x) {
                if (new_clip.x + new_clip.w > parent.x + parent.w) {
                    if (parent.x + parent.w >= new_clip.x) {
                        new_clip.w = parent.x + parent.w - new_clip.x;
                    } else {
                        new_clip.w = 0;
                    }
                } // ELSE NO OP
            } else {
                new_clip.w = std::min(parent.w, new_clip.w);
            }

            if (new_clip.y < parent.y) {
                int diff = parent.y - new_clip.y;
                new_clip.y += diff;
                if (diff < new_clip.h) {
                    new_clip.h = new_clip.h - diff;
                } else {
                    new_clip.h = 0;
                }
            } else if (new_clip.y > parent.y) {
                if (new_clip.y + new_clip.h > parent.y + parent.h) {
                    if (parent.y + parent.h >= new_clip.y) {
                        new_clip.h = parent.y + parent.h - new_clip.y;
                    } else {
                        new_clip.h = 0;
                    }
                } // ELSE NO OP
            } else {
                new_clip.h = std::min(parent.h, new_clip.h);
            }

            return new_clip;
        }

        friend bool operator==(const Rect &lhs, const Rect &rhs) {
            if (lhs.x == rhs.x &&
                lhs.y == rhs.y &&
                lhs.w == rhs.w &&
                lhs.h == rhs.h
            ) {
                return true;
            }

            return false;
        }

        friend std::ostream& operator<<(std::ostream &os, const Rect &rect) {
            return os << "Rect { x: " << rect.x << ", y: " << rect.y << ", w: " << rect.w << ", h: " << rect.h << " }";
        }
    };
#endif
