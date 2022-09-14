#include <iostream>

#include "../src/core/array_list.hpp"
#include "../src/common/expect.hpp"

void test_append() {
    ArrayList<i32> arr = ArrayList<i32>(1);
    expect(arr.size() == 0 and arr.capacity() == 1);

    arr.append(10);
    expect(arr[0] == 10);
    expect(arr.size() == 1 and arr.capacity() == 2);

    arr.append(20);
    expect(arr[1] == 20);
    expect(arr.size() == 2 and arr.capacity() == 4);

    arr.append(30);
    expect(arr[2] == 30);
    expect(arr.size() == 3 and arr.capacity() == 4);

    arr.append(40);
    expect(arr[3] == 40);
    expect(arr.size() == 4 and arr.capacity() == 8);
}

void test_erase() {
    {
        ArrayList<i32> arr = ArrayList<i32>(5);
        arr.append(10);
        arr.append(20);
        arr.append(30);
        arr.append(40);

        expect(arr.erase(0) == 10);
        expect(arr.size() == 3);
        expect(arr.data()[arr.size()] == 40);

        expect(arr.erase(0) == 20);
        expect(arr.size() == 2);
        expect(arr.data()[arr.size()] == 40);

        expect(arr.erase(0) == 30);
        expect(arr.size() == 1);
        expect(arr.data()[arr.size()] == 40);

        expect(arr.erase(0) == 40);
        expect(arr.size() == 0);
        expect(arr.data()[arr.size()] == 40);

        // Assert out of bounds.
        // arr.erase(0);
    }
    {
        ArrayList<i32> arr = ArrayList<i32>(5);
        arr.append(10);
        arr.append(20);
        arr.append(30);
        arr.append(40);
        expect(arr.size() == 4);

        arr.erase(0, 2);
        expect(arr[0] == 30);
        expect(arr[1] == 40);
        expect(arr.size() == 2);
    }
}

void test_insert() {
    {
        ArrayList<i32> arr = ArrayList<i32>(1);
        arr.append(10);
        arr.append(20);
        arr.append(30);
        arr.insert(0, 40);

        expect(arr[0] == 40);
        expect(arr[1] == 10);
        expect(arr[2] == 20);
        expect(arr[3] == 30);
    }
    {
        ArrayList<i32> arr = ArrayList<i32>(1);
        arr.append(10);
        arr.append(20);
        arr.append(30);
        arr.insert(1, 40);

        expect(arr[0] == 10);
        expect(arr[1] == 40);
        expect(arr[2] == 20);
        expect(arr[3] == 30);
    }
    {
        ArrayList<i32> arr = ArrayList<i32>(1);
        arr.append(10);
        arr.append(20);
        arr.append(30);
        arr.insert(2, 40);

        expect(arr[0] == 10);
        expect(arr[1] == 20);
        expect(arr[2] == 40);
        expect(arr[3] == 30);
    }
    {
        ArrayList<i32> arr = ArrayList<i32>(1);
        arr.append(10);
        arr.append(20);
        arr.append(30);
        arr.insert(3, 40);

        expect(arr[0] == 10);
        expect(arr[1] == 20);
        expect(arr[2] == 30);
        expect(arr[3] == 40);
    }
}

void test_emplace() {
    ArrayList<i32> arr = ArrayList<i32>(1);
    arr.emplace(0, 10);
    arr.emplace(1, 20);
    arr.emplace(2, 30);
    arr.emplace(3, 40);

    expect(arr[0] == 10);
    expect(arr[1] == 20);
    expect(arr[2] == 30);
    expect(arr[3] == 40);
}

void test_clear() {
    ArrayList<i32> arr = ArrayList<i32>(1);
    arr.append(10);
    arr.append(20);
    arr.append(30);
    arr.append(40);

    arr.clear();
    expect(arr.data()[0] == 10);
    expect(arr.data()[1] == 20);
    expect(arr.data()[2] == 30);
    expect(arr.data()[3] == 40);
}

void test_shift() {
    {
        ArrayList<i32> arr;
        arr.append(1);
        arr.append(2);
        arr.append(3);
        arr.append(4);

        arr.shiftLeft(2, 2);
        expect(arr[0] == 1);
        expect(arr[1] == 3);
        expect(arr[2] == 4);
        expect(arr[3] == 2);
    }
    {
        ArrayList<i32> arr;
        arr.append(1);
        arr.append(2);
        arr.append(3);
        arr.append(4);

        arr.shiftRight(0, 2);
        expect(arr[0] == 3);
        expect(arr[1] == 1);
        expect(arr[2] == 2);
        expect(arr[3] == 4);
    }
}

int main() {
    test_append();
    test_insert();
    test_emplace();
    test_erase();
    test_clear();
    test_shift();

    return 0;
}
