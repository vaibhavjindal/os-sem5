//Name- Vaibhav Jindal, Roll Number - 170775 
#include<types.h>
#include<mmap.h>

/**
 * Function will invoked whenever there is page fault. (Lazy allocation)
 * 
 * For valid acess. Map the physical page 
 * Return 1
 * 
 * For invalid access,
 * Return -1. 
 */

//helper function to return next multiple of page size
u64 up_by_page_size(u64 num){
	if(num%PAGE_SIZE==0) return num;
	else{
		u64 div = num/PAGE_SIZE;
		return (div+1)*PAGE_SIZE;
	}
}

//last multiple of page size
u64 down_by_page_size(u64 num){
	if(num%PAGE_SIZE==0) return num;
	else{
		u64 div = num/PAGE_SIZE;
		return (div)*PAGE_SIZE;
	}
}

//my copy of map_physical page
u32 my_map_physical_page(unsigned long base, u64 address, u32 access_flags, u32 upfn)
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
   if(!upfn)
      upfn = os_pfn_alloc(USER_REG);
   *ptep = ((u64)upfn << PAGE_SHIFT) | 0x5;
   if(access_flags & MM_WR)
      *ptep |= 0x2;
   return upfn;    
}

//modified map_physical_page
u32 my_unmap_physical_page(unsigned long base, u64 address)
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
   if(*ptep == 0) return 1;

   u8 refcount = get_pfn_info_refcount(get_pfn_info((*ptep & FLAG_MASK) >> PAGE_SHIFT));
   if(refcount > 1){
   	decrement_pfn_info_refcount(get_pfn_info((*ptep & FLAG_MASK) >> PAGE_SHIFT));
   }
   else{
   	os_pfn_free(USER_REG,(*ptep>>PAGE_SHIFT)&0xFFFFFFFF);
   	*ptep=0;
   }
   return 1;    
}

//modified map_physical_page
u32 my_change_protec_physical_page(unsigned long base, u64 address, u32 access_flags)
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


int vm_area_pagefault(struct exec_context *current, u64 addr, int error_code)
{
	struct vm_area *curr_node = current->vm_area;
	while(curr_node!=NULL){
		if(addr>=curr_node->vm_start && addr < curr_node->vm_end){
			if(error_code==4){//READ FAULT
				if(!(curr_node->access_flags & PROT_READ)) return -1;
				u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),addr,curr_node->access_flags,0);
				return 1;
			}
			else if(error_code==6){//WRITE FAULT
				if(!(curr_node->access_flags & PROT_WRITE)) return -1;
				u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),addr,curr_node->access_flags,0);
				return 1;
			}
			else{
				return -1;
			}
		}
		else{
			curr_node=curr_node->vm_next;
		}
	}

    int fault_fixed = -1;
    return fault_fixed;
}

/**
 * mprotect System call Implementation.
 */
