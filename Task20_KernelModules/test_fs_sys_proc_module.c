#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/rwlock.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>

#define TEST_FS_M_STRING "TEST_FS_MODULE"

#define TEST_STR_SIZE 32 /* Макрос размера строки @test_module_string.*/
static rwlock_t rw_lock;

static int major_number = 0;
static char test_module_string[TEST_STR_SIZE] = "[ TEST MODULE ]";
static struct proc_dir_entry *test_proc_dir_entry = NULL;
static struct kobject *test_kobject = NULL;

static ssize_t test_module_read(struct file *fd, char __user *buff, size_t size,
                                loff_t *off) {
  ssize_t read_ret = simple_read_from_buffer(buff, size, off,
                                             test_module_string, TEST_STR_SIZE);

  return read_ret;
}
static ssize_t test_module_write(struct file *fd, const char __user *buff,
                                 size_t size, loff_t *off) {
  write_lock(&rw_lock);
  if (size > TEST_STR_SIZE) {
    write_unlock(&rw_lock);
    return -EINVAL;
  }

  ssize_t write_ret = simple_write_to_buffer(test_module_string, TEST_STR_SIZE,
                                             off, buff, size);
  write_unlock(&rw_lock);

  return write_ret;
}

static ssize_t test_module_proc_write(struct file *fd, const char __user *buff,
                                      size_t size, loff_t *off) {
  /* Если размер вводимой строки больше строки для записи */
  if (size > TEST_STR_SIZE)
    return -EINVAL;

  ssize_t write_return = simple_write_to_buffer(test_module_string,
                                                TEST_STR_SIZE, off, buff, size);

  return write_return;
}
static ssize_t test_module_proc_read(struct file *fd, char __user *buff,
                                     size_t size, loff_t *off) {
  ssize_t read_ret = simple_read_from_buffer(buff, size, off,
                                             test_module_string, TEST_STR_SIZE);

  return read_ret;
}

static ssize_t test_string_show(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf) {
  memcpy(buf, test_module_string, TEST_STR_SIZE);
  ssize_t readed_bytes = strlen(test_module_string);
  return readed_bytes;
}

static ssize_t test_string_store(struct kobject *kobj,
                                 struct kobj_attribute *attr, char const *buf,
                                 size_t count) {
  /* Если размер вводимой строки больше строки для записи */
  if (count > TEST_STR_SIZE)
    return -EINVAL;

  memcpy(test_module_string, buf, count);
  ssize_t writed_bytes = strlen(buf);

  return writed_bytes;
}

/* Инициализация структур */

/** Определение полей структуры @file_operations для [fs]:
 * @owner владелец
 * @read функция чтения
 * @write фунция записи
 */
static const struct file_operations f_ops = {
    .owner = THIS_MODULE, .read = test_module_read, .write = test_module_write};

/** Определение полей структуры @file_operations для [sys_proc]:
 * @read функция чтения
 * @write фунция записи
 */
static const struct proc_ops p_ops = {.proc_read = test_module_proc_read,
                                      .proc_write = test_module_proc_write};

/* Аттрибуты для файловой системы (fs) sys */
static struct kobj_attribute string_attribute =
    __ATTR(test_module_string, 0644, test_string_show, test_string_store);

/* Набор аттрибутов для fs sys*/
static struct attribute *attrs[] = {&string_attribute.attr, NULL};

/* Группа аттрибутов для fs sys */
static const struct attribute_group attr_group = {.attrs = attrs};

/* ИНИЦИАЛИЗАЦИЯ МОДУЛЯ */
int __init test_init_module(void) {

  int ret_val = 0;
  pr_info("[ %s: TRY TO LOAD TEST_FS_MODULE ]\n", TEST_FS_M_STRING);

  rwlock_init(&rw_lock);
  major_number = register_chrdev(major_number, "fs_module", &f_ops);

  if (major_number < 0)
    return major_number;

  test_proc_dir_entry = proc_create("test", 0666, NULL, &p_ops);

  test_kobject = kobject_create_and_add("kobject_test", kernel_kobj);

  if (!test_kobject)
    return -ENOMEM;

  ret_val = sysfs_create_group(test_kobject, &attr_group);

  if (ret_val) {
    kobject_put(test_kobject);
    return ret_val;
  }

  pr_info("[ %s: LOAD SUCCESSFUL ]\n"
          "[ %s: MAJOR NUMBER: %d ]\n",
          TEST_FS_M_STRING, TEST_FS_M_STRING, major_number);

  return 0;
}

/* ОЧИСТКА МОДУЛЯ */
void __exit test_exit_module(void) {
  unregister_chrdev(major_number, "fs_module");
  proc_remove(test_proc_dir_entry);
  kobject_put(test_kobject);

  pr_info("[ %s: UNLOAD SUCCESSFUL ]", "fs_module");
}

/* Определение функций ИНИЦИАЛИЗАЦИИ и ОЧИСТКИ модуля */
module_init(test_init_module);
module_exit(test_exit_module);

MODULE_LICENSE("GPL");
