#include <iostream>
#include <string>
#include <functional>

#include "intrusive_ptr.hpp"

namespace {

    class ref_counter {

        int ref_cnt = 0;

    public:

        ref_counter() = default;

        ref_counter(int value) : ref_cnt(value) {}

        void add_ref() {
            ++ref_cnt;
        }

        void release() {
            --ref_cnt;
        }

        int get_refcnt() {
            return ref_cnt;
        }

    };

    class daniel {

        ref_counter *ref_cnt = nullptr;

    public:

        daniel() = default;

        daniel(int ref_cnt) : ref_cnt(new ref_counter(ref_cnt)) {}

        virtual ~daniel() {
            release();
        }

        void add_ref() {
            if (ref_cnt == nullptr) {
                ref_cnt = new ref_counter();
            }
            ref_cnt->add_ref();
        }

        void release() {
            ref_cnt->release();
            if (ref_cnt->get_refcnt() == 0) {
                delete ref_cnt;
            }
        }

        int get_refcnt() {
            if (ref_cnt == nullptr) {
                return 0;
            } else {
                return ref_cnt->get_refcnt();
            }
        }

    };

    class cooler_daniel : public daniel {

    public:

        cooler_daniel() = default;

        cooler_daniel(int ref_cnt) : daniel(ref_cnt) {}

        ~cooler_daniel() {
            release();
        }

        void flex() {
            std::cout << "FLEXING\n";
        }

    };

}

daniel *five, *four, *zero;
cooler_daniel *cooler_five;

void reinit_data() {
    five = new daniel(5),
    four = new daniel(4),
    zero = new daniel(0);
    cooler_five = new cooler_daniel(5);
}

bool test_construction() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);
    smart_ptr::intrusive_ptr<daniel> zero_ptr(zero);
    smart_ptr::intrusive_ptr<daniel> null_ptr;

    result *= (five_ptr.get() == five);
    result *= (zero_ptr.get() == zero);
    result *= (null_ptr.get() == nullptr);

    return result;
}

bool test_construction_refcnt() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);
    smart_ptr::intrusive_ptr<daniel> four_ptr(four);
    smart_ptr::intrusive_ptr<daniel> zero_ptr(zero);
    smart_ptr::intrusive_ptr<daniel> null_ptr;

    result *= (four_ptr.get_refcnt() != five_ptr.get_refcnt());
    result *= (zero_ptr.get_refcnt() == 1);
    result *= (null_ptr.get_refcnt() == 0);

    return result;
}

bool test_copy_move_refcnt() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);
    smart_ptr::intrusive_ptr<daniel> four_ptr(four);
    smart_ptr::intrusive_ptr<daniel> zero_ptr(zero);

    smart_ptr::intrusive_ptr<daniel> five_ptr_copy(five_ptr);
    smart_ptr::intrusive_ptr<daniel> four_ptr_move(std::move(four_ptr));

    result *= (five_ptr.get_refcnt() == 7);
    result *= (five_ptr_copy.get_refcnt() == 7);
    result *= (four_ptr_move.get_refcnt() == 5);

    five_ptr_copy = zero_ptr;

    result *= (five_ptr.get_refcnt() == 6);
    result *= (zero_ptr.get_refcnt() == 2);

    return result;
}

bool test_destruction_refcnt() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);

    smart_ptr::intrusive_ptr<daniel> five_ptr_copy(five_ptr);
    five_ptr_copy.~intrusive_ptr<daniel>();

    result *= (five_ptr.get_refcnt() == 6);

    return result;
}

bool test_assignment() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);
    smart_ptr::intrusive_ptr<daniel> four_ptr(four);

    smart_ptr::intrusive_ptr<daniel> five_ptr_copy = five_ptr;
    smart_ptr::intrusive_ptr<daniel> four_ptr_move = std::move(four_ptr);

    result *= (five_ptr.get_refcnt() == 7);
    result *= (five_ptr_copy.get_refcnt() == 7);
    result *= (four_ptr_move.get_refcnt() == 5);

    return result;
}

bool test_swap() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);
    smart_ptr::intrusive_ptr<daniel> four_ptr(four);

    five_ptr.swap(four_ptr);

    result *= (five_ptr.get_refcnt() == 5);
    result *= (four_ptr.get_refcnt() == 6);

    return result;
}

bool test_dereferencing() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);

    result *= (five_ptr.get() == five);
    result *= ((*five_ptr).get_refcnt() == five->get_refcnt());
    result *= (five_ptr->get_refcnt() == five->get_refcnt());

    return result;
}

bool test_operator_bool() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(five);
    smart_ptr::intrusive_ptr<daniel> null_ptr;

    result *= ((bool) five_ptr);
    result *= !((bool) null_ptr);

    return result;
}

bool test_dynamic_pointer_cast_and_inheritance_constructor() {
    bool result = true;

    smart_ptr::intrusive_ptr<daniel> five_ptr(cooler_five);
    smart_ptr::intrusive_ptr<daniel> five_ptr2 = five_ptr;
    smart_ptr::intrusive_ptr<cooler_daniel> cooler_five_ptr = smart_ptr::dynamic_pointer_cast<cooler_daniel>(five_ptr);
    auto cooler_five_ptr2 = cooler_five_ptr;

    result *= (cooler_five_ptr.get_refcnt() == five_ptr.get_refcnt());

    return result;
}

int passed = 0, failed = 0;

void launch_test(const std::string &name, bool result) {
    std::string result_readable;
    if (result) {
        ++passed;
        result_readable = "PASSED";
    } else {
        ++failed;
        result_readable = "FAILED";
    }

    std::cout << name << ": " << result_readable << "\n";
    reinit_data();
}

int main() {
    reinit_data();

    launch_test("test_construction", test_construction());
    launch_test("test_construction_refcnt", test_construction_refcnt());
    launch_test("test_copy_move_refcnt", test_copy_move_refcnt());
    launch_test("test_destruction_refcnt", test_destruction_refcnt());
    launch_test("test_assignment", test_assignment());
    launch_test("test_swap", test_swap());
    launch_test("test_dereferencing", test_dereferencing());
    launch_test("test_operator_bool", test_operator_bool());
    launch_test("test_dynamic_pointer_cast_and_inheritance_constructor",
                test_dynamic_pointer_cast_and_inheritance_constructor());
    std::cout << "<-----RESULT----->\n";
    std::cout << "Total passed: " << passed << "; total failed: " << failed;

    return 0;
}