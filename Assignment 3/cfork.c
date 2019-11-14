//Name- Vaibhav Jindal, Roll Number - 170775 

#include <cfork.h>
#include <page.h>
#include <mmap.h>

//my function to change prots
u32 my_change_protec_physical_page2(unsigned long base, u64 address, u32 access_flags)
{
   void *os_addr;
   u64 pfn;
   unsigned long *ptep  = (unsigned long *)base + ((address & PGD_MASK) >> PGD_SHIFT);    
   if(!*ptep)
   {
      pfn = os_pfn_alloc(OS_PT_REG);
      *ptep = (pfn << PAGE_SHIFT) | 0x7; 
      os_addr = osmap(pfn);
      bzero((char *)os_addr, PAGE_SIZE);
   }else 
   {
      os_addr = (void *) ((*ptep) & FLAG_MASK);
   }
   ptep = (unsigned long *)os_addr + ((address & PUD_MASK) >> PUD_SHIFT); 
   if(!*ptep)
   {
      pfn = os_pfn_alloc(OS_PT_REG);
      *ptep = (pfn << PAGE_SHIFT) | 0x7; 
      os_addr = osmap(pfn);
      bzero((char *)os_addr, PAGE_SIZE);
   } else
   {
      os_addr = (void *) ((*ptep) & FLAG_MASK);
   }
   ptep = (unsigned long *)os_addr + ((address & PMD_MASK) >> PMD_SHIFT); 
   if(!*ptep){
      pfn = os_pfn_alloc(OS_PT_REG);
      *ptep = (pfn << PAGE_SHIFT) | 0x7; 
      os_addr = osmap(pfn);
      bzero((char *)os_addr, PAGE_SIZE);
   } else
   {
      os_addr = (void *) ((*ptep) & FLAG_MASK);
   }
   ptep = (unsigned long *)os_addr + ((address & PTE_MASK) >> PTE_SHIFT); 
   if(*ptep!=0){
   	if(access_flags & PROT_WRITE){
   		*ptep |= 0x2;
   	}
   	else{
   		*ptep &= 0xFFFFFFFFFFFFFFFD;
   	}
   }
   return 1;    
}



/* You need to implement cfork_copy_mm which will be called from do_cfork in entry.c. Don't remove copy_os_pts()*/
void cfork_copy_mm(struct exec_context *child, struct exec_context *parent ){

   u64 vaddr; 
   struct mm_segment *seg;
   child->pgd = os_pfn_alloc(OS_PT_REG);

   u64 os_addr = (unsigned long)osmap(child->pgd);
   bzero((char *)os_addr, PAGE_SIZE);
   
   
   //CODE segment
   seg = &parent->mms[MM_SEG_CODE];
   for(vaddr = seg->start; vaddr < seg->next_free; vaddr += PAGE_SIZE){
      	u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
		*parent_pte = ((*parent_pte | 0x2)^(0x2));      	
      	if(parent_pte){
      		install_ptable((u64)osmap(child->pgd), seg, vaddr, (*parent_pte & FLAG_MASK) >> PAGE_SHIFT);
      		increment_pfn_info_refcount(get_pfn_info((*parent_pte & FLAG_MASK) >> PAGE_SHIFT));
      		u64 *child_pte = get_user_pte(child,vaddr,0);
      		*child_pte = *parent_pte;
      	}
    }


   //RODATA segment
   seg = &parent->mms[MM_SEG_RODATA];
   for(vaddr = seg->start; vaddr < seg->next_free; vaddr += PAGE_SIZE){
      	u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
		*parent_pte = ((*parent_pte | 0x2)^(0x2));      	
      	if(parent_pte){
      		install_ptable((u64)osmap(child->pgd), seg, vaddr, (*parent_pte & FLAG_MASK) >> PAGE_SHIFT);
      		increment_pfn_info_refcount(get_pfn_info((*parent_pte & FLAG_MASK) >> PAGE_SHIFT));
      		u64 *child_pte = get_user_pte(child,vaddr,0);
      		*child_pte = *parent_pte;
      	}
    } 

   //DATA segment
   seg = &parent->mms[MM_SEG_DATA];
   for(vaddr = seg->start; vaddr < seg->next_free; vaddr += PAGE_SIZE){
      	u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
		*parent_pte = ((*parent_pte | 0x2)^(0x2));      	
      	if(parent_pte){
      		install_ptable((u64)osmap(child->pgd), seg, vaddr, (*parent_pte & FLAG_MASK) >> PAGE_SHIFT);
      		increment_pfn_info_refcount(get_pfn_info((*parent_pte & FLAG_MASK) >> PAGE_SHIFT));
      		u64 *child_pte = get_user_pte(child,vaddr,0);
      		*child_pte = *parent_pte;
      	}
    }    


  
  	//STACK segment
	seg = &parent->mms[MM_SEG_STACK];
    for(vaddr = seg->end - PAGE_SIZE; vaddr >= seg->next_free; vaddr -= PAGE_SIZE){
      	u64 *parent_pte =  get_user_pte(parent, vaddr, 0); 
     	if(parent_pte){
           	u64 pfn = install_ptable((u64)os_addr, seg, vaddr, 0);  //Returns the blank page  
           	pfn = (u64)osmap(pfn);
           	memcpy((char *)pfn, (char *)(*parent_pte & FLAG_MASK), PAGE_SIZE); 
      	}
  	}


    //vm area copy
  	struct vm_area *parent_curr_node = parent->vm_area;
  	struct vm_area *prev = NULL;
  	int i=0;
  	while(parent_curr_node!=NULL){
  		struct vm_area *temp = alloc_vm_area();
  		temp->vm_start = parent_curr_node->vm_start;
  		temp->vm_end = parent_curr_node->vm_end;
  		temp->access_flags = parent_curr_node->access_flags;

  		u64 temp_addr = temp->vm_start;
		while(temp_addr < temp->vm_end){
			u32 a = my_change_protec_physical_page2((unsigned long)osmap(parent->pgd),temp_addr,PROT_READ);
			u64 *parent_pte = get_user_pte(parent,temp_addr,0);
			*parent_pte = ((*parent_pte | 0x2)^(0x2));
			if(parent_pte){
      		 map_physical_page((u64)osmap(child->pgd), temp_addr, PROT_READ,(*parent_pte & FLAG_MASK) >> PAGE_SHIFT);
      			increment_pfn_info_refcount(get_pfn_info((*parent_pte & FLAG_MASK) >> PAGE_SHIFT));
      			u64 *child_pte = get_user_pte(child,temp_addr,0);
      			*child_pte = *parent_pte;
      		}
			temp_addr+=PAGE_SIZE;
		}
  		parent_curr_node = parent_curr_node->vm_next;
  		if(i==0){
  			child->vm_area = temp;
  			i++;
  			prev = temp;
  		}
  		else{
  			prev->vm_next = temp;
  			prev = temp;
  		}
  	}


    copy_os_pts(parent->pgd, child->pgd);
    return;
    
}

