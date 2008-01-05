/*
 * Copyright (c) 2007 Josef 'Jeff' Sipek
 */

#include <magic.h>
#include <buddy.h>
#include <slab.h>
#include <page.h>

static struct slab *generic[6];

/*
 * Create slab caches for 16, 32, 64, 128, and 256 byte allocations
 */
int init_slab()
{
	generic[0] = create_slab(16, 4);
	if (!generic[0])
		goto out_err;

	generic[1] = create_slab(32, 4);
	if (!generic[1])
		goto out_err;

	generic[2] = create_slab(64, 4);
	if (!generic[2])
		goto out_err;

	generic[3] = create_slab(128, 4);
	if (!generic[3])
		goto out_err;

	generic[4] = create_slab(256, 4);
	if (!generic[4])
		goto out_err;

	generic[5] = create_slab(512, 8);
	if (!generic[5])
		goto out_err;

	return 0;

out_err:
	free_slab(generic[0]);
	free_slab(generic[1]);
	free_slab(generic[2]);
	free_slab(generic[3]);
	free_slab(generic[4]);
	free_slab(generic[5]);

	return -ENOMEM;
}

/**
 * create_slab - Create a new slab
 * @objsize:	object size in bytes
 * @align:	object alignment in bytes (must be a power of two)
 */
struct slab *create_slab(u16 objsize, u8 align)
{
	struct page *page;
	struct slab *slab;

	if (!objsize || !align)
		return NULL;

	page = alloc_pages(0);
	if (!page)
		return NULL;

	slab = page_to_addr(page);
	memset(slab, 0, PAGE_SIZE);

	slab->magic = SLAB_MAGIC;
	INIT_LIST_HEAD(&slab->slabs);
	INIT_LIST_HEAD(&slab->slab_pages);

	align--; /* turn into a mask */

	/* actual object size */
	if (objsize & align)
		objsize += align + 1 - (objsize & align);
	slab->objsize = objsize;

	/* number of objects in a page */
	slab->count = 8 * (PAGE_SIZE - sizeof(struct slab)) / (8 * objsize + 1);
	
	/* offset of the first object */
	slab->startoff = sizeof(struct slab) + (slab->count + 4) / 8;
	if (slab->startoff & align) {
		u16 tmp;

		slab->startoff += align + 1 - (slab->startoff & align);

		/* 
		 * TODO: there's got to be a better way to ensure that we
		 * fit into a single page
		 */
		tmp = slab->startoff + slab->count * slab->objsize;
		if (tmp > PAGE_SIZE)
			slab->count--;
	}

	slab->used = 0;

	return slab;
}

void free_slab(struct slab *passed_slab)
{
	struct slab *slab;

	if (!passed_slab)
		return;

	BUG_ON(passed_slab->magic != SLAB_MAGIC);
	BUG_ON(passed_slab->used != 0);

	list_for_each_entry(slab, &passed_slab->slab_pages, slab_pages) {
		BUG_ON(slab->magic != SLAB_CONT_MAGIC);
		BUG_ON(slab->used != 0);

		/*
		 * Theoretically, we should remove the page from the list,
		 * but no one _really_ cares
		 */

		free_pages(slab, 0);
	}

	free_pages(passed_slab, 0);
}

static inline void *__alloc_slab_obj_newpage(struct slab *slab)
{
	struct page *page;
	struct slab *new;

	page = alloc_pages(0);
	if (!page)
		return ERR_PTR(-ENOMEM);

	new = page_to_addr(page);

	memset(new, 0, PAGE_SIZE);
	new->magic = SLAB_CONT_MAGIC;
	new->objsize = slab->objsize;
	new->startoff = slab->startoff;
	new->count = slab->count;

	/* add it to the current slab */
	list_add_tail(&new->slab_pages, &slab->slab_pages);

	return new;
}

void *alloc_slab_obj(struct slab *passed_slab)
{
	struct slab *slab = passed_slab;
	int objidx;
	u8 *bits;
	u8 mask;

	if (!slab)
		return NULL;

	/*
	 * Does the first slab page have an unused object?
	 */
	if (slab->used < slab->count)
		goto alloc;

	/*
	 * No. Find the first slab page that has unused objects
	 */
	list_for_each_entry(slab, &passed_slab->slab_pages, slab_pages)
		if (slab->used < slab->count)
			goto alloc;

	/*
	 * None of the pages have an unused object. Let's allocate another
	 * page
	 */

	slab = __alloc_slab_obj_newpage(passed_slab);
	if (IS_ERR(slab))
		return NULL;

alloc:
	/* found a page */
	for (objidx = 0; objidx < slab->count; objidx++) {
		bits = slab->bitmap + (objidx/8);

		mask = 1 << (7 - (objidx % 8));

		if (*bits & mask)
			continue;

		slab->used++;
		*bits |= mask;

		return ((u8*) slab) + slab->startoff + slab->objsize * objidx;
	}

	/* SOMETHING STRANGE HAPPENED */
	BUG();
	return NULL;
}

void free_slab_obj(void *ptr)
{
	struct slab *slab;
	int objidx;
	u8 *bits;

	/* get the slab object ptr */
	slab = (struct slab *) (((u64) ptr) & ~0xfff);

	/* calculate the object number */
	objidx = (((u64) ptr) - ((u64) slab) - slab->startoff) / slab->objsize;

	/* update the bitmap */
	bits = slab->bitmap + (objidx/8);
	*bits &= ~(1 << (7 - (objidx % 8)));

	if (--slab->used) {
		/* free the page? */
	}
}

void *malloc(int size)
{
	if (!size)
		return NULL;
	if (size <= 16)
		return alloc_slab_obj(generic[0]);
	if (size <= 32)
		return alloc_slab_obj(generic[1]);
	if (size <= 64)
		return alloc_slab_obj(generic[2]);
	if (size <= 128)
		return alloc_slab_obj(generic[3]);
	if (size <= 256)
		return alloc_slab_obj(generic[4]);
	if (size <= 512)
		return alloc_slab_obj(generic[5]);
	return NULL;
}

void free(void *ptr)
{
	if (ptr)
		free_slab_obj(ptr);
}
