// Name- Vaibhav Jindal
// Roll number- 170775
#include<types.h>
#include<context.h>
#include<file.h>
#include<lib.h>
#include<serial.h>
#include<entry.h>
#include<memory.h>
#include<fs.h>
#include<kbd.h>
#include<pipe.h>


/************************************************************************************/
/***************************Do Not Modify below Functions****************************/
/************************************************************************************/
void free_file_object(struct file *filep)
{
    if(filep)
    {
       os_page_free(OS_DS_REG ,filep);
       stats->file_objects--;
    }
}

struct file *alloc_file()
{
  
  struct file *file = (struct file *) os_page_alloc(OS_DS_REG); 
  file->fops = (struct fileops *) (file + sizeof(struct file)); 
  bzero((char *)file->fops, sizeof(struct fileops));
  stats->file_objects++;
  return file; 
}

static int do_read_kbd(struct file* filep, char * buff, u32 count)
{
  kbd_read(buff);
  return 1;
}

static int do_write_console(struct file* filep, char * buff, u32 count)
{
  struct exec_context *current = get_current_ctx();
  return do_write(current, (u64)buff, (u64)count);
}

struct file *create_standard_IO(int type)
{
  struct file *filep = alloc_file();
  filep->type = type;
  if(type == STDIN)
     filep->mode = O_READ;
  else
      filep->mode = O_WRITE;
  if(type == STDIN){
        filep->fops->read = do_read_kbd;
  }else{
        filep->fops->write = do_write_console;
  }
  filep->fops->close = generic_close;
  filep->ref_count = 1;
  return filep;
}

int open_standard_IO(struct exec_context *ctx, int type)
{
   int fd = type;
   struct file *filep = ctx->files[type];
   if(!filep){
        filep = create_standard_IO(type);
   }else{
         filep->ref_count++;
         fd = 3;
         while(ctx->files[fd])
             fd++; 
   }
   ctx->files[fd] = filep;
   return fd;
}
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/



void do_file_fork(struct exec_context *child)
{
   /*TODO the child fds are a copy of the parent. Adjust the refcount*/

  //increase ref_count of each existing file_object by 1
  for(int i=0;i<MAX_OPEN_FILES;i++){
    if((child->files)[i]!=NULL){
      ((child->files)[i])->ref_count +=1;
    }
  }
 
}

void do_file_exit(struct exec_context *ctx)
{
   /*TODO the process is exiting. Adjust the ref_count
     of files*/

  //if file descriptor is not null, call generic close for that file
  for(int i=0;i<MAX_OPEN_FILES;i++){
    if((ctx->files)[i]!=NULL){
      long temp = generic_close((ctx->files)[i]);
    }
  } 
}

long generic_close(struct file *filep)
{
  /** TODO Implementation of close (pipe, file) based on the type 
   * Adjust the ref_count, free file object
   * Incase of Error return valid Error code 
   * 
   */

    if(filep==NULL) return -EINVAL;
    filep->ref_count -=1;//ref_count has to be decreased

    //adjusting is_ropen and is_wopen
    if(filep->type == PIPE){
      if(filep->mode==O_READ){
        filep->pipe->is_ropen=0;
      }
      else if(filep->mode==O_WRITE){
        filep->pipe->is_wopen=0;
      }
      else{
        return -EINVAL;
      }
    }

    //if ref_count==0, free file object
    if(filep->ref_count==0){
      if(filep->pipe){//free pipe info when both is_ropen and is_wopen are 0
        if(filep->pipe->is_wopen==0&&filep->pipe->is_ropen==0) free_pipe_info(filep->pipe);
      }
      free_file_object(filep);
    } 
    return 0;
}

static int do_read_regular(struct file *filep, char * buff, u32 count)
{
   /** TODO Implementation of File Read, 
    *  You should be reading the content from File using file system read function call and fill the buf
    *  Validate the permission, file existence, Max length etc
    *  Incase of Error return valid Error code 
    * */

    if(count>FILE_SIZE || count < 0) return -EINVAL;
    
    struct inode* inode = filep->inode;
    u32 offp = filep->offp;
    
    if(!(O_READ&filep->mode)) return -EACCES;//when O_READ is not present in mode
    if(offp > FILE_SIZE) return -EINVAL;
    if(offp+count > inode->file_size) return -EINVAL;

    int temp = inode->read(inode,buff,count,&offp);//calling inode->read function
    filep->offp += temp;//adjusting offp
    return temp;
}


static int do_write_regular(struct file *filep, char * buff, u32 count)
{
    /** TODO Implementation of File write, 
    *   You should be writing the content from buff to File by using File system write function
    *   Validate the permission, file existence, Max length etc
    *   Incase of Error return valid Error code 
    * */
    if(count>FILE_SIZE || count < 0) return -EINVAL;

    struct inode* inode = filep->inode;
    u32 offp = (filep->offp);

    if(!(O_WRITE&filep->mode)) return -EACCES;
    if(offp > FILE_SIZE) return -EINVAL;
    if(offp+count > FILE_SIZE) return -EINVAL;

    int temp = inode->write(inode,buff,count,&offp);//calling inode->write
    filep->offp += temp;//adjusting offp
    return temp;
}

