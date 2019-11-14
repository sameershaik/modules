#include <linux/signal.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>

static struct task_struct *thread_st;

static int thread_fn(void * data)
{
  allow_signal(SIGKILL);
  while(!kthread_should_stop()) {
    printk(KERN_INFO"kernel_thread running\n");
    ssleep(5);
    if (signal_pending(thread_st))
      break;
  }
  printk(KERN_INFO"Stopping thread\n");
  do_exit(0);
  return 0;
}

static int  thread_init(void)
{
  printk(KERN_INFO"Creating the thread\n");

  thread_st = kthread_run(thread_fn, NULL, "mythread");

  if (thread_st) {
    printk(KERN_INFO"Thread created successfully\n");
    /* wake_up_process(thread_st); */
  }  else {
    printk(KERN_INFO"Can't be able to create the thread\n");
  }
  return 0;
}

static void thread_close(void)
{
  if (thread_st)
    kthread_stop(thread_st);
  printk(KERN_INFO"Cleaning up the thread\n");

}

MODULE_LICENSE("GPL");
module_init(thread_init);
module_exit(thread_close);

  
  
