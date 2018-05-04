#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "lib/kernel/console.h"
#include "devices/input.h"
#include "userprog/process.h"
#include "threads/synch.h"

static void syscall_handler (struct intr_frame *);
static struct lock file_system_lock;
void* is_valid(const void *);

void exit(int status);
int wait(tid_t pid);
int write(int fd, void* buffer, unsigned size);
int read(int fd, void* buffer, unsigned size);
int create (const char *file, unsigned size);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  printf ("system call!\n");
  lock_init(&file_system_lock);
  
  int sys_call = (int)f->esp;
  switch(sys_call){
	  
	  case SYS_HALT:
	  shutdown_power_off();
	  break;
	  
	  case SYS_WAIT:
	  break;
	  
	  case SYS_REMOVE:
	  break;
	  
	  case SYS_CREATE:
	  break;
	  
	  case SYS_OPEN:
	  break;
	  
	  case SYS_CLOSE:
	  break;
	  
	  case SYS_READ:
	  break;

	  case SYS_WRITE:
	  {
		  int* fd=f->esp+4;
	      void** buffer=f->esp+8;
	      unsigned* size=f->esp+12;
	      is_valid(fd);
	      is_valid(buffer);
	      is_valid(size);
	      lock_acquire(&file_system_lock);
	      write(*fd, *buffer, *size);
	      lock_release(&file_system_lock);
	  }
	  break;
	  
	  case SYS_FILESIZE:
	  break;
	  
	  case SYS_SEEK:
	  break;
	  
	  case SYS_TELL:
	  break;
	  
	  case SYS_EXEC:
	  break;
	  
	  case SYS_EXIT:
	  {	
		  int* status=f->esp+4;
		  is_valid(status);
		  lock_acquire(&file_system_lock);
		  exit(*status);
		  lock_release(&file_system_lock);
	  }
	  break;
  }
}
  
void
exit(int status){
	char* b;
	snprintf(b, "Process terminating with status %d", status);
	write(STDOUT_FILENO, b, 30);
	//struct thread* t=thread_current();
	//t->ret_status=status;
	thread_exit();
}

int write(int fd, void* buffer, unsigned size) {
	if (fd==STDOUT_FILENO) {
		putbuf(buffer, size);
	}
	
}

int wait(tid_t pid) {
	
}

int read(int fd, void* buffer, unsigned size) {
	if (fd==STDIN_FILENO) {
		//(buffer, size);
	}
	
}

int
create (const char *file, unsigned size)
{
  if (!file)
    exit(-1);
  return filesys_create (file, size);
}
  
void* 
is_valid(const void *vaddr)
{
	void *temp = pagedir_get_page(thread_current()->pagedir, vaddr);
	if (!is_user_vaddr(vaddr) || !temp)
	{
		exit(-1);
		return 0;
	}
	return temp;
}
  
 // thread_exit ();
//}
