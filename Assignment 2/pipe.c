// Name- Vaibhav Jindal
// Roll number- 170775
#include<pipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>
/***********************************************************************
 * Use this function to allocate pipe info && Don't Modify below function
 ***********************************************************************/
struct pipe_info* alloc_pipe_info()
{
    struct pipe_info *pipe = (struct pipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);
    pipe ->pipe_buff = buffer;
    return pipe;
}


void free_pipe_info(struct pipe_info *p_info)
{
    if(p_info)
    {
        os_page_free(OS_DS_REG ,p_info->pipe_buff);
        os_page_free(OS_DS_REG ,p_info);
    }
}
/*************************************************************************/
/*************************************************************************/


int pipe_read(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Read the contect from buff (pipe_info -> pipe_buff) and write to the buff(argument 2);
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code 
    */
    if(count>PIPE_MAX_SIZE || count < 0) return -EINVAL;
    if(!((filep->mode)& O_READ)) return -EINVAL;

    int write_pos = filep->pipe->write_pos;
    int read_pos = filep->pipe->read_pos;
    
    if((write_pos-read_pos)<count){//dont have count number of chars in pipe_buff
        return -EINVAL;
    }

    else{
        int temp = memcpy(buff,(filep->pipe->pipe_buff)+read_pos,count);
        int temp2 = count;
        filep->pipe->read_pos = 0;
        int j=0;

        //shifting the left out part of pipe_buff to index 0
        for(int i=count;i<write_pos;i++){
            (filep->pipe->pipe_buff)[j]=(filep->pipe->pipe_buff)[i];
            j++;
        }
        filep->pipe->write_pos = j;
        return temp2;       
    }
}


int pipe_write(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Write the contect from   the buff(argument 2);  and write to buff(pipe_info -> pipe_buff)
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code 
    */
    if(count>PIPE_MAX_SIZE || count < 0) return -EINVAL;
    if(!((filep->mode)& O_WRITE)) return -EINVAL;
    
    int write_pos = filep->pipe->write_pos;
    
    if(write_pos+count<=4096){//when write is valid
        int temp = memcpy(filep->pipe->pipe_buff+write_pos,buff,count);
        filep->pipe->write_pos+=count;
        return count;
    }
    else{//invalid write
        return -ENOMEM;
    }
}

int create_pipe(struct exec_context *current, int *fd)
{
    /**
    *  TODO:: Implementation of Pipe Create
    *  Create file struct by invoking the alloc_file() function, 
    *  Create pipe_info struct by invoking the alloc_pipe_info() function
    *  fill the valid file descriptor in *fd param
    *  Incase of Error return valid Error code 
    */
    
    struct file* read_file = alloc_file();
    struct file* write_file = alloc_file();

    //this will be shared by both read_file and write_file
    struct pipe_info* p_info = alloc_pipe_info();
    p_info->read_pos = 0;
    p_info->write_pos = 0;
    p_info->is_ropen = 1;
    p_info->is_wopen = 1;

    //read_file params
    read_file->type = PIPE;
    read_file->mode = O_READ;
    read_file->offp = 0;
    read_file->ref_count = 1;
    read_file->inode = NULL;
    read_file->fops->read = pipe_read ;
    read_file->fops->write = pipe_write;
    read_file->fops->lseek = NULL;
    read_file->fops->close = generic_close;
    read_file->pipe = p_info;

    //write_file params
    write_file->type = PIPE;
    write_file->mode = O_WRITE;
    write_file->offp = 0;
    write_file->ref_count = 1;
    write_file->inode = NULL;
    write_file->fops->read = pipe_read ;
    write_file->fops->write = pipe_write;
    write_file->fops->lseek = NULL;
    write_file->fops->close = generic_close;
    write_file->pipe = p_info;

    //assigning fd to read_file
    int i;
    for(i=0;i<MAX_OPEN_FILES;i++){
      if((current->files)[i]==NULL){
        (current->files)[i] = read_file;
        fd[0] = i;
        break;
      }
    }
    if(i>=MAX_OPEN_FILES) return -EOTHERS;
    
    //assigning fd to write_file
    int j;
    for(j=i+1;i<MAX_OPEN_FILES;j++){
      if((current->files)[j]==NULL){
        (current->files)[j] = write_file;
        fd[1] = j;
        break;
      }
    }
    if(j>=MAX_OPEN_FILES) return -EOTHERS;
 
    return 0;
}

