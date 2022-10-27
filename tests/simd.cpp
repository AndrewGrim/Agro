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

    assert((simd::Vector<u8, 8>(255).mask()) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(255).mask()) != 0b0000'0000);
    assert((simd::Vector<u8, 8>(1).mask()) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1).mask()) != 0b1111'1111);
    assert((simd::Vector<u8, 8>(1).equal((simd::Vector<u8, 8>(1))).mask()) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1).equal((simd::Vector<u8, 8>(1)))._data[0]) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1).equal((simd::Vector<u8, 8>(1)))._data[7]) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1).equal((simd::Vector<u8, 8>(2))).mask()) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1).equal((simd::Vector<u8, 8>(2)))._data[0]) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1).equal((simd::Vector<u8, 8>(2)))._data[7]) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1) == simd::Vector<u8, 8>(1)) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1) == simd::Vector<u8, 8>(2)) == 0b0000'0000);

    assert((simd::Vector<u8, 8>(1).notEqual((simd::Vector<u8, 8>(1))).mask()) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1).notEqual((simd::Vector<u8, 8>(1)))._data[0]) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1).notEqual((simd::Vector<u8, 8>(1)))._data[7]) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1).notEqual((simd::Vector<u8, 8>(2))).mask()) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1).notEqual((simd::Vector<u8, 8>(2)))._data[0]) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1).notEqual((simd::Vector<u8, 8>(2)))._data[7]) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1) != simd::Vector<u8, 8>(1)) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1) != simd::Vector<u8, 8>(2)) == 0b1111'1111);

    assert((simd::Vector<u8, 8>(1).lessThan((simd::Vector<u8, 8>(2))).mask()) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(1).lessThan((simd::Vector<u8, 8>(1))).mask()) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(1) < simd::Vector<u8, 8>(2)) == 0b1111'1111);

    assert((simd::Vector<u8, 8>(2).greaterThan((simd::Vector<u8, 8>(1))).mask()) == 0b1111'1111);
    assert((simd::Vector<u8, 8>(2).greaterThan((simd::Vector<u8, 8>(3))).mask()) == 0b0000'0000);
    assert((simd::Vector<u8, 8>(2) < simd::Vector<u8, 8>(1)) == 0b0000'0000);

    return 0;
}