/* You need to implement cfork_copy_mm which will be called from do_vfork in entry.c.*/
void vfork_copy_mm(struct exec_context *child, struct exec_context *parent ){

	parent->state = WAITING;
	struct mm_segment *seg = &(parent->mms[MM_SEG_STACK]);
	struct mm_segment *child_seg = &(child->mms[MM_SEG_STACK]);

  //setting relevant variables
	unsigned long difference = (seg->end - seg->next_free);
	(child->regs).rbp -= difference;
	(child->regs).entry_rsp -= difference;

	child_seg->next_free = seg->next_free-difference;
	child_seg->end = seg->next_free;

  //copying stack
	u64 vaddr;
	for(vaddr = seg->end - PAGE_SIZE; vaddr >= seg->next_free; vaddr -= PAGE_SIZE){
      	u64 *parent_pte =  get_user_pte(parent, vaddr, 0); 
     	if(parent_pte){
           	u64 pfn = install_ptable((u64)osmap(child->pgd), child_seg, vaddr-difference, 0);  //Returns the blank page  
           	pfn = (u64)osmap(pfn);
           	memcpy((char *)pfn, (char *)(*parent_pte & FLAG_MASK), PAGE_SIZE); 
      	}
  	}

    return;
    
}

/*You need to implement handle_cow_fault which will be called from do_page_fault 
incase of a copy-on-write fault

* For valid acess. Map the physical page 
 * Return 1
 * 
 * For invalid access,
 * Return -1. 
*/

