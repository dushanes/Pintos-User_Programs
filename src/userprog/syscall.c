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
#include <list.h>
#include "filesys/file.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);
static struct lock file_system_lock;
void* is_valid(const void *);

void exit(int status);
int wait(tid_t pid);
int write(int fd, void* buffer, unsigned size);
int read(int fd, void* buffer, unsigned size);
int create (const char *file, unsigned size);

struct fd_elem
{
	int value;
	struct file * _file;
	struct list_elem elem;
}fd_elem;

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
	  read(f->esp+4, f->esp+8, f->esp+12);
	  break;

	  case SYS_WRITE:
	  {
		  is_valid(f->esp+4); //check validity of the pointer before doing anything else -dushane
	      is_valid(f->esp+8);
	      is_valid(f->esp+12);
		  
		  int* fd=f->esp+4;
	      void** buffer=f->esp+8;
	      unsigned* size=f->esp+12;

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
		  is_valid(f->esp+4);
		  int* status=f->esp+4;
		  
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
	
	struct file * _file;
	int _return = -1;;
	
	lock_acquire(&file_system_lock);
	
	if (fd == STDIN_FILENO) {
		for (int i = 0; i != size; ++i)
        *(uint8_t *)(buffer + i) = input_getc ();
		_return = size;
		lock_release(&file_system_lock);
		return _return;
	}else if(fd == STDOUT_FILENO){
		lock_release(&file_system_lock);
		return _return;
	}else{
		if(!_file){                     //Incomplete -Dushane
			lock_release(&file_system_lock);
			return _return;
		}
		_return = file_read(_file, buffer, size);
		lock_release(&file_system_lock);
		return _return;
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

struct file *
find_file(int fd){
	struct fd_elem * _return;
	struct list_elem *i;
	struct thread * t = thread_current();
  
	for (i = list_begin (&t->open_files); i != list_end (&t->open_files); i = list_next (i))
	{
		_return = list_entry (i, struct fd_elem, elem);
		if (_return->value == fd)
			return _return->_file;
    }
}
  
 // thread_exit ();
//}
