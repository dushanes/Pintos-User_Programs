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
#include "pagedir.h"

static void syscall_handler (struct intr_frame *);
static struct lock file_system_lock;
void* is_valid(const void *);

void exit(int status);
int write(int fd, void* buffer, unsigned size);
int read(int fd, void* buffer, unsigned size);
int create (const char *file, unsigned size);
struct file * find_file(int fd);

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
	static int l = 0;
	l++;
  printf ("system call!%d", l);
  lock_init(&file_system_lock);
  
  int * temp = f->esp;
  int sys_call = *temp;
  is_valid(temp);
  printf("'System call #%d'\n", sys_call);
  switch(sys_call){
	  
	  case SYS_HALT:
	  shutdown_power_off();
	  break;
	  
	  case SYS_WAIT:
	  is_valid(temp+1);
	  f->eax = process_wait(temp+1);
	  break;
	  
	  case SYS_REMOVE:
	  break;
	  
	  case SYS_CREATE:
	  /*is_valid(temp+1); 
	  is_valid(temp+2);
	  
	  lock_acquire(&file_system_lock);
	  create(temp+1, temp+2);
	  lock_release(&file_system_lock);*/
	  break;
	  
	  case SYS_OPEN:
	  break;
	  
	  case SYS_CLOSE:
	  break;
	  
	  case SYS_READ:
	  is_valid(temp+5); 
	  is_valid(temp+6);
	  is_valid(temp+7);
	  
	  read(temp+5, temp+6, temp+7);
	  break;

	  case SYS_WRITE:
	  {
		  is_valid(temp+5); 
	      is_valid(temp+6);
	      is_valid(temp+7);
		  
		  int *fd = *(temp+5);
		  printf("mode: %d\n",fd);
	      void* buffer = *(temp+6);
		  printf("string: '%s'\n",buffer);
	      unsigned* size = *(temp+7);;

	      lock_acquire(&file_system_lock);
	      write(fd, buffer, size);
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
		  is_valid(f->esp+1);
		  int status=(*(temp+1));
		  
		  lock_acquire(&file_system_lock);
		  exit(status);
		  lock_release(&file_system_lock);
	  }
	  break;
  }
}
  
void
exit(int status){

	struct thread* t=thread_current();
	t->return_status = status;
	thread_exit();
	//return -1;
}

int write(int fd, void* buffer, unsigned size) {
	
	struct file * _file;
	int _return = -1;;
	
	if (fd==STDOUT_FILENO) {
		putbuf(buffer, size);
	}else if (fd == STDIN_FILENO){
		lock_release(&file_system_lock);
		return -1;
	}else{
		_file = find_file(fd);
		if(!_file){
			lock_release(&file_system_lock);
			return -1;
		}
		_return = file_write(_file, buffer, size);
	}
	return -1;
	
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
		_file = find_file(fd);
		if(!_file){                     
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