int vm_area_mprotect(struct exec_context *current, u64 addr, int length, int prot)
{
	u64 addr_orig  = addr;
	int length_up = up_by_page_size(length);
	struct vm_area *curr_node = current->vm_area;
	int flag=0;

	//to check discontinuos area
	while(curr_node!=NULL){
		if(addr>= curr_node->vm_start && addr < curr_node->vm_end){
			if(addr + length_up <= curr_node->vm_end){
				flag=1;
				break;
			}
			else{
				if(curr_node->vm_next==NULL || curr_node->vm_next->vm_start!= curr_node->vm_end){
					flag=0;
					break;
				}
				else{
					length_up -= (curr_node->vm_end-addr);
					addr = curr_node->vm_end;
					curr_node=curr_node->vm_next;
				}
			}
		}
		else{
			curr_node=curr_node->vm_next;
		}
	}
	if(flag==0) return -1;

	addr = addr_orig;
	length_up = up_by_page_size(length);

	u64 temp = addr;
	while(temp < addr+length_up){
		asm volatile ("invlpg (%0);" 
	                    :: "r"(temp) 
	                    : "memory");   // Flush TLB
		u32 a = my_change_protec_physical_page((unsigned long)osmap(current->pgd),temp,prot);
		temp+=PAGE_SIZE;
	}

	curr_node = current->vm_area;
	u64 last_end_pos = MMAP_AREA_START;
	struct vm_area *prev = NULL;
	while(curr_node!=NULL){
		
		//calculating vm area count
		int vm_area_count = 0;
		struct vm_area *curr_node2 = current->vm_area;
		while(curr_node2!=NULL){
			vm_area_count++;
			curr_node2=curr_node2->vm_next;
		}

		if(addr >= curr_node->vm_start && addr < curr_node->vm_end){//address lies in curr_node
			if(curr_node->vm_start == addr && curr_node->vm_end == addr+length_up){
				if(prev==NULL){
					if(curr_node->vm_next==NULL){
						curr_node->access_flags=prot;
						return 0;
					}
					else{
						if(curr_node->vm_next->access_flags==prot && curr_node->vm_end == curr_node->vm_next->vm_start){
							curr_node->vm_next->vm_start = curr_node->vm_start;
							current->vm_area = curr_node->vm_next;
							dealloc_vm_area(curr_node);
							return 0;
						}
						else{
							curr_node->access_flags = prot;
							return 0;
						}
					}
				}
				else{//prev not NULL
					if(curr_node->vm_next!=NULL){
						if(curr_node->vm_start == prev->vm_end && prot == prev->access_flags){
							if(curr_node->vm_end == curr_node->vm_next->vm_start && prot == curr_node->vm_next->access_flags){
								prev->vm_end = curr_node->vm_next->vm_end;
								prev->vm_next = curr_node->vm_next->vm_next;
								dealloc_vm_area(curr_node->vm_next);
								dealloc_vm_area(curr_node);
								return 0;
							}
							else{
								prev->vm_end = curr_node->vm_end;
								prev->vm_next = curr_node->vm_next;
								dealloc_vm_area(curr_node);
								return 0;
							}
						}
						else{
							if(curr_node->vm_end == curr_node->vm_next->vm_start && prot == curr_node->vm_next->access_flags){
								curr_node->vm_end = curr_node->vm_next->vm_end;
								curr_node->access_flags = prot;
								struct vm_area* temp= curr_node->vm_next->vm_next;
								dealloc_vm_area(curr_node->vm_next);
								curr_node->vm_next= temp;
								return 0;
							}
							else{
								curr_node->access_flags=prot;
								return 0;
							}
						}
					}
					else{
						if(curr_node->vm_start == prev->vm_end && prot == prev->access_flags){
							prev->vm_end = curr_node->vm_end;
							prev->vm_next = curr_node->vm_next;
							dealloc_vm_area(curr_node);
							return 0;
						}
						else{
							curr_node->access_flags = prot;
							return 0;
						}
					}
				}
			}
			else if(curr_node->vm_start != addr && curr_node->vm_end == addr+length_up){
				if(prot == curr_node->access_flags) return 0;
				else{
					if(curr_node->vm_next!=NULL && curr_node->vm_next->vm_start == curr_node->vm_end && curr_node->vm_next->access_flags == prot){
						curr_node->vm_end = addr;
						curr_node->vm_next->vm_start = addr;
						return 0;
					}
					else{
						if(vm_area_count==128) return -1;
						curr_node->vm_end = addr;
						struct vm_area* new_node = alloc_vm_area();
						new_node->vm_start=addr;
						new_node->vm_end = addr+length_up;
						new_node->access_flags = prot;
						new_node->vm_next = curr_node->vm_next;
						curr_node->vm_next = new_node;
						return 0;
					}
				}
			}
			else if(curr_node->vm_start == addr && curr_node->vm_end > addr+length_up){
				if(prot == curr_node->access_flags) return 0;
				else{
					if(prev!=NULL&&prev->vm_end==curr_node->vm_start&&prev->access_flags==prot){
						prev->vm_end = addr+length_up;
						curr_node->vm_start += length_up;
						return 0; 
					}
					else{
						if(vm_area_count==128) return -1;
						struct vm_area* new_node = alloc_vm_area();
						new_node->vm_start = addr;
						new_node->vm_end = addr+length_up;
						new_node->access_flags = prot;
						new_node->vm_next = curr_node;
						curr_node->vm_start = addr+length_up;
						if(prev==NULL) current->vm_area = new_node;
						else prev->vm_next = new_node;
						return 0;
 					}
				}
			}
			else if(curr_node->vm_start == addr && curr_node->vm_end < addr+length_up){
				if(curr_node->vm_next==NULL || curr_node->vm_next->vm_start!=curr_node->vm_end) return -1;
				if(prot!=curr_node->vm_next->access_flags){
					if(prev!=NULL&&prev->vm_end==curr_node->vm_start&&prev->access_flags==prot){
						prev->vm_end = curr_node->vm_end;
						prev->vm_next = curr_node->vm_next;
						length_up -= (curr_node->vm_next->vm_start-addr);
						addr =  curr_node->vm_next->vm_start;
						dealloc_vm_area(curr_node);
						curr_node = prev->vm_next;
					}
					else{
						curr_node->access_flags = prot;
						length_up -= (curr_node->vm_next->vm_start-addr);
						addr =  curr_node->vm_next->vm_start;
						prev = curr_node;
						curr_node = curr_node->vm_next;
					}
				}
				else{
					length_up -= (curr_node->vm_next->vm_start-addr);
					addr =  curr_node->vm_next->vm_start;
					curr_node->vm_next->vm_start=curr_node->vm_start;
					prev->vm_next = curr_node->vm_next;
					dealloc_vm_area(curr_node);
					curr_node = prev->vm_next;
				}
			}
			else if(curr_node->vm_start != addr && curr_node->vm_end < addr+length_up){
				if(curr_node->vm_next==NULL || curr_node->vm_next->vm_start!=curr_node->vm_end) return -1;
				if(prot==curr_node->access_flags){
					addr = curr_node->vm_next->vm_start;
					length_up -= (curr_node->vm_next->vm_start-addr);
					prev = curr_node;
					curr_node = curr_node->vm_next;
				}
				else{
					if(prot==curr_node->vm_next->access_flags){
						length_up -= (curr_node->vm_next->vm_start-addr);
						curr_node->vm_end = addr;
						curr_node->vm_next->vm_start = addr;
						prev = curr_node;
						curr_node = curr_node->vm_next;
					}
					else{
						if(vm_area_count==128) return -1;
						struct vm_area* new_node = alloc_vm_area();
						new_node->vm_start = addr;
						new_node->vm_end = curr_node->vm_end;
						new_node->access_flags = prot;
						new_node->vm_next = curr_node->vm_next;
						curr_node->vm_end = addr;
						curr_node->vm_next=new_node;
						length_up = (new_node->vm_end - addr);
						addr = new_node->vm_end;
						curr_node = new_node->vm_next;
						prev = new_node;
					}
				}
			}
			else{
				if(prot == curr_node->access_flags) return 0;
				if(vm_area_count==128) return -1;
				struct vm_area* new_node = alloc_vm_area();
				new_node->vm_start = curr_node->vm_start;
				new_node->access_flags = curr_node->access_flags;
				new_node->vm_end = addr;
				struct vm_area* new_node2 = alloc_vm_area();
				new_node2->vm_start=addr;
				new_node2->vm_end=addr+length_up;
				new_node2->access_flags=prot;
				new_node->vm_next=new_node2;
				new_node2->vm_next = curr_node;
				curr_node->vm_start = addr+length_up;
				if(prev==NULL) current->vm_area = new_node;
				else prev->vm_next = new_node;
				return 0;
			}
		}
		else{//addr not in curr_node
			prev = curr_node;
			last_end_pos = curr_node->vm_end;
			curr_node = curr_node->vm_next;	
		}
	}

    int isValid = -1;
    return isValid;
}
/**
 * mmap system call implementation.
 */


