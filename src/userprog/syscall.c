#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  
  int sys_call = f->esp;
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
	  break;
	  
	  case SYS_FILESIZE:
	  break;
	  
	  case SYS_SEEK:
	  break;
	  
	  case SYS_TELL:
	  break;
	  
	  case SYS_EXEC:
	  break;
  }
  
void
exit(int status){
	
}
  
void* 
is_valid(const void *vaddr)
{
	void *temp = pagedir_get_page(thread_current()->pagedir, vaddr);
	if (!is_user_vaddr(vaddr))
	{
		exit_proc(-1);
		return 0;
	}
	if (!temp)
	{
		exit_proc(-1);
		return 0;
	}
	return temp;
}
  
  thread_exit ();
}
