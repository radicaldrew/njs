
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) NGINX, Inc.
 */

#include <nxt_types.h>
#include <nxt_clang.h>
#include <nxt_stub.h>
#include <nxt_array.h>
#include <nxt_lvlhsh.h>
#include <nxt_random.h>
#include <nxt_mem_cache_pool.h>
#include <njscript.h>
#include <njs_vm.h>
#include <njs_object.h>
#include <njs_regexp.h>
#include <njs_variable.h>
#include <njs_parser.h>
#include <stdio.h>


static void njs_disassemble(u_char *start, u_char *end);


typedef struct {
    njs_vmcode_operation_t     operation;
    size_t                     size;
    nxt_str_t                  name;
} njs_code_name_t;


static njs_code_name_t  code_names[] = {

    { njs_vmcode_object, sizeof(njs_vmcode_object_t),
          nxt_string("OBJECT          ") },
    { njs_vmcode_function, sizeof(njs_vmcode_function_t),
          nxt_string("FUNCTION        ") },
    { njs_vmcode_regexp, sizeof(njs_vmcode_regexp_t),
          nxt_string("REGEXP          ") },
    { njs_vmcode_object_copy, sizeof(njs_vmcode_object_copy_t),
          nxt_string("OBJECT COPY     ") },

    { njs_vmcode_property_get, sizeof(njs_vmcode_prop_get_t),
          nxt_string("PROPERTY GET    ") },
    { njs_vmcode_property_set, sizeof(njs_vmcode_prop_set_t),
          nxt_string("PROPERTY SET    ") },
    { njs_vmcode_property_in, sizeof(njs_vmcode_3addr_t),
          nxt_string("PROPERTY IN     ") },
    { njs_vmcode_property_delete, sizeof(njs_vmcode_3addr_t),
          nxt_string("PROPERTY DELETE ") },
    { njs_vmcode_instance_of, sizeof(njs_vmcode_instance_of_t),
          nxt_string("INSTANCE OF     ") },

    { njs_vmcode_function_call, sizeof(njs_vmcode_function_call_t),
          nxt_string("FUNCTION CALL   ") },
    { njs_vmcode_return, sizeof(njs_vmcode_return_t),
          nxt_string("RETURN          ") },
    { njs_vmcode_stop, sizeof(njs_vmcode_stop_t),
          nxt_string("STOP            ") },

    { njs_vmcode_increment, sizeof(njs_vmcode_3addr_t),
          nxt_string("INC             ") },
    { njs_vmcode_decrement, sizeof(njs_vmcode_3addr_t),
          nxt_string("DEC             ") },
    { njs_vmcode_post_increment, sizeof(njs_vmcode_3addr_t),
          nxt_string("POST INC        ") },
    { njs_vmcode_post_decrement, sizeof(njs_vmcode_3addr_t),
          nxt_string("POST DEC        ") },

    { njs_vmcode_delete, sizeof(njs_vmcode_2addr_t),
          nxt_string("DELETE          ") },
    { njs_vmcode_void, sizeof(njs_vmcode_2addr_t),
          nxt_string("VOID            ") },
    { njs_vmcode_typeof, sizeof(njs_vmcode_2addr_t),
          nxt_string("TYPEOF          ") },

    { njs_vmcode_unary_plus, sizeof(njs_vmcode_2addr_t),
          nxt_string("PLUS            ") },
    { njs_vmcode_unary_negation, sizeof(njs_vmcode_2addr_t),
          nxt_string("NEGATION        ") },

    { njs_vmcode_addition, sizeof(njs_vmcode_3addr_t),
          nxt_string("ADD             ") },
    { njs_vmcode_substraction, sizeof(njs_vmcode_3addr_t),
          nxt_string("SUBSTRACT       ") },
    { njs_vmcode_multiplication, sizeof(njs_vmcode_3addr_t),
          nxt_string("MULTIPLY        ") },
    { njs_vmcode_division, sizeof(njs_vmcode_3addr_t),
          nxt_string("DIVIDE          ") },
    { njs_vmcode_remainder, sizeof(njs_vmcode_3addr_t),
          nxt_string("REMAINDER       ") },

    { njs_vmcode_left_shift, sizeof(njs_vmcode_3addr_t),
          nxt_string("LEFT SHIFT      ") },
    { njs_vmcode_right_shift, sizeof(njs_vmcode_3addr_t),
          nxt_string("RIGHT SHIFT     ") },
    { njs_vmcode_unsigned_right_shift, sizeof(njs_vmcode_3addr_t),
          nxt_string("USGN RIGHT SHIFT") },

    { njs_vmcode_logical_not, sizeof(njs_vmcode_2addr_t),
          nxt_string("LOGICAL NOT     ") },

    { njs_vmcode_bitwise_not, sizeof(njs_vmcode_2addr_t),
          nxt_string("BINARY NOT      ") },
    { njs_vmcode_bitwise_and, sizeof(njs_vmcode_3addr_t),
          nxt_string("BINARY AND      ") },
    { njs_vmcode_bitwise_xor, sizeof(njs_vmcode_3addr_t),
          nxt_string("BINARY XOR      ") },
    { njs_vmcode_bitwise_or, sizeof(njs_vmcode_3addr_t),
          nxt_string("BINARY OR       ") },

    { njs_vmcode_equal, sizeof(njs_vmcode_3addr_t),
          nxt_string("EQUAL           ") },
    { njs_vmcode_not_equal, sizeof(njs_vmcode_3addr_t),
          nxt_string("NOT EQUAL       ") },
    { njs_vmcode_less, sizeof(njs_vmcode_3addr_t),
          nxt_string("LESS            ") },
    { njs_vmcode_less_or_equal, sizeof(njs_vmcode_3addr_t),
          nxt_string("LESS OR EQUAL   ") },
    { njs_vmcode_greater, sizeof(njs_vmcode_3addr_t),
          nxt_string("GREATER         ") },
    { njs_vmcode_greater_or_equal, sizeof(njs_vmcode_3addr_t),
          nxt_string("GREATER OR EQUAL") },

    { njs_vmcode_strict_equal, sizeof(njs_vmcode_3addr_t),
          nxt_string("STRICT EQUAL    ") },
    { njs_vmcode_strict_not_equal, sizeof(njs_vmcode_3addr_t),
          nxt_string("STRICT NOT EQUAL") },

    { njs_vmcode_move, sizeof(njs_vmcode_move_t),
          nxt_string("MOVE            ") },
    { njs_vmcode_validate, sizeof(njs_vmcode_validate_t),
          nxt_string("VALIDATE        ") },

    { njs_vmcode_throw, sizeof(njs_vmcode_throw_t),
          nxt_string("THROW           ") },
    { njs_vmcode_finally, sizeof(njs_vmcode_finally_t),
          nxt_string("FINALLY         ") },

};


