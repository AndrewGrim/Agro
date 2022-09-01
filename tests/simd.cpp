#include <cassert>

#include "../src/application.hpp"
#include "../src/core/intrinsics.hpp"

int main(int argc, char **argv) {
    #ifdef AVX2
        assert(
            (simd::Vector<u8, SIMD_WIDTH>('1').notEqual(simd::Vector<u8, SIMD_WIDTH>('1')).mask()) ==
            0b0000'0000'0000'0000'0000'0000'0000'0000
        );
        assert(
            (simd::Vector<u8, SIMD_WIDTH>('1').notEqual(simd::Vector<u8, SIMD_WIDTH>('2')).mask()) ==
            0b1111'1111'1111'1111'1111'1111'1111'1111
        );
        assert(
            (simd::Vector<u8, SIMD_WIDTH>('1') != simd::Vector<u8, SIMD_WIDTH>('1')) ==
            0b0000'0000'0000'0000'0000'0000'0000'0000
        );
        assert(
            (simd::Vector<u8, SIMD_WIDTH>('1') != simd::Vector<u8, SIMD_WIDTH>('2')) ==
            0b1111'1111'1111'1111'1111'1111'1111'1111
        );
        assert(
            (simd::Vector<u8, SIMD_WIDTH>('1') < simd::Vector<u8, SIMD_WIDTH>('2')) ==
            0b1111'1111'1111'1111'1111'1111'1111'1111
        );
        assert(
            (simd::Vector<u8, SIMD_WIDTH>('1') < simd::Vector<u8, SIMD_WIDTH>('0')) ==
            0b0000'0000'0000'0000'0000'0000'0000'0000
        );
        assert(
            (simd::Vector<u8, SIMD_WIDTH>('1') < simd::Vector<u8, SIMD_WIDTH>('1')) ==
            0b0000'0000'0000'0000'0000'0000'0000'0000
        );
    #endif

    return 0;
}