static long do_lseek_regular(struct file *filep, long offset, int whence)
{
    /** TODO Implementation of lseek 
    *   Set, Adjust the ofset based on the whence
    *   Incase of Error return valid Error code 
    * */
    long return_val;
    unsigned int file_end = FILE_SIZE;

    //handling different values of whence
    if(whence==SEEK_SET){
      if(offset > file_end) return -EINVAL;
      filep->offp = offset;
    }
    else if(whence==SEEK_CUR){
      if((filep->offp)+offset > file_end) return -EINVAL;
      filep->offp += offset;
    }
    else if(whence==SEEK_END){
      if((filep->inode->file_size)+offset > file_end) return -EINVAL; 
      filep->offp = filep->inode->file_size + offset; 
    }
    else{
      return -EINVAL;
    }

    return_val=filep->offp;
    return return_val;
}

extern int do_regular_file_open(struct exec_context *ctx, char* filename, u64 flags, u64 mode)
{ 
  /**  TODO Implementation of file open, 
    *  You should be creating file(use the alloc_file function to creat file), 
    *  To create or Get inode use File system function calls, 
    *  Handle mode and flags 
    *  Validate file existence, Max File count is 32, Max Size is 4KB, etc
    *  Incase of Error return valid Error code 
    * */
    //catching errors in filename
    if(filename==NULL) return -EINVAL;
    if(strlen(filename)>256) return -EINVAL;

    struct inode *file_inode = lookup_inode(filename);
    struct file* f_object;

    //when file doesn't exist
    if(file_inode == NULL){
      if(flags & O_CREAT){//O_CREAT is necessary in this case
        file_inode = create_inode(filename,mode);
        if(file_inode==NULL) return -EOTHERS;

        //creating a file object
        f_object = alloc_file();
        f_object->inode = file_inode;
        f_object->type = REGULAR;
        f_object->mode = mode;
        f_object->offp = 0;
        f_object->ref_count = 1; 
        f_object->pipe = NULL;
        f_object->fops->read = do_read_regular;
        f_object->fops->write = do_write_regular;
        f_object->fops->lseek = do_lseek_regular;
        f_object->fops->close = generic_close;
      }
      else{
        return -EINVAL;
      }
    }
    else{//when file exists

      u64 mode_inode = file_inode->mode;
      //handling error cases
      if( (O_READ & mode) & (!(O_READ & mode_inode)) ) return -EACCES;
      if( (O_WRITE & mode) & (!(O_WRITE & mode_inode)) ) return -EACCES;
      if( (O_EXEC & mode) & (!(O_EXEC & mode_inode)) ) return -EACCES;
      if(flags&O_CREAT) return -EINVAL;

      //creating file object
      f_object = alloc_file();
      f_object->inode = file_inode;
      f_object->type = REGULAR;
      f_object->mode = mode;
      f_object->offp = 0;
      f_object->ref_count = 1; 
      f_object->pipe = NULL;
      f_object->fops->read = do_read_regular;
      f_object->fops->write = do_write_regular;
      f_object->fops->lseek = do_lseek_regular;
      f_object->fops->close = generic_close;
    }
    
    //search for null file descriptor
    int i;
    for(i=3;i<MAX_OPEN_FILES;i++){
      if((ctx->files)[i]==NULL){
        (ctx->files)[i] = f_object;
        break;
      }
    }
    int ret_fd = -EINVAL;
    if(i<MAX_OPEN_FILES) ret_fd = i; 
    return ret_fd;
}

int fd_dup(struct exec_context *current, int oldfd)
{
     /** TODO Implementation of dup 
      *  Read the man page of dup and implement accordingly 
      *  return the file descriptor,
      *  Incase of Error return valid Error code 
      * */
    
    struct file* f_object = (current->files)[oldfd];
    if(f_object == NULL) return -EINVAL;
    int i;

    //search for a null file descriptor
    for(i=0;i<MAX_OPEN_FILES;i++){
      if((current->files)[i]==NULL){
        (current->files)[i] = f_object;
        break;
      }
    }
    int ret_fd = -EOTHERS;
    if(i<MAX_OPEN_FILES) ret_fd = i;
    return ret_fd;
}


int fd_dup2(struct exec_context *current, int oldfd, int newfd)
{
  /** TODO Implementation of the dup2 
    *  Read the man page of dup2 and implement accordingly 
    *  return the file descriptor,
    *  Incase of Error return valid Error code 
    * */

    if(newfd>=MAX_OPEN_FILES) return -EINVAL;
    if(oldfd>=MAX_OPEN_FILES) return -EINVAL;

    //close newfd if it exists
    struct file* newfd_fobject = (current->files)[newfd];
    free_file_object(newfd_fobject);
    
    
    struct file* f_object = (current->files)[oldfd];
    if(f_object == NULL) return -EINVAL;

    (current->files)[newfd] = f_object;
    int ret_fd = newfd;
    return ret_fd;
}