void
njs_disassembler(njs_vm_t *vm)
{
    nxt_uint_t     n;
    njs_vm_code_t  *code;

    code = vm->code->start;
    n = vm->code->items;

    while(n != 0) {
        njs_disassemble(code->start, code->end);
        code++;
        n--;
    }
}


static void
njs_disassemble(u_char *start, u_char *end)
{
    u_char                       *p;
    nxt_str_t                    *name;
    nxt_uint_t                   n;
    const char                   *sign;
    njs_code_name_t              *code_name;
    njs_vmcode_jump_t            *jump;
    njs_vmcode_1addr_t           *code1;
    njs_vmcode_2addr_t           *code2;
    njs_vmcode_3addr_t           *code3;
    njs_vmcode_array_t           *array;
    njs_vmcode_catch_t           *catch;
    njs_vmcode_try_end_t         *try_end;
    njs_vmcode_try_start_t       *try_start;
    njs_vmcode_operation_t       operation;
    njs_vmcode_cond_jump_t       *cond_jump;
    njs_vmcode_test_jump_t       *test_jump;
    njs_vmcode_prop_next_t       *prop_next;
    njs_vmcode_equal_jump_t      *equal;
    njs_vmcode_prop_foreach_t    *prop_foreach;
    njs_vmcode_method_frame_t    *method;
    njs_vmcode_function_frame_t  *function;

    p = start;

    /*
     * On some 32-bit platform uintptr_t is int and compilers warn
     * about %l format modifier.  size_t has the size as pointer so
     * there is no run-time overhead.
     */

    while (p < end) {
        operation = *(njs_vmcode_operation_t *) p;

        if (operation == njs_vmcode_array) {
            array = (njs_vmcode_array_t *) p;
            p += sizeof(njs_vmcode_array_t);

            printf("ARRAY             %04zX %zd\n",
                   (size_t) array->retval, (size_t) array->length);

            continue;
        }

        if (operation == njs_vmcode_if_true_jump) {
            cond_jump = (njs_vmcode_cond_jump_t *) p;
            p += sizeof(njs_vmcode_cond_jump_t);
            sign = (cond_jump->offset >= 0) ? "+" : "";

            printf("JUMP IF TRUE      %04zX %s%zd\n",
                   (size_t) cond_jump->cond, sign, (size_t) cond_jump->offset);

            continue;
        }

        if (operation == njs_vmcode_if_false_jump) {
            cond_jump = (njs_vmcode_cond_jump_t *) p;
            p += sizeof(njs_vmcode_cond_jump_t);
            sign = (cond_jump->offset >= 0) ? "+" : "";

            printf("JUMP IF FALSE     %04zX %s%zd\n",
                   (size_t) cond_jump->cond, sign, (size_t) cond_jump->offset);

            continue;
        }

        if (operation == njs_vmcode_jump) {
            jump = (njs_vmcode_jump_t *) p;
            p += sizeof(njs_vmcode_jump_t);
            sign = (jump->offset >= 0) ? "+" : "";

            printf("JUMP              %s%zd\n", sign, (size_t) jump->offset);

            continue;
        }

        if (operation == njs_vmcode_if_equal_jump) {
            equal = (njs_vmcode_equal_jump_t *) p;
            p += sizeof(njs_vmcode_equal_jump_t);

            printf("JUMP IF EQUAL     %04zX %04zX +%zd\n",
                   (size_t) equal->value1, (size_t) equal->value2,
                   (size_t) equal->offset);

            continue;
        }

        if (operation == njs_vmcode_test_if_true) {
            test_jump = (njs_vmcode_test_jump_t *) p;
            p += sizeof(njs_vmcode_test_jump_t);

            printf("TEST IF TRUE      %04zX %04zX +%zd\n",
                   (size_t) test_jump->retval, (size_t) test_jump->value,
                   (size_t) test_jump->offset);

            continue;
        }

        if (operation == njs_vmcode_test_if_false) {
            test_jump = (njs_vmcode_test_jump_t *) p;
            p += sizeof(njs_vmcode_test_jump_t);

            printf("TEST IF FALSE     %04zX %04zX +%zd\n",
                   (size_t) test_jump->retval, (size_t) test_jump->value,
                   (size_t) test_jump->offset);

            continue;
        }

        if (operation == njs_vmcode_function_frame) {
            function = (njs_vmcode_function_frame_t *) p;
            p += sizeof(njs_vmcode_function_frame_t);

            printf("FUNCTION FRAME    %04zX %zd%s\n",
                   (size_t) function->name, function->nargs,
                   function->code.ctor ? " CTOR" : "");

            continue;
        }

        if (operation == njs_vmcode_method_frame) {
            method = (njs_vmcode_method_frame_t *) p;
            p += sizeof(njs_vmcode_method_frame_t);

            printf("METHOD FRAME      %04zX %04zX %zd%s\n",
                   (size_t) method->object, (size_t) method->method,
                   method->nargs, method->code.ctor ? " CTOR" : "");

            continue;
        }

        if (operation == njs_vmcode_property_foreach) {
            prop_foreach = (njs_vmcode_prop_foreach_t *) p;
            p += sizeof(njs_vmcode_prop_foreach_t);

            printf("PROPERTY FOREACH  %04zX %04zX +%zd\n",
                   (size_t) prop_foreach->next, (size_t) prop_foreach->object,
                   (size_t) prop_foreach->offset);

            continue;
        }

        if (operation == njs_vmcode_property_next) {
            prop_next = (njs_vmcode_prop_next_t *) p;
            p += sizeof(njs_vmcode_prop_next_t);

            printf("PROPERTY NEXT     %04zX %04zX %04zX %zd\n",
                   (size_t) prop_next->retval, (size_t) prop_next->object,
                   (size_t) prop_next->next, (size_t) prop_next->offset);

            continue;
        }

        if (operation == njs_vmcode_try_start) {
            try_start = (njs_vmcode_try_start_t *) p;
            p += sizeof(njs_vmcode_try_start_t);

            printf("TRY START         %04zX +%zd\n",
                   (size_t) try_start->value, (size_t) try_start->offset);

            continue;
        }

        if (operation == njs_vmcode_catch) {
            catch = (njs_vmcode_catch_t *) p;
            p += sizeof(njs_vmcode_catch_t);

            printf("CATCH             %04zX +%zd\n",
                   (size_t) catch->exception, (size_t) catch->offset);

            continue;
        }

        if (operation == njs_vmcode_try_end) {
            try_end = (njs_vmcode_try_end_t *) p;
            p += sizeof(njs_vmcode_try_end_t);

            printf("TRY END           +%zd\n", (size_t) try_end->offset);

            continue;
        }

        code_name = code_names;
        n = nxt_nitems(code_names);

        do {
             if (operation == code_name->operation) {
                 name = &code_name->name;

                 if (code_name->size == sizeof(njs_vmcode_3addr_t)) {
                     code3 = (njs_vmcode_3addr_t *) p;

                     printf("%*s  %04zX %04zX %04zX\n",
                            (int) name->len, name->data, (size_t) code3->dst,
                            (size_t) code3->src1, (size_t) code3->src2);

                 } else if (code_name->size == sizeof(njs_vmcode_2addr_t)) {
                     code2 = (njs_vmcode_2addr_t *) p;

                     printf("%*s  %04zX %04zX\n",
                            (int) name->len, name->data,
                            (size_t) code2->dst, (size_t) code2->src);

                 } else if (code_name->size == sizeof(njs_vmcode_1addr_t)) {
                     code1 = (njs_vmcode_1addr_t *) p;

                     printf("%*s  %04zX\n",
                            (int) name->len, name->data,
                            (size_t) code1->index);
                 }

                 p += code_name->size;

                 goto next;
             }

             code_name++;
             n--;

        } while (n != 0);

        p += sizeof(njs_vmcode_operation_t);

        printf("UNKNOWN           %04zX\n", (size_t) (uintptr_t) operation);

    next:

        continue;
    }
}
