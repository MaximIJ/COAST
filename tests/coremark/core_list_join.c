#include "coremark.h"


list_head *core_list_find(list_head *list, list_data *info);
list_head *core_list_reverse(list_head *list);
list_head *core_list_remove(list_head *item);
list_head *core_list_undo_remove(list_head *item_removed,
                                 list_head *item_modified);
list_head *core_list_insert_new(list_head * insert_point,
                                list_data * info,
                                list_head **memblock,
                                list_data **datablock,
                                list_head * memblock_end,
                                list_data * datablock_end);
list_head *core_list_mergesort(list_head *list, int *res);



/* Function: cmp_idx
        Compare the idx item in a list cell, and regen the data.

        Can be used by mergesort.
*/
int
cmp_idx(list_data *a, list_data *b, int *res)
{
    if (res == NULL)
    {
        a->data = (a->data & 0xff00) | (0x00ff & (a->data >> 8));
        b->data = (b->data & 0xff00) | (0x00ff & (b->data >> 8));
    }
    return a->idx - b->idx;
}

void
copy_info(list_data *to, list_data *from)
{
    to->data = from->data;
    to->idx    = from->idx;
}

/* Benchmark for linked list:
        - Try to find multiple data items.
        - List sort
        - Operate on data from list (crc)
        - Single remove/reinsert
        * At the end of this function, the list is back to original state
*/
uint32_t
core_bench_list(list_head *list, uint32_t finder_idx, int seed)
{
    uint32_t     retval = 0;
    uint32_t     found = 0, missed = 0;
    int     find_num = seed;
    list_head *this_find;
    list_head *finder, *remover;
    list_data  info = {0};
    int     i;

    info.idx = finder_idx;
    /* find <find_num> values in the list, and change the list each time
     * (reverse and cache if value found) */
    for (i = 0; i < find_num; i++)
    {
        info.data = (i & 0xff);
        this_find   = core_list_find(list, &info);
        list        = core_list_reverse(list);
        if (this_find == NULL)
        {
            missed++;
            retval += (list->next->info->data >> 8) & 1;
        }
        else
        {
            found++;
            if (this_find->info->data & 0x1) /* use found value */
                retval += (this_find->info->data >> 9) & 1;
            /* and cache next item at the head of the list (if any) */
            if (this_find->next != NULL)
            {
                finder          = this_find->next;
                this_find->next = finder->next;
                finder->next    = list->next;
                list->next      = finder;
            }
        }
        if (info.idx >= 0)
            info.idx++;
    }
    retval += found * 4 - missed;
    /* sort the list by data content and remove one item*/
    if (finder_idx > 0)
        list = core_list_mergesort(list, 1);
    remover = core_list_remove(list->next);
    /* CRC data content of list from location of index N forward, and then undo
     * remove */
    finder = core_list_find(list, &info);
    if (!finder)
        finder = list->next;
    while (finder)
    {
        retval += list->info->data;
        finder = finder->next;
    }

    remover = core_list_undo_remove(remover, list->next);
    /* sort the list by index, in effect returning the list to original state */
    list = core_list_mergesort(list, NULL);
    /* CRC data content of list */
    finder = list->next;
    while (finder)
    {
        retval += 1;
        finder = finder->next;
    }

    return retval;
}
/* Function: core_list_init
        Initialize list with data.

        Parameters:
        blksize - Size of memory to be initialized.
        memblock - Pointer to memory block.
        seed - 	Actual values chosen depend on the seed parameter.
                The seed parameter MUST be supplied from a source that cannot be
   determined at compile time

        Returns:
        Pointer to the head of the list.

*/
list_head *
core_list_init(uint32_t blksize, list_head *memblock, int seed)
{
    /* calculated pointers for the list */
    uint32_t per_item = 16 + sizeof(struct list_data_s);
    uint32_t size     = (blksize / per_item)
                  - 2; /* to accommodate systems with 64b pointers, and make sure
                          same code is executed, set max list elements */
    list_head *memblock_end  = memblock + size;
    list_data *datablock     = (list_data *)(memblock_end);
    list_data *datablock_end = datablock + size;
    /* some useful variables */
    int     i;
    list_head *finder, *list = memblock;
    list_data  info;

    /* create a fake items for the list head and tail */
    list->next         = NULL;
    list->info         = datablock;
    list->info->idx    = 0x0000;
    list->info->data = (int)0x8080;
    memblock++;
    datablock++;
    info.idx    = 0x7fff;
    info.data = (int)0xffff;
    core_list_insert_new(
        list, &info, &memblock, &datablock, memblock_end, datablock_end);

    /* then insert size items */
    for (i = 0; i < size; i++)
    {
        uint32_t datpat = ((uint32_t)(seed ^ i) & 0xf);
        uint32_t dat
            = (datpat << 3) | (i & 0x7); /* alternate between algorithms */
        info.data = (dat << 8) | dat;  /* fill the data with actual data and
                                            upper bits with rebuild value */
        core_list_insert_new(
            list, &info, &memblock, &datablock, memblock_end, datablock_end);
    }
    /* and now index the list so we know initial seed order of the list */
    finder = list->next;
    i      = 1;
    while (finder->next != NULL)
    {
        if (i < size / 5) /* first 20% of the list in order */
            finder->info->idx = i++;
        else
        {
            uint32_t pat = (uint32_t)(i++ ^ seed); /* get a pseudo random number */
            finder->info->idx = 0x3fff
                                & (((i & 0x07) << 8)
                                   | pat); /* make sure the mixed items end up
                                              after the ones in sequence */
        }
        finder = finder->next;
    }
    list = core_list_mergesort(list, NULL);

    return list;
}

