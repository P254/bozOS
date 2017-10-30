#ifndef TESTS_H
#define TESTS_H

// decide which tests we want to launch
#define READ_DATA_TEST_ENABLE 1
#define READ_DENTRY_NAME_TEST_ENABLE 1
#define READ_DENTRY_IDX_TEST_ENABLE 1
#define PRINT_ALL_DIR_TEST_ENABLE 1
#define TEMRINAL_WRITE_TEST_ENABLE 1
#define TEMRINAL_READ_TEST_ENABLE 1
#define RTC_TEST_ENABLE 0

// test-launching function
void launch_tests();

#endif /* TESTS_H */
