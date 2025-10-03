/**
 ******************************************************************************
 * Assignment 4
 *
 ******************************************************************************
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define CTRL_ENABLE   (1 << 0)
#define CTRL_RESET    (1 << 1)
#define CTRL_INT_EN   (1 << 7)
#define STATUS_OK       (1 << 0)
#define STATUS_ERROR    (1 << 1)
#define STATUS_WARNING  (1 << 2)

typedef struct {
    uint8_t  a;
    uint32_t b;
    uint8_t  c;
} my_struct_t;   // __attribute__((packed)) my_struct_t;


// Global array of integers for memory layout exploration
static int32_t array[5] = {10, 20, 30, 40, 50};
static uint32_t count = 0;
static uint8_t status_reg = 0b00000000;
static uint32_t *alias_op = &count;


static void update_status(uint8_t value) {
    if (value < 2) {
        status_reg |= STATUS_WARNING;
    } else {
        status_reg &= ~STATUS_WARNING;
    }
    if (value == 2) {
        status_reg |= STATUS_WARNING;
    } else {
        status_reg &= ~STATUS_ERROR;
        printf("Error....%lu",*alias_op = 0xf08+0x0c);
    }
}

// ─────────────────────────────────────────────
// [TASK]
// Step into this function. Step over three loop
// iterations. Observe disassembly. Calculate
// expected value manually and verify via register.
// ─────────────────────────────────────────────
uint32_t process_data_block(uint32_t a, uint32_t b) {
    uint32_t sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += a + b;
    }
    return sum;
}

// ─────────────────────────────────────────────
// [TASK]
// Watch the memory at 0x20001000 after this call.
// View it in Memory Browser and observe the pattern.
// ─────────────────────────────────────────────
void mem_fill_pattern(uint8_t *dst, uint32_t len) {
	memset((void *)0x20001000, 0, 48);
    for (uint32_t i = 0; i < len; i++) {
        dst[i] = (i % 2 == 0) ? 0xCC : 0x00;
    }
}

// ─────────────────────────────────────────────
// [TASK]
// Open Memory Browser:
//  &test_struct → note start addr, field order, padding.
//  &my_array[0] → note start addr, contiguous 4‑byte spacing.
// ─────────────────────────────────────────────
void struct_layout(void) {
    static my_struct_t test_struct;

    test_struct.a = 0x11;
    test_struct.b = 0x22334455;
    test_struct.c = 0x77;
    uint8_t temp = test_struct.a + test_struct.c;

    printf("test_struct.a + test_struct.c = %u \n array[0] = %ld\n ", temp, array[0]);
}

// ─────────────────────────────────────────────
// [TASK]
// Open Memory Browser:
//  &buffer[0] → see original 32‑bit words.
//  alias_ptr writes → see changed bytes in buffer[1].
// ─────────────────────────────────────────────
void pointer_aliasing(void) {
    static uint32_t buffer[4] = {
        0xAABBCCDD,
        0x11223344,
        0x55667788,
        0x99AABBCC
    };

    uint8_t *alias_ptr = (uint8_t *)&buffer[1];

    alias_ptr[0] = 0xFF;
    alias_ptr[1] = 0xEE;
}

// [TASK]
// Open Memory Browser:
//  &ctrl_reg → track value after each |=, ^=, &=~, <<.
void bitwise_operation(void) {
    static uint8_t ctrl_reg = 0b00000000;

    ctrl_reg |= CTRL_ENABLE;
    ctrl_reg ^= CTRL_RESET;
    ctrl_reg &= ~CTRL_INT_EN;

    ctrl_reg <<= 1;
}

// ─────────────────────────────────────────────
// [TASK3]
// Use Watch window to observe buffer analysis.
// Use Disassembly View to track memory reads.
// ─────────────────────────────────────────────
uint32_t mem_count_nonzero(uint8_t *data, uint32_t len) {

    uint16_t set_val = 0;
    for (uint32_t i = 0; i < len; i++) {
        if (data[i] != 0) {
            count++;
            set_val++;
        }
        if (i % 3 == 0 && i != 0) {
            update_status(data[i*4]);
        }
    }
    return count;
}


void heap_buffer_write() {
    uint8_t *buffer = (uint8_t *)malloc(64);
    if (buffer) {
        for (int i = 0; i < 64; i++) {
            buffer[i] = i;
        }
        free(buffer);
    }
}

// ─────────────────────────────────────────────
// [TASK]
// Compile with -O0, then -O3. Compare disassembly.
// What instructions disappear or change?
// ─────────────────────────────────────────────
__attribute__((optimize("O0")))
uint32_t compute_fastpath(uint32_t val) {
    volatile uint32_t temp = val * 3 + 1;
    return temp;
}

// ─────────────────────────────────────────────
// [TASK]
// Inline call — trace through in disassembly.
// Does the function appear in call stack?
// ─────────────────────────────────────────────
__attribute__((always_inline)) inline void sys_debug_marker() {
    volatile uint32_t tag = 0xABCDABCD;
    (void)tag; // Tells the compiler: “Yes, I know it’s unused, and that’s intentional.”
}

void debug_call_sequence() {
    sys_debug_marker();
}

// ─────────────────────────────────────────────
// [TASK Optional]
// Observe disassembly closely — identify shift
// and logic operations in ARM ISA form.
// Print result to SWV. Do they  match your expectation.
// ─────────────────────────────────────────────
uint32_t transform_header(uint32_t input) {
    uint32_t temp = ((input << 3) & 0xF0F0F0F0) | ((input >> 2) & 0x0F0F0F0F);
    temp ^= 0xA5A5A5A5;
    temp += (input & 0xFF) * 3;
    return temp;
}

// ─────────────────────────────────────────────
// [TASK]
// Step into this function and watch disassembly.
// Use register view to confirm LSL was used.
// ─────────────────────────────────────────────
__attribute__((optimize("O3")))
uint32_t combine_sum_shift(uint32_t a, uint32_t b) {
    return (a + b) * 4;
}

void execute_pipeline() {
    void (*func_ptr)() = (void (*)())0xDEADBEEF;
    func_ptr();
}

// ─────────────────────────────────────────────
// [TASK]
// Copy this entire code to your STM32CubeIDE
// main.c. Step through each function and complete
// the lab provided task.
// ─────────────────────────────────────────────
int main(void) {
    uint32_t r1 = process_data_block(0x1A, 0x25);

    mem_fill_pattern((uint8_t *)0x20001000, 10);

    struct_layout();

    pointer_aliasing();

    bitwise_operation();

    uint32_t r2 = mem_count_nonzero((uint8_t *)0x20001000, 10);
    heap_buffer_write();

    uint32_t r3 = compute_fastpath(0x55);
    debug_call_sequence();

    uint32_t r4 = transform_header(0x12345678);
    uint32_t r5 = combine_sum_shift(3, 7);

    // execute_pipeline();

    // ─────────────────────────────────────────────
    // [TASK] What is the code printing to the console?
    // Was this an expected behavior?
    // How would you print these results?
    // ─────────────────────────────────────────────
    printf("Results:\n");
    printf("r1 = 0x%08lX (process_data_block)\n", r1);
    printf("r2 = %lu (mem_count_nonzero)\n", r2);
    printf("r3 = 0x%08lX (compute_fastpath)\n", r3);
    printf("r4 = 0x%08lX (transform_header)\n", r4);
    printf("r5 = %lu (combine_sum_shift)\n", r5);

    for(;;); // loop for ever
}