long my_vm_area_map(struct exec_context *current, u64 addr, int length, int prot, int flags)
{
	int is_PROT_READ = PROT_READ & prot;
	int is_PROT_WRITE = PROT_WRITE & prot;
	int is_MAP_FIXED = MAP_FIXED & flags;
	int is_MAP_POPULATE = MAP_POPULATE & flags;
	
	int length_up = up_by_page_size(length);

	//if addr is NULL
	if(addr==0){
		if(current->vm_area == NULL){
			if(length_up > (MMAP_AREA_END - MMAP_AREA_START)){//too large length
				return -1;
			}
			else{
				current->vm_area = alloc_vm_area();
				current->vm_area->vm_start = MMAP_AREA_START;				
				current->vm_area->vm_end = MMAP_AREA_START+length_up;
				current->vm_area->access_flags = prot;
				//code region begin for MAP_POULATE
				if(is_MAP_POPULATE){
					u64 temp_addr = MMAP_AREA_START;
					while(temp_addr<MMAP_AREA_START+length_up){
						u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
						temp_addr+=PAGE_SIZE;
					}
				}
				//code region end for MAP_POPULATE
				return MMAP_AREA_START;
			}
		}
		else{//current->vm_area is not null
			struct vm_area *curr_node = current->vm_area;
			u64 last_end_pos = MMAP_AREA_START;
			struct vm_area *prev = NULL;
			while(curr_node!=NULL){
				if((curr_node->vm_start - last_end_pos) > length_up){
					if(prev == NULL){
						struct vm_area* new_node = alloc_vm_area();
						new_node->vm_start = MMAP_AREA_START;				
						new_node->vm_end = MMAP_AREA_START+length_up;
						new_node->access_flags = prot;
						new_node->vm_next = curr_node;
						current->vm_area = new_node;
						//code region begin for MAP_POULATE
						if(is_MAP_POPULATE){
							u64 temp_addr = MMAP_AREA_START;
							while(temp_addr<MMAP_AREA_START+length_up){
								u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
								temp_addr+=PAGE_SIZE;
							}
						}
						//code region end for MAP_POPULATE
						return MMAP_AREA_START;
					}
					else{
						u32 prev_access_flags = prev->access_flags;
						if(prev_access_flags!=prot){
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = prev->vm_end;				
							new_node->vm_end = prev->vm_end+length_up;
							new_node->access_flags = prot;
							new_node->vm_next = curr_node;
							prev->vm_next = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = new_node->vm_start;
								while(temp_addr < new_node->vm_start+length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return new_node->vm_start;
						}
						else{
							prev->vm_end += length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = (prev->vm_end - length_up);
								while(temp_addr < (prev->vm_end - length_up) +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return (prev->vm_end - length_up);
						} 
					}
				}
				else if(curr_node->vm_start - last_end_pos == length_up){
					if(prev == NULL){
						if(curr_node->access_flags==prot){
							curr_node->vm_start = MMAP_AREA_START;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = MMAP_AREA_START;
								while(temp_addr < MMAP_AREA_START +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return MMAP_AREA_START;
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = MMAP_AREA_START;				
							new_node->vm_end = MMAP_AREA_START+length_up;
							new_node->access_flags = prot;
							new_node->vm_next = curr_node;
							current->vm_area = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = MMAP_AREA_START;
								while(temp_addr < MMAP_AREA_START +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return MMAP_AREA_START;
						}
					}
					else{
						u32 prev_access_flags = prev->access_flags;
						u32 curr_access_flags = curr_node->access_flags;
						if(prot!=prev_access_flags && prot!=curr_access_flags){
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = prev->vm_end;
							new_node->vm_end = prev->vm_end+length_up;
							new_node->access_flags = prot;
							new_node->vm_next = curr_node;
							prev->vm_next = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = new_node->vm_start;
								while(temp_addr < new_node->vm_start +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return new_node->vm_start;
						}
						else if(prot==prev_access_flags && prot!=curr_access_flags){
							prev->vm_end += length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = (prev->vm_end - length_up);
								while(temp_addr < (prev->vm_end - length_up) +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return (prev->vm_end - length_up);
						} 
						else if(prot!=prev_access_flags && prot==curr_access_flags){
							curr_node->vm_start -= length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = curr_node->vm_start;
								while(temp_addr < curr_node->vm_start +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return curr_node->vm_start;
						}
						else{
							unsigned long prev_previous_end = prev->vm_end;
							prev->vm_end = curr_node->vm_end;
							prev->vm_next = curr_node->vm_next;
							dealloc_vm_area(curr_node);
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = prev_previous_end;
								while(temp_addr < prev_previous_end +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return prev_previous_end;
						}
					}
				}
				else{
					prev = curr_node;
					last_end_pos = curr_node->vm_end;
					curr_node = curr_node->vm_next;
				}
			}
			if(prev->access_flags!=prot){
				struct vm_area* new_node = alloc_vm_area();
				new_node->vm_start = prev->vm_end;
				new_node->vm_end = prev->vm_end+length_up;
				new_node->access_flags = prot;
				prev->vm_next = new_node;
				//code region begin for MAP_POULATE
				if(is_MAP_POPULATE){
					u64 temp_addr = new_node->vm_start;
					while(temp_addr < new_node->vm_start +length_up){
						u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
						temp_addr+=PAGE_SIZE;
					}
				}
				//code region end for MAP_POPULATE
				return new_node->vm_start;
			}
			else{
				prev->vm_end += length_up;
				//code region begin for MAP_POULATE
				if(is_MAP_POPULATE){
					u64 temp_addr = (prev->vm_end - length_up);
					while(temp_addr < (prev->vm_end - length_up) +length_up){
						u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
						temp_addr+=PAGE_SIZE;
					}
				}
				//code region end for MAP_POPULATE
				return (prev->vm_end - length_up);
			}

		}
	}

	//if addr is not NULL
	else{
		if(addr<MMAP_AREA_START || addr >= MMAP_AREA_END) return -1;
		if(is_MAP_FIXED && down_by_page_size(addr)!= addr) return -1;
		else addr = down_by_page_size(addr);

		if(current->vm_area == NULL){
			if(addr+length_up > (MMAP_AREA_END - MMAP_AREA_START)){//too large length
				return -1;
			}
			else{
				current->vm_area = alloc_vm_area();
				current->vm_area->vm_start = addr;				
				current->vm_area->vm_end = addr+length_up;
				current->vm_area->access_flags = prot;
				//code region begin for MAP_POULATE
				if(is_MAP_POPULATE){
					u64 temp_addr = addr;
					while(temp_addr < addr +length_up){
						u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
						temp_addr+=PAGE_SIZE;
					}
				}
				//code region end for MAP_POPULATE
				return addr;
			}
		}

		else{
			struct vm_area *curr_node = current->vm_area;
			u64 last_end_pos = MMAP_AREA_START;
			struct vm_area *prev = NULL;
			int myflag=0;
			struct vm_area* my_prev=NULL;
			while(curr_node!=NULL){
				if(addr >= last_end_pos && addr+length_up < curr_node->vm_start){
					myflag=1;
					my_prev=prev;
					break;
				}
				else{
					prev = curr_node;
					last_end_pos = curr_node->vm_end;
					curr_node = curr_node->vm_next;
				}
			}
			if(addr >= prev->vm_end){
				if(addr+length_up<MMAP_AREA_END){
					myflag=1;
					my_prev = prev;
				}
			}

			if (myflag==0 && is_MAP_FIXED) return -1;
			if (myflag==1){
				if(my_prev==NULL){
					if(current->vm_area->vm_start == addr+length_up){
						if(prot==current->vm_area->access_flags){
							current->vm_area->vm_start = addr;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = addr;
							new_node->vm_end = addr+length_up;
							new_node->access_flags = prot;
							new_node->vm_next = current->vm_area;
							current->vm_area = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = new_node->vm_start;
								while(temp_addr < new_node->vm_start +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return new_node->vm_start;	
						}
					}
					else{
						struct vm_area* new_node = alloc_vm_area();
						new_node->vm_start = addr;
						new_node->vm_end = addr+length_up;
						new_node->access_flags = prot;
						new_node->vm_next = current->vm_area;
						current->vm_area = new_node;
						//code region begin for MAP_POULATE
						if(is_MAP_POPULATE){
							u64 temp_addr = new_node->vm_start;
							while(temp_addr < new_node->vm_start +length_up){
								u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
								temp_addr+=PAGE_SIZE;
							}
						}
						//code region end for MAP_POPULATE
						return new_node->vm_start;
					}
				}
				else if(my_prev->vm_next==NULL){
					if(my_prev->vm_end==addr){
						if(prot == my_prev->access_flags){
							if(my_prev->vm_end+length_up <= MMAP_AREA_END){
								my_prev->vm_end += length_up;
								//code region begin for MAP_POULATE
								if(is_MAP_POPULATE){
									u64 temp_addr = (my_prev->vm_end - length_up);
									while(temp_addr < (my_prev->vm_end - length_up) +length_up){
										u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
										temp_addr+=PAGE_SIZE;
									}
								}
								//code region end for MAP_POPULATE
								return (my_prev->vm_end - length_up);
							}
							else return -1; 
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = addr;
							new_node->vm_end = addr+length_up;
							new_node->access_flags = prot;
							my_prev->vm_next = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = new_node->vm_start;
								while(temp_addr < new_node->vm_start +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return new_node->vm_start;
						}
					}
					else{
						struct vm_area* new_node = alloc_vm_area();
						new_node->vm_start = addr;
						new_node->vm_end = addr+length_up;
						new_node->access_flags = prot;
						my_prev->vm_next = new_node;
						//code region begin for MAP_POULATE
						if(is_MAP_POPULATE){
							u64 temp_addr = new_node->vm_start;
							while(temp_addr < new_node->vm_start +length_up){
								u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
								temp_addr+=PAGE_SIZE;
							}
						}
						//code region end for MAP_POPULATE
						return new_node->vm_start;
					}
				}
				else{
					curr_node = my_prev->vm_next;
					if(addr == my_prev->vm_end && addr+length_up == curr_node->vm_start){
						if(prot==my_prev->access_flags && prot==curr_node->access_flags){
							my_prev->vm_end = curr_node->vm_end;
							my_prev->vm_next = curr_node->vm_next;
							dealloc_vm_area(curr_node);
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
						else if(prot!=my_prev->access_flags && prot==curr_node->access_flags){
							curr_node->vm_start = addr;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
						else if(prot==my_prev->access_flags && prot!=curr_node->access_flags){
							my_prev->vm_end = addr+length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = addr;
							new_node->vm_end = addr+length_up;
							new_node->access_flags = prot;
							new_node->vm_next = curr_node;
							my_prev->vm_next = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
					}
					else if(addr != my_prev->vm_end && addr+length_up == curr_node->vm_start){
						if(prot == curr_node->access_flags){
							curr_node->vm_start = addr;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr+length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = addr;
							new_node->vm_end = addr+length_up;
							new_node->access_flags = prot;
							new_node->vm_next = curr_node;
							my_prev->vm_next = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
					}
					else if(addr == my_prev->vm_end && addr+length_up != curr_node->vm_start){
						if(prot == my_prev->access_flags){
							my_prev->vm_end = addr+length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = addr;
							new_node->vm_end = addr+length_up;
							new_node->access_flags = prot;
							new_node->vm_next = curr_node;
							my_prev->vm_next = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = addr;
								while(temp_addr < addr +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return addr;
						}
					}
					else{
						struct vm_area* new_node = alloc_vm_area();
						new_node->vm_start = addr;
						new_node->vm_end = addr+length_up;
						new_node->access_flags = prot;
						new_node->vm_next = curr_node;
						my_prev->vm_next = new_node;
						//code region begin for MAP_POULATE
						if(is_MAP_POPULATE){
							u64 temp_addr = addr;
							while(temp_addr < addr +length_up){
								u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
								temp_addr+=PAGE_SIZE;
							}
						}
						//code region end for MAP_POPULATE
						return addr;
					}
				}
			}
			else{//myflag==0
				struct vm_area *curr_node = current->vm_area;
				u64 last_end_pos = MMAP_AREA_START;
				struct vm_area *prev = NULL;
				int myflag2=0;
				while(curr_node!=NULL){
					if(last_end_pos >= addr && (curr_node->vm_start-last_end_pos)>length_up){
						if(prot==prev->access_flags){
							prev->vm_end += length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = (prev->vm_end - length_up);
								while(temp_addr < (prev->vm_end - length_up) +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return (prev->vm_end - length_up) ; 
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start = last_end_pos;
							new_node->vm_end = last_end_pos + length_up;
							new_node->access_flags=prot;
							new_node->vm_next = curr_node;
							prev->vm_next = new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = last_end_pos;
								while(temp_addr < last_end_pos +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return last_end_pos;
						}
					}
					else if(last_end_pos >= addr && (curr_node->vm_start-last_end_pos)==length_up){
						if(prot==prev->access_flags && prot == curr_node->access_flags){
							prev->vm_end = curr_node->vm_end;
							prev->vm_next = curr_node->vm_next;
							dealloc_vm_area(curr_node);
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = last_end_pos;
								while(temp_addr < last_end_pos +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return last_end_pos;
						}
						else if(prot!=prev->access_flags && prot == curr_node->access_flags){
							curr_node->vm_start = last_end_pos;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = last_end_pos;
								while(temp_addr < last_end_pos +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return last_end_pos;
						}
						else if(prot==prev->access_flags && prot != curr_node->access_flags){
							prev->vm_end = last_end_pos + length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = last_end_pos;
								while(temp_addr < last_end_pos +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return last_end_pos;
						}
						else{
							struct vm_area* new_node = alloc_vm_area();
							new_node->vm_start= last_end_pos;
							new_node->vm_end = last_end_pos+length_up;
							new_node->access_flags=prot;
							new_node->vm_next=curr_node->vm_next;
							prev->vm_next=new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = last_end_pos;
								while(temp_addr < last_end_pos +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return last_end_pos;
						}
					}
					else{
						prev = curr_node;
						last_end_pos = curr_node->vm_end;
						curr_node = curr_node->vm_next;
					}
				}
				if(addr >= last_end_pos && last_end_pos+length_up<= MMAP_AREA_END){
					if(prev->access_flags==prot){
						prev->vm_end += length_up;
						//code region begin for MAP_POULATE
						if(is_MAP_POPULATE){
							u64 temp_addr = last_end_pos;
							while(temp_addr < last_end_pos +length_up){
								u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
								temp_addr+=PAGE_SIZE;
							}
						}
						//code region end for MAP_POPULATE
						return last_end_pos;
					}
					else{
						struct vm_area* new_node=alloc_vm_area();
						new_node->vm_start=last_end_pos;
						new_node->vm_end=last_end_pos+length_up;
						new_node->access_flags=prot;
						prev->vm_next=new_node;
						//code region begin for MAP_POULATE
						if(is_MAP_POPULATE){
							u64 temp_addr = last_end_pos;
							while(temp_addr < last_end_pos +length_up){
								u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
								temp_addr+=PAGE_SIZE;
							}
						}
						//code region end for MAP_POPULATE
						return last_end_pos;
					}
				}

				if(myflag2==0){
					struct vm_area *curr_node = current->vm_area;
					u64 last_end_pos = MMAP_AREA_START;
					struct vm_area *prev = NULL;
					while(curr_node!=NULL){
						if((curr_node->vm_start-last_end_pos)>length_up){
							if(prev!=NULL){
								if(prot==prev->access_flags){
									prev->vm_end += length_up;
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = (prev->vm_end - length_up);
										while(temp_addr < (prev->vm_end - length_up) +length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return (prev->vm_end - length_up) ; 
								}
								else{
									struct vm_area* new_node = alloc_vm_area();
									new_node->vm_start = last_end_pos;
									new_node->vm_end = last_end_pos + length_up;
									new_node->access_flags=prot;
									new_node->vm_next = curr_node;
									prev->vm_next = curr_node;
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = last_end_pos;
										while(temp_addr < last_end_pos +length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return last_end_pos;
								}
							}
							else{
								struct vm_area* new_node = alloc_vm_area();
								new_node->vm_start = last_end_pos;
								new_node->vm_end = last_end_pos + length_up;
								new_node->access_flags=prot;
								new_node->vm_next = curr_node;
								current->vm_area = new_node;
								//code region begin for MAP_POULATE
								if(is_MAP_POPULATE){
									u64 temp_addr = last_end_pos;
									while(temp_addr < last_end_pos +length_up){
										u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
										temp_addr+=PAGE_SIZE;
									}
								}
								//code region end for MAP_POPULATE
								return last_end_pos;	
							}
						}
						else if((curr_node->vm_start-last_end_pos)==length_up){
							if(prev!=NULL){
								if(prot==prev->access_flags && prot == curr_node->access_flags){
									prev->vm_end = curr_node->vm_end;
									prev->vm_next = curr_node->vm_next;
									dealloc_vm_area(curr_node);
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = last_end_pos;
										while(temp_addr < last_end_pos +length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return last_end_pos;
								}
								else if(prot!=prev->access_flags && prot == curr_node->access_flags){
									curr_node->vm_start = last_end_pos;
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = last_end_pos;
										while(temp_addr < last_end_pos +length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return last_end_pos;
								}
								else if(prot==prev->access_flags && prot != curr_node->access_flags){
									prev->vm_end = last_end_pos + length_up;
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = last_end_pos;
										while(temp_addr < last_end_pos +length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return last_end_pos;
								}
								else{
									struct vm_area* new_node = alloc_vm_area();
									new_node->vm_start= last_end_pos;
									new_node->vm_end = last_end_pos+length_up;
									new_node->access_flags=prot;
									new_node->vm_next=curr_node->vm_next;
									prev->vm_next=new_node;
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = last_end_pos;
										while(temp_addr < last_end_pos+length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return last_end_pos;
								}
							}
							else{
								if(prot==curr_node->access_flags){
									curr_node->vm_start=last_end_pos;
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = last_end_pos;
										while(temp_addr < last_end_pos +length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return last_end_pos;
								}
								else{
									struct vm_area* new_node = alloc_vm_area();
									new_node->vm_start= last_end_pos;
									new_node->vm_end = last_end_pos+length_up;
									new_node->access_flags=prot;
									new_node->vm_next=curr_node->vm_next;
									current->vm_area=new_node;
									//code region begin for MAP_POULATE
									if(is_MAP_POPULATE){
										u64 temp_addr = last_end_pos;
										while(temp_addr < last_end_pos +length_up){
											u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
											temp_addr+=PAGE_SIZE;
										}
									}
									//code region end for MAP_POPULATE
									return last_end_pos;	
								}
							}
						}
						else{
							prev = curr_node;
							last_end_pos = curr_node->vm_end;
							curr_node = curr_node->vm_next;
						}
					}
					if(last_end_pos+length_up<= MMAP_AREA_END){
						if(prev->access_flags==prot){
							prev->vm_end += length_up;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = last_end_pos;
								while(temp_addr < last_end_pos +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return last_end_pos;
						}
						else{
							struct vm_area* new_node=alloc_vm_area();
							new_node->vm_start=last_end_pos;
							new_node->vm_end=last_end_pos+length_up;
							new_node->access_flags=prot;
							prev->vm_next=new_node;
							//code region begin for MAP_POULATE
							if(is_MAP_POPULATE){
								u64 temp_addr = last_end_pos;
								while(temp_addr < last_end_pos +length_up){
									u32 a = my_map_physical_page((unsigned long)osmap(current->pgd),temp_addr,prot,0);
									temp_addr+=PAGE_SIZE;
								}
							}
							//code region end for MAP_POPULATE
							return last_end_pos;
						}
					}


				}	
			}

		}
	}

    long ret_addr = -1;
    return ret_addr;

}

long vm_area_map(struct exec_context *current, u64 addr, int length, int prot, int flags)
{
	long res = my_vm_area_map(current,addr,length,prot,flags);//helper function
	if(res==-1) return -1;
	else{//handle case of 128 vm_areas
		int i=0;
		struct vm_area* curr_node = current->vm_area;
		while(curr_node!=NULL){
			i++;
			curr_node=curr_node->vm_next;
		}
		if(i>128){
			int a = vm_area_unmap(current,res,length);
			return -1;
		}
		else return res;
	}
}
/**
 * munmap system call implemenations
 */
int vm_area_unmap(struct exec_context *current, u64 addr, int length)
{	
	int length_up = up_by_page_size(length);

	int vm_area_count = 0;
	struct vm_area *curr_node2 = current->vm_area;
	while(curr_node2!=NULL){
		vm_area_count++;
		curr_node2=curr_node2->vm_next;
	}

	struct vm_area *curr_node = current->vm_area;
	u64 last_end_pos = MMAP_AREA_START;
	struct vm_area *prev = NULL;
	if(addr < MMAP_AREA_START || addr+length_up > MMAP_AREA_END) return -1;
	if(curr_node==NULL) return 0;
	if(curr_node->vm_start > addr && curr_node->vm_start <addr+length_up) addr = curr_node->vm_start;
	int ret = -1;
	while(curr_node!=NULL){
		if(addr >= curr_node->vm_start && addr < curr_node->vm_end){
			if(curr_node->vm_start == addr && curr_node->vm_end == addr+length_up){
				if(prev==NULL){
					current->vm_area = curr_node->vm_next;
					//code for handling head in case of vfork
					if(current->pgd == get_ctx_by_pid(current->ppid)->pgd){
						get_ctx_by_pid(current->ppid)->vm_area = current->vm_area;
					}
					//end of vfork case
					//begin code for freeing pages
					u64 temp_addr = curr_node->vm_start;
					u64 temp2 = temp_addr;
					while(temp2 < temp_addr + length_up){
						asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
						u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
						temp2 += PAGE_SIZE;
					}
					//end code for freeing pages
					dealloc_vm_area(curr_node);
					return 0;
				}
				else{
					prev->vm_next=curr_node->vm_next;
					//begin code for freeing pages
					u64 temp_addr = curr_node->vm_start;
					u64 temp2 = temp_addr;
					while(temp2 < temp_addr + length_up){
						asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
						u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
						temp2 += PAGE_SIZE;
					}
					//end code for freeing pages
					dealloc_vm_area(curr_node);
					return 0;
				}
			}
			else if(curr_node->vm_start != addr && curr_node->vm_end == addr+length_up){
				//begin code for freeing pages
				u64 temp_addr = addr;
				u64 temp2 = temp_addr;
				while(temp2 < temp_addr + length_up){
					asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
					u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
					temp2 += PAGE_SIZE;				
				}
				//end code for freeing pages
				curr_node->vm_end -= length_up;
				return 0;
			}
			else if(curr_node->vm_start == addr && curr_node->vm_end > addr+length_up){
				//begin code for freeing pages
				u64 temp_addr = addr;
				u64 temp2 = temp_addr;
				while(temp2 < temp_addr + length_up){
					asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
					u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
					temp2 += PAGE_SIZE;
				}
				//end code for freeing pages
				curr_node->vm_start += length_up;
				return 0; 
			}
			else if (curr_node->vm_start != addr && addr + length_up < curr_node->vm_end){
				if(vm_area_count ==128) return -1;
				struct vm_area* new_node=alloc_vm_area();
				new_node->vm_start = addr+length_up;
				new_node->vm_end = curr_node->vm_end;
				new_node->access_flags = curr_node->access_flags;
				new_node->vm_next = curr_node->vm_next;
				curr_node->vm_end = addr;
				curr_node->vm_next= new_node;
				//begin code for freeing pages
				u64 temp_addr = addr;
				u64 temp2 = temp_addr;
				while(temp2 < temp_addr + length_up){
					asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
					u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
					temp2 += PAGE_SIZE;
				}
				//end code for freeing pages
				return 0;
			}
			else if(curr_node->vm_start == addr && curr_node->vm_end < addr+length_up){
				if(curr_node->vm_next!=NULL){
					if(addr+length_up > curr_node->vm_next->vm_start){
						length_up -= (curr_node->vm_next->vm_start - addr);
						addr = curr_node->vm_next->vm_start;
						if(prev==NULL){
							//begin code for freeing pages
							u64 temp_addr = curr_node->vm_start;
							u64 temp2 = temp_addr;
							while(temp2 < curr_node->vm_end){
								asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
								u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
								temp2 += PAGE_SIZE;
							}
							//end code for freeing pages
							current->vm_area = curr_node->vm_next;
							//code for handling head in case of vfork
							if(current->pgd == get_ctx_by_pid(current->ppid)->pgd){
								get_ctx_by_pid(current->ppid)->vm_area = current->vm_area;
							}
							//end of vfork case

							dealloc_vm_area(curr_node);
							curr_node = current->vm_area;
						}
						else{
							//begin code for freeing pages
							u64 temp_addr = curr_node->vm_start;
							u64 temp2 = temp_addr;
							while(temp2 < curr_node->vm_end){
								asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
								u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
								temp2 += PAGE_SIZE;
							}
							//end code for freeing pages
							prev->vm_next = curr_node->vm_next;
							dealloc_vm_area(curr_node);
							curr_node = prev->vm_next;
						}
					}
					else{
						if(prev==NULL){
							//begin code for freeing pages
							u64 temp_addr = curr_node->vm_start;
							u64 temp2 = temp_addr;
							while(temp2 < curr_node->vm_end){
								asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
								u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
								temp2 += PAGE_SIZE;
							}
							//end code for freeing pages
							current->vm_area = curr_node->vm_next;
							//code for handling head in case of vfork
							if(current->pgd == get_ctx_by_pid(current->ppid)->pgd){
								get_ctx_by_pid(current->ppid)->vm_area = current->vm_area;
							}
							//end of vfork case
							dealloc_vm_area(curr_node);
							return 0;
						}
						else{
							//begin code for freeing pages
							u64 temp_addr = curr_node->vm_start;
							u64 temp2 = temp_addr;
							while(temp2 < curr_node->vm_end){
								asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
								u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
								temp2 += PAGE_SIZE;
							}
							//end code for freeing pages
							prev->vm_next = curr_node->vm_next;
							dealloc_vm_area(curr_node);
							return 0;
						}
					}
				}
				else{
					if(prev==NULL){
						//begin code for freeing pages
						u64 temp_addr = curr_node->vm_start;
						u64 temp2 = temp_addr;
						while(temp2 < curr_node->vm_end){
							asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
							u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
							temp2 += PAGE_SIZE;
						}
						//end code for freeing pages
						current->vm_area = NULL;
						//code for handling head in case of vfork
						if(current->pgd == get_ctx_by_pid(current->ppid)->pgd){
							get_ctx_by_pid(current->ppid)->vm_area = current->vm_area;
						}
						//end of vfork case
						dealloc_vm_area(curr_node);
						return 0;
					}
					else{
						//begin code for freeing pages
						u64 temp_addr = curr_node->vm_start;
						u64 temp2 = temp_addr;
						while(temp2 < curr_node->vm_end){
							asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
							u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
							temp2 += PAGE_SIZE;
						}
						//end code for freeing pages
						prev->vm_next = NULL;
						dealloc_vm_area(curr_node);
						return 0;
					}
				}
			}
			else{//(curr_node->vm_start != addr && curr_node->vm_end < addr+length_up){

				//begin code for freeing pages
				u64 temp_addr = addr;
				u64 temp2 = temp_addr;
				while(temp2 < curr_node->vm_end){
					asm volatile ("invlpg (%0);" 
						                    :: "r"(temp2) 
						                    : "memory");   // Flush TLB
					u32 a = my_unmap_physical_page((unsigned long)osmap(current->pgd),temp2);
					temp2 += PAGE_SIZE;
				}
				//end code for freeing pages
				
				if(curr_node->vm_next!=NULL){
					curr_node->vm_end = addr;
					if(addr+length_up > curr_node->vm_next->vm_start){
						length_up -= (curr_node->vm_next->vm_start - addr);
						addr = curr_node->vm_next->vm_start;
						prev = curr_node;

						curr_node=curr_node->vm_next;
					}
					else{
						return 0;
					}
				}
				else{
					curr_node->vm_end = addr;
					return 0;
				}
			}
		}
		else{
			prev = curr_node;
			last_end_pos = curr_node->vm_end;
			curr_node = curr_node->vm_next;	
		}
	}

    int isValid = -1;
    return isValid;
}
