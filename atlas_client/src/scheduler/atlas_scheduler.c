#include <stdlib.h>
#include <stdint.h>
#include "atlas_scheduler.h"
#include "../logger/atlas_logger.h"

typedef struct _atlas_sched_entry
{
    /* File descriptor */
    int fd;
    
    /* Callback */
    atlas_sched_cb_t callback;
    
    /* Indicates if the entry is dirty */
    uint8_t dirty;

    /* Next entry */
    struct _atlas_sched_entry *next;
 } atlas_sched_entry_t;

static atlas_sched_entry_t *sched_entry;

void
atlas_sched_add_entry(int fd, atlas_sched_cb_t cb)
{
    atlas_sched_entry_t *ent, *p;

    if (fd < 0 || !cb)
        return;

    ent = (atlas_sched_entry_t*) malloc(sizeof(atlas_sched_entry_t));
    ent->fd = fd;
    ent->callback = cb;
    ent->dirty = 0;
    ent->next = NULL;

    if (!sched_entry)
        sched_entry = ent;
    else {
        p = sched_entry;
        while(p->next) p = p->next;

        p->next = ent;
    }
}

void
atlas_sched_del_entry(int fd)
{
    atlas_sched_entry_t *p;

    for (p = sched_entry; p; p = p->next)
        if (p->fd == fd) {
            p->dirty = 1;
	    break;
	}
}

void
atlas_sched_loop()
{
    atlas_sched_entry_t *ent, *ent_next, *ent_prev;
    int result, max_fd;
    fd_set readfds;

    ATLAS_LOGGER_DEBUG("Starting scheduler main loop...");

    while (1) {
        FD_ZERO(&readfds);
        max_fd = 0;
        for (ent = sched_entry; ent; ent = ent_next) {
            ent_next = ent->next;

            /* If entry should be deleted */	    
	    if (ent->dirty) {
                if (ent == sched_entry)
                    sched_entry = sched_entry->next;
                else
                    ent_prev->next = ent->next;

		free(ent);
			
	    } else {
                if (ent->fd > max_fd)
                    max_fd = ent->fd;

                FD_SET(ent->fd, &readfds);

		ent_prev = ent;
	    }
        }

        result = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (result > 0) {
            for (ent = sched_entry; ent; ent = ent->next)
                if (!ent->dirty && FD_ISSET(ent->fd, &readfds)) {
                    ent->callback(ent->fd);
                    FD_CLR(ent->fd, &readfds);
                }
        }
    }
}

