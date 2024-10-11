#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rwlock.h>
#include <linux/uaccess.h>

#define CHAR_COUNT 32
#define TEST_FS_M_STRING "TEST_FS_MODULE"

static int major_number = 0;
static rwlock_t rw_lock;
static char module_string[CHAR_COUNT] = "[ TEST MODULE ]";

ssize_t test_module_read(struct file *fd, char __user *buff, size_t size,
                         loff_t *off) {
  ssize_t read_ret =
      simple_read_from_buffer(buff, size, off, module_string, CHAR_COUNT);

  return read_ret;
}

ssize_t test_module_write(struct file *fd, const char __user *buff, size_t size,
                          loff_t *off) {
  write_lock(&rw_lock);
  if (size > CHAR_COUNT) {
    write_unlock(&rw_lock);
    return -EINVAL;
  }
  ssize_t write_ret =
      simple_write_to_buffer(module_string, CHAR_COUNT, off, buff, size);
  write_unlock(&rw_lock);

  return write_ret;
}

static struct file_operations f_ops = {
    .owner = THIS_MODULE, .read = test_module_read, .write = test_module_write};

int init_module(void) {
  pr_info("[ %s: TRY TO LOAD TEST_FS_MODULE ]\n", TEST_FS_M_STRING);
  rwlock_init(&rw_lock);
  major_number = register_chrdev(major_number, "fs_module", &f_ops);

  if (major_number < 0)
    return major_number;

  pr_info("[ %s: LOAD SUCCESSFUL ]\n"
          "[ %s: MAJOR NUMBER: %d ]\n",
          TEST_FS_M_STRING, TEST_FS_M_STRING, major_number);

  return 0;
}

void cleanup_module(void) {
  unregister_chrdev(major_number, "fs_module");

  pr_info("[ %s: UNLOAD SUCCESSFUL ]", "fs_module");
}

MODULE_LICENSE("GPL");