int handle_cow_fault(struct exec_context *current, u64 cr2){


	u64 vaddr; 
  struct mm_segment *seg;

   	//CODE Segment
   	seg = &(current->mms[MM_SEG_CODE]);
   	if(cr2 >= seg->start && cr2 < seg->next_free){
   		if(seg->access_flags & PROT_WRITE){
          u64 *pte = get_user_pte(current, cr2,0);
          u64 old_pte = *pte; 
          u32 new_pfn = os_pfn_alloc(USER_REG);
          memcpy(osmap(new_pfn), osmap((*pte >> PAGE_SHIFT) & 0xFFFFFFFF), 4096);
          decrement_pfn_info_refcount(get_pfn_info((*pte & FLAG_MASK) >> PAGE_SHIFT));
          *pte = ((u64)new_pfn << PAGE_SHIFT) | 0x7; 
          if(get_pfn_info_refcount(get_pfn_info((old_pte & FLAG_MASK) >> PAGE_SHIFT)) == 0){
            os_pfn_free(USER_REG,(old_pte >> PAGE_SHIFT) & 0xFFFFFFFF);
          }
          asm volatile ("invlpg (%0);" 
                                :: "r"(cr2) 
                                : "memory");
          return 1;
   		}
   		else return -1;
   	}

    //RODATA Segment
   	seg = &(current->mms[MM_SEG_RODATA]);
   	if(cr2 >= seg->start && cr2 < seg->next_free){
   		if(seg->access_flags & PROT_WRITE){
          u64 *pte = get_user_pte(current, cr2,0);
          u64 old_pte = *pte; 
          u32 new_pfn = os_pfn_alloc(USER_REG);
          memcpy(osmap(new_pfn), osmap((*pte >> PAGE_SHIFT) & 0xFFFFFFFF), 4096);
          decrement_pfn_info_refcount(get_pfn_info((*pte & FLAG_MASK) >> PAGE_SHIFT));
          *pte = ((u64)new_pfn << PAGE_SHIFT) | 0x7; 
          if(get_pfn_info_refcount(get_pfn_info((old_pte & FLAG_MASK) >> PAGE_SHIFT)) == 0){
            os_pfn_free(USER_REG,(old_pte >> PAGE_SHIFT) & 0xFFFFFFFF);
          }
          asm volatile ("invlpg (%0);" 
                                :: "r"(cr2) 
                                : "memory");
          return 1;
   		}
   		else return -1;
   	}

    //DATA Segment
   	seg = &(current->mms[MM_SEG_DATA]);
   	if(cr2 >= seg->start && cr2 < seg->next_free){
   		if(seg->access_flags & PROT_WRITE){
          u64 *pte = get_user_pte(current, cr2,0);
          u64 old_pte = *pte; 
          u32 new_pfn = os_pfn_alloc(USER_REG);
          memcpy(osmap(new_pfn), osmap((*pte >> PAGE_SHIFT) & 0xFFFFFFFF), 4096);
          decrement_pfn_info_refcount(get_pfn_info((*pte & FLAG_MASK) >> PAGE_SHIFT));
          *pte = ((u64)new_pfn << PAGE_SHIFT) | 0x7; 
          if(get_pfn_info_refcount(get_pfn_info((old_pte & FLAG_MASK) >> PAGE_SHIFT)) == 0){
            os_pfn_free(USER_REG,(old_pte >> PAGE_SHIFT) & 0xFFFFFFFF);
          }
          asm volatile ("invlpg (%0);" 
                                :: "r"(cr2) 
                                : "memory");
          return 1;
   		}
   		else return -1;
   	}

    //STACK Segment
   	seg = &(current->mms[MM_SEG_STACK]);
   	if(cr2 >= seg->next_free && cr2 < seg->end ){
   		if(seg->access_flags & PROT_WRITE){
   			  u64 *pte = get_user_pte(current, cr2,0);
          u64 old_pte = *pte; 
          u32 new_pfn = os_pfn_alloc(USER_REG);
          memcpy(osmap(new_pfn), osmap((*pte >> PAGE_SHIFT) & 0xFFFFFFFF), 4096);
          decrement_pfn_info_refcount(get_pfn_info((*pte & FLAG_MASK) >> PAGE_SHIFT));
          *pte = ((u64)new_pfn << PAGE_SHIFT) | 0x7; 
          if(get_pfn_info_refcount(get_pfn_info((old_pte & FLAG_MASK) >> PAGE_SHIFT)) == 0){
            os_pfn_free(USER_REG,(old_pte >> PAGE_SHIFT) & 0xFFFFFFFF);
          }
          asm volatile ("invlpg (%0);" 
                                :: "r"(cr2) 
                                : "memory");
          return 1;
   		}
   		else return -1;
   	}


    //VM AREA 
   	struct vm_area* curr_node = current->vm_area;
   	while(curr_node!=NULL){
   		if(cr2>=curr_node->vm_start && cr2 < curr_node->vm_end){
   			if(curr_node->access_flags & PROT_WRITE){            
          u64 *pte = get_user_pte(current, cr2,0);
          u64 old_pte = *pte; 
          u32 new_pfn = os_pfn_alloc(USER_REG);
          memcpy(osmap(new_pfn), osmap((*pte >> PAGE_SHIFT) & 0xFFFFFFFF), 4096);
          decrement_pfn_info_refcount(get_pfn_info((*pte & FLAG_MASK) >> PAGE_SHIFT));
          *pte = ((u64)new_pfn << PAGE_SHIFT) | 0x7; 
          if(get_pfn_info_refcount(get_pfn_info((old_pte & FLAG_MASK) >> PAGE_SHIFT)) == 0){
            os_pfn_free(USER_REG,(old_pte >> PAGE_SHIFT) & 0xFFFFFFFF);
          }
          asm volatile ("invlpg (%0);" 
                                :: "r"(cr2) 
                                : "memory");
          return 1;

   			}
   			else return -1;
   		}
   		curr_node=curr_node->vm_next;
   	}

    return -1;
}

/* You need to handle any specific exit case for vfork here, called from do_exit*/
void vfork_exit_handle(struct exec_context *ctx){
  struct exec_context* parent = get_ctx_by_pid(ctx->ppid);
  parent->state = READY;	
  return;
}