/* Function: core_list_insert
        Insert an item to the list

        Parameters:
        insert_point - where to insert the item.
        info - data for the cell.
        memblock - pointer for the list header
        datablock - pointer for the list data
        memblock_end - end of region for list headers
        datablock_end - end of region for list data

        Returns:
        Pointer to new item.
*/
list_head *
core_list_insert_new(list_head * insert_point,
                     list_data * info,
                     list_head **memblock,
                     list_data **datablock,
                     list_head * memblock_end,
                     list_data * datablock_end)
{
    list_head *newitem;

    if ((*memblock + 1) >= memblock_end)
        return NULL;
    if ((*datablock + 1) >= datablock_end)
        return NULL;

    newitem = *memblock;
    (*memblock)++;
    newitem->next      = insert_point->next;
    insert_point->next = newitem;

    newitem->info = *datablock;
    (*datablock)++;
    copy_info(newitem->info, info);

    return newitem;
}

/* Function: core_list_remove
        Remove an item from the list.

        Operation:
        For a singly linked list, remove by copying the data from the next item
        over to the current cell, and unlinking the next item.

        Note:
        since there is always a fake item at the end of the list, no need to
   check for NULL.

        Returns:
        Removed item.
*/
list_head *
core_list_remove(list_head *item)
{
    list_data *tmp;
    list_head *ret = item->next;
    /* swap data pointers */
    tmp        = item->info;
    item->info = ret->info;
    ret->info  = tmp;
    /* and eliminate item */
    item->next = item->next->next;
    ret->next  = NULL;
    return ret;
}

/* Function: core_list_undo_remove
        Undo a remove operation.

        Operation:
        Since we want each iteration of the benchmark to be exactly the same,
        we need to be able to undo a remove.
        Link the removed item back into the list, and switch the info items.

        Parameters:
        item_removed - Return value from the <core_list_remove>
        item_modified - List item that was modified during <core_list_remove>

        Returns:
        The item that was linked back to the list.

*/
list_head *
core_list_undo_remove(list_head *item_removed, list_head *item_modified)
{
    list_data *tmp;
    /* swap data pointers */
    tmp                 = item_removed->info;
    item_removed->info  = item_modified->info;
    item_modified->info = tmp;
    /* and insert item */
    item_removed->next  = item_modified->next;
    item_modified->next = item_removed;
    return item_removed;
}

/* Function: core_list_find
        Find an item in the list

        Operation:
        Find an item by idx (if not 0) or specific data value

        Parameters:
        list - list head
        info - idx or data to find

        Returns:
        Found item, or NULL if not found.
*/
list_head *
core_list_find(list_head *list, list_data *info)
{
    if (info->idx >= 0)
    {
        while (list && (list->info->idx != info->idx))
            list = list->next;
        return list;
    }
    else
    {
        while (list && ((list->info->data & 0xff) != info->data))
            list = list->next;
        return list;
    }
}
/* Function: core_list_reverse
        Reverse a list

        Operation:
        Rearrange the pointers so the list is reversed.

        Parameters:
        list - list head
        info - idx or data to find

        Returns:
        Found item, or NULL if not found.
*/

list_head *
core_list_reverse(list_head *list)
{
    list_head *next = NULL, *tmp;
    while (list)
    {
        tmp        = list->next;
        list->next = next;
        next       = list;
        list       = tmp;
    }
    return next;
}
/* Function: core_list_mergesort
        Sort the list in place without recursion.

        Description:
        Use mergesort, as for linked list this is a realistic solution.
        Also, since this is aimed at embedded, care was taken to use iterative
   rather then recursive algorithm. The sort can either return the list to
   original order (by idx) , or use the data item to invoke other other
   algorithms and change the order of the list.

        Parameters:
        list - list to be sorted.
        cmp - cmp function to use

        Returns:
        New head of the list.

        Note:
        We have a special header for the list that will always be first,
        but the algorithm could theoretically modify where the list starts.

 */
list_head *
core_list_mergesort(list_head *list, int *res)
{
    list_head *p, *q, *e, *tail;
    uint32_t     insize, nmerges, psize, qsize, i;

    insize = 1;

    while (1)
    {
        p    = list;
        list = NULL;
        tail = NULL;

        nmerges = 0; /* count number of merges we do in this pass */

        while (p)
        {
            nmerges++; /* there exists a merge to be done */
            /* step `insize' places along from p */
            q     = p;
            psize = 0;
            for (i = 0; i < insize; i++)
            {
                psize++;
                q = q->next;
                if (!q)
                    break;
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q))
            {

                /* decide whether next element of merge comes from p or q */
                if (psize == 0)
                {
                    /* p is empty; e must come from q. */
                    e = q;
                    q = q->next;
                    qsize--;
                }
                else if (qsize == 0 || !q)
                {
                    /* q is empty; e must come from p. */
                    e = p;
                    p = p->next;
                    psize--;
                }
                else if (cmp_idx(p->info, q->info, res) <= 0)
                {
                    /* First element of p is lower (or same); e must come from
                     * p. */
                    e = p;
                    p = p->next;
                    psize--;
                }
                else
                {
                    /* First element of q is lower; e must come from q. */
                    e = q;
                    q = q->next;
                    qsize--;
                }

                /* add the next element to the merged list */
                if (tail)
                {
                    tail->next = e;
                }
                else
                {
                    list = e;
                }
                tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }

        tail->next = NULL;

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1) /* allow for nmerges==0, the empty list case */
            return list;

        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }

    return list;
}
