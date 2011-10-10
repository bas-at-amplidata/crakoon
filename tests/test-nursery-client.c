/*
 * This file is part of Arakoon, a distributed key-value store.
 *
 * Copyright (C) 2010 Incubaid BVBA
 *
 * Licensees holding a valid Incubaid license may use this file in
 * accordance with Incubaid's Arakoon commercial license agreement. For
 * more information on how to enter into this agreement, please contact
 * Incubaid (contact details can be found on http://www.arakoon.org/licensing).
 *
 * Alternatively, this file may be redistributed and/or modified under
 * the terms of the GNU Affero General Public License version 3, as
 * published by the Free Software Foundation. Under this license, this
 * file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU Affero General Public License for more details.
 * You should have received a copy of the
 * GNU Affero General Public License along with this program (file "COPYING").
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arakoon.h"
#include "arakoon-nursery.h"

#include "memory.h"
#include "utils.h"

int main(int argc, char **argv) {
        ArakoonCluster *keeper = NULL;
        ArakoonNursery *nursery = NULL;
        arakoon_rc rc = 0;
        int i = 0;

        const ArakoonMemoryHooks hooks = {
                check_arakoon_malloc,
                check_arakoon_free,
                check_arakoon_realloc
        };

        if((argc - 2) % 3 != 0 || argc < 5) {
                fprintf(stderr, "Usage: %s cluster [name host port]*\n",
                        argv[0]);
                return 1;
        }

        arakoon_memory_set_hooks(&hooks);
        arakoon_log_set_handler(arakoon_log_get_stderr_handler());

        keeper = arakoon_cluster_new(argv[1]);
        ABORT_IF_NULL(keeper, "arakoon_cluster_new");

        for(i = 2; i < argc; i++) {
            rc = arakoon_cluster_add_node_tcp(keeper, argv[i], argv[i + 1],
                    argv[i + 2]);
            ABORT_IF_NOT_SUCCESS(rc, "arakoon_cluster_add_node_tcp");
            i += 2;
        }

        rc = arakoon_cluster_connect_master(keeper, NULL);
        ABORT_IF_NOT_SUCCESS(rc, "arakoon_cluster_connect_master");

        nursery = arakoon_nursery_new(keeper);
        ABORT_IF_NULL(nursery, "arakoon_nursery_new");

        rc = arakoon_nursery_update_config(nursery, NULL);
        ABORT_IF_NOT_SUCCESS(rc, "arakoon_nursery_update_config");

        arakoon_nursery_free(nursery);
        arakoon_cluster_free(keeper);

        return 0;
}
